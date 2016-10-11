#include <iostream>
#include <stdio.h>
#include <stdlib.h>

#include <fstream>
#include <opencv2\highgui\highgui.hpp>

#include "rectangle.h"

using namespace std;
using namespace cv;

#define NMatrix 8


void HormoEPFL(double x, double y, double z, double&u, double&v, int icam);
void inverseHormoEPFL(double x, double y, double z, double&u, double&v, int icam);
void inverseMatrix();
void setPositionEPFL(IplImage* view2, int X, int Y, int down);
void grid_to_tv(int pos, int grid_width, int grid_height, 
                float tv_origin_x, float tv_origin_y, float tv_width,
                float tv_height, double &tv_x, double &tv_y);
Rectangle* findRectangle(int pos, IplImage* img, int icam);
void setupEPFL();
