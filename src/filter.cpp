#include <iostream>
#include <config.h>
#include <filter.h>


void Filter::setConfigParam(std::string& param, std::string& key, std::string& value, int verboseLevel){
  if (verboseLevel) std::cout << "Layer::Filter::setConfigParam(" << param << ", " << key << ", " << value << ")" << std::endl;
  if (param=="type"){
    if (value=="GAUSSIAN-BLUR" || value=="0") type=Type::GaussianBlur;
  }else if (param=="kernel"){
    kernelX=kernelY=std::stoi(value);
  }else if (param=="kernel-x"){
    kernelX=std::stoi(value);
  }else if (param=="kernel-y"){
    kernelY=std::stoi(value);
  }else if (param=="r"){
    
  }else if (param=="g"){
    
  }else if (param=="b"){
    
  }else if (param=="a"){
    
  }
}

void Filter::loadConfig(std::string& config, int verboseLevel){
  std::vector<std::pair<std::pair<std::string, std::string>, std::string> > ans;
  configReader::readConfig(config, ans);
  for (auto conf:ans) setConfigParam(conf.first.first, conf.first.second, conf.second, verboseLevel);
}
void Filter::apply(int, cv::Mat*){
}


