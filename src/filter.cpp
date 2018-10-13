#include <iostream>
#include <filter.h>


void Filter::setConfigParam(std::string& param, std::string& key, std::string& value, int verboseLevel){
  if (verboseLevel>1) std::cout << "[I] Filter::setConfigParam " << this << "('" << param << "', '" << key << "', '" << value << "')" << std::endl;
  if (param=="type"){
    if (value=="GAUSSIAN-BLUR" || value=="0") type=Type::GaussianBlur;
  }else if (param=="kernel"){
    kernelX.parse(value, verboseLevel);
    kernelY.parse(value, verboseLevel);
  }else if (param=="kernel-x"){
    kernelX.parse(value, verboseLevel);
  }else if (param=="kernel-y"){
    kernelY.parse(value, verboseLevel);
  }else if (param=="r"){
    
  }else if (param=="g"){
    
  }else if (param=="b"){
    
  }else if (param=="a"){
    
  }else if (verboseLevel){
    std::cout << "[W] Filter::setConfigParam " << this << ", unknown parameter '" << param << "'" << std::endl;
  }
}

void Filter::loadConfig(std::string& config, int verboseLevel){
  std::vector<std::pair<std::pair<std::string, std::string>, std::string> > ans;
  configReader::readConfig(config, ans);
  for (auto conf:ans) setConfigParam(conf.first.first, conf.first.second, conf.second, verboseLevel);
}
void Filter::apply(cv::Mat* frame1, cv::Mat* frame2, int verboseLevel){
  if (verboseLevel>1) std::cout << "[I] Filter::apply " << this << std::endl; 
  std::vector<double> tmp;
  fpe.updateValues(tmp);
  if (type==GaussianBlur){
    int kX=kernelX.value();
    int kY=kernelY.value();
    cv::GaussianBlur(*frame1, *frame2, cv::Size((kX<<1)|1,(kY<<1)|1), 0, 0);
  }
}


