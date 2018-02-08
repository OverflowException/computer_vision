#include "opencv2/opencv.hpp"
#include "platformcam.h"
#include <iostream>
#include <vector>
#include <utility>

//Only capable of mapping 2d histogram map
void histMap(const cv::Mat& hist, cv::Mat& map, const cv::Size& eleSize)
{
  cv::Mat hist_8u;
  hist.convertTo(hist_8u, CV_8U);
  cv::Size hist_size = hist_8u.size();
  map = cv::Mat(hist_size.height * eleSize.height,
		hist_size.width * eleSize.width,
		hist_8u.type());
  
  int dim1_index, dim2_index;
  //Traverse histogram element
  cv::Point left_up, right_down;
  for(dim1_index = 0; dim1_index < hist_size.width; ++dim1_index)
    for(dim2_index = 0; dim2_index < hist_size.height; ++dim2_index)
      {
	left_up = {dim1_index * eleSize.width, dim2_index * eleSize.height};
	right_down = {left_up.x + eleSize.width - 1, left_up.y + eleSize.height - 1};
	cv::rectangle(map, left_up, right_down,
		      cv::Scalar_<uchar>::all(hist_8u.at<uchar>(dim2_index, dim1_index)),
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
	  hist.at<uchar>( img.at<cv::Vec3b>(row_index, col_index)[0] / 6,
			  img.at<cv::Vec3b>(row_index, col_index)[1] / 8);
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
  
  //Grab the first frame to determine geometry
  CAM_CAP_FRAME(s_vid, frame_rgb);

  //sample roi per row, per col
  cv::Size sz_arr_num(3, 3);
  //ROI gap
  cv::Size sz_gap(frame_rgb.cols / 30, frame_rgb.rows / 20);
  //size of a simgle ROI
  cv::Size sz_roi(12, 12);
  //sample roi starting point. (left up)
  cv::Point sample_roi_beg(int(frame_rgb.cols * 0.4), int(frame_rgb.rows * 0.4));
  //These 4 parameter will determine sample roi array.

  
  //9 sample rectangle roi. width = height = 10
  std::vector<cv::Rect> arr_sample_roi;
  arr_sample_roi.resize(sz_arr_num.width * sz_arr_num.height);
  
  //ROI array stating point
  //Initialize these roi rectangles
  int row_index, col_index;
  for(row_index = 0; row_index < sz_arr_num.height; ++row_index)
    {
      for(col_index = 0; col_index < sz_arr_num.width; ++col_index)
	arr_sample_roi[row_index * sz_arr_num.width + col_index] =
	  cv::Rect(cv::Point(sample_roi_beg.x + col_index * sz_gap.width,
		     sample_roi_beg.y + row_index * sz_gap.height),
	   sz_roi);
    }

  //Draw on image, wait for confirmation
  for(;;)
    {
      CAM_CAP_FRAME(s_vid, frame_rgb);
      for(cv::Rect& roi_ele : arr_sample_roi)
	cv::rectangle(frame_rgb, roi_ele, cv::Scalar_<uchar>(0, 255, 0), 2, cv::FILLED);

      imshow("Sampling. Press C to confirm", frame_rgb);

      if(cv::waitKey(1) == 'c')
	break;
    }

  cv::destroyWindow("Sampling. Press C to confirm");

  //capture the last frame
  CAM_CAP_FRAME(s_vid, frame_rgb);

  std::vector<cv::Mat> arr_sample_img;
  cv::Mat whole_sample(cv::Size(sz_roi.width * sz_arr_num.width, sz_roi.height * sz_arr_num.height),
  		       frame_rgb.type());
  arr_sample_img.resize(arr_sample_roi.size());
  for(size_t index = 0; index < arr_sample_roi.size(); ++index)
    {
      //Constuct all the subimages
      arr_sample_img[index] = cv::Mat(frame_rgb, arr_sample_roi[index]);
      //Convert all subimages to hsv. Coresponding position in frame_rgb image is changed.
      //cv::cvtColor(arr_sample_img[index], arr_sample_img[index], cv::COLOR_BGR2HSV);
      //Copy to one big image
      arr_sample_img[index].copyTo(whole_sample(cv::Rect((index % sz_arr_num.width) * sz_roi.width,  //left-up x
							 (index / sz_arr_num.width) * sz_roi.height, //left-up y
							 arr_sample_img[index].cols,
							 arr_sample_img[index].rows)));
    }
  cv::cvtColor(whole_sample, whole_sample, cv::COLOR_BGR2HSV);

  
  //60 bins for hue, 3 hue levels per bin
  //64 bins for saturation, 4 saturation levels per bin 
  int hist_size[] = {30, 32};
  
  //Cover entire hue and saturation spectrum
  float hranges[] = {0, 180};
  float sranges[] = {0, 256};
  const float* ranges[] = {hranges, sranges};

  // //Only calculate hue and saturation channel. Ignore value channel.
  // std::vector<int> channels(2 * arr_sample_img.size());
  // for(size_t img_index = 0; img_index < arr_sample_img.size(); ++img_index)
  //   for(size_t channel_index = 0; channel_index < 2; ++channel_index)
  //     channels[img_index * 2 + channel_index] = img_index * 3 + channel_index;
  int channels[] = {0, 1};
  
  cv::Mat sample_hist;
  //Calculate histogram
  cv::calcHist(&whole_sample, 1, channels, cv::Mat(),
  	       sample_hist, 2, hist_size, ranges,
  	       true,
  	       false);

  //Normalize histogram
  cv::normalize(sample_hist, sample_hist, 255, 0, cv::NORM_MINMAX, CV_8U);

  //obtain hist map
  cv::Mat hist_map;
  histMap(sample_hist, hist_map, cv::Size(10, 10));
  imshow("H-S histogram map", hist_map);
  cv::waitKey(0);
  cv::destroyWindow("H-S histogram map");

  std::vector<cv::Point> nonzero_pos_i;
  std::vector<cv::Point2f> nonzero_pos_f;
  decltype(nonzero_pos_i)::iterator it_pos_i;
  decltype(nonzero_pos_f)::iterator it_pos_f;
  
  std::vector<int> points_labels;
  cv::Mat cluster_centers_coords;
  std::vector<cv::Point> cluster_centers;
  for(;;)
    {
      CAM_CAP_FRAME(s_vid, frame_rgb);
      cv::cvtColor(frame_rgb, frame_hsv, cv::COLOR_BGR2HSV);
      
      //This will cause bus error on raspberry pi
      // cv::calcBackProject(&frame_hsv, 1, channels,
      //  			  templ_hist, backproj,
      //  			  ranges);
      backProject(frame_hsv, sample_hist, backproj);
      cv::imshow("Original back project", backproj);
      //cv::medianBlur(backproj, backproj, 5);
      //cv::findContours(backproj, );
      cv::threshold(backproj, backproj, 0, 255, cv::THRESH_OTSU);
      cv::imshow("Thresholded", backproj);

      
      //Find clusters
      cv::findNonZero(backproj, nonzero_pos_i);
      nonzero_pos_f.resize(nonzero_pos_i.size());
      for(it_pos_i = nonzero_pos_i.begin(), it_pos_f = nonzero_pos_f.begin();
	  it_pos_i != nonzero_pos_i.end();
	  ++it_pos_i, ++it_pos_f)
	*it_pos_f = cv::Point2f(float(it_pos_i->x), float(it_pos_i->y));
	
      //std::cout << "here_1" << std::endl;
      cv::kmeans(nonzero_pos_f, 3, points_labels,
		 cv::TermCriteria(cv::TermCriteria::COUNT + cv::TermCriteria::EPS, 10, 0.1),
		 3, cv::KMEANS_PP_CENTERS, cluster_centers_coords);

      cluster_centers.resize(cluster_centers_coords.rows);
      for(size_t center_index = 0; center_index < cluster_centers.size(); ++center_index)
	cluster_centers[center_index] =
	  cv::Point(int(cluster_centers_coords.at<float>(center_index, 0)),
		    int(cluster_centers_coords.at<float>(center_index, 1)));

      for(cv::Point center : cluster_centers)
	cv::circle(frame_rgb, center, 10, cv::Scalar_<uchar>(255, 255, 0), 3, cv::FILLED);

      
      cv::imshow("Cluster centers", frame_rgb);
      // //morphology open. Denoising
      // cv::morphologyEx(backproj, backproj, cv::MORPH_OPEN,
      // 		       cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3, 3)));
      // //morphology dilate. Denoising
      // cv::morphologyEx(backproj, backproj, cv::MORPH_DILATE,
      // 		       cv::getStructuringElement(cv::MORPH_RECT, cv::Size(9, 9)));
      
      if(cv::waitKey(1) == 'q')
  	break;
    }
  s_vid.release();

  
  return 0;
}
