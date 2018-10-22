#include <wav.h>
#include <iostream>

int WavFile::Riff::read(std::vector<unsigned char> data, unsigned int& p){
  if (p+12 > data.size()) return WAV_ERROR_RIFF_ERROR;
  chunkID=(data[p]<<24)|(data[p+1]<<16)|(data[p+2]<<8)|data[p+3];
  chunkSize=(data[p+7]<<24)|(data[p+6]<<16)|(data[p+5]<<8)|data[p+4];
  format=(data[p+8]<<24)|(data[p+9]<<16)|(data[p+10]<<8)|data[p+11];
  if (chunkID!=0x52494646) return WAV_ERROR_RIFF_ERROR;
  p+=12;
  return 0;
}

int WavFile::FmtChunk::read(std::vector<unsigned char> data, unsigned int& p){
  if (p+24 > data.size()) return WAV_ERROR_INVALID_FMT_CHUNK_HEADER;
  chunkID=(data[p]<<24)|(data[p+1]<<16)|(data[p+2]<<8)|data[p+3];
  chunkSize=(data[p+7]<<24)|(data[p+6]<<16)|(data[p+5]<<8)|data[p+4];
  audioFormat=(data[p+9]<<8)|data[p+8];
  numChannels=(data[p+11]<<8)|data[p+10];
  sampleRate=(data[p+15]<<24)|(data[p+14]<<16)|(data[p+13]<<8)|data[p+12];
  byteRate=(data[p+19]<<24)|(data[p+18]<<16)|(data[p+17]<<8)|data[p+16];
  blockAlign=(data[p+21]<<8)|data[p+20];
  bitsPerSample=(data[p+23]<<8)|data[p+22];  
  if (chunkID!=0x666d7420) return WAV_ERROR_INVALID_FMT_CHUNK_HEADER;
  p+=8+chunkSize;
  return 0;
}


int WavFile::DataChunk::read(std::vector<unsigned char> fdata, unsigned int& p){
  while (1){
    if (p+8 > fdata.size()) return WAV_ERROR_INVALID_DATA_CHUNK_HEADER;
    chunkID=(fdata[p]<<24)|(fdata[p+1]<<16)|(fdata[p+2]<<8)|fdata[p+3];
    chunkSize=(fdata[p+7]<<24)|(fdata[p+6]<<16)|(fdata[p+5]<<8)|fdata[p+4];
    p+=8;
//     std::cout << chunkSize << std::endl;
//     std::cout << chunkID << std::endl;
    if (chunkID!=0x64617461 && chunkID!=0x494e464f){
      p+=8+chunkSize;
      continue;
    }
    unsigned int ep=fdata.size();
    if (p+chunkSize<ep) ep=p+chunkSize;
    data.resize(ep-p);
    for (int i=0;i<(int)data.size();++i) data[i]=fdata[p+i];
    return 0;
  }
}


int WavFile::read(const char* filename){
  std::ifstream is(filename, std::ios::binary);
  if (!is.is_open() || !is.good()) return WAV_ERROR_OPENING_ERROR;
  
  std::vector<char> fdatac;
  is.seekg(0, std::ios::end);
  fdatac.resize(is.tellg());
  is.seekg(0, std::ios::beg);
  is.read(&fdatac[0], fdatac.size());
  is.close();
  
  std::vector<unsigned char> fdata;
  fdata.resize(fdatac.size());
  for (int i=0;i<(int)fdata.size();++i)fdata[i]=(unsigned int)fdatac[i];

  unsigned int p=0;
  if (int ERROR=riffHeader.read(fdata, p)) return ERROR;
  if (riffHeader.format != 0x57415645) return WAV_ERROR_NOT_A_WAV_FILE;
  if (int ERROR=fmt.read(fdata, p)) return ERROR;
  if (int ERROR=data.read(fdata, p)) return ERROR;
  
  
  //Extract data...
  channels.resize(fmt.numChannels);
  for (int i=0;i+fmt.blockAlign<=(int)data.data.size();i+=fmt.blockAlign){
    for (int j=0;j<fmt.numChannels;++j){
      int no=0;
      for (int b=0;b<fmt.bitsPerSample;++b){
        no<<=1;
        int s=j*fmt.bitsPerSample+fmt.bitsPerSample-1-b;
        no|=!!(data.data[i+(s/8)]&(1<<(s%8)));
      }
      if (fmt.bitsPerSample==16 && no>=(1<<15)) no-=1<<16;
//       if (fmt.bitsPerSample==32){
//         float t=*(float*)&no;
//         cout << t << endl;
//         long double tf=t;
//         tf*=(1<<31)-1;
//         no=(int)tf;
//         no-=1<<16;
//       }
      channels[j].push_back(no);
    }
  }
//   channelMins.resize(fmt.numChannels);
//   for (int i=0;i<fmt.numChannels;++i) channelMins[i].init(channels[i]);
  
  return 0;
}


double WavFile::valueForJL(std::vector<long long>& sm, double jl,  int K){
  double rv=0;
  for (int bgp=0;bgp<jl/2;++bgp){
    double res=0;
    int e=bgp;
    for (int k=1;bgp+k*jl<sm.size() && k<K;++k){
      int n=bgp+k*jl;
      int h=e+(n-e)/2;
      res+=sm[n]-2*sm[h]+sm[e];
      // jl/3
      int d=(n-e)/6;
      int h1=e+1*d;
      int h2=e+2*d;
      int h3=e+3*d;
      int h4=e+4*d;
      int h5=e+5*d;
      res-=(sm[n]-2*sm[h5]+2*sm[h4]-2*sm[h3]+2*sm[h2]-2*sm[h1]+sm[e])/3.0;
      e=n;
    }
    int length=e-bgp;
    if (length>0) res/=length;
    if (res<0) res=-res;
    if (res>rv) rv=res;
  }
  return rv;
}
void WavFile::singleSpectrum(int sp, int ep, int ch, std::vector<double>& fs, std::vector<double>& ans, std::vector<long long>& sm, double thr, int verboseLevel){
  if (verboseLevel>1) std::cout << "[I] WavFile::singleSpectrum(" << sp << ", " << ep << ", " << ch << ", &fs, &ans, &sm, " << thr << ", " << verboseLevel << ")" << std::endl;
  ans.resize(fs.size());
  for (int i=0;i<(int)ans.size();++i) ans[i]=0;
  if (ep>(int)channels[ch].size()) ep=channels[ch].size();
  sm.resize(ep-sp);
  sm[0]=channels[ch][sp];
  for (int i=sp+1;i<ep;++i) sm[i-sp]=sm[i-sp-1]+channels[ch][i];
  if (verboseLevel>2){
    std::cout << "[X] WavFile::singleSpectum: ";
    for (int j=0;j<(int)fs.size();++j){
      std::cout << j << ":" << fs[j] << "hZ ";
    }
    std::cout << std::endl << "[X] WavFile::singleSpectum: ";
  }
  for (int j=0;j<(int)fs.size();++j){
    double jl=fmt.sampleRate/fs[j];
    ans[j]=0;
    double res=valueForJL(sm, jl, 18);
    
    if (res>ans[j]) ans[j]=res;
    
    double k=(1<<(fmt.bitsPerSample));
    ans[j]/=k;
    if (ans[j]<thr) ans[j]=0;
    if (verboseLevel>2) std::cout <<  j << ":" << ans[j] << " ";
  }
  if (verboseLevel>2) std::cout << std::endl;
}



void WavFile::print(){
  std::cout << "NumChannels: " << fmt.numChannels << "\n";
  std::cout << "SampleRate: " << fmt.sampleRate << "\n";
  std::cout << "ByteRate: " << fmt.byteRate << "\n";
  std::cout << "BlockAlign: " << fmt.blockAlign << "\n";
  std::cout << "BitsPerSample: " << fmt.bitsPerSample << "\n";
  std::cout << std::endl;
  std::cout << "DATA: ";
  for (int i=0;i+fmt.blockAlign<=(int)data.data.size() && i<256;i+=fmt.blockAlign){
    if (i) std::cout << "  ";
    std::cout << "(";
    for (int j=0;j<fmt.numChannels;++j){
      if (j) std::cout << ", ";
      int no=0;
      for (int b=0;b<fmt.bitsPerSample;++b){
        no<<=1;
        int s=j*fmt.bitsPerSample+fmt.bitsPerSample-1-b;
        no|=!!(data.data[i+(s/8)]&(1<<(s%8)));
      }
//       ans[j]=std::max(ans[j], res>=0?res:-res);
      if (fmt.bitsPerSample==16 && no>=(1<<15)) no-=1<<16;
      std::cout << no;
    }
    std::cout << ")";
  }
  if (256<data.data.size()) std::cout << "...";
  std::cout << std::endl;
  std::cout << std::endl;
  
  std::vector<double> hz;
  double b=88;
  hz.push_back(b);
  while (b<10000){
    b*=1.2;
    hz.push_back(b);
  }
    
  std::vector<int> det;
  for (int i=hz.size()-1;i>=0;--i){
    det.push_back(fmt.sampleRate/hz[i]);
  }
    
//   int t;
//   cin >> t;
  std::vector<double> ans;
  for (int f=0;f<40;++f){
//     long long gp=1000.0/20.0;
//     usleep(gp*1000);
      
    double t0=f;
    double t1=f+1;
    t0/=20;
    t1/=20;
    std::cout << "SPECTRUM FROM POSITION " << t0 << "s to " << t1 << "s on channel 0" << std::endl;
    std::vector<long long> sm;
    singleSpectrum(fmt.sampleRate*t0, fmt.sampleRate*t1, 0, hz, ans, sm, 0, 0);
    for (int i=1;i<(int)hz.size();++i){
      //cout << hz[i-1] << "-" << hz[i] << ":" << ans[ans.size()-i] << "\n";
      for (int j=0;j<ans[ans.size()-i]*10;++j) std::cout << "#"; std::cout << "\n";
    }
    std::cout << std::endl;
  }
  
}


double WavFile::length(int ch){
  if (ch<0 || ch>=(int)channels.size()) return 0;
  return channels[ch].size()/fmt.sampleRate;
}

void WavFile::spectrums(int channel, double fmp, double f0, double f1, double chlen, double thr, std::vector<std::vector<double> >& rv, std::vector<double>& times, int verboseLevel){
  std::vector<double> hz;
  double b=f0;
  while (b<f1){
    hz.push_back(b);
    b*=fmp;
  }
  std::vector<long long> sm;
  for (double time : times){
    rv.push_back(std::vector<double>());
    double t0=time;
    double t1=time+chlen;
    time=t1;
    singleSpectrum(fmt.sampleRate*t0, fmt.sampleRate*t1, channel, hz, rv.back(), sm, thr, verboseLevel);
    if (fmt.sampleRate*time>channels[0].size()) break;
  }
}