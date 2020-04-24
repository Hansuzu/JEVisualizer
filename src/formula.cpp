#include <log.h>
#include <formula.h>


double Formula::value(){
  if (isConstant){
    return c;
  }else{
    std::vector<double>& values=fpe->getValues();
    double ans=0;
    if (C) ans+=C->value();
    for (int i=0;i<(int)std::min(values.size(), variables.size());++i){
      ans+=values[i]*variables[i]->value();
    }
    if (sinK && inSin){
      double k=sinK->value();
      if (k) ans+=k*sin(inSin->value());
    }
    if (minV){
      double v=minV->value();
      if (ans<v) ans=v;
    }
    if (maxV){
      double v=maxV->value();
      if (ans>v) ans=v;
    }
    return ans;
  }
}


double Formula::pixelValue(double r, double g, double b, double a){
  if (isConstant){
    return c;
  }else{
    double ans=0;
    double v;
    if (C) ans+=C->isConstant?C->c:C->pixelValue(r, g, b, a);
    if (variables.size()) {
      Formula* f;
      std::vector<double>& values=fpe->getValues();
      for (int i=0;i<(int)std::min(values.size(), variables.size());++i){
        f=variables[i];
        ans+=values[i]*(f->isConstant?f->c:f->pixelValue(r, g, b, a));
      }
    }
    if (sinK && inSin) {
      v=sinK->isConstant?sinK->c:sinK->pixelValue(r, g, b, a);
      if (v) ans+=v*sin(inSin->isConstant?inSin->c:inSin->pixelValue(r, g, b, a));
    }
    if (vR) {
      ans+=r*(vR->isConstant?vR->c:vR->pixelValue(r, g, b, a))
          +g*(vG->isConstant?vG->c:vG->pixelValue(r, g, b, a))
          +b*(vB->isConstant?vB->c:vB->pixelValue(r, g, b, a))
          +a*(vA->isConstant?vA->c:vA->pixelValue(r, g, b, a));
    }
    if (minV) {
      v=minV->isConstant?minV->c:minV->pixelValue(r, g, b, a);
      ans=ans<v?v:ans;
    }
    if (maxV) {
      v=maxV->isConstant?maxV->c:maxV->pixelValue(r, g, b, a);
      ans=ans>v?v:ans;
    }
    return ans;
  }
}

void Formula::split(std::string& s, std::vector<std::string>& res){
  std::string prev="";
  int d=0;
  for (int i=0;i<(int)s.size();++i){
    if (s[i]==')' || s[i]==']')--d;
    if (d && s[i]!=' ' && s[i]!='\t' && !(d==1 && s[i]==',')) prev.push_back(s[i]);
    if (s[i]=='(' || s[i]=='[')++d;
    if (d==1 && s[i]==','){
      res.push_back("");
      std::swap(res.back(), prev);
    }
  }
  if (prev.size()){
    res.push_back("");
    std::swap(res.back(), prev);
  }
}

void Formula::parse(std::string& s){
  if (globalSettings::verboseLevel>1) lout << "[I] Formula::parse " << this << "('" << s << "')" << LEND;
  if (s.size()==0) return;
  bool br=0;
  for (int i=0;i<(int)s.size();++i) if (s[i]=='('){br=1; break;}
  if (br){
    isConstant=0;
    std::vector<std::string> params;
    split(s, params);
    for (int i=0;i<(int)params.size();++i){
      if (i==0){
        C=new Formula(fpe);
        C->parse(params[i]);
      }else if (i==1){
        std::vector<std::string> array;
        split(params[i], array);
        for (int j=0;j<(int)array.size();++j){
          variables.push_back(new Formula(fpe));
          variables.back()->parse(array[j]);
        }
      }else if (i==2){
        minV=new Formula(fpe);
        minV->parse(params[i]);
      }else if (i==3){
        maxV=new Formula(fpe);
        maxV->parse(params[i]);
      }else if (i==4){
        sinK=new Formula(fpe);
        sinK->parse(params[i]);
      }else if (i==5){
        inSin=new Formula(fpe);
        inSin->parse(params[i]);
      }else if (i==6){
        std::vector<std::string> array;
        split(params[i], array);
        vR=new Formula(fpe);
        vG=new Formula(fpe);
        vB=new Formula(fpe);
        vA=new Formula(fpe);
        for (int j=0;j<std::min((int)array.size(), 4);++j){
          if (j==0)vR->parse(array[j]);
          if (j==1)vG->parse(array[j]);
          if (j==2)vB->parse(array[j]);
          if (j==3)vA->parse(array[j]);
        }
      }
    }
  }else{
    isConstant=1;
    c=std::stod(s);
  }
}
