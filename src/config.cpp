#include <config.h>

namespace configReader{
  char toLowerCase(char c){
    return c>='A' && c<='Z'?c-'A'+'a':c;
  }
  void readConfig(std::istream& co, std::vector<std::pair<std::pair<std::string, std::string>, std::string> >& ans, bool lower){
    std::string line;
    int istr=0;
    bool isComment=0;
    char previous=0;
    std::string b="";
    std::string v="";
    bool ec=0;
    bool esc=0;
    while (std::getline(co, line)){
      if (line.size()==0) continue;
      if (!istr){
        previous=0;
        b="";
        v="";
        ec=0;
      }else{
        if (!ec) b.push_back('\n');
        else v.push_back('\n');
      }
      for (int i=0;i<(int)line.size();++i){
        if (!istr && !isComment){
          if (line[i]==' ' || line[i]=='\t') continue;
          if (line[i]=='/' && previous=='/') break;
        }
        if (!isComment && !istr && line[i]=='*' && i && line[i-1]=='/'){
          isComment=1;
        }else if (isComment && line[i]=='/' && i && line[i-1]=='*'){
          isComment=0;
        }else if (!isComment){
          if (line[i]=='"' && !istr){ istr=1; continue;}
          if (line[i]=='"' && istr==1 && !esc){istr=0;continue;}
          if (line[i]=='\'' && !istr){istr=2; continue;}
          if (line[i]=='\'' && istr==2 && !esc){istr=0; continue;}
          if (line[i]=='{' && !istr && !esc){istr=3; continue;}
          if (line[i]=='}' && istr==3 && !esc){istr=0; continue;}
          if (istr && !esc && line[i]=='\\'){esc=1; continue;}
          else esc=0;
          
          if (!istr && line[i]=='=') ec=1;
          else{
            if (!ec) b.push_back(line[i]);
            else v.push_back(line[i]);
          }
          previous=line[i];
        }
      }
      if (ec && !istr){
        if (b=="#include"){
          readConfigFromFile(v.c_str(), ans);
        }else{
          std::string no;
          bool c=0;
          std::string p="";
          for (int i=0;i<(int)b.size();++i){
            if (b[i]=='#') c=1;
            else if (!c){
              if (lower) p.push_back(toLowerCase(b[i]));
              else       p.push_back(b[i]);
            }else{
              no.push_back(b[i]);
            }
          }
          ans.push_back({{p, no}, v});
        }
      }
    }
  }
  void readConfig(std::string co,  std::vector<std::pair<std::pair<std::string, std::string>, std::string> >& ans, bool lower){
    std::istringstream coi(co);
    readConfig(coi, ans, lower);
  }
  void readConfigFromFile(const char* fname,  std::vector<std::pair<std::pair<std::string, std::string>, std::string> >& ans, bool lower){
    std::ifstream iff(fname);
    readConfig(iff, ans, lower);
  }
}