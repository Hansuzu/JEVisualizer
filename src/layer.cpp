#include <iostream>
#include <layer.h>


void Layer::setFPEV(std::string& key, std::string& value, int verboseLevel){
  if (verboseLevel>1) std::cout << "[I] Layer::setFPEV " << this << "('" << key << "', '" << value << "')" << std::endl;
  int index=std::stoi(key);
  if (value=="layer-width") fpe.setIndex(index, 1);
  else if (value=="layer-height") fpe.setIndex(index, 2);
  else if (value.size()>12 && value.substr(0,12)=="track-values"){
    fpeTracks.push_back(std::vector<TrackController::Index>());
    tc->parseTrackIndices(value.substr(12,value.size()-12), fpeTracks.back());
    fpe.setIndex(index, 2+fpeTracks.size());
  }else if (value=="null"||value=="0") fpe.setIndex(index, 0);
  else if (verboseLevel){
     std::cout << "[W] Layer:setFPEV " << this << ", unknown FPV: '" << value << "'" << std::endl;
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
  if (verboseLevel>1) std::cout << "[I] Layer::drawDrawers " << this << "(" << cframe << ")" << std::endl;
  for (int i=0;i<(int)drawers.size();++i){
    drawers[i]->draw(cframe, frame1, verboseLevel);
  }
}

bool Layer::isVisible(int cframe){
  return cframe>=firstFrame && cframe<=lastFrame; // TODO: visibility parameter in config...
}

void Layer::draw(int cframe, cv::Mat* oframe, int verboseLevel){
  if (verboseLevel>1) std::cout << "[I] Layer::draw " << this << "(" << cframe << ")" << std::endl;
  if (isVisible(cframe)){
    updateFPE(cframe);
    int w=oframe->size().width;
    int h=oframe->size().height;
    // draw layer
    if (hasBgColor && !hasBgImage){ // TODO: bgcolor && bgimage?
      cv::Scalar vLU=LU.value();
      cv::Scalar vLD=LD.value();
      cv::Scalar vRU=RU.value();
      cv::Scalar vRD=RD.value();
      for (int x=0;x<w;++x){
        cv::Scalar U=(vLU*(w-x)+vRU*x)/w;
        cv::Scalar D=(vLD*(w-x)+vRD*x)/w;
        for (int y=0;y<h;++y){
          cv::Scalar color=(U*(h-y)+D*y)/h;
          cv::Vec4b c(color[0], color[1], color[2], color[3]);
          frame1->at<cv::Vec4b>(cv::Point(x, y))=c;
        }
      }
    }else if (hasBgImage){
      background->copyTo(*frame1);
    }else if (!hasBgColor && !hasBgImage) { // init layer to fully transparent black
      *frame1=cv::Scalar(0,0,0,0);
    }
    
    drawDrawers(cframe, verboseLevel);
    
    // apply layer filters
    for (int i=0;i<(int)layerFilters.size();++i){
      if (~i&1) layerFilters[i]->apply(frame1, frame2, verboseLevel);
      else      layerFilters[i]->apply(frame2, frame1, verboseLevel);
    }
    
    // draw layer to image
    if ((layerFilters.size())&1) swap(frame1, frame2);
    
    for (int y=0;y<h;++y){
      for (int x=0;x<w;++x){
          cv::Scalar a=frame1->at<cv::Vec4b>(cv::Point(x, y));
          cv::Scalar o=oframe->at<cv::Vec3b>(cv::Point(x, y));
          int alpha=255-(255-a[3])*(255-o[3])/255/255;
          o=(a[3]*a+(255-a[3])*o)/255;
          frame1->at<cv::Vec4b>(cv::Point(x, y))=cv::Vec4b(o[0], o[1], o[2], alpha);
      }
    }
    
    // apply image filters
    for (int i=0;i<(int)imageFilters.size();++i){
      if (~i&1) imageFilters[i]->apply(frame1, frame2, verboseLevel);
      else      imageFilters[i]->apply(frame2, frame1, verboseLevel);
    }
    
    if ((imageFilters.size())&1) swap(frame1, frame2);
    
    for (int y=0;y<h;++y){
      for (int x=0;x<w;++x){
          cv::Scalar a=frame1->at<cv::Vec4b>(cv::Point(x, y));
          oframe->at<cv::Vec3b>(cv::Point(x, y))=cv::Vec3b(a[0], a[1], a[2]);
      }
    }
  }
}

void Layer::setConfigParam(std::string& param, std::string& key, std::string& value, int verboseLevel){
  if (verboseLevel>1) std::cout << "[I] Layer::setConfigParam " << this << "('" << param << "', '" << key << "', '" << value << "')" << std::endl;
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
  }else if (param=="bg-image"){
    cv::Mat bgimage=cv::imread(value, cv::IMREAD_UNCHANGED);
    if (bgimage.data){
      hasBgImage=1;
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
    }else std::cout << "[E] Layer::setConfigParam " << this << ", couldn't load image '" << value << "'" << std::endl;
  }else if (param=="fpv"){
    setFPEV(key, value, verboseLevel);
  }else if (verboseLevel){
     std::cout << "[W] Layer::setConfigParam " << this << ", unknown parameter '" << value << "'" << std::endl;
  }
}

void Layer::readConfig(const char* configFile, int verboseLevel){
  if (verboseLevel>1) std::cout << "[I] Layer::readConfig " << this << "('" <<configFile << "')" << std::endl;
  std::vector<std::pair<std::pair<std::string, std::string>, std::string> > ans;
  configReader::readConfigFromFile(configFile, ans);
  for (auto& conf:ans) setConfigParam(conf.first.first, conf.first.second, conf.second, verboseLevel);
}
