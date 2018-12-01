#include <fstream>
#include <vector>
#include <string>
#include <globalsettings.h>

#define MMP_ERROR_OPENING_ERROR               1
#define MMP_ERROR_READING_ERROR               2

class MMPFile{
public:
  class XML{
  public:
    enum Type{Root, Unset, Prologue, DOCTYPE, Tag, EndTag, Data, Comment};
    std::string name;
    std::vector<std::pair<std::string, std::string> > params;
    Type type;
    int depth;
    std::vector<XML> contents;
    void print(int);
    int parse(std::string& str, int pos);
    void find(std::string tag, std::vector<XML*>& res);
    std::pair<bool, std::string> findParam(std::string paramName);
    XML* findOne(std::string tag);
    XML():name(),params(),type(Type::Unset),depth(0),contents(){}
  };
private:
  XML root;
  XML* lmmsProjectNode;
  XML* songNode;
  XML* trackContainerNode;
  double BPM;
  class Track{
  public:
    struct Note{
      int timeBegins;
      int timeEnds;
      int key;
      int volume;
      Note(int tb, int te, int k, int v):timeBegins(tb),timeEnds(te),key(k),volume(v){}
      Note(const Note& o):timeBegins(o.timeBegins),timeEnds(o.timeEnds),key(o.key),volume(o.volume){}
      bool operator<(const Note&)const{return 0;}
    };
  private:
    int keyBuffer[108];
    double lastNoteEndTime;
    std::vector<std::pair<int, Note> > events;
    std::vector<Note> notes;
    int currentTime;
    int currentEventIndex;
  public:
    std::vector<Note>& getNotes(){return notes;}
    double getLastNoteEndTime(){return lastNoteEndTime;}
    void setNote(int pos, int len, int key, int vol);
    void setNote(Note& note, int deltaPos); // for beat/bassline notes
    void initSimulation();
    void getTime(int time, std::vector<double>& rv);
    Track(){
      lastNoteEndTime=0;
      setNote(-1000000000, 1000000000, 0, 0); //avoid index-checking on
    }
  };
  std::vector<Track> tracks;
  struct BBInstrument{
    std::vector<std::pair<int, Track> > bbtracks; // steps, track
  };
  std::vector<BBInstrument> bbinstruments;
  std::vector<Track> bbitracks;
  
  void parseFromXML();
  double timeToSecs(double time){return time/48/BPM*60;}
  double timeFromSecs(double time){return time/60*48*BPM;}
public:
  std::vector<XML*> trackNodes;
  int read(const char* filename);

  double length(int track, bool bbi){return timeToSecs(bbi?bbitracks[track].getLastNoteEndTime():tracks[track].getLastNoteEndTime());}
  void spectrums(int track, std::vector<double>& times, std::vector<std::vector<double> >& ans, bool usebb);
  MMPFile():lmmsProjectNode(NULL), trackContainerNode(NULL){}  
};