#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>
//#include <opencv2/highgui/highgui.hpp>

int main(int argc, char** argv)
{
  if(argc != 3)
    {
      std::cerr << "Use like this: rgb2gray [origin img] [gray img]" << std::endl;
      return 0;
    }

  cv::Mat origin_image;
  origin_image = cv::imread(argv[1]);
  
  if(!origin_image.data)
    {
      std::cerr << "Invalid image name!" << std::endl;
      return 0;
    }

  cv::Mat gray_image;
  cv::cvtColor(origin_image, gray_image, cv::COLOR_RGB2GRAY);
  cv::imwrite(argv[2], gray_image);

  return 0;
  
  
}
