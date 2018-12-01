
void ParticleSource::setFPEV(std::string& key, std::string& value){
  if (globalSettings::verboseLevel>1) lout << "[I] ParticleSource::setFPEV " << this << "('" << key << "', '" << value << "')" << LEND;
  int index=std::stoi(key);
  if (value=="rand") fpe.setIndex(index, 1);
  else if (value.size()>12 && value.substr(0,12)=="track-values"){
    fpeTracks.push_back(std::vector<TrackController::Index>());
    tc->parseTrackIndices(value.substr(12,value.size()-12), fpeTracks.back());
    fpe.setIndex(index, 1+fpeTracks.size());
  }else if (value=="null"||value=="0") fpe.setIndex(index, 0);
  else if (globalSettings::verboseLevel){
     lout << "[W] Layer:setFPEV " << this << ", unknown FPV: '" << value << "'" << LEND;
  }
}

void ParticleSource::setParticleFPEV(std::string& key, std::string& value){
  if (globalSettings::verboseLevel>1) lout << "[I] ParticleSource::setParticleFPEV " << this << "('" << key << "', '" << value << "')" << LEND;
  int index=std::stoi(key);
  
  if (value=="rand") particleConf.FPVEs.emplace_back(index, 1);
  else if (value=="speedX") particleConf.FPVEs.emplace_back(index, 2);
  else if (value=="speedY") particleConf.FPVEs.emplace_back(index, 3);
  else if (value=="X") particleConf.FPVEs.emplace_back(index, 4);
  else if (value=="Y") particleConf.FPVEs.emplace_back(index, 5);
  else if (value=="null"||value=="0") particleConf.fpe.setIndex(index, 0);
  else if (globalSettings::verboseLevel){
     lout << "[W] Layer:setFPEV " << this << ", unknown FPV: '" << value << "'" << LEND;
  }
}


void ParticleSource::updateFPE(int){
  if (!fpevs.size()){
    fpevs.resize(1+fpeTracks.size());
  }
  fpevs[0]=rand();
  for (int i=0;i<(int)fpeTracks.size();++i){
    double sum=0;
    for (TrackController::Index& ix: fpeTracks[i]){
      sum+=ix.getValue();
    }
    fpevs[1+i]=sum;
  }
  fpe.updateValues(fpevs);
}

void ParticleSource::initUpdateParticleFPE(int){
  if (!fpevs.size()){
    fpevs.resize(5);
  }
  fpe.updateValues(fpevs);
}

void ParticleSource::updateParticleFPE(int, Particle& particle){
  
  for (auto& p:ponf.FPEVs){
    double value=0;
    switch (p.second){
      case 1: value=rand(); break;
      case 2: value=speedX; break;
      case 3: value=speedY; break;
      case 4: value=x; break;
      case 5: value=y; break;
    }
    fpe.setValue(p.first, p.second);
  }
  
  if (!fpevs.size()){
    fpevs.resize(5);
  }
  fpe.updateValues(fpevs);
}



void ParticleSource::Particle::updateFPE(int){
  if (!fpevs.size()){
    fpevs.resize(5);
  }
  fpevs[0]=rand();
  fpevs[1]=speedX;
  fpevs[2]=speedY;
  fpevs[3]=x;
  fpevs[4]=y;
  fpe.updateValues(fpevs);
}


void ParticleSource::Particle::loadConfig(ParticleConfiguration& pconf){
  // set up fpe
  for (auto& p:ponf.FPEVs){
    fpe.setIndex(p.first, p.second);
  }
  // 
}


void update(int cframe){
  updateFPE(cframe);
  for (
}