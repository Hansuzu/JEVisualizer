#include <string>
#include <vector>
#include <opencv2/imgproc.hpp>
#include <trackcontroller.h>
#include <fpe.h>
#include <formula.h>

#ifndef FILTER_H
#define FILTER_H


class Filter{
  enum Type{Unset, GaussianBlur, Color, Contrast, Brightness};
  void setConfigParam(std::string& param, std::string& key, std::string& value, int verboseLevel);
  Type type;
  int kernelX, kernelY;
  FormulaParameterEngine fpe;
  
  TrackController* tc;
public:
  
  void loadConfig(std::string& config, int verboseLevel);
  void apply(int cframe, cv::Mat* frame);
  Filter(FormulaParameterEngine* pfpe, TrackController* ptc):type(Unset), kernelX(0), kernelY(0), fpe(pfpe), tc(ptc){
    
  }
};

#endif