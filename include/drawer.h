#include <vector>
#include <string>
#include <opencv2/imgproc.hpp>
#include <config.h>
#include <trackcontroller.h>
#include <fpe.h>
#include <formula.h>
#include <formulacolor.h>

#ifndef DRAWER_H
#define DRAWER_H


class Drawer{
private:
  // some magic to make fpe work (basically same as in visualizer.h)
  std::vector<std::vector<TrackController::Index> > fpeTracks;
  std::vector<double> fpevs;
  void setFPEV(std::string& key, std::string& value);
  void updateFPE(int cframe);

  Formula x0, y0; // left point of the spectrum
  Formula x1, y1; // right point of the spectrum
  FormulaColor lineColor0; // color of the border line on the left end of the spectrum
  FormulaColor lineColor1; // color of the border line on the right end of the spectrum
  FormulaColor fillColor0; // filling color on the left end of the spectrum
  FormulaColor fillColor1; // filling color on the right end of the spercrum
  Formula w, h;            // relative width and height multiplier of a single column
  Formula lineThickness;   // the thickness of the border line ... TODO: make this work like better...
  
  FormulaParameterEngine fpe; // fpe magic
  TrackController* tc;
  
  std::vector<double> values;                  // current values of the spectrum, update updates these, calculated from indices
  std::vector<TrackController::Index> indices; // indices of the spectrum

  
  // own functions to draw simple things because opencv wasn't doing what I wanted it to do
  void drawLine(cv::Point a, cv::Point b, cv::Scalar color, double thickness, cv::Mat* frame); 
  void drawRectangle(cv::Point a, cv::Point b, cv::Point c, cv::Point d, cv::Scalar color, cv::Mat* frame);
  
  void update(int cframe); // draw calls first this
  
public:
  void draw(int cframe, cv::Mat* frame); // draws this

private:
  void setParameter(std::string& param, std::string& key, std::string& value); //parse calls this

public:
  void parse(std::string& config); // given a string, parses the configuration
  
  Drawer(FormulaParameterEngine* pfpe, TrackController* ptc) : 
      x0(&fpe), y0(&fpe), x1(&fpe), y1(&fpe), lineColor0(&fpe), lineColor1(&fpe),
      fillColor0(&fpe), fillColor1(&fpe),  w(&fpe), h(&fpe), lineThickness(&fpe),
      fpe(pfpe), tc(ptc) 
  {}
};

#endif