#include <log.h>
#include <trackcontroller.h>


void TrackController::Track::update(std::vector<double>* newValues, double time, double newValueTime){
  if (newValues->size() != values.size()) {
    values.resize(newValues->size());
    for (int j=0;j<(int)values.size();++j)values[j]=0;
  }
  double dtime=time-updateTime;
  double k=dtime/(newValueTime-updateTime);
  
  for (int j=0;j<(int)values.size();++j){
    values[j]=(1-k)*values[j]+k*limitValue(values[j], multiplier*(*newValues)[j], dtime, maxDownSpeed, maxUpSpeed);
  }
  updateTime=time;
}

void TrackController::Track::clean(double time){
  for (int i=0;i<(int)values.size();++i){
    values[i]=0;
  }
  updateTime=time;
}

double TrackController::Index::getValue(){
  if (controller==NULL) return 0;
  if (trackIndex<0 || trackIndex>=(int)controller->tracks.size()) return 0;
  if (noteIndex<0  || noteIndex >=(int)controller->tracks[trackIndex].values.size()) return 0;
  return controller->tracks[trackIndex].values[noteIndex];
}

void TrackController::updateValues(std::vector<std::vector<double>*>& newValues, double time, double newValueTime){
  for (int i=0;i<(int)newValues.size();++i){
    if (i==(int)tracks.size()) tracks.push_back(Track());
    tracks[i].update(newValues[i], time, newValueTime);
  }
}

void TrackController::clean(double time){
  for (int i=0;i<(int)tracks.size();++i){
    tracks[i].clean(time);
  }
}


TrackController::Index TrackController::getIndex(int i, int j) {
  return Index(this, i, j);
}




void TrackController::splitString(const std::string& str, std::vector<std::string>& res, char c){
  res.push_back("");
  for (int i=0;i<(int)str.size();++i){
    if (str[i]==c) res.push_back("");
    else res.back().push_back(str[i]);
  }
}


void TrackController::parseIntegers(const std::string& str, std::vector<int>& ans){
  std::vector<std::string> r;
  splitString(str, r, '-');
  if (r.size()){
    int a=std::stoi(r[0]);
    int b=a;
    if (r.size()>1) b=std::stoi(r[1]);
    if (a<=b) for (int i=a;i<=b;++i) ans.push_back(i);
    else for (int i=a;i>=b;--i)      ans.push_back(i);
  }
}

void TrackController::parseTrackIndices(const std::string& str, std::vector<TrackController::Index>& res){
  std::string pstr="";
  for (char c : str) if (c!=' ' && c!='\t' && c!='\n') pstr.push_back(c);
  std::vector<std::string> elems;
  splitString(pstr, elems, ',');
  int trackIndex=0;
  for (std::string& c : elems){
    std::vector<std::string> p;
    splitString(c, p, ':');
    std::vector<int> noteIndices;
    if (p.size()>1){
      trackIndex=std::stoi(p[0]);
      parseIntegers(p[1], noteIndices);
    }else{
      parseIntegers(p[0], noteIndices);
    }
    for (int index : noteIndices){
      res.push_back(getIndex(trackIndex, index));
    }
  }
}
