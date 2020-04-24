#include <log.h>
#include <chrono>
#include <filter.h>

void Filter::loadColorRule(std::string& ruleStr){
  if (globalSettings::verboseLevel>1) lout << "[I] Filter::loadColorRule " << this << "('" << ruleStr << "')" << LEND;
  std::vector<std::pair<std::pair<std::string, std::string>, std::string> > ans;
  configReader::readConfig(ruleStr, ans);
  ColorRule* rule = new ColorRule(&fpe);
  colorRules.push_back(rule);
  for (auto conf:ans){
    std::string& param=conf.first.first;
    std::string& value=conf.second;
    if (param=="rc"){
      rule->rc.parse(value);
    }else if (param=="gc"){
      rule->gc.parse(value);
    }else if (param=="bc"){
      rule->bc.parse(value);
    }else if (param=="ac"){
      rule->ac.parse(value);
    }else if (param=="r"){
      rule->r.parse(value);
    }else if (param=="g"){
      rule->g.parse(value);
    }else if (param=="b"){
      rule->b.parse(value);
    }else if (param=="a"){
      rule->a.parse(value);
    }else if (param=="shift-x"){
      rule->shiftX.parse(value);
    }else if (param=="shift-y"){
      rule->shiftY.parse(value);
    }else if (globalSettings::verboseLevel){
      lout << "[W] Filter::loadColorRule " << this << ", unrecognized parameter '" << param << "'" << LEND;
    }
  }
}

void Filter::setConfigParam(std::string& param, std::string& key, std::string& value){
  if (globalSettings::verboseLevel>1) lout << "[I] Filter::setConfigParam " << this << "('" << param << "', '" << key << "', '" << value << "')" << LEND;
  if (param=="type"){
    if (value=="GAUSSIAN-BLUR" || value=="0") type=Type::GaussianBlur;
    else if (value=="COLOR" || value=="1") type=Type::Color;
    else if (value=="SUPER-COLOR" || value=="2") type=Type::SuperColor;
    else if (value=="BOX-BLUR" || value=="3") type=Type::BoxBlur;
    else if (value=="SUPER-COLOR-2" || value=="4") type=Type::SuperColor2;
    else if (globalSettings::verboseLevel){
      lout << "[W] Filter::setConfigParam " << this << ", unknown filter-type '" << value<< "'" << LEND;
    }
  }else if (param=="kernel"){
    kernelX.parse(value);
    kernelY.parse(value);
  }else if (param=="kernel-x"){
    kernelX.parse(value);
  }else if (param=="kernel-y"){
    kernelY.parse(value);
  }else if (param=="rc"){
    rc.parse(value);
  }else if (param=="gc"){
    gc.parse(value);
  }else if (param=="bc"){
    bc.parse(value);
  }else if (param=="ac"){
    ac.parse(value);
  }else if (param=="r"){
    r.parse(value);
  }else if (param=="g"){
    g.parse(value);
  }else if (param=="b"){
    b.parse(value);
  }else if (param=="a"){
    a.parse(value);
  }else if (param=="rule"){
    loadColorRule(value);
  }else if (globalSettings::verboseLevel){
    lout << "[W] Filter::setConfigParam " << this << ", unrecognized parameter '" << param << "'" << LEND;
  }
}

void Filter::loadConfig(std::string& config){
  if (globalSettings::verboseLevel>2) lout << "[X] Filter::loadConfig " << this << "('" << config << "')" << LEND;
  std::vector<std::pair<std::pair<std::string, std::string>, std::string> > ans;
  configReader::readConfig(config, ans);
  for (auto conf:ans) setConfigParam(conf.first.first, conf.first.second, conf.second);
  if (globalSettings::verboseLevel>2) lout << "[X] Filter::loadConfig " << this << " OK " << LEND;
}


double min(double a, double b){
  return a<b?a:b;
}
double max(double a, double b){
  return a>b?a:b;
}

void Filter::applyBoxBlur(cv::Mat* frame1, cv::Mat* frame2){
  int kX=2*kernelX.value();
  int kY=2*kernelY.value();
  cv::blur(*frame1, *frame2, cv::Size(kX|1,kY|1));
}
void Filter::applyGaussianBlur(cv::Mat* frame1, cv::Mat* frame2){
  int kX=2*kernelX.value();
  int kY=2*kernelY.value();
  cv::GaussianBlur(*frame1, *frame2, cv::Size(kX|1,kY|1), 0, 0);
}
void Filter::applyColor(cv::Mat* frame1, cv::Mat* frame2){
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
      p2[x*4+0]=min(max(nb, 0.0), 255.0);
      p2[x*4+1]=min(max(ng, 0.0), 255.0);
      p2[x*4+2]=min(max(nr, 0.0), 255.0);
      p2[x*4+3]=min(max(na, 0.0), 255.0);
    }
  }
}
void Filter::applySuperColorRule(cv::Mat* frame1, cv::Mat* frame2, int i, int k){
  int w=frame1->size().width;
  int h=frame1->size().height;
  
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
  int dy=colorRules.size();
  int y0=std::max(0, shiftY);
  y0+=(i+k)%dy;//With same k, no two colorRules should write to same indices in array
  int y1=std::min(h, h+shiftY);
  int x0=std::max(0, shiftX);
  int x1=std::min(w, w+shiftX);
  
  uchar* p1;
  uchar* p2;
  double nr, ng, nb, na;
  for (int y=y0;y<y1;y+=dy){
    p1=frame1->ptr<uchar>(y-shiftY);
    p2=frame2->ptr<uchar>(y);
    for (int x=x0;x<x1;++x){
      nr=p2[4*x+2]+rcv+rv[0]*p1[(x-shiftX)*4+0]+rv[1]*p1[(x-shiftX)*4+1]+rv[2]*p1[(x-shiftX)*4+2]+rv[3]*p1[(x-shiftX)*4+3];
      ng=p2[4*x+1]+gcv+gv[0]*p1[(x-shiftX)*4+0]+gv[1]*p1[(x-shiftX)*4+1]+gv[2]*p1[(x-shiftX)*4+2]+gv[3]*p1[(x-shiftX)*4+3];
      nb=p2[4*x+0]+bcv+bv[0]*p1[(x-shiftX)*4+0]+bv[1]*p1[(x-shiftX)*4+1]+bv[2]*p1[(x-shiftX)*4+2]+bv[3]*p1[(x-shiftX)*4+3];
      na=p2[4*x+3]+acv+av[0]*p1[(x-shiftX)*4+0]+av[1]*p1[(x-shiftX)*4+1]+av[2]*p1[(x-shiftX)*4+2]+av[3]*p1[(x-shiftX)*4+3];
      p2[x*4+0]=min(max(nb, 0.0), 255.0);
      p2[x*4+1]=min(max(ng, 0.0), 255.0);
      p2[x*4+2]=min(max(nr, 0.0), 255.0);
      p2[x*4+3]=min(max(na, 0.0), 255.0);
    }
  }
}

void Filter::applySuperColorRule2(cv::Mat* frame1, cv::Mat* frame2, cv::Mat* refFrame, int i, int k){
  // supercolorrule that is applied to each pixel separately (very slow :((( but allows to use refFrame to make some cool stuff )
  int w=frame1->size().width;
  int h=frame1->size().height;
  
  double rcv, gcv, bcv, acv;
  int shiftX, shiftY;
  cv::Scalar rv, gv, bv, av;
  
  int dy=colorRules.size();
  int y0=(i+k)%dy;//With same k, no two colorRules should write to same indices in array
  
  uchar* p1;
  uchar* p2;
  uchar* rp;
  double nr, ng, nb, na;
  double r0, g0, b0, a0;
  
  int xx, yy;
  
  if (!refFrame){
    refFrame=frame1; /// TODO: some better solution..?
  }
  
  ColorRule* cr=colorRules[i];
  for (int y=y0;y<h;y+=dy){
    p2=frame2->ptr<uchar>(y);
    rp=refFrame->ptr<uchar>(y);
    for (int x=0;x<w;++x){
        r0=rp[x*4+2];g0=rp[x*4+1];b0=rp[x*4+0];a0=rp[x*4+3];
        
        rcv=255*(cr->rc.isConstant?cr->rc.c:cr->rc.pixelValue(r0, g0, b0, a0));
        gcv=255*(cr->gc.isConstant?cr->gc.c:cr->gc.pixelValue(r0, g0, b0, a0));
        bcv=255*(cr->bc.isConstant?cr->bc.c:cr->bc.pixelValue(r0, g0, b0, a0));
        acv=255*(cr->ac.isConstant?cr->ac.c:cr->ac.pixelValue(r0, g0, b0, a0));
        shiftX=(cr->shiftX.isConstant?cr->shiftX.c:cr->shiftX.pixelValue(r0, g0, b0, a0));
        shiftY=(cr->shiftY.isConstant?cr->shiftY.c:cr->shiftY.pixelValue(r0, g0, b0, a0));
        rv=cr->r.pixelValue(r0, g0, b0, a0);
        gv=cr->g.pixelValue(r0, g0, b0, a0);
        bv=cr->b.pixelValue(r0, g0, b0, a0);
        av=cr->a.pixelValue(r0, g0, b0, a0);
        
        xx=x-shiftX; if (xx<0)xx=0;if (xx>=w) xx=w-1;
        yy=y-shiftY; if (yy<0)yy=0;if (yy>=h) yy=h-1;
        p1=frame1->ptr<uchar>(yy);
        
        
        nr=p2[4*x+2]+rcv+rv[0]*p1[(xx)*4+0]+rv[1]*p1[(xx)*4+1]+rv[2]*p1[(xx)*4+2]+rv[3]*p1[(xx)*4+3];
        ng=p2[4*x+1]+gcv+gv[0]*p1[(xx)*4+0]+gv[1]*p1[(xx)*4+1]+gv[2]*p1[(xx)*4+2]+gv[3]*p1[(xx)*4+3];
        nb=p2[4*x+0]+bcv+bv[0]*p1[(xx)*4+0]+bv[1]*p1[(xx)*4+1]+bv[2]*p1[(xx)*4+2]+bv[3]*p1[(xx)*4+3];
        na=p2[4*x+3]+acv+av[0]*p1[(xx)*4+0]+av[1]*p1[(xx)*4+1]+av[2]*p1[(xx)*4+2]+av[3]*p1[(xx)*4+3];
        p2[x*4+0]=min(max(nb, 0.0), 255.0);
        p2[x*4+1]=min(max(ng, 0.0), 255.0);
        p2[x*4+2]=min(max(nr, 0.0), 255.0);
        p2[x*4+3]=min(max(na, 0.0), 255.0);
    }
  }
}
void Filter::apply(cv::Mat* frame1, cv::Mat* frame2, cv::Mat* refFrame){
  if (globalSettings::verboseLevel>1) lout << "[I] Filter::apply " << this << LEND; 
  std::vector<double> tmp;
  fpe.updateValues(tmp);
  if (type==GaussianBlur){
    if (globalSettings::verboseLevel>2) lout << "[X] Filter::apply " << this << " type==GaussianBlur" << LEND;
    auto start = std::chrono::high_resolution_clock::now();
    applyGaussianBlur(frame1, frame2);
    auto finish = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = finish - start;
    if (globalSettings::verboseLevel>2) lout << "[P] Filter::apply " << this <<  " type==GaussianBlur finished, elapsed time: " << elapsed.count()  << LEND;
  }else if(type==BoxBlur){
    if (globalSettings::verboseLevel>2) lout << "[X] Filter::apply " << this << " type==BoxBlur" << LEND;
    auto start = std::chrono::high_resolution_clock::now();
    applyBoxBlur(frame1, frame2);
    auto finish = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = finish - start;
    if (globalSettings::verboseLevel>2) lout << "[P] Filter::apply " << this <<  " type==BoxBlur finished, elapsed time: " << elapsed.count()  << LEND;
  }else if (type==Color){
    if (globalSettings::verboseLevel>2) lout << "[X] Filter::apply " << this << " type==Color" << LEND; 
    auto start = std::chrono::high_resolution_clock::now();
    applyColor(frame1, frame2);
    auto finish = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = finish - start;
    if (globalSettings::verboseLevel>2) lout << "[P] Filter::apply " << this << " type==Color finished, elapsed time: " << elapsed.count() << LEND;
  }else if (type==SuperColor){
    if (globalSettings::verboseLevel>2) lout << "[X] Filter::apply " << this << " type==SuperColor " << LEND; 
    auto start = std::chrono::high_resolution_clock::now();
    *frame2=cv::Vec4b(0,0,0,0);
    for (int k=0;k<(int)colorRules.size();++k){
      //Stuff to make multithreading possible...
      //With same k, no two colorRules should write to same indices in array
      if (globalSettings::threadingLevel&4){
        fs.clear();
        for (int i=0;i<(int)colorRules.size();++i){
          fs.push_back(std::async(std::launch::async, &Filter::applySuperColorRule, this, frame1, frame2, i, k));
        }
        for (std::future<void>& fut : fs) fut.get();
      }else{
        for (int i=0;i<(int)colorRules.size();++i){
          applySuperColorRule(frame1, frame2, i, k);
        }
      }
    }
    auto finish = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = finish - start;
    if (globalSettings::verboseLevel>2) lout << "[P] Filter::apply " << this << " type==SuperColor finished, elapsed time: " << elapsed.count() << LEND;
  } else if (type==SuperColor2) {
    if (globalSettings::verboseLevel>2) lout << "[X] Filter::apply " << this << " type==SuperColor2 " << LEND; 
    auto start = std::chrono::high_resolution_clock::now();
    *frame2=cv::Vec4b(0,0,0,0);
    for (int k=0;k<(int)colorRules.size();++k){
      //Stuff to make multithreading possible...
      //With same k, no two colorRules should write to same indices in array
      if (globalSettings::threadingLevel&4){
        fs.clear();
        for (int i=0;i<(int)colorRules.size();++i){
          fs.push_back(std::async(std::launch::async, &Filter::applySuperColorRule2, this, frame1, frame2, refFrame, i, k));
        }
        for (std::future<void>& fut : fs) fut.get();
      }else{
        for (int i=0;i<(int)colorRules.size();++i){
          applySuperColorRule2(frame1, frame2, refFrame, i, k);
        }
      }
    }
    auto finish = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = finish - start;
    if (globalSettings::verboseLevel>2) lout << "[P] Filter::apply " << this << " type==SuperColor2 finished, elapsed time: " << elapsed.count() << LEND;
  
    
  }
}


