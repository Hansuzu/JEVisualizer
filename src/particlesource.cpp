#include <log.h>
#include <particlesource.h>
#include <drawingfunctions.h>

void ParticleSource::setFPEV(std::string& key, std::string& value){
  if (globalSettings::verboseLevel>1) lout << "[I] ParticleSource::setFPEV " << this << "('" << key << "', '" << value << "')" << LEND;
  int index=std::stoi(key);
  if (value.size()>=4 && value.substr(0,4)=="rand"){
    std::string rand_seed=value.substr(4);
    if (rand_seed.size()==0) rand_seed="0";
    int random_seed=std::stoi(rand_seed);
    random_generators.emplace_back(random_seed);
    fpemap.push_back(random_generators.size());
    fpe.setIndex(index, fpemap.size());
  }else if (value.size()>12 && value.substr(0,12)=="track-values"){
    fpeTracks.push_back(std::vector<TrackController::Index>());
    tc->parseTrackIndices(value.substr(12,value.size()-12), fpeTracks.back());
    fpemap.push_back(-fpeTracks.size());
    fpe.setIndex(index, fpemap.size());
  }else if (value=="null"||value=="0") fpe.setIndex(index, 0);
  else if (globalSettings::verboseLevel){
     lout << "[W] ParticleSource::setFPEV " << this << ", unknown FPV: '" << value << "'" << LEND;
  }
}

void ParticleSource::setParticleFPEV(std::string& key, std::string& value){
  if (globalSettings::verboseLevel>1) lout << "[I] ParticleSource::setParticleFPEV " << this << "('" << key << "', '" << value << "')" << LEND;
  int index=std::stoi(key);
  
  if (value=="speedX"){
    particleConf.FPEVs.emplace_back(index, 1);
    particleConf.fpe.setIndex(index, 1);
  }else if (value=="speedY"){
    particleConf.FPEVs.emplace_back(index, 2);
    particleConf.fpe.setIndex(index, 2);
  }else if (value=="X"){
    particleConf.FPEVs.emplace_back(index, 3);
    particleConf.fpe.setIndex(index, 3);
  }else if (value=="Y"){
    particleConf.FPEVs.emplace_back(index, 4);
    particleConf.fpe.setIndex(index, 4);
  }else if (value.size()>=4 && value.substr(0,4)=="rand"){
    std::string rand_seed=value.substr(4);
    if (rand_seed.size()==0) rand_seed="0";
    int random_seed=0;
    if (rand_seed[0]=='#'){ // use fpev:s value as a seed.
      random_seed=-std::stoi(rand_seed.substr(1))-1;
    }else{
      random_seed=std::stoi(rand_seed);
    }
    particleConf.random_generator_seeds.push_back(random_seed);
    particleConf.FPEVs.emplace_back(index, 4+particleConf.random_generator_seeds.size());
    particleConf.fpe.setIndex(index, 4+particleConf.random_generator_seeds.size());
  }
  else if (value=="null"||value=="0") particleConf.fpe.setIndex(index, 0);
  else if (globalSettings::verboseLevel){
     lout << "[W] ParticleSource::setParticleFPEV " << this << ", unknown FPV: '" << value << "'" << LEND;
  }
}


void ParticleSource::updateFPE(int){
  if (!fpevs.size()){
    fpevs.resize(fpemap.size());
  }
  for (int i=0;i<(int)fpemap.size();++i){
    if (fpemap[i]>0){
      fpevs[i]=distribution(random_generators[fpemap[i]-1]);
    }else{
      double sum=0;
      for (TrackController::Index& ix: fpeTracks[-fpemap[i]-1]){
        sum+=ix.getValue();
      }
      fpevs[i]=sum;
    }
  }
  fpe.updateValues(fpevs);
}

void ParticleSource::initUpdateParticleFPE(int){
  if (!particleConf.fpevs.size()){
    particleConf.fpevs.resize(4+particleConf.random_generator_seeds.size());
    for (int i=0;i<(int)(4+particleConf.random_generator_seeds.size());++i) particleConf.fpevs[i]=0;
  }
  particleConf.fpe.updateValues(particleConf.fpevs);
  std::vector<double> v=particleConf.fpe.getValues();
}

void ParticleSource::updateParticleFPE(int, Particle& particle){
  for (auto& p:particleConf.FPEVs){
    double value=0;
    switch (p.second){
      case 1: value=particle.speedX; break;
      case 2: value=particle.speedY; break;
      case 3: value=particle.x; break;
      case 4: value=particle.y; break;
      default: value=distribution(particle.generators[p.second-5]); break;
    }
    particleConf.fpe.setValue(p.first, value);
  }
}


void ParticleSource::initParticle(Particle& particle){
  // set up random generators
  for (int seed : particleConf.random_generator_seeds){
    if (!seed){
      particle.generators.emplace_back((1<<31)*distribution(random_generator));
    }else if (seed>0){
      particle.generators.emplace_back(seed);
    }else{
      particle.generators.emplace_back(particleConf.fpe.getValue(-seed-1));
    }
  }
  updateParticleFPE(0, particles.back());
  // set fpe value
  
  particle.x=particleConf.startX.value();
  particle.y=particleConf.startY.value();
  particle.speedX=particleConf.startSpeedX.value();
  particle.speedY=particleConf.startSpeedY.value();
  particle.lifeTime=(int)(particleConf.lifeTime.value()+0.5);
}

void ParticleSource::updateParticle(int cframe, Particle& particle){
  updateParticleFPE(cframe, particle);
  // update particles speed and position
  std::vector<double> values = particleConf.dSpeedX.fpe->getValues();
  
  particle.speedX+=particleConf.dSpeedX.value();
  particle.speedY+=particleConf.dSpeedY.value();
  particle.x+=particle.speedX;
  particle.y+=particle.speedY;
  // Check if the particle should die
  if (particle.x<particleConf.minX.value() || particle.x>particleConf.maxX.value() || particle.y<particleConf.minY.value() || particle.y>particleConf.maxX.value()){
    particle.isDead=1;
  }
  --particle.lifeTime;
  if (particle.lifeTime<=0) particle.isDead=1;
}

void ParticleSource::update(int cframe){
  updateFPE(cframe);
  initUpdateParticleFPE(cframe);
  // create new particles
  int nnew = (int)(new_particles.value()+0.5);
  for (int i=0;i<nnew;++i){
    particles.emplace_back();
    initParticle(particles.back());
  }
  // update particles
  for (int i=0;i<(int)particles.size();++i){
    updateParticle(cframe, particles[i]);
    if (particles[i].isDead){ // if the particle died, remove it.
      particles[i].swap(particles.back());
      particles.pop_back();
      --i;
    }
  }
}



void ParticleSource::setParticleConfigParam(std::string& param, std::string& key, std::string& value){
  if (globalSettings::verboseLevel>1) lout << "[I] ParticleSource::setParticleConfigParam " << this << "('" << param << "', '" << key << "', '" << value << "')" << LEND;
  if (param=="start-x"){
    particleConf.startX.parse(value);
  }else if (param=="start-y"){
    particleConf.startY.parse(value);
  }else if (param=="start-speed-x"){
    particleConf.startSpeedX.parse(value);
  }else if (param=="start-speed-y"){
    particleConf.startSpeedY.parse(value);
  }else if (param=="delta-speed-x"){
    particleConf.dSpeedX.parse(value);
  }else if (param=="delta-speed-y"){
    particleConf.dSpeedY.parse(value);
  }else if (param=="min-x"){
    particleConf.minX.parse(value);
  }else if (param=="max-x"){
    particleConf.maxX.parse(value);
  }else if (param=="min-y"){
    particleConf.minY.parse(value);
  }else if (param=="max-y"){
    particleConf.maxY.parse(value);
  }else if (param=="life-time"){
    particleConf.lifeTime.parse(value);
  }else if (param=="fpv"){
    setParticleFPEV(key, value);
  }else if (globalSettings::verboseLevel){
    lout << "[W] ParticleSource::setParticleConfigParam " << this << ", unrecognized parameter '" << param << "'" << LEND;
  }
}

void ParticleSource::loadParticleConfig(std::string& config){
  if (globalSettings::verboseLevel>2) lout << "[X] ParticleSource::loadParticleConfig " << this << "('" << config << "')" << LEND;
  std::vector<std::pair<std::pair<std::string, std::string>, std::string> > ans;
  configReader::readConfig(config, ans);
  for (auto conf:ans) setParticleConfigParam(conf.first.first, conf.first.second, conf.second);
  if (globalSettings::verboseLevel>2) lout << "[X] ParticleSource::loadParticleConfig " << this << " OK " << LEND;
  
}

void ParticleSource::setConfigParam(std::string& param, std::string& key, std::string& value){
  if (globalSettings::verboseLevel>1) lout << "[I] ParticleSource::setConfigParam " << this << "('" << param << "', '" << key << "', '" << value << "')" << LEND;
  if (param=="random-seed"){
    random_generator.seed(std::stoi(value));
  }else if (param=="new-particles") {
    new_particles.parse(value);
  }else if (param=="fpv"){
    setFPEV(key, value);
  }else if (param=="particle"){
    loadParticleConfig(value);
  }else if (globalSettings::verboseLevel){
    lout << "[W] ParticleSource::setConfigParam " << this << ", unrecognized parameter '" << param << "'" << LEND;
  }
}
void ParticleSource::loadConfig(std::string& config){
  if (globalSettings::verboseLevel>2) lout << "[X] ParticleSource::loadConfig " << this << "('" << config << "')" << LEND;
  std::vector<std::pair<std::pair<std::string, std::string>, std::string> > ans;
  configReader::readConfig(config, ans);
  for (auto conf:ans) setConfigParam(conf.first.first, conf.first.second, conf.second);
  if (globalSettings::verboseLevel>2) lout << "[X] ParticleSource::loadConfig " << this << " OK " << LEND;
}



void ParticleSource::draw(int cframe, cv::Mat* frame, double xScale, double yScale){
  if (globalSettings::verboseLevel>2) lout << "[X] ParticleSource::draw " << this << " (" << cframe << ", frame*, " << xScale << ", " << yScale << ")" << LEND;
  update(cframe);
  for (Particle& particle: particles){
    // TODO
    // Draw a particle
    double x = particle.x*xScale;
    double y = particle.y*yScale;
    cv::Point p1(x-10, y-10);
    cv::Point p2(x-10, y+10);
    cv::Point p3(x+10, y+10);
    cv::Point p4(x+10, y-10);
    cv::Scalar color(255, 255, 255, 255);
    drawingFunctions::drawRectangle(p1, p2, p3, p4, color, frame);
  }
}
