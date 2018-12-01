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
  Formula p_new; // the number of new particles wil be rand(0,1)*2*p_new rounded to the nearest integer.
                 // the expected number of new particles is thus p_new
  Formula x;
  Formula y;
  
  struct ParticleConfiguration{
    vector<pair<int, int> > FPEVs;
    
    FormulaParameterEngine fpe;
    // each Particle uses the fpe of ParticleConfiguration, except non-inherited values are updated
    // (because there is can be a big number of Particles, it would be inefficient to copy all
    //  the inherited FPEVs every time.)
    
    Formula dSpeedX;
    Formula dSpeedY;
  }
  ParticleConfiguration particleConf;
  struct Particle{
    FormulaParameterEngine* fpe;
    double x;
    double y;
    double speedX;
    double speedY;
    Formula dSpeedX;
    Formula dSpeedY;
    std::vector<double> fpevs;
    
    void loadConfig(ParticleConfiguration& pconf);
    
    Particle(ParticleConfiguration& pconf):fpe(pfpe),dSpeedX(fpe),dSpeedY(dpe){
      loadConfig(pconf);
    }
  };
  
  
  vector<Particle> particles;
  
  
  void update(int cframe); // draw calls this
  
  FormulaParameterEngine fpe;
  std::vector<double> fpevs;
  //magic to make fpe work
  void setParticleFPEV(std::string& key, std::string& value); //saves setting to particleConf
  void setFPEV(std::string& key, std::string& value);
  void updateFPE(int cframe);
  // updating particle fpe:
  // first a call to initUpdateParticleFPE: sets up fpe inherited values etc.
  // then for each particle call updateParticleFPE: changes the particle-specific values.
  void initUpdateParticleFPE(int cframe);
  void updateParticleFPE(int cframe, Particle& particle); // assumes initUpdateParticleFPE is called before calling this
  
  TrackController* tc;
public:
  void draw(int cframe, cv::Mat* frame, double xScale, double yScale); // draws this
  
  ParticleSource(FormulaParameterEngine* pfpe, TrackController* ptc): 
    fpe(pfpe), tc(ptc)
    {
    
  }
};