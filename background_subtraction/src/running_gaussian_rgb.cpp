#include "opencv2/opencv.hpp"
#include <iostream>
#include <sstream>
#include <queue>
#include <vector>
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

  

  
}

