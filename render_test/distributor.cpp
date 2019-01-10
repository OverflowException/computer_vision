#include <iostream>
#include <vector>
#include <array>
#include <ctime>

struct Color
{
  int r;
  int g;
  int b;
};

inline void saturate(int& c)
{
  if(c > 255)
    c = 255;
  else if(c < 0)
    c = 0;
}

inline int myrand(int lower, int upper)
{
  return rand() % (upper - lower) + lower;
}

int main(int argc, char** argv)
{
  if(argc != 2)
    {
      std::cout << "Usage: " << argv[0] << " [number of data sets]" << std::endl;
      return 0;
    }
  
  std::array<Color, 4> inits;
  
  inits[0] = {0, 0, 0};
  inits[1] = {255, 0, 0};
  inits[2] = {0, 255, 0};
  inits[3] = {0, 0, 255};

  srand((unsigned char)time(NULL));
  int num = atoi(argv[1]);
  for(int idx = 0; idx < num; ++idx)
    {
      for(int pos = 0; pos < 4; ++pos)
	{	  
	  saturate(inits[pos].r += myrand(-20, 20));
	  saturate(inits[pos].g += myrand(-20, 20));
	  saturate(inits[pos].b += myrand(-20, 20));
			  
	  
	  std::cout << inits[pos].r << " "
	       << inits[pos].g << " "
	       << inits[pos].b << " ";
	}
      std::cout << std::endl;
    }
  
}
