#include <vector>
#include <opencv2/core.hpp>        // Basic OpenCV structures (cv::Mat)
#include <opencv2/highgui.hpp>     // Video write
#include <fstream>
#include <config.h>
#include <trackcontroller.h>
#include <fpe.h>
#include <layer.h>

#ifndef VISUALIZER_H
#define VISUALIZER_H

//class Visualizer is a class that creates visualized video from music, when given an array of spectres of that music



class Visualizer{
private:
  double fps; //fps of output video
  int w, h;   //width and height of output video
  
  cv::VideoWriter outputVideo;
  cv::Mat* oframe; // a frame that is passed to layers to write in, this is then written to outputVideo
  
  double ctime; // current time
  int cframe;   // current frame
  std::string ovfname; // the filename of output video
  
  FormulaParameterEngine fpe; // Contains information about parameters that can be used in formulas
  TrackController tc; // Contains values of tracks and other stuff, a pointer to this is passed to layers
private:
  int firstFrame;  // first frame to actually render (nextFrame checks if firstFrame<=cframe)
  int lastFrame;   // last frame to actually render  (nextFrame checks if cframe<=lastFrame)
public:
private:
  std::vector<Layer*> layers; // Layers that are rendered
  
  // stuff to make FormulaParameterEngine work
  std::vector<std::vector<TrackController::Index> > fpeTracks; // used when some formula-parameter value depends on some values of tracks
  std::vector<double> fpevs;  // values that are passed to fpe when it is updated
  void setFPEV(std::string& key, std::string& value, int verboseLevel); // handles the formula-parameters and pushes them to fpe, setConfigParam calls this
  void updateFPE(); // calls fpe.update
  
  void nextFrame(int verboseLevel); //Renders a frame of video
public:
  void next(double time, std::vector<std::vector<double>* >& newTrackValues, int verboseLevel); //calls nextFrame() when it's needed

private:
  void setConfigParam(std::string& param, std::string& paramKey, std::string& value, int verboseLevel); // Handles one statement of config-file
  void readConfig(std::istream& co, int verboseLevel); // loads a configuration from a file, calls setConfigParam for each configuratino statement
public:
  
  Visualizer(const char* configFile, int verboseLevel):fps(20), w(640), h(480),ctime(0), cframe(0),ovfname("default.ogv"),fpe(NULL),firstFrame(0),lastFrame(1000000000){
    std::cout << "hhui" << std::endl;
    std::ifstream co;
    co.open(configFile);
    readConfig(co, verboseLevel);
    std::cout << "hhui" << std::endl;
    
    oframe=new cv::Mat(cv::Size(w, h), CV_8UC3);
    outputVideo.open(ovfname, cv::VideoWriter::fourcc('t','h','e','o'), fps, cv::Size(w, h));
  }
  ~Visualizer(){
    delete oframe;
  }
};


#endif