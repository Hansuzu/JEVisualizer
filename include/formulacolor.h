#include <vector>
#include <string>
#include <opencv2/core.hpp> 
#include <globalsettings.h>
#include <fpe.h>
#include <formula.h>

#ifndef FORMULACOLOR_H
#define FORMULACOLOR_H

class FormulaColor{
public:
  Formula r, g, b, a;
  cv::Scalar value(){
    return cv::Scalar(b.value(), g.value(), r.value(), a.value());
  }
  cv::Scalar value(double vr, double vg, double vb, double va){
    return cv::Scalar(b.value(vr, vg, vb, va), g.value(vr, vg, vb, va), r.value(vr, vg, vb, va), a.value(vr, vg, vb, va));
  }
  FormulaColor& parse(std::string& s);
  FormulaColor& operator=(const FormulaColor& o);
  FormulaColor(FormulaParameterEngine* fpe):r(fpe),g(fpe),b(fpe),a(fpe){}
};

#endif