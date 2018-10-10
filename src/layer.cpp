#include <layer.h>
#include <iostream>


void Layer::setFPEV(std::string& key, std::string& value, int verboseLevel){
  int index=std::stoi(key);
  if (verboseLevel) std::cout << "Layer::setFPEV(" << index << ", " << value << ")" << std::endl;
  if (value=="layer-width") fpe.setIndex(index, 1);
  else if (value=="layer-height") fpe.setIndex(index, 2);
  else if (value.size()>12 && value.substr(0,12)=="track-values"){
    fpeTracks.push_back(std::vector<TrackController::Index>());
    tc->parseTrackIndices(value.substr(12,value.size()-12), fpeTracks.back());
    fpe.setIndex(index, 2+fpeTracks.size());
  }else if (value=="null"||value=="0") fpe.setIndex(index, 0);
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
  if (verboseLevel>1) std::cout << "Layer::drawDrawers (" << this << ")" << std::endl;
  for (int i=0;i<(int)drawers.size();++i){
    drawers[i]->draw(cframe, frame);
  }
}

bool Layer::isVisible(int cframe){
  return cframe>=firstFrame && cframe<=lastFrame; // TODO: visibility parameter in config...
}

void Layer::draw(int cframe, cv::Mat* oframe, int verboseLevel){
  if (verboseLevel>1) std::cout << "Layer::draw(" << cframe << ")" << std::endl;
  if (isVisible(cframe)){
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
          frame->at<cv::Vec4b>(cv::Point(x, y))=c;
        }
      }
    }else if (hasBgImage){
      background->copyTo(*frame);
    }else if (!hasBgColor && !hasBgImage) { // init layer to fully transparent black
      *frame=cv::Scalar(0,0,0,0);
    }
    
    drawDrawers(cframe, verboseLevel);
    
    // apply layer filters
    for (int i=0;i<(int)layerFilters.size();++i){
      layerFilters[i]->apply(cframe, frame);
    }
    // draw layer to image
    for (int y=0;y<h;++y){
      for (int x=0;x<w;++x){
          cv::Scalar a=frame->at<cv::Vec4b>(cv::Point(x, y));
          cv::Scalar o=oframe->at<cv::Vec3b>(cv::Point(x, y));
          o=(a[3]*a+(255-a[3])*o)/255;
          oframe->at<cv::Vec3b>(cv::Point(x, y))=cv::Vec3b(o[0], o[1], o[2]);
      }
    }
    // apply image filters
    for (int i=0;i<(int)imageFilters.size();++i){
      imageFilters[i]->apply(cframe, oframe);
    }
  }
}

void Layer::setConfigParam(std::string& param, std::string& key, std::string& value, int verboseLevel){
  if (verboseLevel) std::cout << "Layer::setConfigParam(" << param << ", " << key << ", " << value << ")" << std::endl;
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
    k->parse(value);
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
        RD=LD=RU=LU.parse(res[i]);
      }else if (i==1){
        RD=LD.parse(res[i]);
      }else if (i==3){
        RD.parse(res[i]);
      }else{
        RU.parse(res[i]);
      }
    }
  }else if (param=="bg-image"){
    cv::Mat bgimage=cv::imread(value, cv::IMREAD_UNCHANGED);
    if (bgimage.data){
      hasBgImage=1;
      background=new cv::Mat(frame->size(), CV_8UC4);
      
      if (bgimage.channels()!=4){
        cv::Mat nbgimage(bgimage.size(), CV_MAKE_TYPE(bgimage.depth(), 4));
        int fromTo[] = { 0,0, 1,1, 2,2, -1,3 };
        cv::mixChannels(&bgimage,1,&nbgimage,1,fromTo, bgimage.channels());
        cv::resize(nbgimage, *background, frame->size());
        for (int x=0;x<frame->size().width;++x){
          for (int y=0;y<frame->size().height;++y){
            background->at<cv::Vec4b>(cv::Point(x, y))[3]=255;
          }
        }
      }else{
        cv::resize(bgimage, *background, frame->size());
      }
    }else std::cout << "Error in loading image '" << value << "'" << std::endl;
  }else if (param=="fpv"){
    setFPEV(key, value, verboseLevel);
  }
}

void Layer::readConfig(const char* configFile, int verboseLevel){
  if (verboseLevel) std::cout << "Layer::readConfig(" <<configFile << ")" << std::endl;
  std::vector<std::pair<std::pair<std::string, std::string>, std::string> > ans;
  configReader::readConfigFromFile(configFile, ans);
  for (auto& conf:ans) setConfigParam(conf.first.first, conf.first.second, conf.second, verboseLevel);
}
