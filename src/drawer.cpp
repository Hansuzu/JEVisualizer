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




void Drawer::drawLine(cv::Point& a, cv::Point& b, cv::Scalar& color, double thickness, cv::Mat* frame, int verboseLevel){
  if (verboseLevel>3) std::cout << "[4] Drawer::drawLine " << this << "(<" << a.x << "," << a.y << ">, <" << b.x << "," << b.y << ">, <" << color[0] << "," << color[1] << "," << color[2] << "," << color[3] << ">, " << thickness << ", *frame)" << std::endl; 
  cv::line(*frame, a, b, color, thickness);
}
void Drawer::drawRectangle(cv::Point& a, cv::Point& b, cv::Point& c, cv::Point& d, cv::Scalar& color, cv::Mat* frame, int verboseLevel){
  if (verboseLevel>3) std::cout << "[4] Drawer::drawRectangle " << this << "(... <" << color[0] << "," << color[1] << "," << color[2] << "," << color[3] << ">, ..." << std::endl;
  while (a.y>b.y){swap(a, b);swap(b, c);swap(c, d);}
  while (a.y>d.y){swap(a, d);swap(d, c);swap(c, b);}
  if (b.x>d.x) swap(b, d);
  int by=std::max(std::min(a.y, frame->rows-1), 0);
  int ey=std::max(std::min(c.y, frame->rows-1), 0);
  uchar* p;
  int bx, ex;
  for (int y=by;y<=ey;++y){
    if (y<b.y)  bx=(b.y-a.y)?a.x+(b.x-a.x)*(y-a.y)/(b.y-a.y):std::min(a.x, b.x);
    else        bx=(c.y-b.y)?b.x+(c.x-b.x)*(y-b.y)/(c.y-b.y):std::min(b.x, c.x);
    if (y<d.y)  ex=(d.y-a.y)?a.x+(d.x-a.x)*(y-a.y)/(d.y-a.y):std::max(a.x, d.x);
    else        ex=(c.y-d.y)?d.x+(c.x-d.x)*(y-d.y)/(c.y-d.y):std::max(d.x, c.x);
    bx=std::max(std::min(bx, frame->cols-1), 0);
    ex=std::max(std::min(ex, frame->cols-1), 0);
    p=frame->ptr<uchar>(y);
    for (int x=bx;x<=ex;++x){
      p[4*x+0]=(color[3]*color[0]+(255-color[3])*p[4*x+0])/255;
      p[4*x+1]=(color[3]*color[1]+(255-color[3])*p[4*x+1])/255;
      p[4*x+2]=(color[3]*color[2]+(255-color[3])*p[4*x+2])/255;
      p[4*x+3]=255-(255-p[4*x+3])*(255-color[3])/255;
    }
  }
}
void Drawer::drawTriangle(cv::Point& a, cv::Point& b, cv::Point& c, cv::Scalar& color, cv::Mat* frame, int verboseLevel){
  if (verboseLevel>3) std::cout << "[4] Drawer::drawTriangle " << this << "(... <" << color[0] << "," << color[1] << "," << color[2] << "," << color[3] << ">, ..." << std::endl;
  if (a.y>b.y) swap(a, b);
  if (a.y>c.y) swap(a, c);
  if (b.y>c.y) swap(b, c);
  
  int by=std::max(std::min(a.y, frame->rows-1), 0);
  int ey=std::max(std::min(c.y, frame->rows-1), 0);
  uchar* p;
  int bx, ex;
  for (int y=by;y<=ey;++y){
    if (y<b.y)  bx=(b.y-a.y)?a.x+(b.x-a.x)*(y-a.y)/(b.y-a.y):std::min(a.x, b.x);
    else        bx=(c.y-b.y)?b.x+(c.x-b.x)*(y-b.y)/(c.y-b.y):std::min(b.x, c.x);
    ex=(c.y-a.y)?a.x+(c.x-a.x)*(y-a.y)/(c.y-a.y):std::max(a.x, c.x);
    
    bx=std::max(std::min(bx, frame->cols-1), 0);
    ex=std::max(std::min(ex, frame->cols-1), 0);
    if (bx>ex) std::swap(bx, ex);
    
    p=frame->ptr<uchar>(y);
    for (int x=bx;x<=ex;++x){
      p[4*x+0]=(color[3]*color[0]+(255-color[3])*p[4*x+0])/255;
      p[4*x+1]=(color[3]*color[1]+(255-color[3])*p[4*x+1])/255;
      p[4*x+2]=(color[3]*color[2]+(255-color[3])*p[4*x+2])/255;
      p[4*x+3]=255-(255-p[4*x+3])*(255-color[3])/255;
    }
  }
}
void Drawer::drawPolygon(std::vector<cv::Point>& pts, cv::Scalar& color, cv::Mat* frame, int verboseLevel){
  if (verboseLevel>3) std::cout << "[4] Drawer::drawPolygon " << this << "(...[" << pts.size() << "], <" << color[0] << "," << color[1] << "," << color[2] << "," << color[3] << ">, ..." << std::endl;
  for (int i=0;i+2<(int)pts.size();++i){
    drawTriangle(pts[i], pts[i+1], pts[i+2], color, frame, verboseLevel);
  }
}



void Drawer::update(int cframe){
  for (int i=0;i<(int)indices.size();++i){
    if (i==(int)values.size()) values.push_back(0);
    values[i]=indices[i].getValue();
  }
  updateFPE(cframe);
}


void Drawer::draw(int cframe, cv::Mat* frame, double xScale, double yScale, int verboseLevel){
  if (verboseLevel>3) std::cout << "[4] Drawer::draw " << this << "(" << cframe << ") " << std::endl;
  update(cframe);
  
  double x0d=xScale*x0.value();
  double y0d=yScale*y0.value();
  double x1d=xScale*x1.value();
  double y1d=yScale*y1.value();
  
  cv::Scalar lColor0=lineColor0.value();
  cv::Scalar lColor1=lineColor1.value();
  cv::Scalar fColor0=fillColor0.value();
  cv::Scalar fColor1=fillColor1.value();
  double vthickness=lineThickness.value();
  double wd=w.value();
  double hd=h.value();
  double dx=x1d-x0d;
  double dy=y1d-y0d;
  
  polyCornerVs.resize(polyCorners.size());
  for (int i=0;i<(int)polyCorners.size();++i){
    polyCornerVs[i]={polyCorners[i].first->value(), polyCorners[i].second->value()};
  }
  polygonToDraw.resize(polyCorners.size());
  
  for (int i=0;i<(int)values.size();++i){
    double v=values[i];
    double vali=1.0/values.size();
    int x1i=x0d+i*dx*vali+(1-wd)/2*dx*vali;
    int x2i=x0d+i*dx*vali+(1-wd)/2*dx*vali+wd*dx*vali;
    int y1i=y0d+i*dy*vali+(1-wd)/2*dy*vali;
    int y2i=y0d+i*dy*vali+(1-wd)/2*dy*vali+wd*dy*vali;
    int avx=hd*v*dy*vali;
    int avy=-hd*v*dx*vali;
    
    double p=0;
    if (values.size()==1) p=1;
    else p=(double)i/(double)(values.size()-1);
    
    cv::Scalar color(255*(p*lColor0+(1-p)*lColor1));
    cv::Scalar fillColor(255*(p*fColor0+(1-p)*fColor1));
    
    if (columnType==Rectangle){
      cv::Point a(x1i, y1i);
      cv::Point b(x2i, y2i);
      cv::Point c(x2i+avx, y2i+avy);
      cv::Point d(x1i+avx, y1i+avy);
      drawRectangle(a, b, c, d, fillColor, frame, verboseLevel);
      drawLine(a, b, color, vthickness, frame, verboseLevel);
      drawLine(b, c, color, vthickness, frame, verboseLevel);
      drawLine(c, d, color, vthickness, frame, verboseLevel);
      drawLine(d, a, color, vthickness, frame, verboseLevel);
    }else if (columnType==Triangle){
      cv::Point a(x1i, y1i);
      cv::Point b(x2i, y2i);
      cv::Point c((x2i+x1i)/2+avx, (y2i+y1i)/2+avy);
      drawTriangle(a, b, a, fillColor, frame, verboseLevel);
      drawLine(a, b, color, vthickness, frame, verboseLevel);
      drawLine(b, c, color, vthickness, frame, verboseLevel);
      drawLine(c, a, color, vthickness, frame, verboseLevel);
    }else if (columnType==Polygon){
      cv::Point a(x1i, y1i);
      cv::Point b(x2i, y2i);
      cv::Point d(x1i+avx, y1i+avy);
      cv::Point x(b-a);
      cv::Point y(d-a);
      for (int j=0;j<(int)polyCornerVs.size();++j){
        polygonToDraw[j]=a+x*polyCornerVs[j].first+y*polyCornerVs[j].second;
      }
      drawPolygon(polygonToDraw, fillColor, frame, verboseLevel);
      for (int j=0;j<(int)polygonToDraw.size()-1;++j) drawLine(polygonToDraw[j], polygonToDraw[j+1], color, vthickness, frame, verboseLevel);
      if (polygonToDraw.size()>2) drawLine(polygonToDraw[0], polygonToDraw[polygonToDraw.size()-1], color, vthickness, frame, verboseLevel);
    }
  }
}



void Drawer::setParameter(std::string& param, std::string& key, std::string& value, int verboseLevel){
  if (verboseLevel>1) std::cout << "[I] Drawer::setParameter " << this << "('" << param << "', '" << key << "', '" << value << "') " << std::endl;
  if (param=="line-color-0") lineColor0.parse(value, verboseLevel);
  else if (param=="line-color-1") lineColor1.parse(value, verboseLevel);
  else if (param=="fill-color-0") fillColor0.parse(value, verboseLevel);
  else if (param=="fill-color-1") fillColor1.parse(value, verboseLevel);
  else if (param=="column-type"){
    if (value=="RECTANGLE" || value=="0") columnType=Rectangle;
    else if (value=="TRIANGLE" || value=="0") columnType=Triangle;
    else if (value=="POLYGON" || value=="0") columnType=Polygon;
    else if (verboseLevel){
      std::cout << "[W] Drawer::setParameter " << this << ", unknown column-type '" << value << "'" << std::endl;
    }
  }else if (param=="shape"){
    std::vector<std::string> res;
    Formula::split(value, res);
    if (res.size()<3 && verboseLevel){
        std::cout << "[W] Drawer::setParameter " << this << ", problem in shape, less than 3 points '" << value << "'" << std::endl;
    }
    for (auto point : res){
      std::vector<std::string> xy;
      Formula::split(point, xy);
      if (xy.size()!=2 && verboseLevel){
        std::cout << "[W] Drawer::setParameter " << this << ", problem in shape, invalid point '" << point << "'" << std::endl;
      }else{
        Formula* x = new Formula(&fpe);
        x->parse(xy[0], verboseLevel);
        Formula* y = new Formula(&fpe);
        y->parse(xy[1], verboseLevel);
        polyCorners.push_back({x, y});
      }
    }
  }
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