#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <iostream>
#include <algorithm>

int main(int argc, char** argv)
{
  cv::Mat src;
  if(argc != 2 || (src = imread(argv[1], cv::IMREAD_GRAYSCALE)).empty())
    {
      std::cerr << "Image name as parameter!" << std::endl;
      return 0;
    }

  //1 dimentional histogram
  std::cout << "Calculating gray histogram with calcHist()..." << std::endl;
  cv::Mat hist_1;
  int histSize[] = {256};
  float gray_range[] = {0, 256};
  const float* ranges[] = {gray_range};
  int channels[] = {0};
  cv::calcHist(&src, 1, channels, cv::Mat(),
	       hist_1, 1, histSize, ranges,
	       true, false);

  
  std::cout << "Calculating gray histogram myself..." << std::endl;
  //Initialize histogram. Same as hist_1
  cv::Mat hist_2(cv::Size(1, 256), CV_32FC1);
  hist_2 = cv::Scalar_<float>(0);
  for(cv::MatIterator_<uchar> src_it = src.begin<uchar>();
      src_it != src.end<uchar>(); ++src_it)
    ++hist_2.at<float>(0, *src_it);


  //Compare my histo with opencv's calcHisto
  std::cout.setf(std::ios::boolalpha);
  std::cout << "Equal? " <<
    std::equal(hist_1.begin<float>(), hist_1.end<float>(), hist_2.begin<float>()) << std::endl;



  return 0;
  
}
