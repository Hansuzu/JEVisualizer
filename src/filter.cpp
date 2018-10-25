#include <iostream>
#include <filter.h>

void Filter::loadColorRule(std::string& ruleStr, int verboseLevel){
  if (verboseLevel>1) std::cout << "[I] Filter::loadColorRule " << this << "('" << ruleStr << "')" << std::endl;
  std::vector<std::pair<std::pair<std::string, std::string>, std::string> > ans;
  configReader::readConfig(ruleStr, ans);
  ColorRule* rule = new ColorRule(&fpe);
  colorRules.push_back(rule);
  for (auto conf:ans){
    std::string& param=conf.first.first;
    std::string& value=conf.second;
    if (param=="rc"){
      rule->rc.parse(value, verboseLevel);
    }else if (param=="gc"){
      rule->gc.parse(value, verboseLevel);
    }else if (param=="bc"){
      rule->bc.parse(value, verboseLevel);
    }else if (param=="ac"){
      rule->ac.parse(value, verboseLevel);
    }else if (param=="r"){
      rule->r.parse(value, verboseLevel);
    }else if (param=="g"){
      rule->g.parse(value, verboseLevel);
    }else if (param=="b"){
      rule->b.parse(value, verboseLevel);
    }else if (param=="a"){
      rule->a.parse(value, verboseLevel);
    }else if (param=="shift-x"){
      rule->shiftX.parse(value, verboseLevel);
    }else if (param=="shift-y"){
      rule->shiftY.parse(value, verboseLevel);
    }else if (verboseLevel){
      std::cout << "[W] Filter::loadColorRule " << this << ", unrecognized parameter '" << param << "'" << std::endl;
    }
  }
}

void Filter::setConfigParam(std::string& param, std::string& key, std::string& value, int verboseLevel){
  if (verboseLevel>1) std::cout << "[I] Filter::setConfigParam " << this << "('" << param << "', '" << key << "', '" << value << "')" << std::endl;
  if (param=="type"){
    if (value=="GAUSSIAN-BLUR" || value=="0") type=Type::GaussianBlur;
    else if (value=="COLOR" || value=="1") type=Type::Color;
    else if (value=="SUPER-COLOR" || value=="2") type=Type::SuperColor;
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
  }else if (param=="rule"){
    loadColorRule(value, verboseLevel);
  }else if (verboseLevel){
    std::cout << "[W] Filter::setConfigParam " << this << ", unrecognized parameter '" << param << "'" << std::endl;
  }
}

void Filter::loadConfig(std::string& config, int verboseLevel){
  if (verboseLevel>2) std::cout << "[X] Filter::loadConfig " << this << "('" << config << "')" << std::endl;
  std::vector<std::pair<std::pair<std::string, std::string>, std::string> > ans;
  configReader::readConfig(config, ans);
  for (auto conf:ans) setConfigParam(conf.first.first, conf.first.second, conf.second, verboseLevel);
  if (verboseLevel>2) std::cout << "[X] Filter::loadConfig " << this << " OK " << std::endl;
}
void Filter::apply(cv::Mat* frame1, cv::Mat* frame2, int verboseLevel){
  if (verboseLevel>1) std::cout << "[I] Filter::apply " << this << std::endl; 
  std::vector<double> tmp;
  fpe.updateValues(tmp);
  if (type==GaussianBlur){
    if (verboseLevel>2) std::cout << "[X] Filter::apply type==GaussianBlur " << this << std::endl; 
    int kX=kernelX.value();
    int kY=kernelY.value();
    cv::GaussianBlur(*frame1, *frame2, cv::Size((kX<<1)|1,(kY<<1)|1), 0, 0);
  }else if (type==Color){
    if (verboseLevel>2) std::cout << "[X] Filter::apply type==Color " << this << std::endl; 
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
    uchar* p1;
    uchar* p2;
    double nr, ng, nb, na;
    for (int y=0;y<h;++y){
      p1=frame1->ptr<uchar>(y);
      p2=frame2->ptr<uchar>(y);
      for (int x=0;x<w;++x){
        nr=rcv+rv[0]*p1[x*4+0]+rv[1]*p1[x*4+1]+rv[2]*p1[x*4+2]+rv[3]*p1[x*4+3];
        ng=gcv+gv[0]*p1[x*4+0]+gv[1]*p1[x*4+1]+gv[2]*p1[x*4+2]+gv[3]*p1[x*4+3];
        nb=bcv+bv[0]*p1[x*4+0]+bv[1]*p1[x*4+1]+bv[2]*p1[x*4+2]+bv[3]*p1[x*4+3];
        na=acv+av[0]*p1[x*4+0]+av[1]*p1[x*4+1]+av[2]*p1[x*4+2]+av[3]*p1[x*4+3];
        nr=std::min(std::max(nr, 0.0), 255.0);
        ng=std::min(std::max(ng, 0.0), 255.0);
        nb=std::min(std::max(nb, 0.0), 255.0);
        na=std::min(std::max(na, 0.0), 255.0);
        p2[x*4+0]=nb;
        p2[x*4+1]=ng;
        p2[x*4+2]=nr;
        p2[x*4+3]=na;
      }
    }
  }else if (type==SuperColor){
    if (verboseLevel>2) std::cout << "[X] Filter::apply type==SuperColor " << this << std::endl; 
    int w=frame1->size().width;
    int h=frame1->size().height;
    *frame2=cv::Vec4b(0,0,0,0);

    for (int i=0;i<(int)colorRules.size();++i){
      double rcv=255*colorRules[i]->rc.value();
      double gcv=255*colorRules[i]->gc.value();
      double bcv=255*colorRules[i]->bc.value();
      double acv=255*colorRules[i]->ac.value();
      int shiftX=colorRules[i]->shiftX.value();
      int shiftY=colorRules[i]->shiftY.value();
      cv::Scalar rv=colorRules[i]->r.value();
      cv::Scalar gv=colorRules[i]->g.value();
      cv::Scalar bv=colorRules[i]->b.value();
      cv::Scalar av=colorRules[i]->a.value();
      int y0=std::max(0, shiftY);
      int y1=std::min(h, h+shiftY);
      int x0=std::max(0, shiftX);
      int x1=std::min(w, w+shiftX);
      
      uchar* p1;
      uchar* p2;
      double nr, ng, nb, na;
      for (int y=y0;y<y1;++y){
        p1=frame1->ptr<uchar>(y-shiftY);
        p2=frame2->ptr<uchar>(y);
        for (int x=x0;x<x1;++x){
          nr=p2[4*x+2]+rcv+rv[0]*p1[(x-shiftX)*4+0]+rv[1]*p1[(x-shiftX)*4+1]+rv[2]*p1[(x-shiftX)*4+2]+rv[3]*p1[(x-shiftX)*4+3];
          ng=p2[4*x+1]+gcv+gv[0]*p1[(x-shiftX)*4+0]+gv[1]*p1[(x-shiftX)*4+1]+gv[2]*p1[(x-shiftX)*4+2]+gv[3]*p1[(x-shiftX)*4+3];
          nb=p2[4*x+0]+bcv+bv[0]*p1[(x-shiftX)*4+0]+bv[1]*p1[(x-shiftX)*4+1]+bv[2]*p1[(x-shiftX)*4+2]+bv[3]*p1[(x-shiftX)*4+3];
          na=p2[4*x+3]+acv+av[0]*p1[(x-shiftX)*4+0]+av[1]*p1[(x-shiftX)*4+1]+av[2]*p1[(x-shiftX)*4+2]+av[3]*p1[(x-shiftX)*4+3];
          nr=std::min(std::max(nr, 0.0), 255.0);
          ng=std::min(std::max(ng, 0.0), 255.0);
          nb=std::min(std::max(nb, 0.0), 255.0);
          na=std::min(std::max(na, 0.0), 255.0);
          p2[x*4+0]=nb;
          p2[x*4+1]=ng;
          p2[x*4+2]=nr;
          p2[x*4+3]=na;
        }
      }
    }
  }
}


