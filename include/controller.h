#include <wav.h>
#include <mmp.h>
//#include <system.h>
#include <visualizer.h>
#include <fstream>
#include <config.h>

//Class to read main config and do stuff and 

class Controller{
  std::string on_end_command;
  std::string video_config_file;
  int verboseLevel;
  double aFrequency;
  std::vector<double> times;
public:
  class Track{
  private:
  public:
    std::vector<std::vector<double> > values;
    double mLength;
    enum Type{Undefined, WAV, MMP};
    
    //Parameters read from configFile:
    Type type;
    WavFile* wavFile;
    MMPFile* mmpFile;
    double F0,F1,FK,CHLEN,THR;
    int channel,track;
    std::string file;
  public:
    void loadConfig(std::string& configStr);
    Track():
      mLength(0), type(Undefined),wavFile(NULL),mmpFile(NULL),
      F0(16.35),F1(8000),FK(1.0594630943592953),CHLEN(1),THR(0.01),
      channel(0),track(0),file("") { }
    double length(){return mLength;}
    void load(int verboseLevel);
    void createSpectrum(std::vector<double>& times, int verboseLevel);
    std::vector<double>* getValues(int i){return &values[i];}
  };
private:
  std::vector<Track> tracks;
public:
  void loadFiles();
  void createSpectrums();
  void runVisualizer();
  
  void setConfigParam(std::string& param, std::string& paramKey, std::string& value);
  void readMainConfig(std::ifstream& co);
  Controller(std::string config_file_name, int pVerboseLevel): on_end_command(""),video_config_file("visualizer.config"),verboseLevel(pVerboseLevel),aFrequency(40){
    std::ifstream iff(config_file_name);
    readMainConfig(iff);
  }
};