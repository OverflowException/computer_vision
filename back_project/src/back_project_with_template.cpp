#include "opencv2/opencv.hpp"
#include "platformcam.h"
#include <iostream>


//Only capable of painting 2d histogram map
void histMap(const cv::Mat& hist, cv::Mat& map, const cv::Size& eleSize)
{
  cv::Mat hist_8u;
  hist.convertTo(hist_8u, CV_8U);
  cv::Size hist_size = hist_8u.size();
  map = cv::Mat(hist_size.height * eleSize.height,
		hist_size.width * eleSize.width,
		hist.type());
  
  int dim1_index, dim2_index;
  //Traverse histogram element
  cv::Point left_up, right_down;
  for(dim1_index = 0; dim1_index < hist_size.width; ++dim1_index)
    for(dim2_index = 0; dim2_index < hist_size.height; ++dim2_index)
      {
	left_up = {dim1_index * eleSize.width, dim2_index * eleSize.height};
	right_down = {left_up.x + eleSize.width - 1, left_up.y + eleSize.height - 1};
	cv::rectangle(map, left_up, right_down,
		  cv::Scalar::all(hist.at<uchar>(dim1_index, dim2_index)),
		  cv::FILLED);
      }
}

void backProject(const cv::Mat& img, const cv::Mat& hist, cv::Mat& backproj)
{
  backproj = cv::Mat(img.size(), CV_8UC1);
  int row_index, col_index;
  for(row_index = 0; row_index < img.rows; ++row_index)
    for(col_index = 0; col_index < img.cols; ++col_index)
      {
	backproj.at<uchar>(row_index, col_index) =
	  hist.at<uchar>( img.at<cv::Vec3b>(row_index, col_index)[0] / 3,
			  img.at<cv::Vec3b>(row_index, col_index)[1] / 4);
      }
}

cv::Point brightCenter(const cv::Mat& backproj, int threshold)
{
  int x_accu = 0;
  int y_accu = 0;
  int pix_count = 0;
  
  int row_index, col_index;
  for(row_index = 0; row_index < backproj.rows; ++row_index)
    for(col_index = 0; col_index < backproj.cols; ++col_index)
      if(backproj.at<uchar>(row_index, col_index) > threshold)
	{
	  x_accu += col_index;
	  y_accu += row_index;
	  ++pix_count;
	}

  if(pix_count == 0)
    return cv::Point(0, 0);
      
  return cv::Point(x_accu / pix_count, y_accu / pix_count);
}

int main(int argc, char**argv)
{
  cv::Mat templ;
  //read in template as RGB image
  if(argc != 2 || (templ = cv::imread(argv[1], cv::IMREAD_COLOR)).empty())
    {
      std::cerr << "Template name as parameter!" << std::endl;
      return 0;
    }
  
  //convert template from RGB image to HSV image
  //hue: 0 - 179
  //saturation: 0 to 255
  cv::cvtColor(templ, templ, cv::COLOR_BGR2HSV);
  
  //60 bins for hue, 3 hue levels per bin
  //64 bins for saturation, 4 saturation levels per bin 
  int hist_size[] = {60, 64};
  
  //Cover entire hue and saturation spectrum
  float hranges[] = {0, 180};
  float sranges[] = {0, 256};
  const float* ranges[] = {hranges, sranges};

  //Only calculate hue and saturation channel. Ignore value channel.
  int channels[] = {0, 1};

  cv::Mat templ_hist;
  //Calculate histogram
  cv::calcHist(&templ, 1, channels, cv::Mat(),
	       templ_hist, 2, hist_size, ranges,
	       true,
	       false);

  //Normalize histogram
  cv::normalize(templ_hist, templ_hist, 255, 0, cv::NORM_MINMAX, CV_8U);
  
  //obtain hist map
  cv::Mat hist_map;
  histMap(templ_hist, hist_map, cv::Size(5, 5));
  imshow("H-S histogram map", hist_map);
  cv::waitKey(0);

  //open camera to read real time image
  cam_t s_vid;
  s_vid.set(CV_CAP_PROP_FRAME_WIDTH, 640);
  s_vid.set(CV_CAP_PROP_FRAME_HEIGHT, 480);
  if(!OPEN_CAM(s_vid))
    {
      std::cerr << "Unable to open camera!" << std::endl;
      return 0;
    }
  
  cv::Mat frame_rgb, frame_hsv, backproj;
  cv::Point hand_center;
  for(;;)
    {
      CAM_CAP_FRAME(s_vid, frame_rgb);
      cv::cvtColor(frame_rgb, frame_hsv, cv::COLOR_BGR2HSV);
      
      //This will cause bus error on raspberry pi
      // cv::calcBackProject(&frame_hsv, 1, channels,
      //  			  templ_hist, backproj,
      //  			  ranges);
      backProject(frame_hsv, templ_hist, backproj);
      hand_center = brightCenter(backproj, 128);
      cv::circle(frame_rgb, hand_center, 20, cv::Scalar_<uchar>(0, 255, 0));
      cv::imshow("Back project", backproj);
      cv::imshow("Origin", frame_rgb);

      if(cv::waitKey(1) == 'q')
	break;
    }
  s_vid.release();
  return 0;
}
