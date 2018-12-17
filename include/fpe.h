#include <vector>
#include <iostream>
#include <globalsettings.h>

#ifndef FPE_H
#define FPE_H

class FormulaParameterEngine{
private:
  std::vector<double> values;
  std::vector<int> indices;
  FormulaParameterEngine* parent;
public:
  int size(){return indices.size();}
  void setIndex(int i, int j);
  
  void setValue(int i, double& v){ // Particles use setValue to update particle-specific values on their common FPE
    if (i>=0 && i<(int)values.size()) values[i]=v;
  }
  
  void updateValues(std::vector<double>& ivalues);
  
  double getValue(int i){
    if (i>=0 && i<(int)values.size()) return values[i];
    return 0;
  }
  std::vector<double>& getValues(){
    return values;
  }
  FormulaParameterEngine(FormulaParameterEngine* pParent):parent(pParent){
    while (pParent && (int)indices.size()<pParent->size()){
      indices.push_back(-((int)indices.size())-1);
    }
  }
};

#endif