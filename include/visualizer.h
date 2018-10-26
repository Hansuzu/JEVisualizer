#include <vector>
#include <future>
#include <chrono>
#include <opencv2/core.hpp>        // Basic OpenCV structures (cv::Mat)
#include <opencv2/highgui.hpp>     // Video write
#include <fstream>
#include <config.h>
#include <trackcontroller.h>
#include <fpe.h>
#include <layer.h>
#include <globalsettings.h>

#ifndef VISUALIZER_H
#define VISUALIZER_H

//class Visualizer is a class that creates visualized video from music, when given an array of spectres of that music



class Visualizer{
private:
  double fps; //fps of output video
  int w, h;   //width and height of output video
  
  cv::VideoWriter outputVideo;
  cv::Mat* oframe;     // a frame that is passed to layers to write in, this is then written to outputVideo
  cv::Mat* oframeWrite;// a frame that is written to outputVideo, first oframe and oframeWrite are swapped, then oframeWrite is called asynchronously
  
  double ctime; // current time
  int cframe;   // current frame
  std::string ovfname; // the filename of output video
  
  double maxDownSpeed;
  double maxUpSpeed;
  
  FormulaParameterEngine fpe; // Contains information about parameters that can be used in formulas
  TrackController tc; // Contains values of tracks and other stuff, a pointer to this is passed to layers
private:
  int firstFrame;  // first frame to actually render (nextFrame checks if firstFrame<=cframe)
  int lastFrame;   // last frame to actually render  (nextFrame checks if cframe<=lastFrame)
public:
private:
  std::vector<Layer*> layers; // Layers that are rendered
  std::vector<std::future<void> > layerIndependentDrawers;
  
  // stuff to make FormulaParameterEngine work
  std::vector<std::vector<TrackController::Index> > fpeTracks; // used when some formula-parameter value depends on some values of tracks
  std::vector<double> fpevs;  // values that are passed to fpe when it is updated
  void setFPEV(std::string& key, std::string& value); // handles the formula-parameters and pushes them to fpe, setConfigParam calls this
  void updateFPE(); // calls fpe.update
  
  std::future<void> frameWriter;
  void writeNewFrame(int cframe, std::chrono::duration<double> timeElapsedBeforeWrite);
  void launchWriteNewFrame(std::chrono::duration<double> timeElapsedBeforeWrite);
  void waitWriteFrameToFinish();
  
  std::chrono::time_point<std::chrono::high_resolution_clock> lastFrameFinishTime;
  void nextFrame(); //Renders a frame of video
public:
  void next(double time, std::vector<std::vector<double>* >& newTrackValues); //calls nextFrame() when it's needed

private:
  void setConfigParam(std::string& param, std::string& paramKey, std::string& value); // Handles one statement of config-file
  void readConfig(std::istream& co); // loads a configuration from a file, calls setConfigParam for each configuratino statement
public:
  
  Visualizer(const char* configFile):fps(20), w(640), h(480),ctime(0), cframe(0),ovfname("default.ogv"),maxDownSpeed(100000), maxUpSpeed(100000),fpe(NULL),firstFrame(0),lastFrame(1000000000){
    std::ifstream co;
    co.open(configFile);
    readConfig(co);
    oframe=new cv::Mat(cv::Size(w, h), CV_8UC3);
    oframeWrite=new cv::Mat(cv::Size(w, h), CV_8UC3);
    outputVideo.open(ovfname, cv::VideoWriter::fourcc('t','h','e','o'), fps, cv::Size(w, h));
  }
  ~Visualizer(){
    delete oframe;
    waitWriteFrameToFinish();
    delete oframeWrite;
  }
};


#endif