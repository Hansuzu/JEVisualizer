#include <controller.h>
#include <vector>
#include <string>


#include <bits/stdc++.h>

void Controller::Track::loadConfig(std::string& configStr){
//   std::cout << "Track::loadConfig" << configStr << std::endl;
  std::vector<std::pair<std::pair<std::string, std::string>, std::string> > ans;
  configReader::readConfig(configStr, ans);
  for (std::pair<std::pair<std::string, std::string>, std::string>& conf : ans){
    std::string& param=conf.first.first;
//     std::string& key=conf.first.second;
    std::string& value=conf.second;
//     std::cout << param << ", " << value << std::endl;
    if (param=="type"){
      if (value=="WAV" || value=="1") type=WAV;
      else if (value=="MMP" || value=="2") type=MMP;
    }
    else if (param=="f0") F0=std::stof(value);
    else if (param=="f1") F1=std::stof(value);
    else if (param=="fk") FK=std::stof(value);
    else if (param=="chlen") CHLEN=std::stof(value);
    else if (param=="thr") THR=std::stof(value);
    else if (param=="track") track=std::stoi(value);
    else if (param=="channel") channel=std::stoi(value);
    else if (param=="file") file=value;
  }
}

void Controller::Track::load(int verboseLevel){
  if (type==WAV){
    wavFile=new WavFile;
    wavFile->read(file.c_str());
    mLength=wavFile->length(channel);
//     std::cout << "length: " << mLength << std::endl;
  }else if (type==MMP){
    mmpFile=new MMPFile;
    mmpFile->read(file.c_str(), verboseLevel);
    mLength=mmpFile->length(track);
//     std::cout << "length: " << mLength << std::endl;
  }
}

void Controller::Track::createSpectrum(std::vector<double>& times, int verboseLevel){
  values.clear();
  if (type==WAV){
    wavFile->spectrums(channel, FK, F0, F1, CHLEN, THR, values, times, verboseLevel);
  }else if (type==MMP){
//     std::cout << "create Spectrums" << std::endl;
    mmpFile->spectrums(track, times, values, verboseLevel);
//     std::cout << "create Spectrums ok" << std::endl;
  }else{
    for (int i=0;i<(int)times.size();++i){
      values.push_back(std::vector<double>());
    }
  }
}

void Controller::loadFiles(){
  for (int i=0;i<(int)tracks.size();++i){
    tracks[i].load(verboseLevel);
  }
}
void Controller::createSpectrums(){
  times.clear();
  double time=0;
  for (int i=0;i<(int)tracks.size();++i){
    time=std::max(time, tracks[i].length());
  }
  double t=0;
  times.push_back(t);
  while (t<time) times.push_back(t+=1.0/aFrequency);
  
  for (int i=0;i<(int)tracks.size();++i){
    tracks[i].createSpectrum(times, verboseLevel);
  }
}

void Controller::runVisualizer(){
  std::cout << std::endl << std::endl << std::endl;
  if (verboseLevel>1) std::cout << "[I] Controller::runVisualizer " << this << std::endl;
    
  Visualizer visualizer(video_config_file.c_str(), verboseLevel);
  
  std::cout << std::endl << std::endl << std::endl;
  if (verboseLevel>1) std::cout << "[I] Controller::runVisualizer " << this << ", configurations loaded " << std::endl;
  
  for (int i=0;i<(int)times.size();++i){
    if (verboseLevel>1) std::cout << "[I] Controller::runVisualizer " << this << ", TIME: " << times[i] << std::endl;
    std::vector<std::vector<double>* > st;
    for (int j=0;j<(int)tracks.size();++j){
      st.push_back(tracks[j].getValues(i));
    }
    visualizer.next(times[i], st, verboseLevel);
  }
  int v=system(on_end_command.c_str());
  if (v) std::cout << "[E] Controller::runVisualizer " << this << ", system call returned " << v << std::endl;
}

void Controller::setConfigParam(std::string& param, std::string& paramKey, std::string& value){
  if (verboseLevel>1) std::cout << "[I] Controller::setConfigParameter " << this << "('" << param <<"', '" << paramKey << "', '" << value << "')" << std::endl;
  if (param=="track"){
    int index=std::stoi(paramKey);
    if (index<0) std::cout << "[E] Controller::setConfigParameter " << this << ", ERROR IN INDEX" << std::endl;
    else{
      while ((int)tracks.size()<=index) tracks.push_back(Track());
      tracks[index].loadConfig(value);
    }
  }
  else if (param=="visualizer") video_config_file=value;
  else if (param=="on-end") on_end_command=value;
  else if (param=="a-frequency") aFrequency=std::stof(value);
}
void Controller::readMainConfig(std::ifstream& co){
  std::vector<std::pair<std::pair<std::string, std::string>, std::string> > ans;
  configReader::readConfig(co, ans);
  for (auto conf: ans) setConfigParam(conf.first.first, conf.first.second, conf.second);
}