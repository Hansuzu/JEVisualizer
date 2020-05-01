#include <vector>
#include <string>
#include <globalsettings.h>
#include <math.h>
#include <fpe.h>

#ifndef FORMULA_H
#define FORMULA_H


class Formula{
public:
  FormulaParameterEngine* fpe;
  
  bool isConstant;
  double c;
  Formula* C;
  std::vector<Formula*> variables;
  Formula* frameK;
  Formula* minV;
  Formula* maxV;
  Formula* sinK;
  Formula* inSin;
  
  Formula* vR;
  Formula* vG;
  Formula* vB;
  Formula* vA;
  
  
  
  Formula(FormulaParameterEngine* pfpe): fpe(pfpe), isConstant(1), c(0), C(nullptr), frameK(nullptr), minV(nullptr), maxV(nullptr), sinK(nullptr), inSin(nullptr), vR(nullptr), vG(nullptr), vB(nullptr), vA(nullptr) {}
  double value();
  double pixelValue(double, double, double, double);
  static void split(std::string& s, std::vector<std::string>& res);
  void parse(std::string& s);
  Formula* createCopy(){
    Formula* b=new Formula(fpe);
    *b=*this;
    return b;
  }
  void setFPE(FormulaParameterEngine* nfpe){
    fpe = nfpe;
    for (int i=0;i<(int)variables.size();++i) variables[i]->setFPE(fpe);
    if (minV)    minV->setFPE(fpe);
    if (maxV)    maxV->setFPE(fpe);
    if (frameK)  frameK->setFPE(fpe);
    if (sinK)    sinK->setFPE(fpe);
    if (inSin)   inSin->setFPE(fpe);
    if (vR)   vR->setFPE(fpe);
    if (vG)   vG->setFPE(fpe);
    if (vB)   vB->setFPE(fpe);
    if (vA)   vA->setFPE(fpe);
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
    if (o.vR)   vR=o.vR->createCopy();
    if (o.vG)   vG=o.vG->createCopy();
    if (o.vB)   vB=o.vB->createCopy();
    if (o.vA)   vA=o.vA->createCopy();
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
    if (vR) delete vR;
    if (vG) delete vG;
    if (vB) delete vB;
    if (vA) delete vA;
  }
};

#endif