#ifndef _PLATFORMCAM_H
#define _PLATFORMCAM_H

#ifdef RPI_CAM
  #include "raspicam/raspicam_cv.h"
  #define OPEN_CAM(vs) vs.open()
  typedef raspicam::RaspiCam_Cv cam_t;
#else
  #include "opencv2/videoio.hpp"
  #define OPEN_CAM(vs) vs.open(0)
  typedef cv::VideoCapture cam_t;
#endif

#define CAM_CAP_FRAME(s, f) (s).grab();(s).retrieve(f);

#endif
