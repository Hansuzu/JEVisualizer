#include <drawingfunctions.h>
#include <log.h>


namespace drawingFunctions{
  void drawLine(cv::Point& a, cv::Point& b, cv::Scalar& color, double thickness, cv::Mat* frame){
    if (globalSettings::verboseLevel>3) lout << "[4] drawingFunctions::drawLine (<" << a.x << "," << a.y << ">, <" << b.x << "," << b.y << ">, <" << color[0] << "," << color[1] << "," << color[2] << "," << color[3] << ">, " << thickness << ", *frame)" << LEND; 
    // TODO: implement own draw line function
    cv::line(*frame, a, b, color, thickness);
  }

  void drawRectangle(cv::Point& a, cv::Point& b, cv::Point& c, cv::Point& d, cv::Scalar& color, cv::Mat* frame){
    if (globalSettings::verboseLevel>3) lout << "[4] drawingFunctions::drawRectangle (... <" << color[0] << "," << color[1] << "," << color[2] << "," << color[3] << ">, ..." << LEND;
    while (a.y>b.y){swap(a, b);swap(b, c);swap(c, d);}
    while (a.y>d.y){swap(a, d);swap(d, c);swap(c, b);}
    if (b.x>d.x) swap(b, d);
    int by=std::max(std::min(a.y, frame->rows-1), 0);
    int ey=std::max(std::min(c.y, frame->rows-1), 0);
    uchar* p;
    int bx, ex;
    double ac;
    for (int y=by;y<=ey;++y){
      if (y<b.y)  bx=(b.y-a.y)?a.x+(b.x-a.x)*(y-a.y)/(b.y-a.y):std::min(a.x, b.x);
      else        bx=(c.y-b.y)?b.x+(c.x-b.x)*(y-b.y)/(c.y-b.y):std::min(b.x, c.x);
      if (y<d.y)  ex=(d.y-a.y)?a.x+(d.x-a.x)*(y-a.y)/(d.y-a.y):std::max(a.x, d.x);
      else        ex=(c.y-d.y)?d.x+(c.x-d.x)*(y-d.y)/(c.y-d.y):std::max(d.x, c.x);
      bx=std::max(std::min(bx, frame->cols-1), 0);
      ex=std::max(std::min(ex, frame->cols-1), 0);
      p=frame->ptr<uchar>(y);
      for (int x=bx;x<=ex;++x){
        p[4*x+3]=255-(255-p[4*x+3])*(255-color[3])/255;
        if (p[4*x+3]==0) continue;
        ac=color[3]/p[4*x+3];
        p[4*x+0]=ac*color[0]+(1-ac)*p[4*x+0];
        p[4*x+1]=ac*color[1]+(1-ac)*p[4*x+1];
        p[4*x+2]=ac*color[2]+(1-ac)*p[4*x+2];
      }
    }
  }
  
  void drawTriangle(cv::Point& a, cv::Point& b, cv::Point& c, cv::Scalar& color, cv::Mat* frame){
    if (globalSettings::verboseLevel>3) lout << "[4] drawingFunctions::drawTriangle (... <" << color[0] << "," << color[1] << "," << color[2] << "," << color[3] << ">, ..." << LEND;
    if (a.y>b.y) swap(a, b);
    if (a.y>c.y) swap(a, c);
    if (b.y>c.y) swap(b, c);
    
    int by=std::max(std::min(a.y, frame->rows-1), 0);
    int ey=std::max(std::min(c.y, frame->rows-1), 0);
    uchar* p;
    int bx, ex;
    double ac;
    for (int y=by;y<=ey;++y){
      if (y<b.y)  bx=(b.y-a.y)?a.x+(b.x-a.x)*(y-a.y)/(b.y-a.y):std::min(a.x, b.x);
      else        bx=(c.y-b.y)?b.x+(c.x-b.x)*(y-b.y)/(c.y-b.y):std::min(b.x, c.x);
      ex=(c.y-a.y)?a.x+(c.x-a.x)*(y-a.y)/(c.y-a.y):std::max(a.x, c.x);
      
      bx=std::max(std::min(bx, frame->cols-1), 0);
      ex=std::max(std::min(ex, frame->cols-1), 0);
      if (bx>ex) std::swap(bx, ex);
      
      p=frame->ptr<uchar>(y);
      for (int x=bx;x<=ex;++x){
        p[4*x+3]=255-(255-p[4*x+3])*(255-color[3])/255;
        if (p[4*x+3]==0) continue;
        ac=color[3]/p[4*x+3];
        p[4*x+0]=ac*color[0]+(1-ac)*p[4*x+0];
        p[4*x+1]=ac*color[1]+(1-ac)*p[4*x+1];
        p[4*x+2]=ac*color[2]+(1-ac)*p[4*x+2];
      }
    }
  }

  void drawPolygon(std::vector<cv::Point>& pts, cv::Scalar& color, cv::Mat* frame){
    if (globalSettings::verboseLevel>3) lout << "[4] drawingFunctions::drawPolygon (...[" << pts.size() << "], <" << color[0] << "," << color[1] << "," << color[2] << "," << color[3] << ">, ..." << LEND;
    for (int i=0;i+2<(int)pts.size();++i){
      drawTriangle(pts[i], pts[i+1], pts[i+2], color, frame);
    }
  }
}