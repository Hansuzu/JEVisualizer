#include <visualizer.h>
#include <config.h>
#include <string>
#include <bits/stdc++.h>
using namespace std;



void Visualizer::nextFrame(int verboseLevel){
  if (verboseLevel) std::cout << "Visualizer::nextFrame" << std::endl;
  if (cframe>=firstFrame && cframe<=lastFrame){
    trackValues.resize(tracks.size());
    for (int i=0;i<(int)tracks.size();++i)  trackValues[i]=&tracks[i]->sit;
    
    //TODO: => DRAW LAYERS; 
    for (int i=0;i<(int)layers.size();++i){
      layers[i]->draw(cframe, trackValues, oframe, verboseLevel);
    }
    
    outputVideo.write(*oframe);
    cout << cframe << endl;
  }else if (cframe%100==0) cout << "..." << cframe << "..." << endl;
  ++cframe;
}
  

double Visualizer::limitEstValue(double estv, double stv, double dt, double maxDownSpeed, double maxUpSpeed){
  return std::min(std::max(stv, estv-dt*maxDownSpeed), estv+dt*maxUpSpeed);
}

void Visualizer::next(double time, std::vector<std::vector<double>*>& st, int verboseLevel){
  if (verboseLevel) std::cout << "Visualizer::next(" << time << ", &st, " << verboseLevel << ")" << std::endl; 
  for (int i=0;i<(int)st.size();++i){
    if (i==(int)tracks.size()) tracks.push_back(new Track());
    if (st[i]->size()!=tracks[i]->est.size()){
      tracks[i]->est.resize(st[i]->size());
      tracks[i]->sit.resize(st[i]->size());
      for (int j=0;j<(int)tracks[i]->est.size();++j)tracks[i]->est[j]=0;
    }
  }
  while (((double)cframe)/fps<time){
    double ftime=((double)cframe)/fps;
    double k=(ftime-ctime)/(time-ctime);
    for (int i=0;i<(int)st.size();++i){ //TODO check if time-ctime is correct
      for (int j=0;j<(int)tracks[i]->sit.size();++j){
        tracks[i]->sit[j]=k*tracks[i]->est[j]+(1-k)*limitEstValue(tracks[i]->est[j], (*st[i])[j], time-ctime, tracks[i]->maxDownSpeed, tracks[i]->maxUpSpeed);
      }
    }
    nextFrame(verboseLevel);
  }
  for (int i=0;i<(int)st.size();++i){
    for (int j=0;j<(int)st[i]->size();++j){
      tracks[i]->est[j]=limitEstValue(tracks[i]->est[j], (*st[i])[j], time-ctime, tracks[i]->maxDownSpeed, tracks[i]->maxUpSpeed);
    }
  }
  ctime=time;
}


void Visualizer::setConfigParam(std::string& param, std::string& key, std::string& value, int verboseLevel){
  if (verboseLevel) std::cout << "Visualizer::setConfigParam(" << param << ", " << key << ", " << value << ", " << verboseLevel << ")" << std::endl;
  if (param=="fps"){
    fps=std::stof(value);
  }else if (param=="w" || param=="width"){
    w=std::stoi(value);
  }else if (param=="h" || param=="height"){
    h=std::stoi(value);
  }else if (param=="output-file"){
    ovfname=value;
  }else if (param=="first-frame"){
    firstFrame=std::stoi(value);
  }else if (param=="last-frame"){
    lastFrame=std::stoi(value);
  }else if (param=="layer"){
    int index=layers.size();
    if (key.size()) index=std::stoi(key);
    while ((int)layers.size()<=index) layers.push_back(new Layer(w, h));
    layers[index]->readConfig(value.c_str(), verboseLevel);
  }
}

void Visualizer::readConfig(std::istream& co, int verboseLevel){
  std::vector<std::pair<std::pair<std::string, std::string>, std::string> > ans;
  configReader::readConfig(co, ans);
  for (auto conf:ans) setConfigParam(conf.first.first, conf.first.second, conf.second, verboseLevel);
}