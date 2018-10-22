#include <controller.h>
#include <vector>
#include <string>


#include <bits/stdc++.h>

void Controller::Track::loadConfig(std::string& configStr, int verboseLevel){
//   std::cout << "Track::loadConfig" << configStr << std::endl;
  std::vector<std::pair<std::pair<std::string, std::string>, std::string> > ans;
  configReader::readConfig(configStr, ans);
  for (std::pair<std::pair<std::string, std::string>, std::string>& conf : ans){
    std::string& param=conf.first.first;
    std::string& value=conf.second;
    if (param=="type"){
      if (value=="WAV" || value=="1") type=WAV;
      else if (value=="MMP" || value=="2") type=MMP;
      else if (value=="SPCTR" || value=="3") type=SPCTR;
      else if (verboseLevel) std::cout << "[W] Controller::Track::loadConfig " << this << ", unknown value for type '" << value << "'" << std::endl;
    }
    else if (param=="f0") F0=std::stof(value);
    else if (param=="f1") F1=std::stof(value);
    else if (param=="fk") FK=std::stof(value);
    else if (param=="chlen") CHLEN=std::stof(value);
    else if (param=="thr") THR=std::stof(value);
    else if (param=="track") track=std::stoi(value);
    else if (param=="channel") channel=std::stoi(value);
    else if (param=="file") file=value;
    else if (verboseLevel){
      std::cout << "[W] Controller::Track::loadConfig " << this << ", unknown parameter '" << param << "'" << std::endl;
    }
  }
}

void Controller::Track::load(int verboseLevel){
  if (type==WAV){
    wavFile=new WavFile;
    wavFile->read(file.c_str());
    mLength=wavFile->length(channel);
  }else if (type==MMP){
    mmpFile=new MMPFile;
    mmpFile->read(file.c_str(), verboseLevel);
    mLength=mmpFile->length(track);
  }else if (type==SPCTR){
    spctrFile=new SPCTRFile;
    spctrFile->read(file.c_str(), verboseLevel);
    mLength=spctrFile->length();
  }
}

void Controller::Track::createSpectrum(std::vector<double>& times, int verboseLevel){
  values.clear();
  if (type==WAV){
    wavFile->spectrums(channel, FK, F0, F1, CHLEN, THR, values, times, verboseLevel);
  }else if (type==MMP){
    mmpFile->spectrums(track, times, values, verboseLevel);
  }else if (type==SPCTR){
    spctrFile->getSpectrums(times, values, verboseLevel);
  }else{
    for (int i=0;i<(int)times.size();++i){
      values.push_back(std::vector<double>());
    }
  }
  while (values.size()<times.size()){
    values.push_back(std::vector<double>());
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

void Controller::runExtractor(){
  std::cout << std::endl << std::endl << std::endl;
  if (verboseLevel>1) std::cout << "[I] Controller::runExtractor " << this << std::endl;
    
  Extractor extractor(extractor_config_file.c_str(), verboseLevel);
  
  std::cout << std::endl << std::endl << std::endl;
  if (verboseLevel>1) std::cout << "[I] Controller::runExtractor " << this << ", configurations loaded " << std::endl;
  
  for (int i=0;i<(int)times.size();++i){
    if (verboseLevel>1) std::cout << "[I] Controller::runExtractor " << this << ", TIME: " << times[i] << std::endl;
    std::vector<std::vector<double>* > st;
    for (int j=0;j<(int)tracks.size();++j){
      st.push_back(tracks[j].getValues(i));
    }
    extractor.next(times[i], st, verboseLevel);
  }
}


void Controller::runCreateSPCTRs(){
  if (verboseLevel>1) std::cout << "[I] Controller::runCreateSPCTRs " << this << "()" << std::endl;
  for (int i=0;i<(int)tracks.size();++i){
    SPCTRFile of;
    of.fromSpectrum(tracks[i].getAllValues(), times, 1);
    of.write((tracks[i].getFileName()+".spctr").c_str(), 1);
  }
}


void Controller::setConfigParam(std::string& param, std::string& paramKey, std::string& value){
  if (verboseLevel>1) std::cout << "[I] Controller::setConfigParameter " << this << "('" << param <<"', '" << paramKey << "', '" << value << "')" << std::endl;
  if (param=="track"){
    int index=std::stoi(paramKey);
    if (index<0) std::cout << "[E] Controller::setConfigParameter " << this << ", ERROR IN INDEX" << std::endl;
    else{
      while ((int)tracks.size()<=index) tracks.push_back(Track());
      tracks[index].loadConfig(value, verboseLevel);
    }
  }
  else if (param=="visualizer") video_config_file=value;
  else if (param=="extractor") extractor_config_file=value;
  else if (param=="on-end") on_end_command=value;
  else if (param=="a-frequency") aFrequency=std::stof(value);
  else if (verboseLevel) std::cout << "[I] Controller::setConfigParam " << this << ", unknown parameter '" << param << "'" << std::endl;
}
void Controller::readMainConfig(std::ifstream& co){
  std::vector<std::pair<std::pair<std::string, std::string>, std::string> > ans;
  configReader::readConfig(co, ans);
  for (auto conf: ans) setConfigParam(conf.first.first, conf.first.second, conf.second);
}