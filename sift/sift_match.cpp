#include <stdio.h>
#include <iostream>
#include "opencv2/core.hpp"
#include "opencv2/features2d.hpp"
#include "opencv2/xfeatures2d.hpp"
#include "opencv2/highgui.hpp"

using namespace cv;
using namespace cv::xfeatures2d;

/** @function main */
int main( int argc, char** argv )
{
  if(argc != 2)
  {
    std::cout << "Usage: ./" << argv[0] << " [img]" << std::endl;
    return 0;
  }

  Mat img = imread(argv[1], IMREAD_GRAYSCALE);

  if(!img.data)
  {
    std::cout<< " --(!) Error reading images " << std::endl;
    return 0;
  }

  //-- Step 1: Detect the keypoints using SIFT Detector
  int minHessian = 400;

  Ptr<SIFT> detector = SIFT::create(minHessian);

  std::vector<KeyPoint> keypoints;

  detector->detect(img, keypoints);

  //-- Draw keypoints
  Mat img_keypoints;

  drawKeypoints(img, keypoints, img_keypoints, Scalar::all(-1), DrawMatchesFlags::DEFAULT);

  //-- Show detected (drawn) keypoints
  imshow("Keypoints", img_keypoints);

  waitKey(0);

  return 0;
}
