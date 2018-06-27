#include <sys/types.h>
#include <dirent.h>
#include <iostream>
#include <cstdlib>
#include <algorithm>
#include "opencv2/core.hpp"
#include "opencv2/videoio.hpp"
#include "opencv2/highgui.hpp"

#define MAX_IMG_COUNT 2048

int main(int argc, char** argv)
{
  if(argc != 4)
    {
      std::cout << "Usage: " << argv[0] << " [image dir] [video] [fps]" << std::endl;
      return 0;
    }

  std::string str_img_dir(argv[1]);
  std::string str_video_name(argv[2]);
  
  DIR* p_dir;
  struct dirent* p_ent;
  
  //open directory
  if((p_dir = opendir(str_img_dir.c_str())) == NULL)
    {
      std::cout << "Cannot open directory " << str_img_dir << std::endl;
      return 0;
    }

  std::vector<std::string> img_names;
  //Get all the image names under this directory
  while((p_ent = readdir(p_dir)) != NULL && img_names.size() < MAX_IMG_COUNT)
    {
      if(p_ent->d_type != DT_REG)
	continue;
      img_names.emplace_back(p_ent->d_name);
    }
  closedir(p_dir);
  if(img_names.size() == 0)
    {
      std::cout << "No file in directory " << str_img_dir << std::endl;
      return 0;
    }
  
  //Sort image names
  std::sort(img_names.begin(), img_names.end(), [&](const std::string& a, const std::string& b)
	    {return a < b;});
  
  //Initialize video writer utilities
  cv::VideoWriter writer;
  cv::Mat frame = cv::imread(str_img_dir + img_names[0]);
  if(frame.empty())
    {
      std::cout << "Cannot open " << img_names[0] << " as an image!" << std::endl;
      return 0;
    }
  cv::Size frame_size = frame.size();
  
  //Open video writer stream, FFMPEG no loss
  if(!writer.open(str_video_name, cv::VideoWriter::fourcc('M', 'J', 'P', 'G'),
		  (double)atoi(argv[3]), frame_size))
    {
      std::cout << "Cannot open video stream!" << std::endl;
      return 0;
    }

  for(std::string& str : img_names)
    {
      frame = cv::imread(str_img_dir + str);
      writer << frame;
    }

  writer.release();
  return 0;
}
