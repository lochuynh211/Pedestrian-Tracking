#ifndef Z_EPFLDATATERRACE1_H
#define Z_EPFLDATATERRACE1_H
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include <fstream>
#include <opencv2\highgui\highgui.hpp>

#include "rectangle.h"

using namespace std;
using namespace cv;

void test();
void initCamera();
void setPositionEPFL2(IplImage* view2, int X, int Y, int down);





#define NMatrix 8


void HormoEPFLa(double x, double y, double z, double&u, double&v, int icam);
void inverseHormoEPFLa(double x, double y, double z, double&u, double&v, int icam);
void inverseMatrixa();
void setPositionEPFLa(IplImage* view2, int X, int Y, int down);
void grid_to_tva(int pos, int grid_width, int grid_height, 
                float tv_origin_x, float tv_origin_y, float tv_width,
                float tv_height, double &tv_x, double &tv_y);
Rectangle* findRectanglea(int pos, IplImage* img, int icam);
Rectangle* findRectangle_z(int x, int y, IplImage* img, int icam, int height);
void setupEPFLa();

void markPosition(int posx, int posy, IplImage* img, int icam);

#endif