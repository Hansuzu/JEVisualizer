#include <iostream>
#include <vector>
#include <opencv2/core.hpp>        // Basic OpenCV structures (cv::Mat)
#include <opencv2/imgproc.hpp>    // Basic OpenCV structures (cv::Mat)
#include <opencv2/highgui.hpp> 
#include <globalsettings.h>
#include <config.h>
#include <trackcontroller.h>
#include <fpe.h>
#include <formula.h>
#include <formulacolor.h>
#include <drawer.h>
#include <filter.h>
#include <particlesource.h>

#ifndef LAYER_H
#define LAYER_H



class Layer{
private: 
  FormulaParameterEngine fpe; // parameter to formulas
  
  double layerWidth, layerHeight; // The coordinates in Drawers are relative to these values. Values are then scaled to actual width and height
  int firstFrame, lastFrame;      // First and last frame to actually draw
  cv::Mat* frame1;                // Layer is drawn on these (two frames are needed for filters)
  cv::Mat* frame2;
  cv::Mat* frameIndependent;      // After independent drawing is finished, the frame is swapped to here, so independent picture can be used in imageFilters
  cv::Mat* background;            // if background-image is used, it is saved here
  cv::Mat* bgVideoFrame;            // if background-image is used, it is saved here
  cv::Mat* bgVideoHelperFrame;            // if background-image is used, it is saved here
  bool hasBgImage;
  FormulaColor LU, LD, RU, RD;    // if background-color is used
  cv::Scalar lLU, lLD, lRU, lRD;
  bool hasBgColor;
  bool invisibleModeOn; // layers own content is not drawn to output, but can be used as a filter.
  
  std::string bgVideoFileName;
  cv::VideoCapture bgVideo;
  enum VideoOnEnd{None, LastFrame, Replay};
  VideoOnEnd videoOnEnd; //Will the video disappear, will the last frame be left on screen, will the video replay etc.
  Formula bgVideoPlaySpeed; //The speed of the video playback
  int bgVideoFirstFrame, bgVideoLastFrame; //First frame of video to draw, and last
  int bgVideoBegin, bgVideoEnd; //Time of which video begins or ends
  double bgVideoDeltaTime; // The amount of time (videospeed scaled) passed after the last frame
  bool hasBgVideo;
  
  std::vector<Drawer*> drawers;                 // all drawers
  std::vector<Filter*> layerFilters;            // filters that are applied to the layer before it is rendered on the output-frame
  std::vector<Filter*> imageFilters;            // filters that are applied to the output-frame after this layer is rendered on it
  std::vector<ParticleSource*> particleSources; // all particleSources;
  
  TrackController* tc;               // Pointer to the track-controller, through this the Layer has access to the track data
  void drawDrawers(int cframe);   //function to draw all drawers of the layer
  void drawParticles(int cframe); //function to draw all particles of the layer
  
  bool isVisible(int cframe, double ctime); // checks whether the layer is Visible: 
  
  //Magic to make fpe work (basically same as in visualizer.h)
  std::vector<std::vector<TrackController::Index> > fpeTracks;
  std::vector<double> fpevs;
  void setFPEV(std::string& key, std::string& value);
  void updateFPE(int cframe);
  
  
  void bgVideoMoveFrame();
  void bgVideoRewind();
  void bgVideoGrabNextFrame();
  void bgVideoGetFrame(int frameIndex);
  
  void createBgColorBackground(bool force=0);
  void createBgVideoBackground(int cframe, double ctime);
  
  void applyLayerFilters();
public:
  void drawIndependent(int cframe, double ctime); // draw layer-things that can be drawn without oframe (-> )
private:
  void drawImageToLayer(cv::Mat* oframe);
  void applyImageFilters();
  void drawLayerToImage(cv::Mat* oframe);
public:
  void drawToOframe(int cframe, double ctime, cv::Mat* oframe);    // draw layer to oframe (drawIndepent should be called before this)

private:
  void setConfigParam(std::string& param, std::string& key, std::string& value); //readConfig calls this

public:   //TODO: remove debug prints
  void readConfig(const char* configFile);  // load configuration from file
  
  Layer(int w, int h, FormulaParameterEngine* pfpe, TrackController* ptc):
        fpe(pfpe), 
        layerWidth(w), layerHeight(h), firstFrame(0), lastFrame(1000000),
        frame1(nullptr), frame2(nullptr), frameIndependent(nullptr), background(nullptr),
        bgVideoFrame(nullptr), bgVideoHelperFrame(nullptr),
        hasBgImage(0), LU(&fpe), LD(&fpe), RU(&fpe), RD(&fpe), hasBgColor(0), invisibleModeOn(0),
        videoOnEnd(None), bgVideoPlaySpeed(&fpe), bgVideoFirstFrame(0), bgVideoLastFrame(1000000),
        bgVideoBegin(0), bgVideoEnd(100000), bgVideoDeltaTime(0), hasBgVideo(0), 
        tc(ptc)  {
    frame1=new cv::Mat(cv::Size(w, h), CV_8UC4);
    frame2=new cv::Mat(cv::Size(w, h), CV_8UC4);
    frameIndependent=new cv::Mat(cv::Size(w, h), CV_8UC4);
  }
  
  ~Layer(){
    if (bgVideo.isOpened()) bgVideo.release();
    for (int i=0;i<(int)drawers.size();++i) if (drawers[i]) delete drawers[i];
    for (int i=0;i<(int)layerFilters.size();++i) if (layerFilters[i]) delete layerFilters[i];
    for (int i=0;i<(int)imageFilters.size();++i) if (imageFilters[i]) delete imageFilters[i];
    delete frame1;
    delete frame2;
    if (background) delete background;
    if (bgVideoFrame) delete bgVideoFrame;
    if (bgVideoHelperFrame) delete bgVideoHelperFrame;
  }
};

#endif