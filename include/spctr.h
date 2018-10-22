#include <vector>

#ifndef SPCTR_H
#define SPCTR_H

class SPCTRFile{
private:
  std::vector<std::pair<double, std::vector<double> > > spectrums;
  int N, M;
public:
  void read(const char* filename, int verboseLevel);
  void write(const char* filename, int verboseLevel);
  
  double length(){
    if (!spectrums.size()) return 0;
    return spectrums.back().first;
  }
  void fromSpectrum(std::vector<std::vector<double> >& spectrum, std::vector<double>& times, int verboseLevel);
private:
  void singleSpectrum(std::vector<double>& ans, double time, int verboseLevel);
public:
  void getSpectrums(std::vector<double>& times, std::vector<std::vector<double> >& ans, int verboseLevel);
  
  SPCTRFile(){}  
};

#endif