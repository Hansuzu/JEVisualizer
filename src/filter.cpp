#include <iostream>
#include <filter.h>


void Filter::setConfigParam(std::string& param, std::string& key, std::string& value, int verboseLevel){
  if (verboseLevel>1) std::cout << "[I] Filter::setConfigParam " << this << "('" << param << "', '" << key << "', '" << value << "')" << std::endl;
  if (param=="type"){
    if (value=="GAUSSIAN-BLUR" || value=="0") type=Type::GaussianBlur;
    else if (value=="COLOR" || value=="1") type=Type::Color;
    else if (verboseLevel){
      std::cout << "[W] Filter::setConfigParam " << this << ", unknown filter-type '" << value<< "'" << std::endl;
    }
  }else if (param=="kernel"){
    kernelX.parse(value, verboseLevel);
    kernelY.parse(value, verboseLevel);
  }else if (param=="kernel-x"){
    kernelX.parse(value, verboseLevel);
  }else if (param=="kernel-y"){
    kernelY.parse(value, verboseLevel);
  }else if (param=="rc"){
    rc.parse(value, verboseLevel);
  }else if (param=="gc"){
    gc.parse(value, verboseLevel);
  }else if (param=="bc"){
    bc.parse(value, verboseLevel);
  }else if (param=="ac"){
    ac.parse(value, verboseLevel);
  }else if (param=="r"){
    r.parse(value, verboseLevel);
  }else if (param=="g"){
    g.parse(value, verboseLevel);
  }else if (param=="b"){
    b.parse(value, verboseLevel);
  }else if (param=="a"){
    a.parse(value, verboseLevel);
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
  }else if (type==Color){
    int w=frame1->size().width;
    int h=frame1->size().height;
    double rcv=255*rc.value();
    double gcv=255*gc.value();
    double bcv=255*bc.value();
    double acv=255*ac.value();
    cv::Scalar rv=r.value();
    cv::Scalar gv=g.value();
    cv::Scalar bv=b.value();
    cv::Scalar av=a.value();
    for (int y=0;y<h;++y){
      for (int x=0;x<w;++x){
        cv::Scalar color=frame1->at<cv::Vec4b>(cv::Point(x, y));
        cv::Scalar newColor(rcv, gcv, bcv, acv);
        newColor[2]+=rv[0]*color[0]+rv[1]*color[1]+rv[2]*color[2]+rv[3]*color[3];
        newColor[1]+=gv[0]*color[0]+gv[1]*color[1]+gv[2]*color[2]+gv[3]*color[3];
        newColor[0]+=bv[0]*color[0]+bv[1]*color[1]+bv[2]*color[2]+bv[3]*color[3];
        newColor[3]+=av[0]*color[0]+av[1]*color[1]+av[2]*color[2]+av[3]*color[3];
        newColor[0]=std::min(std::max(newColor[0], 0.0), 255.0);
        newColor[1]=std::min(std::max(newColor[1], 0.0), 255.0);
        newColor[2]=std::min(std::max(newColor[2], 0.0), 255.0);
        newColor[3]=std::min(std::max(newColor[3], 0.0), 255.0);
        frame2->at<cv::Vec4b>(cv::Point(x, y))=cv::Vec4b(newColor);
      }
    }
  }
}


