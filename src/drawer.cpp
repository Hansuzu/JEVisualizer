#include <log.h>
#include <drawer.h>
#include <drawingfunctions.cpp>

void Drawer::setFPEV(std::string& key, std::string& value){
  if (globalSettings::verboseLevel>1) lout << "[I] Drawer::setFPEV " << this << "('" << key << "', '" << value << "')" << LEND;
  int index=std::stoi(key);
  if (value.size()>12 && value.substr(0,12)=="track-values"){
    fpeTracks.push_back(std::vector<TrackController::Index>());
    tc->parseTrackIndices(value.substr(12,value.size()-12), fpeTracks.back());
    fpe.setIndex(index, 0+fpeTracks.size());
  }else if (value=="null"||value=="0") fpe.setIndex(index, 0);
  else if (globalSettings::verboseLevel) {
     lout << "[W] Drawer::setFPEV " << this << ", unknown FPV: '" << value << "'" << LEND;
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






void Drawer::update(int cframe){
  for (int i=0;i<(int)indices.size();++i){
    if (i==(int)values.size()) values.push_back(0);
    values[i]=indices[i].getValue();
  }
  updateFPE(cframe);
}


void Drawer::draw(int cframe, cv::Mat* frame, double xScale, double yScale){
  if (globalSettings::verboseLevel>3) lout << "[4] Drawer::draw " << this << "(" << cframe << ") " << LEND;
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
      drawingFunctions::drawRectangle(a, b, c, d, fillColor, frame);
      drawingFunctions::drawLine(a, b, color, vthickness, frame);
      drawingFunctions::drawLine(b, c, color, vthickness, frame);
      drawingFunctions::drawLine(c, d, color, vthickness, frame);
      drawingFunctions::drawLine(d, a, color, vthickness, frame);
    }else if (columnType==Triangle){
      cv::Point a(x1i, y1i);
      cv::Point b(x2i, y2i);
      cv::Point c((x2i+x1i)/2+avx, (y2i+y1i)/2+avy);
      drawingFunctions::drawTriangle(a, b, c, fillColor, frame);
      drawingFunctions::drawLine(a, b, color, vthickness, frame);
      drawingFunctions::drawLine(b, c, color, vthickness, frame);
      drawingFunctions::drawLine(c, a, color, vthickness, frame);
    }else if (columnType==Polygon){
      cv::Point a(x1i, y1i);
      cv::Point b(x2i, y2i);
      cv::Point d(x1i+avx, y1i+avy);
      cv::Point x(b-a);
      cv::Point y(d-a);
      for (int j=0;j<(int)polyCornerVs.size();++j){
        polygonToDraw[j]=a+x*polyCornerVs[j].first+y*polyCornerVs[j].second;
      }
      drawingFunctions::drawPolygon(polygonToDraw, fillColor, frame);
      for (int j=0;j<(int)polygonToDraw.size()-1;++j) drawingFunctions::drawLine(polygonToDraw[j], polygonToDraw[j+1], color, vthickness, frame);
      if (polygonToDraw.size()>2) drawingFunctions::drawLine(polygonToDraw[0], polygonToDraw[polygonToDraw.size()-1], color, vthickness, frame);
    }
  }
}



void Drawer::setParameter(std::string& param, std::string& key, std::string& value){
  if (globalSettings::verboseLevel>1) lout << "[I] Drawer::setParameter " << this << "('" << param << "', '" << key << "', '" << value << "') " << LEND;
  if (param=="line-color-0") lineColor0.parse(value);
  else if (param=="line-color-1") lineColor1.parse(value);
  else if (param=="fill-color-0") fillColor0.parse(value);
  else if (param=="fill-color-1") fillColor1.parse(value);
  else if (param=="column-type"){
    if (value=="RECTANGLE" || value=="0") columnType=Rectangle;
    else if (value=="TRIANGLE" || value=="1") columnType=Triangle;
    else if (value=="POLYGON" || value=="2") columnType=Polygon;
    else if (globalSettings::verboseLevel){
      lout << "[W] Drawer::setParameter " << this << ", unknown column-type '" << value << "'" << LEND;
    }
  }else if (param=="shape"){
    std::vector<std::string> res;
    Formula::split(value, res);
    if (res.size()<3 && globalSettings::verboseLevel){
        lout << "[W] Drawer::setParameter " << this << ", problem in shape, less than 3 points '" << value << "'" << LEND;
    }
    for (auto point : res){
      std::vector<std::string> xy;
      Formula::split(point, xy);
      if (xy.size()!=2 && globalSettings::verboseLevel){
        lout << "[W] Drawer::setParameter " << this << ", problem in shape, invalid point '" << point << "'" << LEND;
      }else{
        Formula* x = new Formula(&fpe);
        x->parse(xy[0]);
        Formula* y = new Formula(&fpe);
        y->parse(xy[1]);
        polyCorners.push_back({x, y});
      }
    }
  }
  else if (param=="w") w.parse(value);
  else if (param=="h") h.parse(value);
  else if (param=="x0") x0.parse(value);
  else if (param=="x1") x1.parse(value);
  else if (param=="y0") y0.parse(value);
  else if (param=="y1") y1.parse(value);
  else if (param=="thickness") lineThickness.parse(value);
  else if (param=="fpv") setFPEV(key, value);
  else if (param=="i"){
    tc->parseTrackIndices(value, indices);
  }else if (globalSettings::verboseLevel){
    lout << "[W] Drawer::setParameter " << this << ", unknown parameter '" << param << "'" << LEND;
  }
}

void Drawer::parse(std::string& config){
  if (globalSettings::verboseLevel>2) lout << "[X] Drawer::parse " << this << " '" << config << "'" << LEND; 
  std::vector<std::pair<std::pair<std::string, std::string>, std::string> > ans;
  configReader::readConfig(config.c_str(), ans);
  for (auto conf:ans)setParameter(conf.first.first, conf.first.second, conf.second);
}