#include <opencv2/opencv.hpp>
#include <iostream>
#include <ctime>

double sample(cv::VideoCapture& cap, int nframes)
{
  cv::Mat frame;
  
  long long begin =  cv::getTickCount();
  for(int idx = 0; idx < nframes; ++idx)
    cap >> frame;
  long long end = cv::getTickCount();

  std::cout << "duration = " << (double)(end - begin) / ((double)cv::getTickFrequency()) << std::endl;
  return (double)(end - begin) / ((double)cv::getTickFrequency()) / nframes;
}
