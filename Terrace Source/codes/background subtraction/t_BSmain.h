
#ifndef t_BSmain_H
#define t_BSmain_H

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <fstream>

using namespace std;

#pragma warning ( disable : 4800 ) 

#include <cv.h>
#include <cxcore.h>
#include <highgui.h>

#include "..\z_video_converter.h"
#include "t_AdaptiveMedianBGS.hpp"
#include "t_GrimsonGMM.hpp"
#include "t_ZivkovicAGMM.hpp"
#include "t_MeanBGS.hpp"
#include "t_WrenGA.hpp"
#include "t_PratiMediodBGS.hpp"
#include "t_Eigenbackground.hpp"



void find_connected_components(
  IplImage *mask,
  int poly1_hull0,
  float perimScale,
  int *num,
  CvRect *bbs,
  CvPoint *centers
);
void makeBS(char* inFolder, char* outFolder, char* inName, int nView, int beginFrame, int nFrame);

#endif