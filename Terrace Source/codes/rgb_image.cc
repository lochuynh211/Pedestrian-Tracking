
//////////////////////////////////////////////////////////////////////////////////
// This program is free software: you can redistribute it and/or modify         //
// it under the terms of the version 3 of the GNU General Public License        //
// as published by the Free Software Foundation.                                //
//                                                                              //
// This program is distributed in the hope that it will be useful, but          //
// WITHOUT ANY WARRANTY; without even the implied warranty of                   //
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU             //
// General Public License for more details.                                     //
//                                                                              //
// You should have received a copy of the GNU General Public License            //
// along with this program. If not, see <http://www.gnu.org/licenses/>.         //
//                                                                              //
// Written by Francois Fleuret                                                  //
// (C) Ecole Polytechnique Federale de Lausanne                                 //
// Contact <pom@epfl.ch> for comments & bug reports                             //
//////////////////////////////////////////////////////////////////////////////////


#include "rgb_image.h"


void RGBImage::allocate() {
  _bit_plans = new unsigned char **[RGB_DEPTH];
  _bit_lines = new unsigned char *[RGB_DEPTH * _height];
  _bit_map = new unsigned char [_width * _height * RGB_DEPTH];
  for(int k = 0; k < RGB_DEPTH; k++) _bit_plans[k] = _bit_lines + k * _height;
  for(int k = 0; k < RGB_DEPTH * _height; k++) _bit_lines[k] = _bit_map + k * _width;
}

void RGBImage::deallocate() {
  delete[] _bit_plans;
  delete[] _bit_lines;
  delete[] _bit_map;
}

RGBImage::RGBImage() : _bit_plans(0), _bit_lines(0), _bit_map(0) { }

RGBImage::RGBImage(int width, int height) : _width(width), _height(height) {
  allocate();
  memset(_bit_map, 0, _width * _height * RGB_DEPTH * sizeof(unsigned char));
}

RGBImage::~RGBImage() {
  deallocate();
}

/*
void RGBImage::write_ppm(char *filename) {
  FILE *outfile;

  if ((outfile = fopen (filename, "wb")) == 0) {
    fprintf (stderr, "Can't open %s for reading\n", filename);
    exit(1);
  }

  fprintf(outfile, "P6\n%d %d\n255\n", _width, _height);

  char *raw = new char[_width * _height * 3];

  int k = 0;
  for(int y = 0; y < _height; y++) for(int x = 0; x < _width; x++) {
    raw[k++] = _bit_map[x + _width * (y + _height * RED)];
    raw[k++] = _bit_map[x + _width * (y + _height * GREEN)];
    raw[k++] = _bit_map[x + _width * (y + _height * BLUE)];
  }

  fwrite((void *) raw, sizeof(unsigned char), _width * _height * 3, outfile);
  fclose(outfile);

  delete[] raw;
}

void RGBImage::read_ppm(char *filename) {
  const int buffer_size = 1024;
  FILE *infile;
  char buffer[buffer_size];
  int max;
  char *res;
  size_t res_size;

  deallocate();

  if((infile = fopen (filename, "r")) == 0) {
    fprintf (stderr, "Can't open %s for reading\n", filename);
    exit(1);
  }

  res = fgets(buffer, buffer_size, infile);

  if(strncmp(buffer, "P6", 2) == 0) {

    do {
      res = fgets(buffer, buffer_size, infile);
    } while((buffer[0] < '0') || (buffer[0] > '9'));
    sscanf(buffer, "%d %d", &_width, &_height);
    res = fgets(buffer, buffer_size, infile);
    sscanf(buffer, "%d", &max);

    allocate();

    unsigned char *raw = new unsigned char[_width * _height * RGB_DEPTH];
    res_size = fread(raw, sizeof(unsigned char), _width * _height * RGB_DEPTH, infile);

    int k = 0;
    for(int y = 0; y < _height; y++) for(int x = 0; x < _width; x++) {
      _bit_plans[RED][y][x] = raw[k++];
      _bit_plans[GREEN][y][x] = raw[k++];
      _bit_plans[BLUE][y][x] = raw[k++];
    }

    delete[] raw;

  } else if(strncmp(buffer, "P5", 2) == 0) {

    do {
      res = fgets(buffer, buffer_size, infile);
    } while((buffer[0] < '0') || (buffer[0] > '9'));
    sscanf(buffer, "%d %d", &_width, &_height);
    res = fgets(buffer, buffer_size, infile);
    sscanf(buffer, "%d", &max);

    allocate();

    unsigned char *pixbuf = new unsigned char[_width * _height];
    res_size = fread(buffer, sizeof(unsigned char), _width * _height, infile);

    int k = 0, l = 0;
    for(int y = 0; y < _height; y++) for(int x = 0; x < _width; x++) {
      unsigned char c = pixbuf[k++];
      _bit_map[l++] = c;
      _bit_map[l++] = c;
      _bit_map[l++] = c;
    }

    delete[] pixbuf;

  } else {
    cerr << "Can not read ppm of type [" << buffer << "] from " << filename << ".\n";
    exit(1);
  }
}
*/
void RGBImage::read_png(char* name) {
  // This is the number of bytes the read_png routine will read to
  // decide if the file is a PNG or not. According to the png
  // documentation, it can be 1 to 8 bytes, 8 being the max and the
  // best.

  IplImage* img = cvLoadImage(name, 0);
  _width = img->width;
  _height = img->height;
  allocate();

  int color_type = 0; //0 grayscale   1 RGB
  switch (color_type) {
  case 0:
    {
      unsigned char pixel = 0;
      for (int y = 0; y < _height; y++) for (int x = 0; x < _width; x++) {
		  pixel = (unsigned char)(img->imageData[y*img->widthStep + x]);
        _bit_plans[RED][y][x] = pixel;
        _bit_plans[GREEN][y][x] = pixel;
        _bit_plans[BLUE][y][x] = pixel;
      }
    }
    break;

  case 1:
    break;
  }

  cvReleaseImage(&img);// release memory
}

void RGBImage::write_png(char *name) {
  
  IplImage* des = cvCreateImage(cvSize(_width,_height),8,3);	
  int h = des->widthStep;
  for (int y = 0; y < _height; y++) {
	  int k = 0;
    for (int x = 0; x < _width; x++) {
		des->imageData[y*h + k] = _bit_map[x + _width * (y + _height * BLUE)];
		k++;
        des->imageData[y*h + k] = _bit_map[x + _width * (y + _height * GREEN)];
		k++;
        des->imageData[y*h + k] = _bit_map[x + _width * (y + _height * RED)];
		k++;
    }
  }
  cvSaveImage(name,des,0);
  cvReleaseImage(&des);
}
