#include <opencv2/opencv.hpp>
#include <iostream>
#include <string>
#include "fps_sample.cpp"

int main(int argc, char** argv)
{
  if(argc < 3)
    {
      std::cout << "Usage: " << argv[0] << " [ip address]:[port number] [video name]" << std::endl;
      return 0;
    }
  
  cv::VideoCapture reader;
  cv::VideoWriter writer;
  reader.set(CV_CAP_PROP_FRAME_WIDTH, 320);
  reader.set(CV_CAP_PROP_FRAME_HEIGHT, 240);

  std::string src_endpoint = std::string("http://") + argv[1] + std::string("/?action=stream");
  std::cout << "Acquiring video stream from" << src_endpoint << "..." << std::endl;

  //Open reader stream
  if(!reader.open(src_endpoint))
    {
      std::cerr << "Unable to acquire!" << std::endl;
      return 0;
    }
  std::cout << "Succcessfully acquired!" << std::endl;

  //Sample frame latency
  std::cout << "Sampling frame rate..." << std::endl;
  double latency = sample(reader, 30);
  double fps = 1 / latency;
  std::cout << "Frame rate = " << fps << std::endl;
  
  //Open writer
  if(!writer.open(argv[2], cv::VideoWriter::fourcc('M', 'J', 'P', 'G'),
		  fps, cv::Size(640, 480)))
    {
      std::cerr << "Cannot open " << argv[2] << " to write!" << std::endl;
      return 0;
    }
  
  cv::Mat frame;
  std::string windowname = "wlan reader";
  cv::namedWindow(windowname, cv::WINDOW_AUTOSIZE);

  bool recording = false;
  char key;
  
  while(true)
    {
      //Check keys
      key = cv::waitKey(1);
      //quit
      if(key == 'q')
	break;
      //start recording
      else if(key == 's')
	{
	  std::cout << "start recording..." << std::endl;
	  recording = true;
	}
      else if(key == 'e')
	{
	  std::cout << "stop recording..." << std::endl;
	  recording = false;
	}

      
      reader >> frame;
      if(frame.empty())
	{
	  std::cout << "Frame lost!" << std::endl;
	  break;
	}

      imshow(windowname, frame);
      if(recording)
	writer << frame;

    }

  reader.release();
  writer.release();
  
  return 0;
}
