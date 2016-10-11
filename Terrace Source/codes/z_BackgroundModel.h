#ifndef Z_BACKGROUNDMODEL_H
#define Z_BACKGROUNDMODEL_H

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <fstream>

using namespace std;

#pragma warning ( disable : 4800 ) 

#include <cv.h>
#include <cxcore.h>
#include <highgui.h>

#include "z_video_converter.h"
#include "background subtraction\t_Bgs.hpp"
#include "background subtraction\t_AdaptiveMedianBGS.hpp"
#include "background subtraction\t_GrimsonGMM.hpp"
#include "background subtraction\t_ZivkovicAGMM.hpp"
#include "background subtraction\t_MeanBGS.hpp"
#include "background subtraction\t_WrenGA.hpp"
#include "background subtraction\t_PratiMediodBGS.hpp"
#include "background subtraction\t_Eigenbackground.hpp"




enum RESULT_TYPE { IMAGE_DMS, IMAGE_SM_WALLFLOWER, VIDEO };
#define CVCLOSE_ITR 1


const int ter_nCam = 4;

class BackgroundModel
{
	private:


		//terrace variables
		IplImage* terFirstFrame[ter_nCam];
		BwImage ter_low_threshold_mask[ter_nCam];
		BwImage ter_high_threshold_mask[ter_nCam];
		Algorithms::BackgroundSubtraction::AdaptiveMedianParams params[ter_nCam];
		Algorithms::BackgroundSubtraction::AdaptiveMedianBGS bgsModel[ter_nCam];
		RgbImage frame_data; 
		IplImage* tempImg;

		//terrace function
		void initBackgroundModel();
		void BackgroundModel::find_connected_components(IplImage *mask, int poly1_hull0, float perimScale, int *num, CvRect *bbs, CvPoint *centers);

	public:
		BackgroundModel(IplImage* view[]);

		//terrace function
		IplImage* makeBS(IplImage* img, int iCam, int nFrameS);
};



#endif