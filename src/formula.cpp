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
//   cout << "Formula::parse(" << s << ")" << endl;
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
        sinK=new Formula(fpe);
        sinK->parse(params[i]);
      }else if (i==3){
        inSin=new Formula(fpe);
        inSin->parse(params[i]);
      }else if (i==4){
        minV=new Formula(fpe);
        minV->parse(params[i]);
      }else if (i==5){
        maxV=new Formula(fpe);
        maxV->parse(params[i]);
      }
    }
  }else{
    c=std::stod(s);
  }
}