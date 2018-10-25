#include <iostream>
#include <vector>
#include <opencv2/core.hpp>        // Basic OpenCV structures (cv::Mat)
#include <opencv2/imgproc.hpp>    // Basic OpenCV structures (cv::Mat)
#include <opencv2/highgui.hpp> 
#include <config.h>
#include <trackcontroller.h>
#include <fpe.h>
#include <formula.h>
#include <formulacolor.h>
#include <drawer.h>
#include <filter.h>

#ifndef LAYER_H
#define LAYER_H



class Layer{
private: 
  double layerWidth, layerHeight; // The coordinates in Drawers are relative to these values. Values are then scaled to actual width and height
  int firstFrame, lastFrame;      // First and last frame to actually draw
  cv::Mat* frame1;                // Layer is drawn on these (two frames are needed for filters)
  cv::Mat* frame2;
  cv::Mat* background;            // if background-image is used, it is saved here
  bool hasBgImage;
  FormulaColor LU, LD, RU, RD;    // if background-color is used
  cv::Scalar lLU, lLD, lRU, lRD;
  bool hasBgColor;
  
  std::vector<Drawer*> drawers;      // all drawers
  std::vector<Filter*> layerFilters; // filters that are applied to the layer before it is rendered on the output-frame
  std::vector<Filter*> imageFilters; // filters that are applied to the output-frame after this layer is rendered on it
  TrackController* tc;               // Pointer to the track-controller, through this the Layer has access to the track data
  void drawDrawers(int cframe, int verboseLevel); //function to draw all drawers of the layer
  
  bool isVisible(int cframe); // checks whether the layer is Visible: 
  FormulaParameterEngine fpe; // parameter to formulas
  
  //Magic to make fpe work (basically same as in visualizer.h)
  std::vector<std::vector<TrackController::Index> > fpeTracks;
  std::vector<double> fpevs;
  void setFPEV(std::string& key, std::string& value, int verboseLevel);
  void updateFPE(int cframe);
  
  void createBgBackground(bool force=0);
public:
  void draw(int cframe, cv::Mat* oframe, int verboseLevel); // draw this layer to the oframe

private:
  void setConfigParam(std::string& param, std::string& key, std::string& value, int verboseLevel); //readConfig calls this

public:   //TODO: remove debug prints
  void readConfig(const char* configFile, int verboseLevel);  // load configuration from file
  
  Layer(int w, int h, FormulaParameterEngine* pfpe, TrackController* ptc):
      layerWidth(w), layerHeight(h), firstFrame(0), lastFrame(1000000),
        frame1(nullptr), frame2(nullptr), background(nullptr),
        hasBgImage(0), LU(&fpe), LD(&fpe), RU(&fpe), RD(&fpe), hasBgColor(0), tc(ptc), fpe(pfpe) {
    frame1=new cv::Mat(cv::Size(w, h), CV_8UC4);
    frame2=new cv::Mat(cv::Size(w, h), CV_8UC4);
  }
  
  ~Layer(){
    for (int i=0;i<(int)drawers.size();++i) if (drawers[i]) delete drawers[i];
    for (int i=0;i<(int)layerFilters.size();++i) if (layerFilters[i]) delete layerFilters[i];
    for (int i=0;i<(int)imageFilters.size();++i) if (imageFilters[i]) delete imageFilters[i];
    delete frame1;
    delete frame2;
  }
};

#endif