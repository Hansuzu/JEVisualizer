#include <extractor.h>
#include <iostream>

void Extractor::nextFrame(int verboseLevel){
  if (verboseLevel>1) std::cout << "[I] Extractor::nextFrame " << this << std::endl;
  std::vector<int> values;
  for (int i=0;i<tc.size();++i){
    for (int j=1;j<tc.size(i)-1;++j){
      double a=tc.getIndex(i, j-1).getValue();
      double b=tc.getIndex(i, j).getValue();
      double c=tc.getIndex(i, j+1).getValue();
      if (a<b && b>c){
        for (int k=0;k<(int)map.size();++k){
          if (map[k].first.first==i && map[k].first.second==j){
            values.push_back(k);
          }
        }
      }
    }
  }
  ofile << "EXTRACTER FRAME " << cframe << " TIME " << ctime << std::endl;
  ofile << values.size() << " NOTES" << std::endl;
  for (auto index: values){
    ofile << " " << map[index].second << std::endl;
  }
  ofile << std::endl;
  ++cframe;
}



void Extractor::next(double time, std::vector<std::vector<double>*>& newTrackValues, int verboseLevel){
  if (verboseLevel>1) std::cout << "[I] Extractor::next(" << time << ", &st, " << verboseLevel << ")" << std::endl; 

  while (((double)cframe)/fps<time){
    double ftime=(double)cframe/fps;
    
    tc.updateValues(newTrackValues, ftime, time);
    
    ctime=(double)cframe/fps;
    nextFrame(verboseLevel);
  }
  tc.updateValues(newTrackValues, time, time);
}

std::pair<int, int> Extractor::split(std::string& value, int verboseLevel){
  if (verboseLevel>2) std::cout << "[X] Ectracter::loadMap " << this << "('" << value << "')" << std::endl;
  std::string a="";
  std::string b="";
  bool bb=0;
  for (auto c: value){
    if (c==':') bb=1;
    else if (!bb) a.push_back(c);
    else          b.push_back(c);
  }
  return {std::stoi(a), std::stoi(b)};
}

void Extractor::loadMap(std::string& conf, int verboseLevel){
  if (verboseLevel>1) std::cout << "[I] Ectracter::loadMap " << this << "('" << conf << "')" << std::endl;
  std::vector<std::pair<std::pair<std::string, std::string>, std::string> > ans;
  configReader::readConfig(conf, ans);
  for (auto confp : ans){
    map.push_back({split(confp.first.first, verboseLevel), confp.second});
  }
}

void Extractor::setConfigParam(std::string& param, std::string& paramKey, std::string& value, int verboseLevel){
  if (verboseLevel>1) std::cout << "[I] Ectracter::setConfigParameter " << this << "('" << param <<"', '" << paramKey << "', '" << value << "')" << std::endl;
  if (param=="fps") fps=std::stod(value);
  else if (param=="filename") ofilename=value;
  else if (param=="map"){
    loadMap(value, verboseLevel);
  }
  else if (verboseLevel) std::cout << "[I] Ectracter::setConfigParam " << this << ", unknown parameter '" << param << "'" << std::endl;
}
void Extractor::readConfig(std::istream& co, int verboseLevel){
  std::vector<std::pair<std::pair<std::string, std::string>, std::string> > ans;
  configReader::readConfig(co, ans);
  for (auto conf: ans) setConfigParam(conf.first.first, conf.first.second, conf.second, verboseLevel);
}