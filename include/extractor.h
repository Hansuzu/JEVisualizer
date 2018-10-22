#include <vector>
#include <opencv2/core.hpp>        // Basic OpenCV structures (cv::Mat)
#include <opencv2/highgui.hpp>     // Video write
#include <fstream>
#include <config.h>
#include <trackcontroller.h>

#ifndef EXTRACTOR_H
#define EXTRACTOR_H

//class Extractor is a class that analyses data and creates a text file about the results.



class Extractor{
private:
  std::string ofilename;
  std::ofstream ofile;
  double fps;
  double limit;
  double peekLimit;
  double maxDownSpeed;
  double maxUpSpeed;
  double ctime; // current time
  int cframe;   // current frame

  TrackController tc; // Contains values of tracks and other stuff, a pointer to this is passed to layers
  
  std::vector<std::pair<std::pair<int, int>, std::string> > map;
  std::vector<std::pair<std::string, std::vector<std::string> > > groups;
  std::vector<std::pair<std::string, std::vector<std::string> > > chords;
  
  void nextFrame(int verboseLevel);
public:
  void next(double time, std::vector<std::vector<double>* >& newTrackValues, int verboseLevel);
  
  std::pair<int, int> split(std::string& value, int verboseLevel);
  void getArray(std::string& value, std::vector<std::string>& res, int verboseLevel);
  
  void loadMap(std::string& param, int verboseLevel);
  void loadGroups(std::string& param, int verboseLevel);
  void loadChords(std::string& param, int verboseLevel);
private:
  void setConfigParam(std::string& param, std::string& paramKey, std::string& value, int verboseLevel); // Handles one statement of config-file
  void readConfig(std::istream& co, int verboseLevel); // loads a configuration from a file, calls setConfigParam for each configuratino statement
public:
  Extractor(const char* configFile, int verboseLevel):ofilename(""), fps(10), limit(1e-4), peekLimit(0), maxDownSpeed(10000), maxUpSpeed(1000), ctime(0), cframe(0){
    std::ifstream co;
    co.open(configFile);
    readConfig(co, verboseLevel);
    ofile.open(ofilename.c_str());
  }
  ~Extractor(){
    ofile.close();
  }
};


#endif