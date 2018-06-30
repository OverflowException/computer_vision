/*----------------------------------------------
 * Usage:
 * example_tracking_multitracker <video_name> [algorithm]
 *
 * example:
 * example_tracking_multitracker Bolt/img/%04d.jpg
 * example_tracking_multitracker faceocc2.webm KCF
 *--------------------------------------------------*/

#include <opencv2/core/utility.hpp>
#include <opencv2/tracking.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>
#include <iostream>
#include <cstring>
#include <ctime>
#include <utility>
#include "samples_utility.hpp"

using namespace std;
using namespace cv;

template<class _T>
void genCrosshair(const Rect_<_T>& rect,
		  Point_<_T>& ch_center,
		  pair<Point_<_T>, Point_<_T>>& ch_h,
		  pair<Point_<_T>, Point_<_T>>& ch_v)
{
  ch_center = Point2d(rect.x + rect.width / 2, rect.y + rect.height / 2);
  
  ch_v.first = Point2d(ch_center.x, ch_center.y - 0.7 * rect.height);
  ch_v.second = Point2d(ch_center.x, ch_center.y + 0.7 * rect.height);
  ch_h.first = Point2d(ch_center.x - 0.7 * rect.width, ch_center.y);
  ch_h.second = Point2d(ch_center.x + 0.7 * rect.width, ch_center.y);
}

int main( int argc, char** argv )
{
  // show help
  if(argc<2)
    {
      cout << "Usage: example_tracking_multitracker <video_name> [algorithm]\n" << endl;
      return 0;
    }

  // set the default tracking algorithm
  std::string trackingAlg = "MEDIAN_FLOW";

  // set the tracking algorithm from parameter
  if(argc>2)
    trackingAlg = argv[2];

  // create the tracker
  MultiTracker trackers;

  // container of the tracked objects
  vector<Rect2d> objects;

  // set input video
  std::string video = argv[1];
  VideoCapture cap(video);

  Mat frame;

  // get bounding box
  cap >> frame;
  vector<Rect> ROIs;
  selectROIs("tracker",frame,ROIs);
  
  //////////////////////
  std::cout << "ROIs: " << std::endl;
  for(const auto& rec : ROIs)
    std::cout << rec << std::endl;

  //quit when the tracked object(s) is not provided
  if(ROIs.size() < 1)
    return 0;

  // initialize the tracker
  std::vector<Ptr<Tracker> > algorithms;
  for (size_t i = 0; i < ROIs.size(); i++)
  {
      algorithms.push_back(createTrackerByName(trackingAlg));
      objects.push_back(ROIs[i]);
  }
  trackers.add(algorithms,frame,objects);


  pair<Point2d, Point2d> crosshair_v;
  pair<Point2d, Point2d> crosshair_h;
  Point2d crosshair_center;
  
  // do the tracking
  cout << "Start the tracking process, press ESC to quit.\n" << endl;
  
  std::cout << "#Object = " << objects.size() << std::endl;
  for ( ;; )
    {
      //get frame from the video
      cap >> frame;

      //stop the program if no more images
      if(frame.rows==0 || frame.cols==0)
	break;

      //update the tracking result
      trackers.update(frame, objects);

      //draw the tracked object
      for(const Rect2d& obj : objects)
	{
	  rectangle(frame, obj, Scalar(0, 255, 0), 2, 1);
	  genCrosshair(obj,  crosshair_center, crosshair_h, crosshair_v);
	  cout << crosshair_center << endl;
	  line(frame, crosshair_v.first, crosshair_v.second, Scalar(0, 255, 0), 2, 1);
	  line(frame, crosshair_h.first, crosshair_h.second, Scalar(0, 255, 0), 2, 1);
	}
          
      //show image with the tracked object
      imshow("tracker",frame);

      //quit on ESC button
      if(waitKey(1)==27)break;
    }

}
