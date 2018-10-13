#include <string>
#include <vector>
#include <opencv2/imgproc.hpp>
#include <config.h>
#include <trackcontroller.h>
#include <fpe.h>
#include <formula.h>

#ifndef FILTER_H
#define FILTER_H


class Filter{
  enum Type{Unset, GaussianBlur, Color, Contrast, Brightness};
  void setConfigParam(std::string& param, std::string& key, std::string& value, int verboseLevel);
  Type type;
  Formula kernelX, kernelY;
  FormulaParameterEngine fpe;
  
  TrackController* tc;
public:
  
  void loadConfig(std::string& config, int verboseLevel);
  void apply(cv::Mat* frame1, cv::Mat* frame2, int verboseLevel);
  Filter(FormulaParameterEngine* pfpe, TrackController* ptc):type(Unset), kernelX(&fpe), kernelY(&fpe), fpe(pfpe), tc(ptc){
  }
};

#endif