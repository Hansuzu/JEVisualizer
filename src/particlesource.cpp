#include <particlesource.h>
#include <drawingfunctions.cpp>

void ParticleSource::setFPEV(std::string& key, std::string& value){
  if (globalSettings::verboseLevel>1) lout << "[I] ParticleSource::setFPEV " << this << "('" << key << "', '" << value << "')" << LEND;
  int index=std::stoi(key);
  if (value.size()>=4 && value.substr(0,4)="rand"){
    std::string rand_seed=value.substr(4);
    if (rand_seed.size()==0) rand_seed="0";
    rand_seed=std::stoi(rand_seed);
    random_generators.emplace_back(rand_seed);
    fpemap.push_back(random_generators.size());
    fpe.setIndex(index, fpemap.size());
  }else if (value.size()>12 && value.substr(0,12)=="track-values"){
    fpeTracks.push_back(std::vector<TrackController::Index>());
    tc->parseTrackIndices(value.substr(12,value.size()-12), fpeTracks.back());
    fpemap.push_back(-fpeTracks.size());
    fpe.setIndex(index, fpemap.size());
  }else if (value=="null"||value=="0") fpe.setIndex(index, 0);
  else if (globalSettings::verboseLevel){
     lout << "[W] Layer:setFPEV " << this << ", unknown FPV: '" << value << "'" << LEND;
  }
}

void ParticleSource::setParticleFPEV(std::string& key, std::string& value){
  if (globalSettings::verboseLevel>1) lout << "[I] ParticleSource::setParticleFPEV " << this << "('" << key << "', '" << value << "')" << LEND;
  int index=std::stoi(key);
  
  
  if (value=="speedX") particleConf.FPVEs.emplace_back(index, 1);
  else if (value=="speedY") particleConf.FPVEs.emplace_back(index, 2);
  else if (value=="X") particleConf.FPVEs.emplace_back(index, 3);
  else if (value=="Y") particleConf.FPVEs.emplace_back(index, 4);
  else if (value.size()>=4 && value.substr(0,4)="rand"){
    std::string rand_seed=value.substr(4);
    if (rand_seed.size()==0) rand_seed="0";
    if (rand_seed[0]=='#'){ // use fpev:s value as a seed.
      rand_seed=-std::stoi(rand_seed.substr(1))-1;
    }else{
      rand_seed=std::stoi(rand_seed);
    }
    particleConf.random_generator_seeds.push_back(random_seed);
    particleConf.FPVEs.emplace_back(index, 4+particleConf.random_generator_seeds.size());
  }
  else if (value=="null"||value=="0") particleConf.fpe.setIndex(index, 0);
  else if (globalSettings::verboseLevel){
     lout << "[W] Layer:setFPEV " << this << ", unknown FPV: '" << value << "'" << LEND;
  }
}


void ParticleSource::updateFPE(int){
  if (!fpevs.size()){
    fpevs.resize(fpemap.size());
  }
  for (int i=0;i<fpemap.size();++i){
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
  if (!fpevs.size()){
    fpevs.resize(4+random_generator_seeds.size());
    for (int i=0;i<4+random_generator_seeds.size();++i) fpevs[i]=0;
  }
  fpe.updateValues(fpevs);
}

void ParticleSource::updateParticleFPE(int, Particle& particle){
  for (auto& p:ponf.FPEVs){
    double value=0;
    switch (p.second){
      case 1: value=speedX; break;
      case 2: value=speedY; break;
      case 3: value=x; break;
      case 4: value=y; break;
      default: value=distribution(particle.generators[p.second-5]); break;
    }
    fpe.setValue(p.first, p.second);
  }
}


void ParticleSource::Particle::loadConfig(ParticleConfiguration& pconf){
  // set up random generators
  for (int seed : pconf.generators){
    if (seed>=0){
      generators.emplace_back(seed);
    }else{
      generators.emplace_back(pconf.fpe->getValue(-seed-1));
    }
  }
  // TODO: x, y, speedX, speedY, lifeTime etc.
  
}

void updateParticle(int cframe, Particle& particle){
  updateParticleFPE(cframe, particle);
  particle.speedX.value+=particleConf.dSpeedX.value();
  particle.speedY.value+=particleConf.dSpeedY.value();
  particle.x+=particle.speedX;
  particle.y+=particle.speedY;
  --particle.lifeTime;
  if (particle.lifeTime<=0) particle.isDead=1;
}

void update(int cframe){
  updateFPE(cframe);
  // create new particles
  int nnew = (int)(new_particles.value()+0.5);
  for (int i=0;i<nnew;++i){
    particles.emplace_back(particleConf);
  }
  // update particles
  initUpdateParticleFPE(cframe);
  for (int i=0;i<particles.size();++i){
    updateParticle(cframe, particles[i]);
    if (particles[i].isDead){ // if the particle died, remove it.
      particles[i].swap(particles.back());
      particles.pop_back();
      --i;
    }
  }
}


void draw(int cframe, cv::Mat* frame, double xScale, double yScale){
  update(cframe);
  for (Particle& particle: particles){
    // TODO
    // Draw a particle
    double x = particle.x*xScale;
    double y = particle.y*yScale;
    cv::Point p(x, y);
    cv::Scalar color(1, 1, 1, 1)
    drawingFunctions::drawTriangle(p, p, 2, color, frame);
  }
}