#include <controller.h>
#include <log.h>
#include <vector>
#include <string>

void Controller::Track::loadConfig(std::string& configStr){
//   lout << "Track::loadConfig" << configStr << LEND;
  std::vector<std::pair<std::pair<std::string, std::string>, std::string> > ans;
  configReader::readConfig(configStr, ans);
  for (std::pair<std::pair<std::string, std::string>, std::string>& conf : ans){
    std::string& param=conf.first.first;
    std::string& value=conf.second;
    if (param=="type"){
      if (value=="WAV" || value=="1") type=WAV;
      else if (value=="MMP" || value=="2") type=MMP;
      else if (value=="SPCTR" || value=="3") type=SPCTR;
      else if (globalSettings::verboseLevel) lout << "[W] Controller::Track::loadConfig " << this << ", unknown value for type '" << value << "'" << LEND;
    }
    else if (param=="f0") F0=std::stof(value);
    else if (param=="f1") F1=std::stof(value);
    else if (param=="fk") FK=std::stof(value);
    else if (param=="chlen") CHLEN=std::stof(value);
    else if (param=="thr") THR=std::stof(value);
    else if (param=="track") track=std::stoi(value);
    else if (param=="channel") channel=std::stoi(value);
    else if (param=="file") file=value;
    else if (globalSettings::verboseLevel){
      lout << "[W] Controller::Track::loadConfig " << this << ", unknown parameter '" << param << "'" << LEND;
    }
  }
}

void Controller::Track::load(){
  if (type==WAV){
    wavFile=new WavFile;
    wavFile->read(file.c_str());
    mLength=wavFile->length(channel);
  }else if (type==MMP){
    mmpFile=new MMPFile;
    mmpFile->read(file.c_str());
    mLength=mmpFile->length(track);
  }else if (type==SPCTR){
    spctrFile=new SPCTRFile;
    spctrFile->read(file.c_str());
    mLength=spctrFile->length();
  }
}

void Controller::Track::createSpectrum(std::vector<double>& times){
  values.clear();
  if (type==WAV){
    wavFile->spectrums(channel, FK, F0, F1, CHLEN, THR, values, times);
  }else if (type==MMP){
    mmpFile->spectrums(track, times, values);
  }else if (type==SPCTR){
    spctrFile->getSpectrums(times, values);
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
    tracks[i].load();
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
    tracks[i].createSpectrum(times);
  }
}

void Controller::runVisualizer(){
  lout << LEND << LEND << LEND;
  if (globalSettings::verboseLevel>1) lout << "[I] Controller::runVisualizer " << this << LEND;
    
  Visualizer visualizer(video_config_file.c_str());
  
  lout << LEND << LEND << LEND;
  if (globalSettings::verboseLevel>1) lout << "[I] Controller::runVisualizer " << this << ", configurations loaded " << LEND;
  
  for (int i=0;i<(int)times.size();++i){
    if (globalSettings::verboseLevel>1) lout << "[I] Controller::runVisualizer " << this << ", TIME: " << times[i] << LEND;
    std::vector<std::vector<double>* > st;
    for (int j=0;j<(int)tracks.size();++j){
      st.push_back(tracks[j].getValues(i));
    }
    visualizer.next(times[i], st);
  }
  int v=system(on_end_command.c_str());
  if (v) lout << "[E] Controller::runVisualizer " << this << ", system call returned " << v << LEND;
}

void Controller::runExtractor(){
  lout << LEND << LEND << LEND;
  if (globalSettings::verboseLevel>1) lout << "[I] Controller::runExtractor " << this << LEND;
    
  Extractor extractor(extractor_config_file.c_str());
  
  lout << LEND << LEND << LEND;
  if (globalSettings::verboseLevel>1) lout << "[I] Controller::runExtractor " << this << ", configurations loaded " << LEND;
  
  for (int i=0;i<(int)times.size();++i){
    if (globalSettings::verboseLevel>1) lout << "[I] Controller::runExtractor " << this << ", TIME: " << times[i] << LEND;
    std::vector<std::vector<double>* > st;
    for (int j=0;j<(int)tracks.size();++j){
      st.push_back(tracks[j].getValues(i));
    }
    extractor.next(times[i], st);
  }
}


void Controller::runCreateSPCTRs(){
  if (globalSettings::verboseLevel>1) lout << "[I] Controller::runCreateSPCTRs " << this << "()" << LEND;
  for (int i=0;i<(int)tracks.size();++i){
    SPCTRFile of;
    of.fromSpectrum(tracks[i].getAllValues(), times);
    of.write((tracks[i].getFileName()+".spctr").c_str());
  }
}


void Controller::setConfigParam(std::string& param, std::string& paramKey, std::string& value){
  if (globalSettings::verboseLevel>1) lout << "[I] Controller::setConfigParameter " << this << "('" << param <<"', '" << paramKey << "', '" << value << "')" << LEND;
  if (param=="track"){
    int index=std::stoi(paramKey);
    if (index<0) lout << "[E] Controller::setConfigParameter " << this << ", ERROR IN INDEX" << LEND;
    else{
      while ((int)tracks.size()<=index) tracks.push_back(Track());
      tracks[index].loadConfig(value);
    }
  }
  else if (param=="visualizer") video_config_file=value;
  else if (param=="extractor") extractor_config_file=value;
  else if (param=="on-end") on_end_command=value;
  else if (param=="a-frequency") aFrequency=std::stof(value);
  else if (globalSettings::verboseLevel) lout << "[I] Controller::setConfigParam " << this << ", unknown parameter '" << param << "'" << LEND;
}
void Controller::readMainConfig(std::ifstream& co){
  std::vector<std::pair<std::pair<std::string, std::string>, std::string> > ans;
  configReader::readConfig(co, ans);
  for (auto conf: ans) setConfigParam(conf.first.first, conf.first.second, conf.second);
}