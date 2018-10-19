#include <controller.h>
#include <bits/stdc++.h>
using namespace std;

int verboseLevel=2;

int mode=0;

int main(int argc, const char** args){
  string config="main.config";
  for (int i=0;i<argc;++i){
    if (std::string(args[i])=="--config"){
      if (++i==argc) break;
      config=args[i];
    }else if (std::string(args[i])=="--verbose0" || std::string(args[i])=="-v0"){
      verboseLevel=0;
    }else if (std::string(args[i])=="--verbose" || std::string(args[i])=="--verbose1" || std::string(args[i])=="-v" || std::string(args[i])=="-v1"){
      verboseLevel=1;
    }else if (std::string(args[i])=="--verbose2" || std::string(args[i])=="-v2"){
      verboseLevel=2;
    }else if (std::string(args[i])=="--verbose3" || std::string(args[i])=="-v3"){
      verboseLevel=3;
    }else if (std::string(args[i])=="--no-run"){
      mode=1;
    }else if (std::string(args[i])=="--extractor"){
      mode=2;
    }
  }
  Controller controller(config, verboseLevel);
  controller.loadFiles();
  controller.createSpectrums();
  cout << "spectrums ok" << endl;
  if (mode==0){
    controller.runVisualizer();
  }else if (mode==2){
    controller.runExtractor();
  }
  /*
  std::ifstream co(config);
  readMainConfig(co);
  if (!audio_file.size()){
    cout << "No audio file." << endl;
    return 0;
  }
  WavFile wfile;
  int a=wfile.read(audio_file.c_str());
  cout << a << endl;
  if (!a){
    std::vector<std::vector<double> > spectrums;
    std::vector<double> times;
    cout << "Analysing music..." << endl;
    wfile.spectrums(FK, F0, F1, CHLEN, THR, spectrums, times, verboseLevel);
    cout << "Analysis ready." << endl;
    cout << "Will start visualizer" << endl;
    if (spectrums.size()!=times.size()){
      cout << "Internal error: spectrums.size()!=times.size()" << std::endl;
    }else if (spectrums.size()){
      Visualizer o(video_config_file.c_str(), spectrums[0].size(), verboseLevel);
      cout << "Writing video..." << endl;
      for (int i=0;i<(int)spectrums.size();++i){
        if (verboseLevel>1){
          cout << times[i] << endl;
          for (int j=0;j<(int)spectrums[i].size();++j){
            cout << spectrums[i][j] << ", ";
          }cout << endl;
        }
        o.next(times[i], spectrums[i], verboseLevel);
      }
      if (on_end.size()) cout << system(on_end.c_str()) << endl;
    }
  }*/
}