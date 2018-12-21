#include <controller.h>
#include <bits/stdc++.h>
using namespace std;


int mode=0;

int main(int argc, const char** args){
  string config="main.config";
  globalSettings::verboseLevel=1;
  globalSettings::threadingLevel=7;
  globalSettings::outputFormat=0; // 
  for (int i=1;i<argc;++i){
    std::string arg=args[i];
    if (arg=="--config"){
      if (++i==argc) break;
      config=args[i];
    }else if (arg.size()>9 && arg.substr(0, 9)=="--verbose"){
      globalSettings::verboseLevel=std::stoi(arg.substr(9));
    }else if (arg.size()>2 && arg.substr(0, 2)=="-v"){
      globalSettings::verboseLevel=std::stoi(arg.substr(2));
    }else if (arg.size()>11 && arg.substr(0, 11)=="--threading"){
      globalSettings::threadingLevel=std::stoi(arg.substr(11));
    }else if (arg.size()>2 && arg.substr(0, 2)=="-t"){
      globalSettings::threadingLevel=std::stoi(arg.substr(2));
    }else if (arg=="--no-run"){
      mode=1;
    }else if (arg=="-G"){
      // python-GUI-readable output
      globalSettings::verboseLevel=0;
      globalSettings::outputFormat=1;
    }else if (arg=="--extractor"){
      mode=2;
    }else if (arg=="--spctr"){
      mode=3;
    }else{
      cout << "unrecognized parameter: '" << arg << "'" << endl;
    }
  }
  Controller controller(config);
  controller.loadFiles();
  controller.createSpectrums();
  if (globalSettings::outputFormat==0){
    cout << "spectrums loaded" << endl;
  }
  if (mode==0){
    controller.runVisualizer();
  }else if (mode==2){
    controller.runExtractor();
  }else if (mode==3){
    controller.runCreateSPCTRs();
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
    wfile.spectrums(FK, F0, F1, CHLEN, THR, spectrums, times, globalSettings::verboseLevel);
    cout << "Analysis ready." << endl;
    cout << "Will start visualizer" << endl;
    if (spectrums.size()!=times.size()){
      cout << "Internal error: spectrums.size()!=times.size()" << LEND;
    }else if (spectrums.size()){
      Visualizer o(video_config_file.c_str(), spectrums[0].size(), globalSettings::verboseLevel);
      cout << "Writing video..." << endl;
      for (int i=0;i<(int)spectrums.size();++i){
        if (globalSettings::verboseLevel>1){
          cout << times[i] << endl;
          for (int j=0;j<(int)spectrums[i].size();++j){
            cout << spectrums[i][j] << ", ";
          }cout << endl;
        }
        o.next(times[i], spectrums[i], globalSettings::verboseLevel);
      }
      if (on_end.size()) cout << system(on_end.c_str()) << endl;
    }
  }*/
}