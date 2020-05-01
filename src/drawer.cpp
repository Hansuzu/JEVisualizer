#include <log.h>
#include <drawer.h>
#include <drawingfunctions.h>

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
  double vthr=thr.value();
  for (int i=0;i<(int)indices.size();++i){
    if (i==(int)values.size()) values.push_back(0);
    values[i]=indices[i].getValue();
    if (values[i]<vthr) values[i]=0;
  }
  
  for (int ii=0;ii<flatten;++ii) {
    if (values.size()<2) break;
    double p=values[values.size()-1];
    double tt=values[0];
    values[0]=(values[0]+p+values[1])/3.0;
    p=tt;
    double t;
    for (int i=1;i<(int)values.size()-1;++i) {
      t=values[i];
      values[i]=(p+values[i]+values[i+1])/3.0;
      p=t;
    }
    values[values.size()-1]=(p+values[values.size()-1]+tt)/3.0;
  }
  
  updateFPE(cframe);
}


std::pair<double, double> Drawer::getSubDrawerDir(int i, double xScale, double yScale) {
  if (i>=(int)subDrawers.size()) i=0;
  if (i<0) i=subDrawers.size()-1;
  double x0d=xScale*(subDrawers[i]->x0.value());
  double y0d=yScale*(subDrawers[i]->y0.value());
  double x1d=xScale*(subDrawers[i]->x1.value());
  double y1d=yScale*(subDrawers[i]->y1.value());
  double dx=x1d-x0d;
  double dy=y1d-y0d;
  return {dx, dy};
}

void Drawer::drawSubDrawer(int cframe, cv::Mat* frame, double xScale, double yScale, int subDrawerIndex){
  if (globalSettings::verboseLevel>3) lout << "[4] Drawer::drawSubDrawer " << this << "(" << cframe << ") " << LEND;
  SubDrawer* sd=subDrawers[subDrawerIndex];
  
  //std::cout << "drawSubDrawer" << std::endl;
  double x0d=xScale*(sd->x0.value());
  double y0d=yScale*(sd->y0.value());
  double x1d=xScale*(sd->x1.value());
  double y1d=yScale*(sd->y1.value());
  
  cv::Scalar lColor0=sd->lineColor0.value();
  cv::Scalar lColor1=sd->lineColor1.value();
  cv::Scalar fColor0=sd->fillColor0.value();
  cv::Scalar fColor1=sd->fillColor1.value();
  double vthickness=sd->lineThickness.value();
  double wd=sd->w.value();
  double hd=sd->h.value();
  double dx=x1d-x0d;
  double dy=y1d-y0d;
  
  sd->polyCornerVs.resize(sd->polyCorners.size());
  for (int i=0;i<(int)sd->polyCorners.size();++i){
    sd->polyCornerVs[i]={sd->polyCorners[i].first->value(), sd->polyCorners[i].second->value()};
  }
  sd->polygonToDraw.resize(polyCorners.size());
  // TODO: index safety
  
  //std::cout << x0d << ", " << y0d << ", " << x1d << ", " << y1d << std::endl;
  
  for (int i=sd->i0;i<=sd->i1;++i){
    double v=values[i];
    double vali=1.0/(sd->i1-sd->i0+1); // TODO: index safety, this also
    int x1i=x0d+(i-sd->i0)*dx*vali+(1-wd)/2*dx*vali;
    int x2i=x0d+(i-sd->i0)*dx*vali+(1-wd)/2*dx*vali+wd*dx*vali;
    int y1i=y0d+(i-sd->i0)*dy*vali+(1-wd)/2*dy*vali;
    int y2i=y0d+(i-sd->i0)*dy*vali+(1-wd)/2*dy*vali+wd*dy*vali;
    int avx=hd*v*dy*vali;
    int avy=-hd*v*dx*vali;
    
    double p=0;
    if (values.size()==1) p=1;
    else p=(double)i/(double)(values.size()-1);
    
    cv::Scalar color(255*(p*lColor1+(1-p)*lColor0));
    cv::Scalar fillColor(255*(p*fColor1+(1-p)*fColor0));
    
    if (sd->columnType==Rectangle){
      cv::Point a(x1i, y1i);
      cv::Point b(x2i, y2i);
      cv::Point c(x2i+avx, y2i+avy);
      cv::Point d(x1i+avx, y1i+avy);
      drawingFunctions::drawRectangle(a, b, c, d, fillColor, frame);
      drawingFunctions::drawLine(a, b, color, vthickness, frame);
      drawingFunctions::drawLine(b, c, color, vthickness, frame);
      drawingFunctions::drawLine(c, d, color, vthickness, frame);
      drawingFunctions::drawLine(d, a, color, vthickness, frame);
    }else if (sd->columnType==Triangle){
      cv::Point a(x1i, y1i);
      cv::Point b(x2i, y2i);
      cv::Point c((x2i+x1i)/2+avx, (y2i+y1i)/2+avy);
      drawingFunctions::drawTriangle(a, b, c, fillColor, frame);
      drawingFunctions::drawLine(a, b, color, vthickness, frame);
      drawingFunctions::drawLine(b, c, color, vthickness, frame);
      drawingFunctions::drawLine(c, a, color, vthickness, frame);
    }else if (sd->columnType==Polygon){
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
    }else if (sd->columnType==Mid){
      double v2;
      if (i<(int)values.size()-1) v2=values[i+1];
      else v2=values[0];
      int avx2=hd*v2*dy*vali;
      int avy2=-hd*v2*dx*vali;
      if (i==sd->i0) {
        std::pair<double, double> t2=getSubDrawerDir(subDrawerIndex-1, xScale, yScale);
        double dxx=(dx+t2.first)/2.0;
        double dyy=(dy+t2.second)/2.0;
        avx=hd*v*dyy*vali;
        avy=-hd*v*dxx*vali;
      }
      if (i==sd->i1) {
        std::pair<double, double> t2=getSubDrawerDir(subDrawerIndex+1, xScale, yScale);
        double dxx=(dx+t2.first)/2.0;
        double dyy=(dy+t2.second)/2.0;
        avx2=hd*v2*dyy*vali;
        avy2=-hd*v2*dxx*vali;
      }
      
      //std::cout << x1i << ", " << x2i << ", " << y1i << ", " << y2i << std::endl;
      //std::cout << color << std::endl;
      
      cv::Point a(x1i, y1i);
      cv::Point b(x2i, y2i);
      cv::Point c(x2i+avx2, y2i+avy2);
      cv::Point d(x1i+avx, y1i+avy);
      //std::cout << a << ", " << b << std::endl;
      
      drawingFunctions::drawTriangle(a, b, c, fillColor, frame);
      drawingFunctions::drawTriangle(a, c, d, fillColor, frame);
      drawingFunctions::drawLine(a, b, color, vthickness, frame);
      drawingFunctions::drawLine(b, c, color, vthickness, frame);
      drawingFunctions::drawLine(c, d, color, vthickness, frame);
      drawingFunctions::drawLine(d, a, color, vthickness, frame);
    }
  }
}
  
void Drawer::draw(int cframe, cv::Mat* frame, double xScale, double yScale){
  if (globalSettings::verboseLevel>3) lout << "[4] Drawer::draw " << this << "(" << cframe << ") " << LEND;
  update(cframe);
  if (subDrawers.size()) {
    for (int i=0;i<(int)subDrawers.size();++i) {
      drawSubDrawer(cframe, frame, xScale, yScale, i);
    }
  } else {
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
      
      cv::Scalar color(255*(p*lColor1+(1-p)*lColor0));
      cv::Scalar fillColor(255*(p*fColor1+(1-p)*fColor0));
      
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
      }else if (columnType==Mid){
        double v2;
        if (i<(int)values.size()-1) v2=values[i+1];
        else v2=values[0];
        int avx2=hd*v2*dy*vali;
        int avy2=-hd*v2*dx*vali;
        
        cv::Point a(x1i, y1i);
        cv::Point b(x2i, y2i);
        cv::Point c(x2i+avx2, y2i+avy2);
        cv::Point d(x1i+avx, y1i+avy);
        drawingFunctions::drawTriangle(a, b, c, fillColor, frame);
        drawingFunctions::drawTriangle(a, c, d, fillColor, frame);
        drawingFunctions::drawLine(a, b, color, vthickness, frame);
        drawingFunctions::drawLine(b, c, color, vthickness, frame);
        drawingFunctions::drawLine(c, d, color, vthickness, frame);
        drawingFunctions::drawLine(d, a, color, vthickness, frame);
      }
    }
  }
}



void Drawer::setParameter(std::string& param, std::string& key, std::string& value) {
  if (globalSettings::verboseLevel>1) lout << "[I] Drawer::setParameter " << this << "('" << param << "', '" << key << "', '" << value << "') " << LEND;
  if (param=="line-color-0") lineColor0.parse(value);
  else if (param=="line-color-1") lineColor1.parse(value);
  else if (param=="fill-color-0") fillColor0.parse(value);
  else if (param=="fill-color-1") fillColor1.parse(value);
  else if (param=="column-type"){
    if (value=="RECTANGLE" || value=="0") columnType=Rectangle;
    else if (value=="TRIANGLE" || value=="1") columnType=Triangle;
    else if (value=="POLYGON" || value=="2") columnType=Polygon;
    else if (value=="MID" || value=="3") columnType=Mid;
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
  else if (param=="thr") thr.parse(value);
  else if (param=="w") w.parse(value);
  else if (param=="h") h.parse(value);
  else if (param=="x0") x0.parse(value);
  else if (param=="x1") x1.parse(value);
  else if (param=="y0") y0.parse(value);
  else if (param=="y1") y1.parse(value);
  else if (param=="thickness") lineThickness.parse(value);
  else if (param=="fpv") setFPEV(key, value);
  else if (param=="flatten") flatten=std::stoi(value);
  else if (param=="subdrawer") {
    // copy inherited values and then parse
    SubDrawer* t = new SubDrawer(&fpe);
    t->columnType=columnType;
    t->polyCorners.resize(polyCorners.size());
    for (int i=0;i<(int)polyCorners.size();++i) t->polyCorners[i]=polyCorners[i];
    t->polyCornerVs.resize(polyCornerVs.size());
    for (int i=0;i<(int)polyCornerVs.size();++i) t->polyCornerVs[i]=polyCornerVs[i];
    t->polygonToDraw.resize(polygonToDraw.size());
    for (int i=0;i<(int)polygonToDraw.size();++i) t->polygonToDraw[i]=polygonToDraw[i];
    t->x0=x0;t->x1=x1;
    t->y0=y0;t->y1=y1;
    t->lineColor0=lineColor0;
    t->lineColor1=lineColor1;
    t->fillColor0=fillColor0;
    t->fillColor1=fillColor1;
    t->w=w;t->h=h;
    t->lineThickness=lineThickness;
    
    t->parse(value);
    
    subDrawers.push_back(t);
  } else if (param=="i"){
    tc->parseTrackIndices(value, indices);
  }else if (globalSettings::verboseLevel){
    lout << "[W] Drawer::setParameter " << this << ", unknown parameter '" << param << "'" << LEND;
  }
}

void Drawer::parse(std::string& config) {
  if (globalSettings::verboseLevel>2) lout << "[X] Drawer::parse " << this << " '" << config << "'" << LEND; 
  std::vector<std::pair<std::pair<std::string, std::string>, std::string> > ans;
  configReader::readConfig(config.c_str(), ans);
  for (auto conf:ans)setParameter(conf.first.first, conf.first.second, conf.second);
}


void Drawer::SubDrawer::setParameter(std::string& param, std::string& key, std::string& value) {
  if (globalSettings::verboseLevel>1) lout << "[I] Drawer::SubDrawer::setParameter " << this << "('" << param << "', '" << key << "', '" << value << "') " << LEND;
  if (param=="line-color-0") lineColor0.parse(value);
  else if (param=="line-color-1") lineColor1.parse(value);
  else if (param=="fill-color-0") fillColor0.parse(value);
  else if (param=="fill-color-1") fillColor1.parse(value);
  else if (param=="column-type"){
    if (value=="RECTANGLE" || value=="0") columnType=Rectangle;
    else if (value=="TRIANGLE" || value=="1") columnType=Triangle;
    else if (value=="POLYGON" || value=="2") columnType=Polygon;
    else if (value=="MID" || value=="3") columnType=Mid;
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
        Formula* x = new Formula(pfpe);
        x->parse(xy[0]);
        Formula* y = new Formula(pfpe);
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
  else if (param=="i0") i0=std::stoi(value);
  else if (param=="i1") i1=std::stoi(value);
  else if (globalSettings::verboseLevel){
    lout << "[W] Drawer::setParameter " << this << ", unknown parameter '" << param << "'" << LEND;
  }
}

void Drawer::SubDrawer::parse(std::string& config) {
  if (globalSettings::verboseLevel>2) lout << "[X] Drawer::SubDrawer::parse " << this << " '" << config << "'" << LEND; 
  std::vector<std::pair<std::pair<std::string, std::string>, std::string> > ans;
  configReader::readConfig(config.c_str(), ans);
  for (auto conf:ans)setParameter(conf.first.first, conf.first.second, conf.second);
}