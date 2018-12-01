#include <vector>
#include <string>
#include <globalsettings.h>

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
    double multiplier;
  public:
    int size(){return values.size();}
    void update(std::vector<double>* newValues, double time, double newValueTime);
    void clean(double time);
    void setMaxDownSpeed(double speed){maxDownSpeed=speed;}
    void setMaxUpSpeed(double speed){maxUpSpeed=speed;}
    void setMultiplier(double k){multiplier=k;}
    Track():maxDownSpeed(100000),maxUpSpeed(100000),updateTime(-1000000),multiplier(1){}
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
  void setMaxDownSpeeds(std::vector<double>& speeds){
    for (int i=0;i<std::min((int)tracks.size(), (int)speeds.size());++i) setMaxDownSpeed(i, speeds[i]);
  }
  void setMaxUpSpeed(int track, double speed){
    if (track>=0 && track<=(int)tracks.size()) tracks[track].setMaxUpSpeed(speed);
  }
  void setMaxUpSpeeds(std::vector<double>& speeds){
    for (int i=0;i<std::min((int)tracks.size(), (int)speeds.size());++i) setMaxUpSpeed(i, speeds[i]);
  }
  void setMultiplier(int track, double multiplier){
    if (track>=0 && track<=(int)tracks.size()) tracks[track].setMultiplier(multiplier);
  }
  void setMultipliers(std::vector<double>& multipliers){
    for (int i=0;i<std::min((int)tracks.size(), (int)multipliers.size());++i) setMultiplier(i, multipliers[i]);
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