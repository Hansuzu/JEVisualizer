#include <string>
#include <vector>
#include <future>
#include <opencv2/imgproc.hpp>
#include <globalsettings.h>
#include <config.h>
#include <trackcontroller.h>
#include <fpe.h>
#include <formula.h>
#include <formulacolor.h>

#ifndef PARTICLESOURCE_H
#define PRATICLESOURCE_H



class ParticleSource {
private:
  Formula new_particles; // the number of new particles in a frame (value of this rounded to a nearest integer)
  Formula x;
  Formula y;
  
  
  // ParticleSource's random generators for fpe
  std::vector<std::default_random_engine> random_generators;
  std::uniform_real_distribution<double> distribution;
    
  
  // ParticleConfiguration: a struct that contains things all particles have in common
  struct ParticleConfiguration{
    FormulaParameterEngine fpe;
    // each Particle uses the fpe of ParticleConfiguration, except non-inherited values are updated
    // (because there is can be a huge number of Particles, it would be inefficient to copy all
    //  the inherited FPEVs every time.)
    std::vector<std::pair<int, int> > FPEVs;
    // FPEVs contains all fpe values that are specific for a particle (not common for each particle)
    // Each particle uses this same list of fpe values but the actual values depend on the particle
    
    std::vector<int> random_generator_seeds;
    // Each particle can contain a number of random generators (no idea why a particle would contain more 
    // than one, but limiting the number would make configuring a bit unintuitive and no reason
    // to limit it anyways)
    // random_generator_seeds contain the seeds. If a seed integer is negative, 
    // the fpe value #(-i-1) is used as a seed.
    
    Formula dSpeedX;
    Formula dSpeedY;
  };
  
  ParticleConfiguration particleConf;
  struct Particle{
    // When particle is created, it gets its own random generator
    // Thus the particles random is independent of what happens outiside the particle after the creation
    std::vector<std::default_random_engine> generators;
    
    double x;
    double y;
    double speedX;
    double speedY;
    
    int lifeTime;
    
    bool isDead;
    
    void loadConfig(ParticleConfiguration& pconf);
    
    void swap(Particle& o){
      generators.swap(o.generators);
      double t;
      int ti;
      bool tb;
      
      t=x; x=o.x; o.x=t;
      t=y; y=o.y; o.y=t;
      t=speedX; speedX=o.speedX; o.speedX=t;
      t=speedY; speedY=o.speedY; o.speedY=t;
      
      ti=lifeTime; lifeTime=o.lifeTime; o.lifeTime=ti;
      
      ti=isDead;
      isDead=o.isDead; o.isDead=ti;
    }
    
    Particle(ParticleConfiguration& pconf): lifeTime(100), isDead(0) {
      loadConfig(pconf);
    }
  };
  
  
  vector<Particle> particles;
  
  void updateParticle(int cframe, Particle& particle) // update calls this for each particle
    
  void update(int cframe); // draw calls this
  
  FormulaParameterEngine fpe;
  std::vector<double> fpevs;
  std::vector<int> fpemap; // there can be a number of random variables and a number of track variables as fpe
  //magic to make fpe work
  void setParticleFPEV(std::string& key, std::string& value); //saves setting to particleConf
  void setFPEV(std::string& key, std::string& value);
  void updateFPE(int cframe);
  // updating particles fpe:
  // first a call to initUpdateParticleFPE: calls fpe.updateValues to set up fpe inherited values etc.
  // then for each particle call updateParticleFPE (before updating the particle): changes the particle-specific values.
  void initUpdateParticleFPE(int cframe);
  void updateParticleFPE(int cframe, Particle& particle); // assumes initUpdateParticleFPE is called before calling this
  
  TrackController* tc;
public:
  void draw(int cframe, cv::Mat* frame, double xScale, double yScale); // draws this
  
  ParticleSource(FormulaParameterEngine* pfpe, TrackController* ptc): 
    fpe(pfpe), tc(ptc), distribution(0,1)
    {
    
  }
};