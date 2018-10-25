#include <string>
#include <vector>
#include <opencv2/imgproc.hpp>
#include <config.h>
#include <trackcontroller.h>
#include <fpe.h>
#include <formula.h>
#include <formulacolor.h>

#ifndef FILTER_H
#define FILTER_H


class Filter{
  enum Type{Unset, GaussianBlur, Color, SuperColor};
  void setConfigParam(std::string& param, std::string& key, std::string& value, int verboseLevel);
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
  std::vector<ColorRule*> colorRules; // When type==SuperColor, these are used.
  
  
  FormulaParameterEngine fpe;
  
  TrackController* tc;
private:
  void loadColorRule(std::string& rule, int verboseLevel);
public:
  
  void loadConfig(std::string& config, int verboseLevel);
  void apply(cv::Mat* frame1, cv::Mat* frame2, int verboseLevel);
  Filter(FormulaParameterEngine* pfpe, TrackController* ptc):
    type(Unset), kernelX(&fpe), kernelY(&fpe), 
    rc(&fpe), gc(&fpe), bc(&fpe), ac(&fpe), 
    r(&fpe), g(&fpe), b(&fpe), a(&fpe), 
    fpe(pfpe), tc(ptc) 
    {
      
  }
  ~Filter(){
    for (int i=0;i<(int)colorRules.size();++i) delete colorRules[i];
  }
};

#endif