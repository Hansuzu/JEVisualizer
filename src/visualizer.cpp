#include <string>
#include <log.h>
#include <visualizer.h>


void Visualizer::setFPEV(std::string& key, std::string& value){
  int index=std::stoi(key);
  if (globalSettings::verboseLevel>1 ) lout << "[I] Visualizer::setFPEV " << this << "('" << index << "', '" << value << "')" << LEND;
  if (value=="frame") fpe.setIndex(index, 1);
  else if (value=="fps") fpe.setIndex(index, 2);
  else if (value=="w") fpe.setIndex(index, 3);
  else if (value=="h") fpe.setIndex(index, 4);
  else if (value=="time") fpe.setIndex(index, 5);
  else if (value.size()>12 && value.substr(0,12)=="track-values"){
    fpeTracks.push_back(std::vector<TrackController::Index>());
    tc.parseTrackIndices(value.substr(12,value.size()-12), fpeTracks.back());
    fpe.setIndex(index, 5+fpeTracks.size());
  }else if (value=="null"||value=="0") fpe.setIndex(index, 0);
  else if (globalSettings::verboseLevel){
     lout << "[W] Drawer::setFPEV " << this << ", unknown FPV: '" << value << "'" << LEND;
  }
}

void Visualizer::updateFPE(){
  if (!fpevs.size()){
    fpevs.resize(5+fpeTracks.size());
    fpevs[1]=fps;
    fpevs[2]=w;
    fpevs[3]=h;
  }
  fpevs[0]=cframe;
  fpevs[4]=ctime;
  for (int i=0;i<(int)fpeTracks.size();++i){
    double sum=0;
    for (TrackController::Index& ix: fpeTracks[i]){
      sum+=ix.getValue();
    }
    fpevs[2+i]=sum;
  }
  fpe.updateValues(fpevs);
}

  
void Visualizer::writeNewFrame(int ccframe, std::chrono::duration<double> timeElapsedBeforeWrite){
  auto start = std::chrono::high_resolution_clock::now();
  outputVideo.write(*oframeWrite);
  if (globalSettings::verboseLevel>2){
    auto finish = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = finish - start;
    lout << "[P] Visualizer::writeNewFrame " << this << " write frame " << ccframe << " to file finished, elapsed time: " << elapsed.count()  << LEND;
    elapsed+=timeElapsedBeforeWrite;
    lout << "[P] Visualizer::writeNewFrame " << this << " write frame " << ccframe << " total elapsed time: " << elapsed.count()  << LEND;
  }
  if (globalSettings::verboseLevel>1) lout << "[I] Visualizer::writNewFrame " << this << " drawn frame " << ccframe << LEND;
}
void Visualizer::launchWriteNewFrame(std::chrono::duration<double> timeElapsedBeforeWrite){
  waitWriteFrameToFinish();
  swap(oframe, oframeWrite);
  if (globalSettings::threadingLevel&1){
    frameWriter=std::async(std::launch::async, &Visualizer::writeNewFrame, this, cframe, timeElapsedBeforeWrite);
  }else{
    writeNewFrame(cframe, timeElapsedBeforeWrite);
  }
}
void Visualizer::waitWriteFrameToFinish(){
  if (frameWriter.valid()) frameWriter.get();
}

void Visualizer::nextFrame(){
  if (globalSettings::verboseLevel>1) lout << "[I] Visualizer::nextFrame " << this << ", frame " << cframe << LEND;
  if (cframe>=firstFrame && cframe<=lastFrame){
    auto start = std::chrono::high_resolution_clock::now();
    updateFPE();
    // draw layers
    layerIndependentDrawers.resize(layers.size());
    if (globalSettings::threadingLevel&2){
      for (int i=0;i<(int)layers.size();++i){
        layerIndependentDrawers[i]=std::async(std::launch::async, &Layer::drawIndependent, layers[i], cframe, ctime);
      }
      for (int i=0;i<(int)layerIndependentDrawers.size();++i){
        layerIndependentDrawers[i].get();
      }
    }else{
      for (int i=0;i<(int)layers.size();++i){
        layers[i]->drawIndependent(cframe, ctime);
      }
    }
    
    for (int i=0;i<(int)layers.size();++i){
      layers[i]->drawToOframe(cframe, ctime, oframe);
    }
    
    auto finish = std::chrono::high_resolution_clock::now();
    times[(++timesi)&15]=finish;
    std::chrono::duration<double> elapsed16=times[timesi&15]-times[(timesi+1)&15];
    double avg_time=elapsed16.count()/16;
    double ofps=1.0/avg_time;
    int frames_left=lastFrame-cframe;
    double expected_time_left=frames_left*avg_time;
    double percents_done = 100.0*((double)cframe-firstFrame+1)/(lastFrame-firstFrame+1);
    
    std::chrono::duration<double> elapsed = finish - start;
    if (globalSettings::verboseLevel>2){
      lout << "[P] Visualizer::nextFrame " << this << " draw layers of frame " << cframe << " finished, elapsed time: " << elapsed.count() << LEND;
    }
    launchWriteNewFrame(elapsed);
    
    finish = std::chrono::high_resolution_clock::now();
    elapsed = finish - lastFrameFinishTime;
    if (globalSettings::verboseLevel>2){
      lout << "[P] Visualizer::nextFrame " << this << " frame " << cframe << " time compared to previous call: " << elapsed.count() << LEND;
    }
    if (globalSettings::outputFormat==0){
      lout << cframe << " " << elapsed.count() << " s " << percents_done << "% " << " FPS: " << ofps << " Expected time left: " << expected_time_left << LEND;
    }else if (globalSettings::outputFormat==1){
      lout << firstFrame << " " << cframe << " " << lastFrame << " " << elapsed.count() << " " << percents_done << " " << ofps << " " << expected_time_left << LEND;
    }
    lastFrameFinishTime=std::chrono::high_resolution_clock::now();
  }else if (cframe%100==0 && globalSettings::verboseLevel>1) lout << "[I] Visualizer::nextFrame " << this << "... skipped frame " << cframe << " ..." << LEND;
  ++cframe;
}
  


void Visualizer::next(double time, std::vector<std::vector<double>*>& newTrackValues){
  if (globalSettings::verboseLevel>1) lout << "[I] Visualizer::next " << this << " (" << time << ", &st)" << LEND; 
  tc.setMaxDownSpeeds(maxDownSpeeds);
  tc.setMaxUpSpeeds(maxUpSpeeds);
  tc.setMultipliers(multipliers);
  
  while (((double)cframe)/fps<time){
    double ftime=(double)cframe/fps;
    
    tc.updateValues(newTrackValues, ftime, time);
    
    ctime=(double)cframe/fps;
    nextFrame();
  }
  tc.updateValues(newTrackValues, time, time);
}

void Visualizer::setEndTime(double time) {
  if (globalSettings::verboseLevel>1) lout << "[I] Visualizer::setEndTime " << this << " (" << time << ")" << LEND; 
  int maxFrame = ceil(time*fps);
  if (firstFrame<0) firstFrame+=maxFrame;
  if (lastFrame<0) lastFrame+=maxFrame; 
  if (maxFrame<lastFrame)lastFrame=maxFrame;
}


void Visualizer::setConfigParam(std::string& param, std::string& key, std::string& value){
  if (globalSettings::verboseLevel>1) lout << "[I] Visualizer::setConfigParam " << this << "('" << param << "', '" << key << "', '" << value << "')" << LEND;
  if (param=="fps"){
    fps=std::stod(value);
  }else if (param=="downspeed"){
    int k=std::stoi(key);
    while ((int)maxDownSpeeds.size()<=k) maxDownSpeeds.push_back(1000000);
    maxDownSpeeds[k]=std::stod(value);
  }else if (param=="upspeed"){
    int k=std::stoi(key);
    while ((int)maxUpSpeeds.size()<=k) maxUpSpeeds.push_back(1000000);
    maxUpSpeeds[k]=std::stod(value);
  }else if (param=="multiplier"){
    int k=std::stoi(key);
    while ((int)multipliers.size()<=k) multipliers.push_back(1);
    multipliers[k]=std::stod(value);
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
  }else if (param=="fpv"){
    setFPEV(key, value);
  }else if (param=="layer"){
    int index=layers.size();
    if (key.size()) index=std::stoi(key);
    while ((int)layers.size()<=index) layers.push_back(new Layer(w, h, &fpe, &tc));
    layers[index]->readConfig(value.c_str());
  }else if (globalSettings::verboseLevel){
    lout << "[W] Visualizer::setConfigParam " << this << ", unknown parameter '" << param << "'" << LEND;
  }
}

void Visualizer::readConfig(std::istream& co){
  std::vector<std::pair<std::pair<std::string, std::string>, std::string> > ans;
  configReader::readConfig(co, ans);
  for (auto conf:ans) setConfigParam(conf.first.first, conf.first.second, conf.second);
}