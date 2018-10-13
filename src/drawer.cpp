#include <iostream>
#include <drawer.h>


void Drawer::setFPEV(std::string& key, std::string& value, int verboseLevel){
  if (verboseLevel>1) std::cout << "[I] Drawer::setFPEV " << this << "('" << key << "', '" << value << "')" << std::endl;
  int index=std::stoi(key);
  if (value.size()>12 && value.substr(0,12)=="track-values"){
    fpeTracks.push_back(std::vector<TrackController::Index>());
    tc->parseTrackIndices(value.substr(12,value.size()-12), fpeTracks.back());
    fpe.setIndex(index, 0+fpeTracks.size());
  }else if (value=="null"||value=="0") fpe.setIndex(index, 0);
  else if (verboseLevel) {
     std::cout << "[W] Drawer::setFPEV " << this << ", unknown FPV: '" << value << "'" << std::endl;
  }
}

void Drawer::updateFPE(int){
  if (!fpevs.size()){
    fpevs.resize(0+fpeTracks.size());
  }
  for (int i=0;i<(int)fpeTracks.size();++i){
    double sum=0;
    for (TrackController::Index& ix: fpeTracks[i]){
      sum+=ix.getValue();
    }
    fpevs[0+i]=sum;
  }
  fpe.updateValues(fpevs);
}




void Drawer::drawLine(cv::Point a, cv::Point b, cv::Scalar color, double thickness, cv::Mat* frame){
//   std::cout << "drawLine(<" << a.x << "," << a.y << ">, <" << b.x << "," << b.y << ">, <" << color[0] << "," << color[1] << "," << color[2] << "," << color[3] << ">, " << thickness << ", *frame)" << std::endl; 
  cv::line(*frame, a, b, color, thickness);
}
void Drawer::drawRectangle(cv::Point a, cv::Point b, cv::Point c, cv::Point d, cv::Scalar color, cv::Mat* frame){
//   std::cout << "drawRectangle(... <" << color[0] << "," << color[1] << "," << color[2] << "," << color[3] << ">, ..." << std::endl;
  while (a.x>b.x){swap(a, b);swap(b, c);swap(c, d);}
  while (a.x>d.x){swap(a, d);swap(d, c);swap(c, b);}
  if (b.y>d.y) swap(b, d);
  int bx=std::max(std::min(a.x, frame->cols), 0);
  int ex=std::max(std::min(c.x, frame->cols), 0);
  for (int x=bx;x<=ex;++x){
    int by, ey;
    if (x<b.x)  by=(b.x-a.x)?a.y+(b.y-a.y)*(x-a.x)/(b.x-a.x):std::min(a.y, b.y);
    else        by=(c.x-b.x)?b.y+(c.y-b.y)*(x-b.x)/(c.x-b.x):std::min(b.y, c.y);
    if (x<d.x)  ey=(d.x-a.x)?a.y+(d.y-a.y)*(x-a.x)/(d.x-a.x):std::max(a.y, d.y);
    else        ey=(c.x-d.x)?d.y+(c.y-d.y)*(x-d.x)/(c.x-d.x):std::max(d.y, c.y);
    by=std::max(std::min(by, frame->rows), 0);
    ey=std::max(std::min(ey, frame->rows), 0);
    for (int y=by;y<=ey;++y){
//       frame->at<cv::Vec4b>(cv::Point(x, y))=cv::Vec4b(color);
      cv::Scalar o=frame->at<cv::Vec4b>(cv::Point(x, y));
      int alpha=255-(255-color[3])*(255-o[3])/255/255;
      o=(color[3]*color+(255-color[3])*o)/255;
      frame->at<cv::Vec4b>(cv::Point(x, y))=cv::Vec4b(o[0], o[1], o[2], alpha);
//       int a1=wframeAlpha->at<cv::Vec3b>(cv::Point(x, y))[0];
//       int a2=fillColor[3];
    }
  }
}




void Drawer::update(int cframe){
  for (int i=0;i<(int)indices.size();++i){
    if (i==(int)values.size()) values.push_back(0);
    values[i]=indices[i].getValue();
  }
  updateFPE(cframe);
}


void Drawer::draw(int cframe, cv::Mat* frame, int verboseLevel){
  if (verboseLevel>1) std::cout << "[I] Drawer::draw " << this << "(" << cframe << ") " << std::endl;
  update(cframe);
  
  double x0d=x0.value();
  double y0d=y0.value();
  double x1d=x1.value();
  double y1d=y1.value();
  cv::Scalar lColor0=lineColor0.value();
  cv::Scalar lColor1=lineColor1.value();
  cv::Scalar fColor0=fillColor0.value();
  cv::Scalar fColor1=fillColor1.value();
  double vthickness=lineThickness.value();
  double wd=w.value();
  double hd=h.value();
  double dx=x1d-x0d;
  double dy=y1d-y0d;
  for (int i=0;i<(int)values.size();++i){
    double v=values[i];
    int x1i=x0d+i*dx/values.size()+(1-wd)/2*dx/values.size();
    int x2i=x0d+i*dx/values.size()+(1-wd)/2*dx/values.size()+wd*dx/values.size();
    int y1i=y0d+i*dy/values.size()+(1-wd)/2*dy/values.size();
    int y2i=y0d+i*dy/values.size()+(1-wd)/2*dy/values.size()+wd*dy/values.size();
    int avx=hd*v*dy/values.size();
    int avy=-hd*v*dx/values.size();
    cv::Point a=cv::Point(x1i, y1i);
    cv::Point b=cv::Point(x2i, y2i);
    cv::Point c=cv::Point(x2i+avx, y2i+avy);
    cv::Point d=cv::Point(x1i+avx, y1i+avy);
    
    double p=0;
    if (values.size()==1) p=1;
    else p=(double)i/(double)(values.size()-1);
    
    cv::Scalar color(255*(p*lColor0+(1-p)*lColor1));
    cv::Scalar fillColor(255*(p*fColor0+(1-p)*fColor1));
    
    drawRectangle(a, b, c, d, fillColor, frame);
    drawLine(a, b, color, vthickness, frame);
    drawLine(b, c, color, vthickness, frame);
    drawLine(c, d, color, vthickness, frame);
    drawLine(d, a, color, vthickness, frame);
  }
}



void Drawer::setParameter(std::string& param, std::string& key, std::string& value, int verboseLevel){
  if (verboseLevel>1) std::cout << "[I] Drawer::setParameter " << this << "('" << param << "', '" << key << "', '" << value << "') " << std::endl;
  if (param=="line-color-0") lineColor0.parse(value, verboseLevel);
  else if (param=="line-color-1") lineColor1.parse(value, verboseLevel);
  else if (param=="fill-color-0") fillColor0.parse(value, verboseLevel);
  else if (param=="fill-color-1") fillColor1.parse(value, verboseLevel);
  else if (param=="w") w.parse(value, verboseLevel);
  else if (param=="h") h.parse(value, verboseLevel);
  else if (param=="x0") x0.parse(value, verboseLevel);
  else if (param=="x1") x1.parse(value, verboseLevel);
  else if (param=="y0") y0.parse(value, verboseLevel);
  else if (param=="y1") y1.parse(value, verboseLevel);
  else if (param=="thickness") lineThickness.parse(value, verboseLevel);
  else if (param=="fpv") setFPEV(key, value, verboseLevel);
  else if (param=="i"){
    tc->parseTrackIndices(value, indices);
  }else if (verboseLevel){
    std::cout << "[W] Drawer::setParameter " << this << ", unknown parameter '" << param << "'" << std::endl;
  }
}

void Drawer::parse(std::string& config, int verboseLevel){
  if (verboseLevel>2) std::cout << "[X] Drawer::parse " << this << " '" << config << "'" << std::endl; 
  std::vector<std::pair<std::pair<std::string, std::string>, std::string> > ans;
  configReader::readConfig(config.c_str(), ans);
  for (auto conf:ans)setParameter(conf.first.first, conf.first.second, conf.second, verboseLevel);
}