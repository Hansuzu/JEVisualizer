#include <log.h>
#include <chrono>
#include <layer.h>


void Layer::setFPEV(std::string& key, std::string& value, int verboseLevel){
  if (verboseLevel>1) lout << "[I] Layer::setFPEV " << this << "('" << key << "', '" << value << "')" << LEND;
  int index=std::stoi(key);
  if (value=="layer-width") fpe.setIndex(index, 1);
  else if (value=="layer-height") fpe.setIndex(index, 2);
  else if (value.size()>12 && value.substr(0,12)=="track-values"){
    fpeTracks.push_back(std::vector<TrackController::Index>());
    tc->parseTrackIndices(value.substr(12,value.size()-12), fpeTracks.back());
    fpe.setIndex(index, 2+fpeTracks.size());
  }else if (value=="null"||value=="0") fpe.setIndex(index, 0);
  else if (verboseLevel){
     lout << "[W] Layer:setFPEV " << this << ", unknown FPV: '" << value << "'" << LEND;
  }
}

void Layer::updateFPE(int){
  if (!fpevs.size()){
    fpevs.resize(2+fpeTracks.size());
    fpevs[0]=layerWidth;
    fpevs[1]=layerHeight;
  }
  for (int i=0;i<(int)fpeTracks.size();++i){
    double sum=0;
    for (TrackController::Index& ix: fpeTracks[i]){
      sum+=ix.getValue();
    }
    fpevs[2+i]=sum;
  }
  fpe.updateValues(fpevs);
}


void Layer::drawDrawers(int cframe, int verboseLevel){
  if (verboseLevel>1) lout << "[I] Layer::drawDrawers " << this << "(" << cframe << ")" << LEND;
  auto start = std::chrono::high_resolution_clock::now();
  double xs=frame1->cols/layerWidth;
  double ys=frame1->rows/layerHeight;
  for (int i=0;i<(int)drawers.size();++i){
    drawers[i]->draw(cframe, frame1, xs, ys, verboseLevel);
  }
  auto finish = std::chrono::high_resolution_clock::now();
  std::chrono::duration<double> elapsed = finish - start;
  if (verboseLevel>2) lout << "[P] Layer::drawDrawers " << this << " finished, elapsed time: " << elapsed.count() << LEND;
}

bool Layer::isVisible(int cframe){
  return cframe>=firstFrame && cframe<=lastFrame; // TODO: visibility parameter in config...
}

double dif(cv::Scalar& a, cv::Scalar&b){
  cv::Scalar c=a-b;
  return std::max(std::max(abs(c[0]), abs(c[1])), std::max(abs(c[2]), abs(c[3])));
}
void Layer::createBgBackground(bool force){
  cv::Scalar vLU=LU.value();
  cv::Scalar vLD=LD.value();
  cv::Scalar vRU=RU.value();
  cv::Scalar vRD=RD.value();
  double df=std::max(std::max(dif(vLU, lLU), dif(vLD, lLD)), std::max(dif(vRU, lRU), dif(vRD, lRD)));
  if (!force && df<1) return;
  int w=frame1->size().width;
  int h=frame1->size().height;
  for (int x=0;x<w;++x){
    cv::Scalar U=(vLU*(w-x)+vRU*x)/w;
    cv::Scalar D=(vLD*(w-x)+vRD*x)/w;
    for (int y=0;y<h;++y){
      cv::Scalar color=(U*(h-y)+D*y)/h;
      cv::Vec4b c(color[0], color[1], color[2], color[3]);
      background->at<cv::Vec4b>(cv::Point(x, y))=c;
    }
  }
  lLU=vLU;lLD=vLD;lRU=vRU;lRD=vRD;
}

void Layer::applyLayerFilters(int verboseLevel){
  if (verboseLevel>2) lout << "[X] Layer::applyLayerFilters " << this << " apply layer filters" << LEND;
  auto start = std::chrono::high_resolution_clock::now();
  for (int i=0;i<(int)layerFilters.size();++i){
    if (~i&1) layerFilters[i]->apply(frame1, frame2, verboseLevel);
    else      layerFilters[i]->apply(frame2, frame1, verboseLevel);
  }
  if ((layerFilters.size())&1) swap(frame1, frame2);
  if (verboseLevel>2){
    auto finish = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = finish - start;
    lout << "[P] Layer::applyLayerFilters " << this << " finished, total elapsed time: " << elapsed.count() << LEND;
  }
}

void Layer::drawIndependent(int cframe, int verboseLevel){
  if (verboseLevel>1) lout << "[I] Layer::drawIndependent " << this << "(" << cframe << ")" << LEND;
  if (isVisible(cframe)){
    auto start = std::chrono::high_resolution_clock::now();
    updateFPE(cframe);
    // draw layer

    if (hasBgImage || hasBgColor){
      if (hasBgColor) createBgBackground();
      background->copyTo(*frame1);
    }else if (!hasBgColor && !hasBgImage) { // init layer to fully transparent black
      *frame1=cv::Scalar(0,0,0,0);
    }
    
    drawDrawers(cframe, verboseLevel);
    applyLayerFilters(verboseLevel);
    
    if (verboseLevel>2){
      auto finish = std::chrono::high_resolution_clock::now();
      std::chrono::duration<double> elapsed = finish - start;
      lout << "[P] Layer::drawIndependent " << this << " finished, total elapsed time: " << elapsed.count() << LEND;
    }
  }
}

void Layer::drawImageToLayer(cv::Mat* oframe, int verboseLevel){
  if (verboseLevel>2) lout << "[X] Layer::drawImageToLayer " << this << LEND;
  auto start = std::chrono::high_resolution_clock::now();
  
  int w=oframe->size().width;
  int h=oframe->size().height;
  uchar* p1;
  uchar* po;
  for (int y=0;y<h;++y){
    p1=frame1->ptr<uchar>(y);
    po=oframe->ptr<uchar>(y);
    for (int x=0;x<w;++x){
      p1[4*x+0]=(p1[4*x+3]*(int)p1[4*x+0]+(255-p1[4*x+3])*(int)po[3*x+0])/255;
      p1[4*x+1]=(p1[4*x+3]*(int)p1[4*x+1]+(255-p1[4*x+3])*(int)po[3*x+1])/255;
      p1[4*x+2]=(p1[4*x+3]*(int)p1[4*x+2]+(255-p1[4*x+3])*(int)po[3*x+2])/255;
      p1[4*x+3]=255;
    }
  }
  if (verboseLevel>2){
    auto finish = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = finish - start;
    lout << "[P] Layer::drawImageToLayer " << this << " finished, elapsed time: " << elapsed.count() << LEND;
  }
}
void Layer::applyImageFilters(int verboseLevel){
  if (verboseLevel>2) lout << "[X] Layer::applyImageFilters" << this << LEND;
  auto start = std::chrono::high_resolution_clock::now();
  for (int i=0;i<(int)imageFilters.size();++i){
    if (~i&1) imageFilters[i]->apply(frame1, frame2, verboseLevel);
    else      imageFilters[i]->apply(frame2, frame1, verboseLevel);
  }
  if ((imageFilters.size())&1) swap(frame1, frame2);
  if (verboseLevel>2){
    auto finish = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = finish - start;
    lout << "[P] Layer::applyImageFilters " << this << " finished, elapsed time: " << elapsed.count() << LEND;
  }
}
void Layer::drawLayerToImage(cv::Mat* oframe, int verboseLevel){
  if (verboseLevel>2) lout << "[X] Layer::drawLayerToImage " << this << LEND;
  auto start = std::chrono::high_resolution_clock::now();
  
  int w=oframe->size().width;
  int h=oframe->size().height;
  uchar* p1;
  uchar* po;
  for (int y=0;y<h;++y){
    p1=frame1->ptr<uchar>(y);
    po=oframe->ptr<uchar>(y);
    for (int x=0;x<w;++x){
      po[3*x]=p1[4*x];
      po[3*x+1]=p1[4*x+1];
      po[3*x+2]=p1[4*x+2];
    }
  }
  if (verboseLevel>2){
    auto finish = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = finish - start;
    lout << "[P] Layer::drawLayerToImage " << this << " finished, elapsed time: " << elapsed.count() << LEND;
  }
}

void Layer::drawToOframe(int cframe, cv::Mat* oframe, int verboseLevel){
  if (verboseLevel>1) lout << "[I] Layer::drawToOframe " << this << "(" << cframe << ")" << LEND;
  if (isVisible(cframe)){
    auto start = std::chrono::high_resolution_clock::now();
    
    drawImageToLayer(oframe, verboseLevel);
    applyImageFilters(verboseLevel);
    drawLayerToImage(oframe, verboseLevel);
    
    auto finish = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = finish - start;
    if (verboseLevel>2) lout << "[P] Layer::drawToOframe " << this << " finished, elapsed time: " << elapsed.count() << LEND;
  }
}

void Layer::setConfigParam(std::string& param, std::string& key, std::string& value, int verboseLevel){
  if (verboseLevel>1) lout << "[I] Layer::setConfigParam " << this << "('" << param << "', '" << key << "', '" << value << "')" << LEND;
  if (param=="w" || param=="width"){
    layerWidth=std::stod(value);
  }else if (param=="h" || param=="height"){
    layerHeight=std::stod(value);
  }else if (param=="first-frame"){
    firstFrame=std::stoi(value);
  }else if (param=="last-frame"){
    lastFrame=std::stoi(value);
  }else if (param=="drawer"){
    Drawer* k=new Drawer(&fpe, tc);
    drawers.push_back(k);
    k->parse(value, verboseLevel);
  }else if (param=="filter"){
    layerFilters.push_back(new Filter(&fpe, tc));
    layerFilters.back()->loadConfig(value, verboseLevel);
  }else if (param=="image-filter"){
    imageFilters.push_back(new Filter(&fpe, tc));
    imageFilters.back()->loadConfig(value, verboseLevel);
  }else if (param=="bg-color"){
    hasBgColor=1;
    std::vector<std::string> res;
    Formula::split(value, res);
    for (int i=0;i<std::min(4, (int)res.size());++i){
      if (i==0){
        RD=LD=RU=LU.parse(res[i], verboseLevel);
      }else if (i==1){
        RD=LD.parse(res[i], verboseLevel);
      }else if (i==3){
        RD.parse(res[i], verboseLevel);
      }else{
        RU.parse(res[i], verboseLevel);
      }
    }
    if (background) delete background;
    background=new cv::Mat(frame1->size(), CV_8UC4);
    createBgBackground(1);
  }else if (param=="bg-image"){
    cv::Mat bgimage=cv::imread(value, cv::IMREAD_UNCHANGED);
    if (bgimage.data){
      hasBgImage=1;
      if (background) delete background;
      background=new cv::Mat(frame1->size(), CV_8UC4);
      
      if (bgimage.channels()!=4){
        cv::Mat nbgimage(bgimage.size(), CV_MAKE_TYPE(bgimage.depth(), 4));
        int fromTo[] = { 0,0, 1,1, 2,2, -1,3 };
        cv::mixChannels(&bgimage,1,&nbgimage,1,fromTo, bgimage.channels());
        cv::resize(nbgimage, *background, frame1->size());
        for (int x=0;x<frame1->size().width;++x){
          for (int y=0;y<frame1->size().height;++y){
            background->at<cv::Vec4b>(cv::Point(x, y))[3]=255;
          }
        }
      }else{
        cv::resize(bgimage, *background, frame1->size());
      }
    }else lout << "[E] Layer::setConfigParam " << this << ", couldn't load image '" << value << "'" << LEND;
  }else if (param=="fpv"){
    setFPEV(key, value, verboseLevel);
  }else if (verboseLevel){
     lout << "[W] Layer::setConfigParam " << this << ", unknown parameter '" << value << "'" << LEND;
  }
}

void Layer::readConfig(const char* configFile, int verboseLevel){
  if (verboseLevel>1) lout << "[I] Layer::readConfig " << this << "('" <<configFile << "')" << LEND;
  std::vector<std::pair<std::pair<std::string, std::string>, std::string> > ans;
  configReader::readConfigFromFile(configFile, ans);
  for (auto& conf:ans) setConfigParam(conf.first.first, conf.first.second, conf.second, verboseLevel);
}
