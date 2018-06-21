#include <iostream>
#include <stdio.h>
#include "opencv2/core.hpp"
#include "opencv2/core/utility.hpp"
#include "opencv2/core/ocl.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/features2d.hpp"
#include "opencv2/calib3d.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/xfeatures2d.hpp"

#define GET_DURATION(PROC) workBegin();PROC;workEnd()
#define SHOW_DURATION(TAG) std::cout<<TAG<<" duration: " << getTime() << std::endl

using namespace cv;
using namespace cv::xfeatures2d;

const int LOOP_NUM = 10;
const int GOOD_PTS_MAX = 50;
const float GOOD_PORTION = 0.15f;

int64 work_begin = 0;
int64 work_end = 0;

static void workBegin()
{
    work_begin = getTickCount();
}

static void workEnd()
{
    work_end = getTickCount() - work_begin;
}

static double getTime()
{
    return work_end / (double)getTickFrequency();
}

struct SURFDetector
{
    Ptr<Feature2D> surf;
    SURFDetector(double hessian = 800.0)
    {
        surf = SURF::create(hessian);
    }
    template<class T>
    void operator()(const T& in, const T& mask, std::vector<cv::KeyPoint>& pts, T& descriptors, bool useProvided = false)
    {
        surf->detectAndCompute(in, mask, pts, descriptors, useProvided);
    }
};

template<class KPMatcher>
struct SURFMatcher
{
    KPMatcher matcher;
    template<class T>
    void match(const T& in1, const T& in2, std::vector<cv::DMatch>& matches)
    {
        matcher.match(in1, in2, matches);
    }
};

static Mat drawGoodMatches(
    const Mat& img1,
    const Mat& img2,
    const std::vector<KeyPoint>& keypoints1,
    const std::vector<KeyPoint>& keypoints2,
    std::vector<DMatch>& matches,
    std::vector<Point2f>& scene_corners_
    )
{
    //-- Sort matches and preserve top 10% matches
    std::sort(matches.begin(), matches.end());
    std::vector<DMatch> good_matches;
    double minDist = matches.front().distance;
    double maxDist = matches.back().distance;

    const int ptsPairs = std::min(GOOD_PTS_MAX, (int)(matches.size() * GOOD_PORTION));
    for( int i = 0; i < ptsPairs; i++ )
    {
        good_matches.push_back( matches[i] );
    }
    std::cout << "\nMax distance: " << maxDist << std::endl;
    std::cout << "Min distance: " << minDist << std::endl;

    std::cout << "Calculating homography using " << ptsPairs << " point pairs." << std::endl;

    // drawing the results
    Mat img_matches;

    // for(auto it = good_matches.begin(); it < good_matches.end(); ++it)
    //   std::cout << it->queryIdx << " ";
    // std::cout << std::endl;
    
    drawMatches( img1, keypoints1, img2, keypoints2,
                 good_matches, img_matches, Scalar::all(-1), Scalar::all(-1),
                 std::vector<char>(), DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS  );

    //-- Localize the object
    std::vector<Point2f> obj;
    std::vector<Point2f> scene;

    for( size_t i = 0; i < good_matches.size(); i++ )
    {
        //-- Get the keypoints from the good matches
        obj.push_back( keypoints1[ good_matches[i].queryIdx ].pt );
        scene.push_back( keypoints2[ good_matches[i].trainIdx ].pt );
    }
    //-- Get the corners from the image_1 ( the object to be "detected" )
    std::vector<Point2f> obj_corners(4);
    obj_corners[0] = Point(0,0);
    obj_corners[1] = Point( img1.cols, 0 );
    obj_corners[2] = Point( img1.cols, img1.rows );
    obj_corners[3] = Point( 0, img1.rows );
    std::vector<Point2f> scene_corners(4);

    Mat H = findHomography( obj, scene, RANSAC );
    perspectiveTransform( obj_corners, scene_corners, H);

    scene_corners_ = scene_corners;

    //-- Draw lines between the corners (the mapped object in the scene - image_2 )
    line( img_matches,
          scene_corners[0] + Point2f( (float)img1.cols, 0), scene_corners[1] + Point2f( (float)img1.cols, 0),
          Scalar( 0, 255, 0), 2, LINE_AA );
    line( img_matches,
          scene_corners[1] + Point2f( (float)img1.cols, 0), scene_corners[2] + Point2f( (float)img1.cols, 0),
          Scalar( 0, 255, 0), 2, LINE_AA );
    line( img_matches,
          scene_corners[2] + Point2f( (float)img1.cols, 0), scene_corners[3] + Point2f( (float)img1.cols, 0),
          Scalar( 0, 255, 0), 2, LINE_AA );
    line( img_matches,
          scene_corners[3] + Point2f( (float)img1.cols, 0), scene_corners[0] + Point2f( (float)img1.cols, 0),
          Scalar( 0, 255, 0), 2, LINE_AA );
    return img_matches;
}

////////////////////////////////////////////////////
// This program demonstrates the usage of SURF_OCL.
// use cpu findHomography interface to calculate the transformation matrix
int main(int argc, char** argv)
{
  if(argc != 2)
    {
      std::cout << "Usage: " << argv[0] << " [template name]" << std::endl;
      return 0;
    }

  //template image
  Mat templ = imread(argv[1], IMREAD_GRAYSCALE);
  if(templ.empty())
    {
      std::cout << argv[1] << " is not a valid image name!" << std::endl;
      return 0;
    }

  //Camera utility
  VideoCapture vs;
  Mat frame;
  if(!vs.open(0))
    {
      std::cout << "Cannot open camera!"  << std::endl;
      return 0;
    }  
  
  //Instantiate data structures
  std::vector<KeyPoint> kp_templ, kp_frame;
  Mat desc_templ, desc_frame;
  std::vector<DMatch> matches;

  //Instantiate detectors/matchers
  SURFDetector surf;
  SURFMatcher<BFMatcher> matcher;

  //Acquire keypoints and descriptors from template
  GET_DURATION(
	       surf(templ, Mat(), kp_templ, desc_templ);
	       );
  SHOW_DURATION("Template acquire feature");
  std::cout << kp_templ.size() << " keypoints in template!" << std::endl;
  
  Mat img_matched;
  while(true)
    {	
      vs >> frame;
      GET_DURATION(
		   cvtColor(frame, frame, COLOR_BGR2GRAY);
		   surf(frame, Mat(), kp_frame, desc_frame);
		   );
      SHOW_DURATION("Frame acquire feature");
      //Check key pressed
      if(waitKey(1) == 'q') break;
      //Check if any keypoints in frame
      if(kp_frame.size() == 0) continue;
      std::cout << kp_frame.size() << " keypoints in frame!" << std::endl;
      
      GET_DURATION(
      		   matcher.match(desc_templ, desc_frame, matches);
      		   );
      SHOW_DURATION("Match");
      
      std::vector<Point2f> corner;
      Mat img_matches = drawGoodMatches(templ, frame, kp_templ, kp_frame, matches, corner);      
      imshow("Surf Matches", img_matches);
    }
  
    return 0;
}
