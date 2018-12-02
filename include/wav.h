#include <fstream>
#include <vector>
#include <unordered_map>
#include <globalsettings.h>

//class WavFile is a class to open and read wav-files and extract soundspectrums out of them

#define WAV_ERROR_OPENING_ERROR               1
#define WAV_ERROR_READING_ERROR               2
#define WAV_ERROR_RIFF_ERROR                  3
#define WAV_ERROR_NOT_A_WAV_FILE              4
#define WAV_ERROR_INVALID_FMT_CHUNK_HEADER    5
#define WAV_ERROR_INVALID_DATA_CHUNK_HEADER   6
#define WAV_ERROR_ERROR_IN_DATA               7


class WavFile{
public:
  class Riff{
  public:
    int chunkID;
    int chunkSize;
    int format;
    int read(std::vector<unsigned char> data, unsigned int& p);
  };
  
  class FmtChunk{
  public:
    int chunkID;
    int chunkSize;
    int audioFormat;
    int numChannels;
    int sampleRate;
    int byteRate;
    int blockAlign;
    int bitsPerSample;
    int read(std::vector<unsigned char> data, unsigned int& p);
  };
  
  class DataChunk{
  public:
    int chunkID;
    int chunkSize;
    std::vector<unsigned char> data;
    int read(std::vector<unsigned char> data, unsigned int& p);
  };
private:

  Riff riffHeader;
  FmtChunk fmt;
  DataChunk data;
  
//   std::vector<Min> channelMins;
  std::vector<std::vector<int> > channels;
  
public:
  int read(const char* filename);
  void print();
  double length(int ch);
private:
  double valueForJL(std::vector<long long>& sm, double jl, int K);
public:
  void singleSpectrum(int sp, int ep, int ch, std::vector<double>& fs, std::vector<double>& ans, std::vector<long long>& sm, double thr);
  void spectrums(int channel, double fmp, double f0, double f1, double chlen, double thr, std::vector<std::vector<double> >& rv, std::vector<double>& times, double shift);
};
