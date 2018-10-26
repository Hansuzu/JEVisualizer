#include <log.h>
#include <formulacolor.h>


FormulaColor& FormulaColor::parse(std::string& s){
  if (globalSettings::verboseLevel>1) lout << "[I] FormulaColor::parse " << this << "('" << s << "')" << LEND;
  std::vector<std::string> res;
  Formula::split(s, res);
  if (res.size())   r.parse(res[0]);
  if (res.size()>1) g.parse(res[1]);
  if (res.size()>2) b.parse(res[2]);
  if (res.size()>3) a.parse(res[3]);
  else              a.c=1.0;
  return *this;
}

FormulaColor& FormulaColor::operator=(const FormulaColor& o){
  r=o.r; g=o.g; b=o.b; a=o.a;
  return *this;
}