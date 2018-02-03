#include "opencv2/opencv.hpp"
#include <iostream>
#include <sstream>
#include <queue>
#include "platformcam.h"


int main(int argc, char** argv)
{
  if(argc != 2)
    {
      std::cerr << "Total frame number needed, to calculate mean!" << std::endl;
      return 0;
    }
  
  std::istringstream iss(argv[1]);
  int total_frame_num;
  iss >> total_frame_num;
  if(!total_frame_num)
    {
      std::cerr << "Invalid frame number" << std::endl;
      return 0;
    }

  cam_t s_vid;
  
  //Set resolution to 480p
  s_vid.set(CV_CAP_PROP_FRAME_WIDTH, 640);
  s_vid.set(CV_CAP_PROP_FRAME_HEIGHT, 480);

  if(!OPEN_CAM(s_vid))
    {
      std::cerr << "Unable to open camera" << std::endl;
      return 0;
    }

  //Initialize acculumated Mat.
  cv::Mat* temp_frame = new cv::Mat();
  CAM_CAP_FRAME(s_vid, (*temp_frame));
  cv::Mat accu_mat(temp_frame->size(), CV_32FC1, cv::Scalar_<float>::all(0));
  delete temp_frame;

  
  std::queue<cv::Mat> frame_queue;
  //Initialize deque
  std::cout << "Filling queue with initial "  << total_frame_num << " frame!" <<std::endl;
  for(int frame_count = 0; frame_count < total_frame_num; ++frame_count)
    {
      frame_queue.push(cv::Mat());
      CAM_CAP_FRAME_GRAY(s_vid, frame_queue.back());
      cv::add(frame_queue.back(), accu_mat, accu_mat, cv::noArray(), CV_32F);
    }
  std::cout << "Done!" << std::endl;
  
  cv::Mat diff, bg;
  cv::Mat divisor_mat = cv::Mat(accu_mat.size(), accu_mat.type(), cv::Scalar_<float>::all(total_frame_num));
  //Dynamic mean
  for(;;)
    {
      //Dynamic mean
      frame_queue.push(cv::Mat());
      CAM_CAP_FRAME_GRAY(s_vid, frame_queue.back());
      cv::add(accu_mat, frame_queue.back(), accu_mat, cv::noArray(), CV_32F);
      cv::subtract(accu_mat, frame_queue.front(), accu_mat, cv::noArray(), CV_32F);
      frame_queue.pop();
      cv::divide(accu_mat, divisor_mat, bg, 1, CV_8U);

      //Background subtraction. Binary threshold can be negotiated
      cv::subtract(frame_queue.back(), bg, diff, cv::noArray(), CV_32F);
      diff = cv::abs(diff);
      cv::threshold(diff, diff, 15, 255, cv::THRESH_BINARY);

      //Show result
      imshow("Background", bg);
      imshow("Segmentation result", diff);

      if(cv::waitKey(1) == 'q')
	break;
    }

  return 0;
}
