#include <log.h>
#include <extractor.h>


void Extractor::nextFrame(){
  if (globalSettings::verboseLevel>1) lout << "[I] Extractor::nextFrame " << this << LEND;
  std::vector<std::pair<std::string, double> > notes;
  for (int k=0;k<(int)map.size();++k){
    int i=map[k].first.first;
    int j=map[k].first.second;
    double a=tc.getIndex(i, j-1).getValue();
    double b=tc.getIndex(i, j).getValue();
    double c=tc.getIndex(i, j+1).getValue();
    if (b>=limit && b-a>peekLimit && b-c>peekLimit){
      notes.push_back({map[k].second, b});
    }
  }
  std::vector<std::pair<std::string, double> > grnotes;
  for (int k=0;k<(int)groups.size();++k){
    double v=0;
    for (int i=0;i<(int)groups[k].second.size();++i){
      for (auto& note: notes){
        if (note.first==groups[k].second[i]){
          if (note.second>v) v=note.second;
        }
      }
    }
    if (v>=limit){
      grnotes.push_back({groups[k].first, v});
    }
  }
  std::vector<std::pair<std::string, std::pair<double, double> > > cchords;
  for (int k=0;k<(int)chords.size();++k){
    double sum_v=0;
    double min_v=1e9;
    for (int i=0;i<(int)chords[k].second.size();++i){
      double v=0;
      for (auto& note: grnotes){
        if (note.first==chords[k].second[i]){
          v+=note.second;
        }
      }
      if (v<min_v) min_v=v;
      sum_v+=v;
    }
    if (min_v>=limit){
      cchords.push_back({chords[k].first, {min_v, sum_v}});
    }
  }
  ofile << "EXTRACTOR FRAME " << cframe << " TIME " << ctime << std::endl;
  for (auto& note: notes){
    ofile << note.first << " (" << note.second << ")  ";
  }
  ofile << std::endl;
  for (auto& note: grnotes){
    ofile << note.first << " (" << note.second << ")  ";
  }
  ofile << std::endl;
  for (auto& chord: cchords){
    ofile << chord.first << " (" << chord.second.first << ", " << chord.second.second << ")  ";
  }
  ofile << std::endl << std::endl;
  ++cframe;
}



void Extractor::next(double time, std::vector<std::vector<double>*>& newTrackValues){
  if (globalSettings::verboseLevel>1) lout << "[I] Extractor::next(" << time << ", &st)" << LEND; 
  tc.setMaxDownSpeed(maxDownSpeed);
  tc.setMaxUpSpeed(maxUpSpeed);
  while (((double)cframe)/fps<time){
    double ftime=(double)cframe/fps;
    
    tc.updateValues(newTrackValues, ftime, time);
    
    ctime=(double)cframe/fps;
    nextFrame();
    tc.clean(ftime);
  }
  tc.updateValues(newTrackValues, time, time);
}

std::pair<int, int> Extractor::split(std::string& value){
  if (globalSettings::verboseLevel>2) lout << "[X] Ectractor::split " << this << "('" << value << "')" << LEND;
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
void Extractor::getArray(std::string& value, std::vector<std::string>& res){
  if (globalSettings::verboseLevel>2) lout << "[X] Ectractor::getArray " << this << "('" << value << "')" << LEND;
  int d=0;
  std::string last="";
  for (auto c : value){
    if (c=='[') ++d;
    else if (c==']'){
      --d;
      res.push_back(last);
      last="";
    }else if (d==1 && c==','){
      res.push_back(last);
      last="";
    }else if (d==1){
      last.push_back(c);
    }else if (globalSettings::verboseLevel) {
      lout << "[W] Extractor::getArray " << this << "('" << value << "'): Out-of-array data: '" << c << "'" << LEND; 
    }
  }
  if (d!=0 && globalSettings::verboseLevel){
      lout << "[W] Extractor::getArray " << this << "('" << value << "'): Array didn't end as expected. " << LEND; 
  }else if (last.size() && globalSettings::verboseLevel){
      lout << "[W] Extractor::getArray " << this << "('" << value << "'): Value '" << last << "' not appended to the array." << LEND; 
  }
}


void Extractor::loadMap(std::string& conf){
  if (globalSettings::verboseLevel>1) lout << "[I] Ectractor::loadMap " << this << "('" << conf << "')" << LEND;
  std::vector<std::pair<std::pair<std::string, std::string>, std::string> > ans;
  configReader::readConfig(conf, ans);
  for (auto confp : ans){
    map.push_back({split(confp.first.first), confp.second});
  }
}

void Extractor::loadGroups(std::string& conf){
  if (globalSettings::verboseLevel>1) lout << "[I] Ectractor::loadGroups " << this << "('" << conf << "')" << LEND;
  std::vector<std::pair<std::pair<std::string, std::string>, std::string> > ans;
  configReader::readConfig(conf, ans, 0);
  for (auto confp : ans){
    groups.push_back({confp.first.first, std::vector<std::string>()});
    getArray(confp.second, groups[groups.size()-1].second);
  }
}
void Extractor::loadChords(std::string& conf){
  if (globalSettings::verboseLevel>1) lout << "[I] Ectractor::loadChords " << this << "('" << conf << "')" << LEND;
  std::vector<std::pair<std::pair<std::string, std::string>, std::string> > ans;
  configReader::readConfig(conf, ans, 0);
  for (auto confp : ans){
    chords.push_back({confp.first.first, std::vector<std::string>()});
    getArray(confp.second, chords[chords.size()-1].second);
  }
}

void Extractor::setConfigParam(std::string& param, std::string& paramKey, std::string& value){
  if (globalSettings::verboseLevel>1) lout << "[I] Ectractor::setConfigParameter " << this << "('" << param <<"', '" << paramKey << "', '" << value << "')" << LEND;
  if (param=="fps") fps=std::stod(value);
  else if (param=="limit") limit=std::stod(value);
  else if (param=="peek-limit") peekLimit=std::stod(value);
  else if (param=="downspeed") maxDownSpeed=std::stod(value);
  else if (param=="upspeed") maxUpSpeed=std::stod(value);
  else if (param=="filename") ofilename=value;
  else if (param=="map"){
    loadMap(value);
  }else if (param=="groups"){
    loadGroups(value);
  }else if (param=="chords"){
    loadChords(value);
  }else if (globalSettings::verboseLevel) lout << "[W] Ectractor::setConfigParam " << this << ", unknown parameter '" << param << "'" << LEND;
}
void Extractor::readConfig(std::istream& co){
  std::vector<std::pair<std::pair<std::string, std::string>, std::string> > ans;
  configReader::readConfig(co, ans);
  for (auto conf: ans) setConfigParam(conf.first.first, conf.first.second, conf.second);
}