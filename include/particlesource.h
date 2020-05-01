#include <string>
#include <random>
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
  FormulaParameterEngine fpe;
  
  Formula new_particles; // the number of new particles in a frame (value of this rounded to a nearest integer)
  
  // ParticleSource's random generators for fpe
  std::vector<std::default_random_engine> random_generators;
  std::default_random_engine random_generator; // random generator that is used to generate values for particles fpe rand
  
  std::uniform_real_distribution<double> distribution;
    
  
  // ParticleConfiguration: a struct that contains things all particles have in common
  struct ParticleConfiguration{
    FormulaParameterEngine fpe;
    std::vector<double> fpevs; // used to init fpe.
    // each Particle uses the fpe of ParticleConfiguration, except non-inherited values are updated
    // (because there is can be a huge number of Particles, it would be inefficient to copy all
    //  the inherited FPEVs every time.)
    std::vector<std::pair<int, int> > FPEVs;
    // FPEVs contains all fpe values that are specific for a particle (not common for each particle)
    // Each particle uses this same list of fpe values but the actual values depend on the particle
    
    std::vector<int> random_generator_seeds;
    // Each particle can contain a number of random generators
    // random_generator_seeds contain the seeds. If a seed integer is negative, 
    // the fpe value #(-i-1) is used as a seed
    // (^ a way to create same random to all particles created at the same moment)
    
    Formula dSpeedX;
    Formula dSpeedY;
    
    // particles initial position and speed is calculated from these
    // TODO some other way to generate random starting position than creating random random generators for each...?
    // (it is quite inefficient to create 4 random generators for each particles that are never used after the creation of the particle...)
    // (on the other hand, it would make sense to create 2 random generators, to init x and y and then update dspeedx and dspeedy
    // with these two and init startspeedx and startspeedy as constants...)
    Formula startX;
    Formula startY;
    Formula startSpeedX;
    Formula startSpeedY;
    
    FormulaColor c;
    Formula size; // size
    
    Formula lifeTime; // used to calculate the lifeTime
    
    Formula minX, maxX, minY, maxY; // limits for particles x and y... If a particle goes out of these limits, it dies.
    
    ParticleConfiguration(FormulaParameterEngine* pfpe):fpe(pfpe), dSpeedX(&fpe), dSpeedY(&fpe), startX(&fpe), startY(&fpe), startSpeedX(&fpe), startSpeedY(&fpe), c(&fpe), size(&fpe), lifeTime(&fpe), minX(&fpe), maxX(&fpe), minY(&fpe), maxY(&fpe) {
      
    }
  };
  
  ParticleConfiguration particleConf;
  struct Particle{
    // When particle is created, it gets its own random generator
    // Thus the particles random is independent of what happens outiside the particle after the creation
    std::vector<std::default_random_engine> generators;
    // TODO multiple different random fpe values with only one generator..?
    
    double x;
    double y;
    double speedX;
    double speedY;
    
    double r, g, b, a;
    double size;
    
    int lifeTime;
    
    bool isDead;
    
    
    void swap(Particle& o){
      generators.swap(o.generators);
      double t;
      int ti;
      
      t=x; x=o.x; o.x=t;
      t=y; y=o.y; o.y=t;
      t=speedX; speedX=o.speedX; o.speedX=t;
      t=speedY; speedY=o.speedY; o.speedY=t;
      t=r; r=o.r; o.r=t;
      t=g; g=o.g; o.g=t;
      t=b; b=o.b; o.b=t;
      t=a; a=o.a; o.a=t;
      t=size; size=o.size; o.size=t;
      
      ti=lifeTime; lifeTime=o.lifeTime; o.lifeTime=ti;
      
      ti=isDead;
      isDead=o.isDead; o.isDead=ti;
    }
    
    Particle(): lifeTime(100), isDead(0) {
    }
  };
  
  void initParticle(Particle& particle); // this is called after a new particle is created. inits all particles values
  
  std::vector<Particle> particles;
  
  void updateParticle(int cframe, Particle& particle); // update calls this for each particle
    
  void update(int cframe); // draw calls this
  
  std::vector<std::vector<TrackController::Index> > fpeTracks;
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
  
  void setParticleConfigParam(std::string& param, std::string& key, std::string& value);
  void loadParticleConfig(std::string& config);
  void setConfigParam(std::string& param, std::string& key, std::string& value);
public:
  void loadConfig(std::string& config);
  
  void draw(int cframe, cv::Mat* frame, double xScale, double yScale); // draws this
  
  
  ParticleSource(FormulaParameterEngine* pfpe, TrackController* ptc): 
    fpe(pfpe), new_particles(&fpe), random_generator(0), distribution(0,1), particleConf(&fpe), tc(ptc)
    {
  }
};


#endif
