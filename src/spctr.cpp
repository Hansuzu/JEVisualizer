#include <log.h>
#include <spctr.h>
#include <fstream>

void SPCTRFile::read(const char* filename){
  if (globalSettings::verboseLevel>1) lout << "[I] SPCTRFile::read " << this << "(" << filename << ")" << LEND;
  std::ifstream is(filename);
  is >> N >> M;
  spectrums.resize(M);
  for (int i=0;i<M;++i){
    is >> spectrums[i].first;
    spectrums[i].second.resize(N);
    for (int j=0;j<N;++j){
      is >> spectrums[i].second[j];
    }
  }
}

void SPCTRFile::write(const char* filename){
  std::ofstream os(filename);
  os << N << " " << M << "\n";
  for (int i=0;i<M;++i){
    os << spectrums[i].first << " ";
    spectrums[i].second.resize(N);
    for (int j=0;j<N;++j){
      os << spectrums[i].second[j];
      if (j+1<N) os << " ";
    }
    os << "\n";
  }
}


void SPCTRFile::fromSpectrum(std::vector<std::vector<double> >& spectrum, std::vector<double>& times){
  M=spectrum.size();
  N=0;
  double ltime=0;
  if (times.size()) ltime=times.back();
  for (int i=0;i<M;++i){
    if (N<(int)spectrum[i].size()) N=spectrum[i].size();
    double time=0;
    if (i>=(int)times.size()){
      time=ltime;
    }else{
      time=times[i];
    }
    spectrums.push_back({time, std::vector<double>()});
  }
  for (int i=0;i<M;++i){
    spectrums[i].second.resize(N);
    for (int j=0;j<N;++j){
      if (j<(int)spectrum[i].size()){
        spectrums[i].second[j]=spectrum[i][j];
      }else{
        spectrums[i].second[j]=0;
      }
    }
  }
}

void SPCTRFile::singleSpectrum(std::vector<double>& ans, double time){
  if (globalSettings::verboseLevel>2) lout << "[X] SPCTRFile::singleSpectrum " << this << "(&ans, " << time << ")" << LEND;
  int a=0;
  int b=spectrums.size()-1;
  while (a<b){
    int m=(a+b)/2;
    if (spectrums[m].first<time) a=m+1;
    else                         b=m;
  }
  ans.resize(N);
  if (a>=(int)spectrums.size()){
    for (int i=0;i<N;++i) ans[i]=0;
  }else if (a==0){
    //TODO: something here...?
    for (int i=0;i<N;++i) ans[i]=0;
  }else{
    double k=(time-spectrums[a-1].first)/(spectrums[a].first-spectrums[a-1].first);
    //if (k>1 || k<0) lout << "QAQ k:" << k << LEND;
    for (int i=0;i<N;++i){
      ans[i]=k*spectrums[a].second[i]+(1-k)*spectrums[a-1].second[i];
   //   if (ans[i]>1 || ans[i]<0) lout << ans[i] << LEND;
    }
  }
}

void SPCTRFile::getSpectrums(std::vector<double>& times, double shift, std::vector<std::vector<double> >& ans){
  if (globalSettings::verboseLevel>1) lout << "[I] SPCTRFile::getSpectrums " << this << "(&times, &ans)" << LEND;
  for (double time : times){
    time+=shift;
    ans.push_back(std::vector<double>());
    singleSpectrum(ans.back(), time);
  }
}