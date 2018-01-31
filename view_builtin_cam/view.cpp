#include <opencv2/opencv.hpp>
#include <iostream>
#include "platformcam.h"


int main(int argc, char** argv)
{
  cam_t s_vid;
  //Set resolution to 480p
  s_vid.set(CV_CAP_PROP_FRAME_WIDTH, 640);
  s_vid.set(CV_CAP_PROP_FRAME_HEIGHT, 480);
  if(!OPEN_CAM(s_vid))
    {
      std::cerr << "Unable to open camera!" << std::endl;
      return 0;
    }

  cv::Mat frame;
  std::string window_name = "Look at yourself, fuckhead!";
  cv::namedWindow(window_name, cv::WINDOW_AUTOSIZE);
  
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
