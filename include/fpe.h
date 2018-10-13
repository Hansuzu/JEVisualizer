#include <vector>
#include <iostream>
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
  
  void updateValues(std::vector<double>& ivalues);
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