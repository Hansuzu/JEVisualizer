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
  cv::Scalar pixelValue(double vr, double vg, double vb, double va){
    return cv::Scalar(b.isConstant?b.c:b.pixelValue(vr, vg, vb, va), g.isConstant?g.c:g.pixelValue(vr, vg, vb, va), r.isConstant?r.c:r.pixelValue(vr, vg, vb, va), a.isConstant?a.c:a.pixelValue(vr, vg, vb, va));
  }
  FormulaColor& parse(std::string& s);
  FormulaColor& operator=(const FormulaColor& o);
  FormulaColor(FormulaParameterEngine* fpe):r(fpe),g(fpe),b(fpe),a(fpe){}
};

#endif