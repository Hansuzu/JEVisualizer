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
  FormulaColor& parse(std::string& s);
  FormulaColor& operator=(const FormulaColor& o);
  FormulaColor(FormulaParameterEngine* fpe):r(fpe),g(fpe),b(fpe),a(fpe){}
};

#endif