#ifndef BMP_WRITER_H
#define BMP_WRITER_H

#include <cstdio>
#include <vector>
#include <string>
#include <cassert>
#include <cmath>

struct Color
{
  Color()
  {
    R = 0;
    G = 0;
    B = 0;
  }
  template<typename T>
  Color(T a, T b, T c)
  {
    R = a > 255 ? 255 : (double)a;
    R = R < 0 ? 0 : R;
		
    G = b > 255 ? 255 : (double)b;
    G = G < 0 ? 0 : G;
		
    B = c > 255 ? 255 : (double)c;
    B = B < 0 ? 0 : B;
  }

  double R;
  double G;
  double B;
};

void write_bmp(const std::string &filename, const unsigned &dpi, const std::vector<std::vector<Color> > &grid)
{
  if(!(filename.find(".bmp") == filename.size() - 4))
    printf("WARNING! Filename doesn't have BMP extension\n");

  FILE* f;
  f = fopen(filename.c_str(), "wb");

  unsigned h = grid.size();
  unsigned w = grid[0].size();

  unsigned s = 4*w*h;
  unsigned filesize = 54 + s;

  unsigned ppm = dpi * 39.375;

  unsigned char file_header[14] = {'B', 'M', 0, 0, 0, 0, 0, 0, 0, 0, 54, 0, 0, 0};
  unsigned char info_header[40] = {40, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 24, 0};

  // File Header
  file_header[2] = filesize;
  file_header[3] = filesize>>8;
  file_header[4] = filesize>>16;
  file_header[5] = filesize>>24;
	
  // Info Header
  info_header[4] = w;
  info_header[5] = w>>8;
  info_header[6] = w>>16;
  info_header[7] = w>>24;

  info_header[8] = h;
  info_header[9] = h>>8;
  info_header[10] = h>>16;
  info_header[11] = h>>24;

  info_header[21] = s;
  info_header[22] = s>>8;
  info_header[23] = s >> 16;
  info_header[24] = s>>24;

  info_header[25] = ppm;
  info_header[26] = ppm>>8;
  info_header[27] = ppm>>16;
  info_header[28] = ppm>>24;
	
  info_header[29] = info_header[25];
  info_header[30] = info_header[26];
  info_header[31] = info_header[27];
  info_header[32] = info_header[28];

  fwrite(file_header, 1, 14, f);
  fwrite(info_header, 1, 40, f);

  // Writing grid values
  for(unsigned i = 0; i < h; i++)
    {
      for(unsigned j = 0; j < w; j++)
	{
	  unsigned char col[3] = {(unsigned char)floor(grid[i][j].B),
				  (unsigned char)floor(grid[i][j].G),
				  (unsigned char)floor(grid[i][j].R)};
	  fwrite(col, 1, 3, f);
	}
    }
  fclose(f);
}

#endif
