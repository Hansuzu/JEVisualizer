#include <vector>
#include <opencv2/core.hpp>        // Basic OpenCV structures (cv::Mat)
#include <opencv2/highgui.hpp>  // Video write
#include <fstream>
#include <layer.h>


//class Visualizer is a class that creates visualized video from music, when given an array of spectres of that music



class Visualizer{
private:
  double fps;
  int w, h;
  
  cv::VideoWriter outputVideo;
  cv::Mat* oframe;
  
  double ctime;
  int cframe;
  std::string ovfname;
  
private:
  int firstFrame;
  int lastFrame;
public:
  struct Track {
    std::vector<double> est;
    std::vector<double> sit;
    double maxDownSpeed;
    double maxUpSpeed;
    Track():maxDownSpeed(10000),maxUpSpeed(100000){
      
    }
  };
private:
  std::vector<Layer*> layers;
  std::vector<Track*> tracks;
  std::vector<std::vector<double>* > trackValues; //Values of track sit. A reference this vector is passed to layers as an information about tracks
  void nextFrame(int verboseLevel); //Renders a frame of video
  double limitEstValue(double estv, double stv, double dt, double maxDownSpeed, double maxUpSpeed);
public:
  void next(double time, std::vector<std::vector<double>* >& st, int verboseLevel); //calls next_frame() when it's needed
  
  void setConfigParam(std::string& param, std::string& paramKey, std::string& value, int verboseLevel);
  void readConfig(std::istream& co, int verboseLevel);
  
  Visualizer(const char* configFile, int verboseLevel):fps(20), w(640), h(480),ctime(0), cframe(0),ovfname("default.ogv"),firstFrame(0),lastFrame(1000000000){
    std::ifstream co;
    co.open(configFile);
    readConfig(co, verboseLevel);
    
    oframe=new cv::Mat(cv::Size(w, h), CV_8UC3);
    outputVideo.open(ovfname, cv::VideoWriter::fourcc('t','h','e','o'), fps, cv::Size(w, h));

  }
  ~Visualizer(){
    delete oframe;
  }
};
