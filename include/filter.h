#include <string>
#include <vector>
#include <future>
#include <opencv2/imgproc.hpp>
#include <globalsettings.h>
#include <config.h>
#include <trackcontroller.h>
#include <fpe.h>
#include <formula.h>
#include <formulacolor.h>

#ifndef FILTER_H
#define FILTER_H


class Filter{
  FormulaParameterEngine fpe;
  
  enum Type{Unset, GaussianBlur, Color, SuperColor, BoxBlur, SuperColor2};
  void setConfigParam(std::string& param, std::string& key, std::string& value);
  Type type;
  Formula kernelX, kernelY;
  Formula rc, gc, bc, ac;  // When type=Color, color of a pixel will be (rc+r*old, gc+g*old, bc+b*old, ac+a*old)
  FormulaColor r, g, b, a; // 
  

  struct ColorRule{
    Formula rc, gc, bc, ac;  // When type=Color, color of a pixel will be (rc+r*old, gc+g*old, bc+b*old, ac+a*old)
    FormulaColor r, g, b, a; // 
    Formula shiftX, shiftY;
    ColorRule(FormulaParameterEngine* fpe):
      rc(fpe),gc(fpe),bc(fpe),ac(fpe),
      r(fpe),g(fpe),b(fpe),a(fpe),
      shiftX(fpe), shiftY(fpe)
      {
      
    }
    
  };
  std::vector<std::future<void> > fs;    //To make async multithreading work...
  std::vector<ColorRule*> colorRules; // When type==SuperColor, these are used.
  
  
  
  TrackController* tc;
private:
  void loadColorRule(std::string& rule);
private:
  void applyGaussianBlur(cv::Mat* frame1, cv::Mat* frame2);
  void applyBoxBlur(cv::Mat* frame1, cv::Mat* frame2);
  void applyColor(cv::Mat* frame1, cv::Mat* frame2);
  void applySuperColorRule(cv::Mat* frame1, cv::Mat* frame2, int i, int k);
  void applySuperColorRule2(cv::Mat* frame1, cv::Mat* frame2, cv::Mat* refFrame, int i, int k); // TODO: rename this weird stuff...
public:
  
  void loadConfig(std::string& config);
  void apply(cv::Mat* frame1, cv::Mat* frame2, cv::Mat* refFrame);
  Filter(FormulaParameterEngine* pfpe, TrackController* ptc):
    fpe(pfpe), 
    type(Unset), kernelX(&fpe), kernelY(&fpe), 
    rc(&fpe), gc(&fpe), bc(&fpe), ac(&fpe), 
    r(&fpe), g(&fpe), b(&fpe), a(&fpe), 
    tc(ptc) 
    {
      
  }
  ~Filter(){
    for (int i=0;i<(int)colorRules.size();++i) delete colorRules[i];
  }
};

#endif