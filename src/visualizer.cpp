#include <string>
#include <iostream>
#include <chrono>
#include <visualizer.h>


void Visualizer::setFPEV(std::string& key, std::string& value, int verboseLevel){
  int index=std::stoi(key);
  if (verboseLevel>1 ) std::cout << "[I] Visualizer::setFPEV " << this << "('" << index << "', '" << value << "')" << std::endl;
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
  else if (verboseLevel){
     std::cout << "[W] Drawer::setFPEV " << this << ", unknown FPV: '" << value << "'" << std::endl;
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

  
void Visualizer::writeNewFrame(int ccframe, std::chrono::duration<double> timeElapsedBeforeWrite, int verboseLevel){
  auto start = std::chrono::high_resolution_clock::now();
  outputVideo.write(*oframeWrite);
  if (verboseLevel>2){
    auto finish = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = finish - start;
    std::cout << "[X] Visualizer::writNewFrame write frame " << ccframe << " to file finished, elapsed time=" << elapsed.count() << " s" << std::endl;
    elapsed+=timeElapsedBeforeWrite;
    std::cout << "[X] Visualizer::writNewFrame write frame " << cframe << " total elapsed time=" << elapsed.count() << " s" << std::endl;
  }
  if (verboseLevel>1) std::cout << "[I] Visualizer::writNewFrame " << this << " drawn frame " << ccframe << std::endl;
}
void Visualizer::launchWriteNewFrame(std::chrono::duration<double> timeElapsedBeforeWrite, int verboseLevel){
  waitWriteFrameToFinish();
  swap(oframe, oframeWrite);
  frameWriter=std::async(std::launch::async, &Visualizer::writeNewFrame, this, cframe, timeElapsedBeforeWrite, verboseLevel);
}
void Visualizer::waitWriteFrameToFinish(){
  if (frameWriter.valid()) frameWriter.get();
}

void Visualizer::nextFrame(int verboseLevel){
  if (verboseLevel>1) std::cout << "[I] Visualizer::nextFrame " << this << ", frame " << cframe << std::endl;
  if (cframe>=firstFrame && cframe<=lastFrame){
    auto start = std::chrono::high_resolution_clock::now();
    updateFPE();
    // draw layers
    for (int i=0;i<(int)layers.size();++i){
      layers[i]->draw(cframe, oframe, verboseLevel);
    }
    auto finish = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = finish - start;
    if (verboseLevel>2){
      std::cout << "[X] Visualizer::nextFrame draw layers " << cframe << "finished, elapsed time=" << elapsed.count() << " s" << std::endl;
    }
    launchWriteNewFrame(elapsed, verboseLevel);
    
    finish = std::chrono::high_resolution_clock::now();
    elapsed = finish - lastFrameFinishTime;
    if (verboseLevel>2){
      std::cout << "[X] Visualizer::nextFrame frame " << cframe << " time compared to previous call=" << elapsed.count() << " s" << std::endl;
    }
    std::cout << cframe << " " << elapsed.count() << " s" << std::endl;
    lastFrameFinishTime=std::chrono::high_resolution_clock::now();
  }else if (cframe%100==0 && verboseLevel>1) std::cout << "[I] Visualizer::nextFrame " << this << "... skipped frame " << cframe << " ..." << std::endl;
  ++cframe;
}
  


void Visualizer::next(double time, std::vector<std::vector<double>*>& newTrackValues, int verboseLevel){
  if (verboseLevel>1) std::cout << "[I] Visualizer::next(" << time << ", &st, " << verboseLevel << ")" << std::endl; 
  tc.setMaxDownSpeed(maxDownSpeed);
  tc.setMaxUpSpeed(maxUpSpeed);

  while (((double)cframe)/fps<time){
    double ftime=(double)cframe/fps;
    
    tc.updateValues(newTrackValues, ftime, time);
    
    ctime=(double)cframe/fps;
    nextFrame(verboseLevel);
  }
  tc.updateValues(newTrackValues, time, time);
}


void Visualizer::setConfigParam(std::string& param, std::string& key, std::string& value, int verboseLevel){
  if (verboseLevel>1) std::cout << "[I] Visualizer::setConfigParam " << this << "('" << param << "', '" << key << "', '" << value << "', " << verboseLevel << ")" << std::endl;
  if (param=="fps"){
    fps=std::stod(value);
  }else if (param=="downspeed"){
    maxDownSpeed=std::stod(value);
  }else if (param=="upspeed"){
    maxUpSpeed=std::stod(value);
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
    setFPEV(key, value, verboseLevel);
  }else if (param=="layer"){
    int index=layers.size();
    if (key.size()) index=std::stoi(key);
    while ((int)layers.size()<=index) layers.push_back(new Layer(w, h, &fpe, &tc));
    layers[index]->readConfig(value.c_str(), verboseLevel);
  }else if (verboseLevel){
    std::cout << "[W] Visualizer::setConfigParam " << this << ", unknown parameter '" << param << "'" << std::endl;
  }
}

void Visualizer::readConfig(std::istream& co, int verboseLevel){
  std::vector<std::pair<std::pair<std::string, std::string>, std::string> > ans;
  configReader::readConfig(co, ans);
  for (auto conf:ans) setConfigParam(conf.first.first, conf.first.second, conf.second, verboseLevel);
}