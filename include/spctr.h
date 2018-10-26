#include <vector>
#include <globalsettings.h>

#ifndef SPCTR_H
#define SPCTR_H

class SPCTRFile{
private:
  std::vector<std::pair<double, std::vector<double> > > spectrums;
  int N, M;
public:
  void read(const char* filename);
  void write(const char* filename);
  
  double length(){
    if (!spectrums.size()) return 0;
    return spectrums.back().first;
  }
  void fromSpectrum(std::vector<std::vector<double> >& spectrum, std::vector<double>& times);
private:
  void singleSpectrum(std::vector<double>& ans, double time);
public:
  void getSpectrums(std::vector<double>& times, std::vector<std::vector<double> >& ans);
  
  SPCTRFile(){}  
};

#endif