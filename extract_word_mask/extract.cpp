#include "opencv2/opencv.hpp"
#include <iostream>
#include <vector>
#include <utility>
#include <limits>
#include <cstdlib>

int main(int argc, char** argv)
{
  if(argc < 2)
    {
      std::cout << "Usage: " << argv[0] << " image [resize height] [resize width]" << std::endl;
      return 0;
    }

  cv::Mat gray;
  if((gray = imread(argv[1], cv::IMREAD_GRAYSCALE)).empty())
    {
      std::cout << "Image name invalid!" << std::endl;
      return 0;
    }

  cv::threshold(gray, gray, 0, 255, cv::THRESH_OTSU);

  cv::Point tl_point(std::numeric_limits<int>::max(), std::numeric_limits<int>::max());
  cv::Point br_point(0, 0);
  
  //Find left boundary
  int col, row;
  for(col = 0; col < gray.cols; ++col)
    {
      for(row = 0; row < gray.rows; ++row)
	if(gray.at<uchar>(row, col) == 0)
	  break;

      if(row != gray.rows)
	break;
    }
  tl_point.x = col;
  
  //Find right boundary
  for(col = gray.cols - 1; col > 0; --col)
    {
      for(row = 0; row < gray.rows; ++row)
	if(gray.at<uchar>(row, col) == 0)
	  break;

      if(row != gray.rows)
	break;
    }
  br_point.x = col;

  //Find top boundary
  for(row = 0; row < gray.rows; ++row)
    {
      for(col = 0; col < gray.cols; ++col)
	if(gray.at<uchar>(row, col) == 0)
	  break;

      if(col != gray.cols)
	break;
    }
  tl_point.y = row;
  
  //Find bottom boundary
  for(row = gray.rows - 1; row > 0; --row)
    {
      for(col = 0; col < gray.cols; ++col)
	if(gray.at<uchar>(row, col) == 0)
	  break;

      if(col != gray.cols)
	break;
    }
  br_point.y = row;

  //Get sub-image
  cv::Mat sub_gray = gray(cv::Rect(tl_point, br_point));

  cv::Mat resized_gray;
  cv::Mat target_gray;
  //resize
  if(argc == 4)
    {
      cv::resize(sub_gray, resized_gray, cv::Size(atoi(argv[2]), atoi(argv[3])), 0, 0);
      target_gray = resized_gray;
    }
  else
    target_gray = sub_gray;


  //Obtain pattern mask
  std::vector<std::pair<int, int>> pattern_mask;
  for(int x = 0; x < target_gray.cols; ++x)
    for(int y = 0; y < target_gray.rows; ++y)
      if(target_gray.at<uchar>(y, x) == 0)	  
  	pattern_mask.emplace_back(x, y);

  //Output info  
  std::cout << "resized size = " << target_gray.size() << std::endl;
  std::cout << "mask size = " << pattern_mask.size() << std::endl;
  imshow("resized", target_gray);
  cv::waitKey(0);
  

  //Output mask
  int wrap_counter = 0;
  for(std::pair<int, int>& coord : pattern_mask)
    {
      std::cout << "{" << coord.first << ", " << coord.second << "}" << ", ";
      if(++wrap_counter == 8)
	{
	  std::cout << std::endl;
	  wrap_counter = 0;
	}
    }
  
}
