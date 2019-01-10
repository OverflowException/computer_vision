#include "bmp_writer.h"
#include <iostream>

using namespace std;

// Test

Color mix(const Color& c1, const Color& c2, double p)
{
  double q = 1 - p;
  return Color(q*c1.R + p*c2.R, q*c1.G + p*c2.G, q*c1.B + p*c2.B);
}

int main(int argc, char** argv)
{
  if(argc != 14)
    {
      cout << "Not enough init values! argc = " << argc << endl;
      return 0;
    }
  
  int width = 360, height = 360;
  vector<vector<Color> > img(height, vector<Color>(width, Color(0, 0, 0)));

  Color& tl = img[0][0];
  Color& tr = img[0][width - 1];
  Color& ll = img[height - 1][0];
  Color& lr = img[height - 1][width - 1];
  
  tl = Color(atoi(argv[1]), atoi(argv[2]), atoi(argv[3]));
  tr = Color(atoi(argv[4]), atoi(argv[5]), atoi(argv[6]));
  ll = Color(atoi(argv[7]), atoi(argv[8]), atoi(argv[9]));
  lr = Color(atoi(argv[10]), atoi(argv[11]), atoi(argv[12]));
  
  for(int row = 0; row < height; ++row)
    for(int col = 0; col < width; ++col)
      {
	double hratio, vratio;
	hratio = (double)col / width;
	vratio = (double)row / height;
	Color h1 = mix(tl, ll, vratio);
	Color h2 = mix(tr, lr, vratio);
	img[row][col] = mix(h1, h2, hratio);
      }
  
  write_bmp(argv[13], 72, img);
}
