#include "opencv2/opencv.hpp"
#include <iostream>
#include <sstream>
#include <type_traits>
#include "platformcam.h"

//For rgb camera only
void camMeanBg(cam_t& vs, size_t nmean, cv::Mat& bg)
{
  //Frame buffer
  cv::Mat frame;
  
  //Capture one frame as a sample, initialize accumulation matrix
  CAM_CAP_FRAME(vs, frame);
  cv::Mat accu_mat(frame.size(), CV_32FC3, cv::Scalar_<float>(0, 0, 0));

  for(size_t frame_count = 0; frame_count < nmean; ++frame_count)
    {
      CAM_CAP_FRAME(vs, frame);
      //Accumulate frames
      cv::add(frame, accu_mat, accu_mat, cv::noArray(), CV_32F);
    }
  
  //Mean, use divide instead of hand-written element-wise division to be more efficient
  cv::Mat divisor_mat(accu_mat.size(), accu_mat.type(), cv::Scalar_<float>::all(nmean));
  cv::divide(accu_mat, divisor_mat, bg, 1, CV_8U);
}


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
    
  cv::Mat bg;
  std::cout << "Callibrating background..." << std::endl;
  camMeanBg(s_vid, total_frame_num, bg);
  std::cout << "Done!" << std::endl;
  imshow("Averaged backgroud", bg);

  cv::Mat frame;
  cv::Mat diff;
  for(;;)
    {
      CAM_CAP_FRAME(s_vid, frame);
      diff = cv::abs(frame - bg);
      
      //Thresholding?
      for(auto diff_it = diff.begin<cv::Vec3b>();
	  diff_it != diff.end<cv::Vec3b>();
	  ++diff_it)
	*diff_it = ((*diff_it)[0] + (*diff_it)[1] + (*diff_it)[2]) > 60 ?
	  cv::Vec<uchar, 3>::all(255) : cv::Vec<uchar, 3>::all(0);
      
      imshow("Segmentation result", diff);

      if(cv::waitKey(1) == 'q')
	break;
    }
}
