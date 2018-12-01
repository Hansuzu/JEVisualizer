#include <vector>
#include <string>
#include <globalsettings.h>
#include <math.h>
#include <fpe.h>

#ifndef FORMULA_H
#define FORMULA_H


class Formula{
public:
  bool isConstant;
  double c;
  Formula* C;
  std::vector<Formula*> variables;
  Formula* frameK;
  Formula* minV;
  Formula* maxV;
  Formula* sinK;
  Formula* inSin;
  FormulaParameterEngine* fpe;
  Formula(FormulaParameterEngine* pfpe): isConstant(1), c(0), C(NULL), frameK(NULL), minV(NULL), maxV(NULL), sinK(NULL), inSin(NULL), fpe(pfpe) {}
  double value();
  static void split(std::string& s, std::vector<std::string>& res);
  void parse(std::string& s);
  Formula* createCopy(){
    Formula* b=new Formula(fpe);
    *b=*this;
    return b;
  }
  void setFPE(FormulaParameterEngine* fpe){
    if (o.C)C=o.C->setFPE(fpe);
    for (int i=0;i<(int)variables.size();++i) variables[i]->setFPE(fpe);
    if (minV)    minV->setFPE(fpe);
    if (maxV)    maxV->setFPE(fpe);
    if (frameK)  frameK->setFPE(fpe);
    if (sinK)    sinK->setFPE(fpe);
    if (inSin)   inSin->setFPE(fpe);
    this.fpe=fpe;
  }
  Formula& operator=(const Formula& o){
    isConstant=o.isConstant;
    c=o.c;
    if (o.C)C=o.C->createCopy();
    for (int i=0;i<(int)o.variables.size();++i) variables[i]=o.variables[i]->createCopy();
    if (o.minV)    minV=o.minV->createCopy();
    if (o.maxV)    maxV=o.maxV->createCopy();
    if (o.frameK)  frameK=o.frameK->createCopy();
    if (o.sinK)    sinK=o.sinK->createCopy();
    if (o.inSin)   inSin=o.inSin->createCopy();
    fpe=o.fpe;
    return *this;
  }
  ~Formula(){
    if (C) delete C;
    if (frameK) delete frameK;
    for (int i=0;i<(int)variables.size();++i)  delete variables[i];
    if (minV) delete minV;
    if (maxV) delete maxV;
    if (sinK) delete sinK;
    if (inSin) delete inSin;
  }
};

#endif