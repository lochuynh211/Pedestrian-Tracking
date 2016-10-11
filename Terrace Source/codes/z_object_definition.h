#ifndef Z_OBJECT_DEFINITION_H
#define Z_OBJECT_DEFINITION_H

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
         
#include <fstream>
#include <opencv2\highgui\highgui.hpp>

#include "rectangle.h"

using namespace std;
using namespace cv;

#define HEIGHT_MIN 1000
#define HEIGHT_MAX 2000
#define HEIGHT_AVERAGE 2000
#define HEIGHT_STEP 100
class z_object_definition
{
	public: 
		int posx, posy;
		int width;
		int height;
		int status;   //0: begin, 1: new, 2: update 3: death
		int nFrameExist;
		double prob;

		z_object_definition();
		~z_object_definition();
};


#endif