#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <iostream>
#include <algorithm>

int main(int argc, char** argv)
{
  cv::Mat src;
  if(argc != 2 || (src = imread(argv[1], cv::IMREAD_COLOR)).empty())
    {
      std::cerr << "Image name as parameter!" << std::endl;
      return 0;
    }

  //3 dimentional histogram
  std::cout << "Calculating rgb histogram with calcHist()..." << std::endl;
  cv::Mat hist_1;
  int histSize[] = {256, 256, 256};
  float channel_range[] = {0, 256};
  const float* ranges[] = {channel_range, channel_range, channel_range};
  int channels[] = {0, 1, 2};
  cv::calcHist(&src, 1, channels, cv::Mat(),
	       hist_1, 3, histSize, ranges,
	       true, false);
  
  std::cout << "Calculating rgb histogram myself..." << std::endl;
  //Initialize histogram. Same as hist_1
  cv::Mat hist_2(3, histSize, CV_32FC1);
  hist_2 = cv::Scalar_<float>(0);
  for(cv::MatIterator_<cv::Vec3b> src_it = src.begin<cv::Vec3b>();
      src_it != src.end<cv::Vec3b>(); ++src_it)
    ++hist_2.at<float>((*src_it)[0], (*src_it)[1], (*src_it)[2]);

  //Compare my histo with opencv's calcHisto
  std::cout.setf(std::ios::boolalpha);
  std::cout << "Equal? " <<
    std::equal(hist_1.begin<float>(), hist_1.end<float>(), hist_2.begin<float>()) << std::endl;

  return 0;
}
