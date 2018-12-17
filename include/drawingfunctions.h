#include <opencv2/imgproc.hpp>

#include <vector>

#ifndef DRAWINGFUNCTIONS_H
#define DRAWINGFUNCTIONS_H

// own functions to draw simple things because opencv wasn't doing what I wanted it to do
namespace drawingFunctions{
  void drawLine(cv::Point& a, cv::Point& b, cv::Scalar& color, double thickness, cv::Mat* frame);
  void drawRectangle(cv::Point& a, cv::Point& b, cv::Point& c, cv::Point& d, cv::Scalar& color, cv::Mat* frame);
  void drawTriangle(cv::Point& a, cv::Point& b, cv::Point& c, cv::Scalar& color, cv::Mat* frame);
  void drawPolygon(std::vector<cv::Point>& pts, cv::Scalar& color, cv::Mat* frame);
}

#endif