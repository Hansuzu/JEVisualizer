#include <layer.h>
#include <config.h>
#include <iostream>

double Layer::Formula::value(int frame, std::vector<double>& values){
  if (isConstant){
    return c;
  }else{
    double ans=0;
    if (C) ans+=C->value(frame, values);
    if (frameK) ans+=frame*frameK->value(frame, values);
    for (int i=0;i<(int)std::min(values.size(), variables.size());++i){
      ans+=values[i]*variables[i]->value(frame, values);
    }
    if (sinK && inSin) ans+=sinK->value(frame, values)*sin(inSin->value(frame, values));
    if (ans<minV) ans=minV;
    if (ans>maxV) ans=maxV;
    return ans;
  }
}
void Layer::Formula::split(std::string& s, std::vector<std::string>& res){
  std::string prev="";
  int d=0;
  for (int i=0;i<(int)s.size();++i){
    if (s[i]==')' || s[i]==']')--d;
    if (d && s[i]!=' ' && s[i]!='\t' && !(d==1 && s[i]==',')) prev.push_back(s[i]);
    if (s[i]=='(' || s[i]=='[')++d;
    if (d==1 && s[i]==','){
      res.push_back("");
      std::swap(res.back(), prev);
    }
  }
  if (prev.size()){
    res.push_back("");
    std::swap(res.back(), prev);
  }
//   std::cout << "Visualizer::Formula::split(" << s << ") =>" << std::endl;
//   for (int i=0;i<(int)res.size();++i) std::cout << " " << res[i] << std::endl;
//   std::cout << std::endl;
}

void Layer::Formula::parse(std::string& s){
//   cout << "Visualizer::Formula::parse(" << s << ")" << endl;
  if (s.size()==0) return;
  bool br=0;
  for (int i=0;i<(int)s.size();++i) if (s[i]=='('){br=1; break;}
  if (br){
    isConstant=0;
    std::vector<std::string> params;
    split(s, params);
    for (int i=0;i<(int)params.size();++i){
      if (i==0){
        C=new Formula();
        C->parse(params[i]);
      }else if (i==1){
        frameK=new Formula();
        frameK->parse(params[i]);
      }else if (i==2){
        std::vector<std::string> array;
        split(params[i], array);
        for (int j=0;j<(int)array.size();++j){
          variables.push_back(new Formula());
          variables.back()->parse(array[j]);
        }
      }else if (i==3){
        sinK=new Formula();
        sinK->parse(params[i]);
      }else if (i==4){
        inSin=new Formula();
        inSin->parse(params[i]);
      }else if (i==5){
        minV=std::stof(params[i]);
      }else if (i==6){
        maxV=std::stof(params[i]);
      }
    }
  }else{
    c=std::stof(s);
  }
}


void Layer::Drawer::setParameter(std::string& param, std::string& value){
  if (param=="line-color-0") lineColor0.parse(value);
  if (param=="line-color-1") lineColor1.parse(value);
  if (param=="fill-color-0") fillColor0.parse(value);
  if (param=="fill-color-1") fillColor1.parse(value);
  if (param=="w") w.parse(value);
  if (param=="h") h.parse(value);
  if (param=="x0") x0.parse(value);
  if (param=="x1") x1.parse(value);
  if (param=="y0") y0.parse(value);
  if (param=="y1") y1.parse(value);
  if (param=="thickness") lineThickness.parse(value);
  if (param=="i"){
    int a=0;
    int b=0;
    bool mn=0;
    for (int i=0;i<(int)value.size();++i){
      if ('0'<=value[i] && value[i]<='9'){
        if (!mn)a=a*10+value[i]-'0';
        else    b=b*10+value[i]-'0';
      }else if (value[i]=='-') mn=1;
      else if (value[i]==','){
        if (mn){
          if (a<=b) for (int j=a;j<=b;++j) indices.push_back({0,j});
          else      for (int j=a;j>=b;--j) indices.push_back({0,j});
        }
        else indices.push_back({0,a});
        a=0;mn=0;
      }
    }
    if (mn){
      if (a<=b) for (int j=a;j<=b;++j) indices.push_back({0,j});
      else      for (int j=a;j>=b;--j) indices.push_back({0,j});
    }
    else indices.push_back({0,a});
  }
}

void Layer::Drawer::parse(std::string& config){
  std::vector<std::pair<std::pair<std::string, std::string>, std::string> > ans;
  configReader::readConfig(config.c_str(), ans);
  for (auto conf:ans)setParameter(conf.first.first, conf.second);
}

void Layer::Drawer::updateValues(std::vector<std::vector<double>* >& pValues){
  for (int i=0;i<(int)indices.size();++i){
    if (i==(int)values.size()) values.push_back(0);
    int i1=indices[i].first;
    int i2=indices[i].second;
    if (i1>=0 && i1<(int)pValues.size() && i1>=0 && i2<(int)pValues[i1]->size()){
      values[i]=(*pValues[i1])[i2];
    }else{
      values[i]=0;
    }
  }
  for (int i=0;i<(int)formula_value_indices.size();++i){
    if (i==(int)formula_values.size()) formula_values.push_back(0);
    formula_values[i]=0;
    for (std::pair<int, int>& ii:formula_value_indices[i]){
      if (ii.first>=0 && ii.first<(int)pValues.size() && ii.second>=0 && ii.second<(int)pValues[ii.first]->size()){
        formula_values[i]+=(*(pValues[ii.first]))[ii.second];
      }
    }
  }
}
void Layer::Drawer::drawLine(cv::Point a, cv::Point b, cv::Scalar color, double thickness, cv::Mat* frame){
//   std::cout << "drawLine(<" << a.x << "," << a.y << ">, <" << b.x << "," << b.y << ">, <" << color[0] << "," << color[1] << "," << color[2] << "," << color[3] << ">, " << thickness << ", *frame)" << std::endl; 
  cv::line(*frame, a, b, color, thickness);
}
void Layer::Drawer::drawRectangle(cv::Point a, cv::Point b, cv::Point c, cv::Point d, cv::Scalar color, cv::Mat* frame){
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
void Layer::Drawer::draw(int cframe, cv::Mat* frame){
//   std::cout << "Visualizer::Drawer::draw(" << cframe << ", *frame)" << std::endl;
  double x0d=x0.value(cframe, formula_values);
  double y0d=y0.value(cframe, formula_values);
  double x1d=x1.value(cframe, formula_values);
  double y1d=y1.value(cframe, formula_values);
  cv::Scalar lColor0=lineColor0.value(cframe, formula_values);
  cv::Scalar lColor1=lineColor1.value(cframe, formula_values);
  cv::Scalar fColor0=fillColor0.value(cframe, formula_values);
  cv::Scalar fColor1=fillColor1.value(cframe, formula_values);
  double vthickness=lineThickness.value(cframe, formula_values);
  double wd=w.value(cframe, formula_values);
  double hd=h.value(cframe, formula_values);
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
//     cv::line(*wframeAlpha, a, b, cv::Scalar(255,0,0), vthickness);
//     cv::line(*wframeAlpha, b, c, cv::Scalar(255,0,0), vthickness);
//     cv::line(*wframeAlpha, c, d, cv::Scalar(255,0,0), vthickness);
//     cv::line(*wframeAlpha, d, a, cv::Scalar(255,0,0), vthickness);
  }
}


void Layer::Filter::setConfigParam(std::string& param, std::string& key, std::string& value, int verboseLevel){
  if (verboseLevel) std::cout << "Layer::Filter::setConfigParam(" << param << ", " << key << ", " << value << ")" << std::endl;
}

void Layer::Filter::loadConfig(std::string& config, int verboseLevel){
  std::vector<std::pair<std::pair<std::string, std::string>, std::string> > ans;
  configReader::readConfig(config, ans);
  for (auto conf:ans) setConfigParam(conf.first.first, conf.first.second, conf.second, verboseLevel);
}
void Layer::Filter::apply(int, std::vector<std::vector<double>* >&, cv::Mat*){
}

void Layer::drawDrawers(int cframe, std::vector<std::vector<double>* >& trackValues, int verboseLevel){
  if (verboseLevel>1) std::cout << "Layer::drawDrawers" << std::endl;
  for (int i=0;i<(int)drawers.size();++i){
    drawers[i]->updateValues(trackValues);
    drawers[i]->draw(cframe, frame);
  }
}

bool Layer::isVisible(int cframe,  std::vector<std::vector<double>* >&){
  return cframe>=firstFrame && cframe<=lastFrame;
}

void Layer::draw(int cframe, std::vector<std::vector<double>* >& trackValues, cv::Mat* oframe, int verboseLevel){
  if (verboseLevel>1) std::cout << "Layer::draw(" << cframe << ")" << std::endl;
  if (isVisible(cframe, trackValues)){
    int w=oframe->size().width;
    int h=oframe->size().height;
    //Draw layer
    if (hasBgColor && !hasBgImage){ //TODO: bgcolor && bgimage?
      std::vector<double> pp; //TODO: pp
      cv::Scalar vLU=LU.value(cframe, pp);
      cv::Scalar vLD=LD.value(cframe, pp);
      cv::Scalar vRU=RU.value(cframe, pp);
      cv::Scalar vRD=RD.value(cframe, pp);
      for (int x=0;x<w;++x){
        cv::Scalar U=(vLU*(w-x)+vRU*x)/w;
        cv::Scalar D=(vLD*(w-x)+vRD*x)/w;
        for (int y=0;y<h;++y){
          cv::Scalar color=(U*(h-y)+D*y)/h;
          cv::Vec4b c(color[0], color[1], color[2], color[3]);
          frame->at<cv::Vec4b>(cv::Point(x, y))=c;
        }
      }
    }
    if (hasBgImage){
      std::cout << "hasBgImage" << std::endl;
      background->copyTo(*frame);
    }
    if (!hasBgColor && !hasBgImage) { //Init layer
      *frame=cv::Scalar(0,0,0,0);
    }
    
    drawDrawers(cframe, trackValues, verboseLevel);
    
    //Apply layer filters
    for (int i=0;i<(int)layerFilters.size();++i){
      layerFilters[i]->apply(cframe, trackValues, frame);
    }
    //Draw layer to image
    for (int y=0;y<h;++y){
      for (int x=0;x<w;++x){
          cv::Scalar a=frame->at<cv::Vec4b>(cv::Point(x, y));
          cv::Scalar o=oframe->at<cv::Vec3b>(cv::Point(x, y));
          o=(a[3]*a+(255-a[3])*o)/255;
          oframe->at<cv::Vec3b>(cv::Point(x, y))=cv::Vec3b(o[0], o[1], o[2]);
      }
    }
    //Apply image filters
    for (int i=0;i<(int)imageFilters.size();++i){
      imageFilters[i]->apply(cframe, trackValues, oframe);
    }
    
    
  //   background->copyTo(*oframe);
  //   background->copyTo(*help);
    
  //   *helpAlpha=cv::Scalar(0,0,0);

//     drawDrawers(cframe, trackValues, verboseLevel);
    
    
    /*
    std::vector<double> e;
    int glowr=glowR.value(cframe, e);
    int glowMax=255*glowMaxAlpha.value(cframe, e);
    int vgr=255*glowColorR.value(cframe, e);
    int vgg=255*glowColorG.value(cframe, e);
    int vgb=255*glowColorB.value(cframe, e);
    if (glowr){
      cv::GaussianBlur(*help, *helpBlur, cv::Size((glowr<<1)|1,(glowr<<1)|1), 0, 0);
      cv::GaussianBlur(*helpAlpha, *helpBlurAlpha, cv::Size((glowr<<1)|1, (glowr<<1)|1), 0, 0);
      for (int y=0;y<h;++y){
        for (int x=0;x<w;++x){
            cv::Scalar a=helpBlur->at<cv::Vec3b>(cv::Point(x, y));
            long double t=255/(std::max(a[0], std::max(a[1], a[2]))+1);
            if (glowType==0){
              a[0]=(int)(t*(a[0]+1));
              a[1]=(int)(t*(a[1]+1));
              a[2]=(int)(t*(a[2]+1));
            }if (glowType==1){
              a=cv::Vec3b(vgb, vgg, vgr);
            }
            a[4]=helpBlurAlpha->at<cv::Vec3b>(cv::Point(x, y))[0];
            if (a[4]>glowMax)a[4]=glowMax;
            if (glowMax>255) a[4]=std::min((int)(a[4]*glowMax/255), 255);
            
            if (helpAlpha->at<cv::Vec3b>(cv::Point(x,y))[0]>178) a[4]=0;
            cv::Scalar o=oframe->at<cv::Vec3b>(cv::Point(x, y));
            o=(a*a[4]+o*(255-a[4]))/255;
            oframe->at<cv::Vec3b>(cv::Point(x, y))=cv::Vec3b(o[0], o[1], o[2]);
        }
      }
    }
    
    for (int y=0;y<h;++y){
      for (int x=0;x<w;++x){
          cv::Scalar a=help->at<cv::Vec3b>(cv::Point(x, y));
          a[4]=helpAlpha->at<cv::Vec3b>(cv::Point(x, y))[0];
          cv::Scalar o=oframe->at<cv::Vec3b>(cv::Point(x, y));
          o=(a*a[4]+o*(255-a[4]))/255;
          oframe->at<cv::Vec3b>(cv::Point(x, y))=cv::Vec3b(o[0], o[1], o[2]);
      }
    }
    */
  }
}

void Layer::setConfigParam(std::string& param, std::string& key, std::string& value, int verboseLevel){
  if (verboseLevel) std::cout << "Layer::setConfigParam(" << param << ", " << key << ", " << value << ")" << std::endl;
  if (param=="w" || param=="width"){
    layerWidth=std::stof(value);
  }else if (param=="h" || param=="height"){
    layerHeight=std::stof(value);
  }else if (param=="first-frame"){
    firstFrame=std::stoi(value);
  }else if (param=="last-frame"){
    lastFrame=std::stoi(value);
  }else if (param=="drawer"){
    Drawer* k=new Drawer();
    drawers.push_back(k);
    k->parse(value);
  }else if (param=="filter"){
    layerFilters.push_back(new Filter());
    layerFilters.back()->loadConfig(value, verboseLevel);
  }else if (param=="image-filter"){
    imageFilters.push_back(new Filter());
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
  }
}

void Layer::readConfig(const char* configFile, int verboseLevel){
  if (verboseLevel) std::cout << "Layer::readConfig(" <<configFile << ")" << std::endl;
  std::vector<std::pair<std::pair<std::string, std::string>, std::string> > ans;
  configReader::readConfigFromFile(configFile, ans);
  for (auto conf:ans) setConfigParam(conf.first.first, conf.first.second, conf.second, verboseLevel);
}
