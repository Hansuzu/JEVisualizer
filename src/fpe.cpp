#include <fpe.h>

void FormulaParameterEngine::setIndex(int i, int j){
  if (i<0) return;
  while (i>=(int)indices.size()) indices.push_back(-indices.size()-1);
  indices[i]=j;
  while (indices.size() && !indices.back()) indices.pop_back();
}

void FormulaParameterEngine::updateValues(std::vector<double>& ivalues){
  std::vector<double> tmp;
  std::vector<double>& pvalues=parent?parent->getValues():tmp;
  values.resize(indices.size());
  for (int i=0;i<(int)indices.size();++i){
    if (indices[i]<0 && -indices[i]-1<(int)pvalues.size()) values[i]=pvalues[-indices[i]-1];
    else if (indices[i]>0 && indices[i]-1<(int)ivalues.size()) values[i]=ivalues[indices[i]-1];
  }
}