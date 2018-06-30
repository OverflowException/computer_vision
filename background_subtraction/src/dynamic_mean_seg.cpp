#include "opencv2/opencv.hpp"
#include <cstdlib>
#include <iostream>
#include <queue>
#include <vector>
#include "platformcam.h"


int main(int argc, char** argv)
{
  if(argc < 2)
    {
      std::cerr << "Total frame number needed, to calculate mean!" << std::endl;
      return 0;
    }
  
  int total_frame_num = atoi(argv[1]);
  if(!total_frame_num)
    {
      std::cerr << "Invalid frame number" << std::endl;
      return 0;
    }

  cv::VideoCapture s_vid;
  
  //No video name specified, open camera
  if(argc < 3)
    {
      if(!s_vid.open(0))
	{
	  std::cout << "Cannot open camera" << std::endl;
	  return 0;
	}
    }
  //Or open specified video
  else
    {
      if(!s_vid.open(argv[2]))
	{
	  std::cout << "Cannot open " << argv[2] << std::endl;
	  return 0;
	}
    }
  
  //Initialize acculumated Mat.
  cv::Mat* temp_frame = new cv::Mat();
  s_vid >> *temp_frame;
  cv::Mat accu_mat(temp_frame->size(), CV_32FC1, cv::Scalar_<float>::all(0));
  delete temp_frame;

  //The number of frames is fixed, so no need to use a queue. Use a fixed-size container.
  std::vector<cv::Mat> frame_vec(total_frame_num, cv::Mat(accu_mat.size(), CV_8UC3, cv::Scalar_<uchar>::all(0)));
  decltype(frame_vec)::iterator frame_it = frame_vec.begin();
  //Initialize vector
  std::cout << "Filling vector with initial "  << total_frame_num << " frame..." <<std::endl;
  for(frame_it = frame_vec.begin(); frame_it != frame_vec.end(); ++frame_it)
    {
      s_vid >> *frame_it;
      cv::cvtColor(*frame_it, *frame_it, cv::COLOR_RGB2GRAY);
      cv::add(accu_mat, *frame_it, accu_mat, cv::noArray(), CV_32F);
    }
  std::cout << "Done!" << std::endl;
  
  cv::Mat diff, bg;
  cv::Mat divisor_mat = cv::Mat(accu_mat.size(), accu_mat.type(), cv::Scalar_<float>::all(total_frame_num));

  //Dynamic mean
  for(frame_it = frame_vec.begin();; ++frame_it)
    {
      if(frame_it == frame_vec.end())
	frame_it = frame_vec.begin();
      
      //Dynamic Mean
      cv::subtract(accu_mat, *frame_it, accu_mat, cv::noArray(), CV_32F);
      s_vid >> *frame_it;
      cv::cvtColor(*frame_it, *frame_it, cv::COLOR_RGB2GRAY);
      cv::add(accu_mat, *frame_it, accu_mat, cv::noArray(), CV_32F);
      cv::divide(accu_mat, divisor_mat, bg, 1, CV_8U);

      //Background subtraction. Binary threshold can be negotiated
      cv::absdiff(*frame_it, bg, diff);
      cv::threshold(diff, diff, 15, 255, cv::THRESH_BINARY);

      //Show result
      imshow("Background", bg);
      imshow("Segmentation result", diff);
      
      if(cv::waitKey(1) == 'q')
	break;
    }

  return 0;
}
