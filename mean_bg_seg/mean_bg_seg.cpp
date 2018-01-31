#include "opencv2/opencv.hpp"
#include <iostream>
#include <sstream>

#define CAM_CAP_FRAME(s, f) (s).grab();(s).retrieve(f);

//For rgb camera only
void cammeanbg(cv::VideoCapture& vs, size_t nmean, cv::Mat& bg)
{
  //Frame buffer
  cv::Mat frame;
  
  //Capture one frame as a sample, initialize accumulation matrix
  CAM_CAP_FRAME(vs, frame);
  cv::Mat accu_mat(frame.size(), CV_32FC3, cv::Scalar_<float>(0, 0, 0));

  int row, col;
  for(size_t frame_count = 0; frame_count < nmean; ++frame_count)
    {
      CAM_CAP_FRAME(vs, frame);
      for(row = 0; row < frame.rows; ++row)
	for(col = 0; col < frame.cols; ++col)
	  accu_mat.at<cv::Vec3f>(row, col) += frame.at<cv::Vec3b>(row, col);
    }

  std::cout << accu_mat.at<cv::Vec3f>(0, 0) << std::endl;
  //Mean
  bg = cv::Mat(frame.size(), CV_8UC3, cv::Scalar_<uchar>::all(0));
  cv::MatIterator_<cv::Vec3f> accu_it;
  cv::MatIterator_<cv::Vec3b> bg_it;
  for(accu_it = accu_mat.begin<cv::Vec3f>(), bg_it = bg.begin<cv::Vec3b>();
      accu_it != accu_mat.end<cv::Vec3f>(), bg_it != bg.end<cv::Vec3b>();
      ++accu_it, ++bg_it)
    {
       (*bg_it)[0] = uchar((*accu_it)[0] / nmean);
       (*bg_it)[1] = uchar((*accu_it)[1] / nmean);
       (*bg_it)[2] = uchar((*accu_it)[2] / nmean);
      //*accu_it = cv::Vec3f((*accu_it)[0] / nmean, (*accu_it)[1] / nmean, (*accu_it)[2] / nmean);
    }
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
  
  cv::VideoCapture s_vid;
  if(!s_vid.open(0))
    {
      std::cerr << "Unable to opencv camera" << std::endl;
      return 0;
    }

  cv::Mat bg;
  cammeanbg(s_vid, total_frame_num, bg);
  std::cout << bg.at<cv::Vec3b>(0, 0) << std::endl;
  imshow("Averaged backgroud", bg);
  cv::waitKey(0);
  
}
