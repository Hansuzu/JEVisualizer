#include <iostream>
#include <vector>
#include <opencv2/core.hpp>        // Basic OpenCV structures (cv::Mat)
#include <opencv2/imgproc.hpp>    // Basic OpenCV structures (cv::Mat)
#include <opencv2/highgui.hpp> 

class Layer{
public:
  class Formula{
  public:
    bool isConstant;
    double c;
    Formula* C;
    std::vector<Formula*> variables;
    Formula* frameK;
    Formula* sinK;
    Formula* inSin;
    double minV, maxV;
    Formula(): isConstant(1), c(0), C(NULL), frameK(NULL), sinK(NULL), inSin(NULL), minV(-1e9), maxV(1e9){}
    double value(int frame, std::vector<double>& variables);
    static void split(std::string& s, std::vector<std::string>& res);
    void parse(std::string& s);
    Formula* createCopy(){
      Formula* b=new Formula();
      *b=*this;
      return b;
    }
    Formula& operator=(const Formula& o){
      isConstant=o.isConstant;
      c=o.c;
      if (o.C)C=o.C->createCopy();
      for (int i=0;i<(int)o.variables.size();++i) variables[i]=o.variables[i]->createCopy();
      if (o.frameK)  frameK=o.frameK->createCopy();
      if (o.sinK)    sinK=o.sinK->createCopy();
      if (o.inSin)   inSin=o.inSin->createCopy();
      minV=o.minV;
      maxV=o.maxV;
      return *this;
    }
    ~Formula(){
      if (C) delete C;
      if (frameK) delete frameK;
      for (int i=0;i<(int)variables.size();++i)  delete variables[i];
      if (sinK) delete sinK;
      if (inSin) delete inSin;
    }
  };
  class FormulaColor{
  public:
    Formula r, g, b, a;
    cv::Scalar value(int frame, std::vector<double>& variables){
      return cv::Scalar(b.value(frame, variables), g.value(frame, variables), r.value(frame, variables), a.value(frame, variables));
    }
    FormulaColor& parse(std::string& s){
      std::vector<std::string> res;
      Formula::split(s, res);
      if (res.size())   r.parse(res[0]);
      if (res.size()>1) g.parse(res[1]);
      if (res.size()>2) b.parse(res[2]);
      if (res.size()>3) a.parse(res[3]);
      else              a.c=1.0;
      return *this;
    }
    FormulaColor& operator=(const FormulaColor& o){
      r=o.r; g=o.g; b=o.b; a=o.a;
      return *this;
    }
  };
  
  class Drawer{
  private:
    void drawLine(cv::Point a, cv::Point b, cv::Scalar color, double thickness, cv::Mat* frame);
    void drawRectangle(cv::Point a, cv::Point b, cv::Point c, cv::Point d, cv::Scalar color, cv::Mat* frame);
  public:
    Formula x0, y0;
    Formula x1, y1;
    FormulaColor lineColor0;
    FormulaColor lineColor1;
    FormulaColor fillColor0;
    FormulaColor fillColor1;
    Formula w, h;
    Formula lineThickness;
    
    std::vector<double> values;
    std::vector<std::pair<int, int> > indices;
    std::vector<double> formula_values;
    std::vector<std::vector<std::pair<int, int> > > formula_value_indices;
    void draw(int cframe, cv::Mat* frame);
    void updateValues(std::vector<std::vector<double>* >& pValues);
    void setParameter(std::string& param, std::string& value);
    void parse(std::string& config);
  };
  class Filter{
    enum Type{GaussianBlur, ColorShift, Contrast, Brightness};
    void setConfigParam(std::string& param, std::string& key, std::string& value, int verboseLevel);
  public:
    void loadConfig(std::string& config, int verboseLevel);
    void apply(int cframe, std::vector<std::vector<double>* >& trackValues, cv::Mat* frame);
  };
private:
  double layerWidth, layerHeight; // Drawing is done 
  int firstFrame, lastFrame;
  cv::Mat* frame;
  cv::Mat* background;
  bool hasBgImage;
  FormulaColor LU;
  FormulaColor LD;
  FormulaColor RU;
  FormulaColor RD;
  bool hasBgColor;
  
  std::vector<Drawer*> drawers;
  std::vector<Filter*> layerFilters;
  std::vector<Filter*> imageFilters;
  void drawDrawers(int cframe, std::vector<std::vector<double>* >& trackValues, int verboseLevel);
  bool isVisible(int cframe, std::vector<std::vector<double>* >& trackValues);
public:
  void draw(int cframe, std::vector<std::vector<double>* >& trackValues, cv::Mat* oframe, int verboseLevel);
  void setConfigParam(std::string& param, std::string& key, std::string& value, int verboseLevel);
  void readConfig(const char* configFile, int verboseLevel);
  Layer(int w, int h):layerWidth(w), layerHeight(h), firstFrame(0), lastFrame(1000000), hasBgImage(0), hasBgColor(0){
    std::cout << "Layer(" << w << ", " << h << ")" << std::endl;
    frame=new cv::Mat(cv::Size(w, h), CV_8UC4);
    std::cout << frame->size().width << ", " << frame->size().height << std::endl;
    std::cout << this << std::endl;
  }
  ~Layer(){
    for (int i=0;i<(int)drawers.size();++i) if (drawers[i]) delete drawers[i];
    for (int i=0;i<(int)layerFilters.size();++i) if (layerFilters[i]) delete layerFilters[i];
    for (int i=0;i<(int)imageFilters.size();++i) if (imageFilters[i]) delete imageFilters[i];
    delete frame;
  }
};