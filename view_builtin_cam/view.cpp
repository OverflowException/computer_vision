#include <opencv2/opencv.hpp>
#include <iostream>

int main(int argc, char** argv)
{
  cv::VideoCapture s_vid;
  if(!s_vid.open(0))
    {
      std::cerr << "Unable to open camera!" << std::endl;
      return 0;
    }

  cv::Mat frame;
  std::string window_name = "Look at yourself, fuckhead!";
  cv::namedWindow(window_name, cv::WINDOW_KEEPRATIO);
  
  for(;;)
    {
      //This is a much faster alternative
      //s_vid >> frame;

      s_vid.grab();
      s_vid.retrieve(frame);
      if(frame.empty())
	break;

      imshow(window_name, frame);
      if(cv::waitKey(1) == 'q')
	break;
    }

  s_vid.release();
  return 0;
}
