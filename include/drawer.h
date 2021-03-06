#include <vector>
#include <string>
#include <opencv2/imgproc.hpp>
#include <globalsettings.h>
#include <config.h>
#include <trackcontroller.h>
#include <fpe.h>
#include <formula.h>
#include <formulacolor.h>

#ifndef DRAWER_H
#define DRAWER_H


class Drawer{
private:
  FormulaParameterEngine fpe; // fpe magic
  
  // some magic to make fpe work (basically same as in visualizer.h)
  std::vector<std::vector<TrackController::Index> > fpeTracks;
  std::vector<double> fpevs;
  void setFPEV(std::string& key, std::string& value);
  void updateFPE(int cframe);
  enum ColumnType{Rectangle, Triangle, Polygon, Mid};
  
  std::vector<std::pair<Formula*, Formula*> > polyCorners;
  std::vector<std::pair<double, double> > polyCornerVs;
  std::vector<cv::Point> polygonToDraw;
  ColumnType columnType;
  
  Formula thr;
  
  Formula x0, y0; // left point of the spectrum
  Formula x1, y1; // right point of the spectrum
  FormulaColor lineColor0; // color of the border line on the left end of the spectrum
  FormulaColor lineColor1; // color of the border line on the right end of the spectrum
  FormulaColor fillColor0; // filling color on the left end of the spectrum
  FormulaColor fillColor1; // filling color on the right end of the spercrum
  Formula w, h;            // relative width and height multiplier of a single column
  Formula lineThickness;   // the thickness of the border line ... TODO: make this work like better...

    
  class SubDrawer { 
  public:
    std::vector<std::pair<Formula*, Formula*> > polyCorners;
    std::vector<std::pair<double, double> > polyCornerVs;
    std::vector<cv::Point> polygonToDraw;
    Drawer::ColumnType columnType;
    
    Formula x0, y0; // left point of the spectrum
    Formula x1, y1; // right point of the spectrum
    FormulaColor lineColor0; // color of the border line on the left end of the spectrum
    FormulaColor lineColor1; // color of the border line on the right end of the spectrum
    FormulaColor fillColor0; // filling color on the left end of the spectrum
    FormulaColor fillColor1; // filling color on the right end of the spercrum
    Formula w, h;            // relative width and height multiplier of a single column
    Formula lineThickness;   // the thickness of the border line ... TODO: make this work like better...
    int i0, i1;
    FormulaParameterEngine* pfpe;
    
  private:
    void setParameter(std::string& param, std::string& key, std::string& value); //parse calls this

  public:
    void parse(std::string& config); // given a string, parses the configuration
    SubDrawer(FormulaParameterEngine* fpe) : 
        columnType(ColumnType::Rectangle), x0(fpe), y0(fpe), x1(fpe), y1(fpe), lineColor0(fpe), lineColor1(fpe),
        fillColor0(fpe), fillColor1(fpe),  w(fpe), h(fpe), lineThickness(fpe),
        i0(0), i1(0), pfpe(fpe)
    {}
  };
  
  std::vector<SubDrawer*> subDrawers;
  
  std::pair<double, double> getSubDrawerDir(int i, double xScale, double yScale); // Return the normal of ith subDrawer
  void drawSubDrawer(int cframe, cv::Mat* frame, double xScale, double yScale, int subDrawerIndex); // drawSubDrawer
  
  int flatten;
  
  TrackController* tc;
  
  std::vector<double> values;                  // current values of the spectrum, update updates these, calculated from indices
  std::vector<TrackController::Index> indices; // indices of the spectrum

  
  
  void update(int cframe); // draw calls first this
  
public:
  void draw(int cframe, cv::Mat* frame, double xScale, double yScale); // draws this

private:
  void setParameter(std::string& param, std::string& key, std::string& value); //parse calls this

public:
  void parse(std::string& config); // given a string, parses the configuration
  Drawer(FormulaParameterEngine* pfpe, TrackController* ptc) : 
      fpe(pfpe), columnType(ColumnType::Rectangle), thr(&fpe), x0(&fpe), y0(&fpe), x1(&fpe), y1(&fpe), lineColor0(&fpe), lineColor1(&fpe),
      fillColor0(&fpe), fillColor1(&fpe),  w(&fpe), h(&fpe), lineThickness(&fpe),
      flatten(0),
       tc(ptc) 
  {}
};

#endif