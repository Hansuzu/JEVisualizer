#include <vector>
#include <string>

#ifndef TRACKCONTROLLER_H
#define TRACKCONTROLLER_H

class TrackController{
public:
  
  class Track {
  public:
    std::vector<double> values;
    static double limitValue(double vOld, double vNew, double dt, double maxDownSpeed, double maxUpSpeed){
      return std::min(std::max(vNew, vOld-dt*maxDownSpeed), vOld+dt*maxUpSpeed);
    }
  private:
    double maxDownSpeed; //TODO: check how these are used and configured...
    double maxUpSpeed;
    double updateTime;
  public:
    int size(){return values.size();}
    void update(std::vector<double>* newValues, double time, double newValueTime);
    void clean(double time);
    void setMaxDownSpeed(double speed){maxDownSpeed=speed;}
    void setMaxUpSpeed(double speed){maxUpSpeed=speed;}
    Track():maxDownSpeed(100000),maxUpSpeed(100000),updateTime(-1000000){}
  };
  
private:
  std::vector<Track> tracks;
public:
  class Index{ // points to one value of a track, getValue checks whether Index points to a valid position and returns the value (0 if invalid)
  private:
    TrackController* controller;
    int trackIndex;
    int noteIndex;
  public:
    double getValue();
    Index(TrackController* pController, int pTrackIndex, int pNoteIndex):controller(pController), trackIndex(pTrackIndex), noteIndex(pNoteIndex) {}
  };
  int size(){return tracks.size();}
  int size(int i){if (i>=0 && i<(int)tracks.size()) return tracks[i].size(); return 0;}
  
  void setMaxDownSpeed(int track, double speed){
    if (track>=0 && track<=(int)tracks.size()) tracks[track].setMaxDownSpeed(speed);
  }
  void setMaxDownSpeed(double speed){
    for (int i=0;i<(int)tracks.size();++i) setMaxDownSpeed(i, speed);
  }
  void setMaxUpSpeed(int track, double speed){
    if (track>=0 && track<=(int)tracks.size()) tracks[track].setMaxUpSpeed(speed);
  }
  void setMaxUpSpeed(double speed){
    for (int i=0;i<(int)tracks.size();++i) setMaxUpSpeed(i, speed);
  }
    
  void updateValues(std::vector<std::vector<double>*>& newValues, double time, double newValueTime); // Update the values of tracks
  void clean(double time); //Set all values to zero
  
  Index getIndex(int i, int j); //returns an Index object to tracks[i][j]
  
  static void splitString(const std::string& str, std::vector<std::string>& res, char c);
  static void parseIntegers(const std::string& str, std::vector<int>& ans);
  void parseTrackIndices(const std::string& str, std::vector<Index>& res); //Parses the indices given as string 
                                                            // ("0:1-4,2:9-8" -> track 0, indices 1,2,3,4, track 2 indices 9,8)
};

#endif