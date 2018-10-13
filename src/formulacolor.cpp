#include <iostream>
#include <formulacolor.h>


FormulaColor& FormulaColor::parse(std::string& s, int verboseLevel){
  if (verboseLevel>1) std::cout << "[I] FormulaColor::parse " << this << "('" << s << "')" << std::endl;
  std::vector<std::string> res;
  Formula::split(s, res);
  if (res.size())   r.parse(res[0], verboseLevel);
  if (res.size()>1) g.parse(res[1], verboseLevel);
  if (res.size()>2) b.parse(res[2], verboseLevel);
  if (res.size()>3) a.parse(res[3], verboseLevel);
  else              a.c=1.0;
  return *this;
}

FormulaColor& FormulaColor::operator=(const FormulaColor& o){
  r=o.r; g=o.g; b=o.b; a=o.a;
  return *this;
}