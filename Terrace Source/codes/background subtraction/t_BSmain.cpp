/****************************************************************************
*
* main.cpp
*
* Purpose: Simple example illustrating use of BGS library.
*
* Author: Donovan Parks, June 2008
*
* Note: You will need to install the HUFFY codex at: 
*					http://neuron2.net/www.math.berkeley.edu/benrg/huffyuv.html
*
******************************************************************************/




#include "t_BSmain.h"

enum RESULT_TYPE { IMAGE_DMS, IMAGE_SM_WALLFLOWER, VIDEO };


#define CVCLOSE_ITR 1
void find_connected_components(
  IplImage *mask,
  int poly1_hull0,
  float perimScale,
  int *num,
  CvRect *bbs,
  CvPoint *centers
) {
  static CvMemStorage*   mem_storage = NULL;
  static CvSeq*          contours    = NULL;
  //CLEAN UP RAW MASK
  //
  cvMorphologyEx( mask, mask, 0, 0, CV_MOP_OPEN,  CVCLOSE_ITR );
  cvMorphologyEx( mask, mask, 0, 0, CV_MOP_CLOSE, CVCLOSE_ITR );
  //FIND CONTOURS AROUND ONLY BIGGER REGIONS
  //
  if( mem_storage==NULL ) {
    mem_storage = cvCreateMemStorage(0);
  } else {
    cvClearMemStorage(mem_storage);
  }
  CvContourScanner scanner = cvStartFindContours(
    mask,
    mem_storage,
    sizeof(CvContour),
    CV_RETR_EXTERNAL,
    CV_CHAIN_APPROX_SIMPLE
  );
  CvSeq* c;
  int numCont = 0;
  while( (c = cvFindNextContour( scanner )) != NULL ) {
    double len = cvContourPerimeter( c );
    // calculate perimeter len threshold:
    //
    double q = (mask->height + mask->width)/perimScale;
    //Get rid of blob if its perimeter is too small:
	//
    if( len < q ) {
       cvSubstituteContour( scanner, NULL );
    } else {
      // Smooth its edges if its large enough
      //
      CvSeq* c_new;
      if( poly1_hull0 ) {
        // Polygonal approximation
        //
        c_new = cvApproxPoly(
          c,
          sizeof(CvContour),
          mem_storage,
          CV_POLY_APPROX_DP,
          1,//CVCONTOUR_APPROX_LEVEL,
          0
        );
      } else {
         // Convex Hull of the segmentation
         //
         c_new = cvConvexHull2(
           c,
           mem_storage,
           CV_CLOCKWISE,
           1
         );
      }
      cvSubstituteContour( scanner, c_new );
      numCont++;
    }
  }
  contours = cvEndFindContours( &scanner );
  // Just some convenience variables
  const CvScalar CVX_WHITE = CV_RGB(0xff,0xff,0xff);
  const CvScalar CVX_BLACK = CV_RGB(0x00,0x00,0x00);
  // PAINT THE FOUND REGIONS BACK INTO THE IMAGE
  //
  cvZero( mask );
  IplImage *maskTemp;
  // CALC CENTER OF MASS AND/OR BOUNDING RECTANGLES
  //
  if(num != NULL) {
    //User wants to collect statistics
    //
    int N = *num, numFilled = 0, i=0;
    CvMoments moments;
    double M00, M01, M10;
    maskTemp = cvCloneImage(mask);
    for(i=0, c=contours; c != NULL; c = c->h_next,i++ ) {
       if(i < N) {
         // Only process up to *num of them
         //
         cvDrawContours(
           maskTemp,
           c,
           CVX_WHITE,
           CVX_WHITE,
           -1,
           CV_FILLED,
           8
         );
         // Find the center of each contour
         //
         if(centers != NULL) {
             cvMoments(maskTemp,&moments,1);
             M00 = cvGetSpatialMoment(&moments,0,0);
             M10 = cvGetSpatialMoment(&moments,1,0);
             M01 = cvGetSpatialMoment(&moments,0,1);
             centers[i].x = (int)(M10/M00);
             centers[i].y = (int)(M01/M00);
         }
         //Bounding rectangles around blobs
         //
         if(bbs != NULL) {
            bbs[i] = cvBoundingRect(c);
         }
         cvZero(maskTemp);
         numFilled++;
       }
       // Draw filled contours into mask
       //
       cvDrawContours(
         mask,
         c,
         CVX_WHITE,
         CVX_WHITE,
         -1,
         CV_FILLED,
		 8
       );
    }                               //end looping over contours
    *num = numFilled;
    cvReleaseImage( &maskTemp);
  }
  // ELSE JUST DRAW PROCESSED CONTOURS INTO THE MASK
  //
  else {
    // The user doesn’t want statistics, just draw the contours
    //
    for( c=contours; c != NULL; c = c->h_next ) {
      cvDrawContours(
      mask,
      c,
      CVX_WHITE,
      CVX_BLACK,
      -1,
      CV_FILLED,
      8
    );
  }
}
  }

void makeBS(char* inFolder, char* outFolder, char* inName, int nView, int beginFrame, int nFrame)
{
	// read data from AVI file
	//CvCapture* readerAvi = cvCaptureFromAVI("C:\\Users\\Kotani-Lab\\Documents\\Visual Studio 2010\\Projects\\POM project\\POM project\\EPFL data\\6p-c3.avi");
	string preName = createPreName(inFolder, inName, nView);
	string fileName = createFileName(preName, beginFrame);
	IplImage* firstFrame = 	cvLoadImage(fileName.c_str(), 1);
	

	// retrieve information about AVI file
	
	int width	= (int) firstFrame->width;
	int height = (int) firstFrame->height;
	
	// setup marks to hold results of low and high thresholding
	BwImage low_threshold_mask = cvCreateImage(cvSize(width, height), IPL_DEPTH_8U, 1);
	low_threshold_mask.Ptr()->origin = IPL_ORIGIN_BL;

	BwImage high_threshold_mask = cvCreateImage(cvSize(width, height), IPL_DEPTH_8U, 1);
	high_threshold_mask.Ptr()->origin = IPL_ORIGIN_BL;

	// setup buffer to hold individual frames from video stream
	RgbImage frame_data;
	frame_data.ReleaseMemory(false);	// AVI frame data is released by with the AVI capture device

	// setup AVI writers (note: you will need to install the HUFFY codex at: 
	//   http://neuron2.net/www.math.berkeley.edu/benrg/huffyuv.html)
	

	// setup background subtraction algorithm
	
	Algorithms::BackgroundSubtraction::AdaptiveMedianParams params;
	params.SetFrameSize(width, height);
	params.LowThreshold() = 50;  //50
	params.HighThreshold() = 2*params.LowThreshold();	// Note: high threshold is used by post-processing 
	params.SamplingRate() = 7;
	params.LearningFrames() = 30;

	Algorithms::BackgroundSubtraction::AdaptiveMedianBGS bgs;
	bgs.Initalize(params);
	

	/*
	Algorithms::BackgroundSubtraction::GrimsonParams params;
	params.SetFrameSize(width, height);
	params.LowThreshold() = 3.0f*3.0f;
	params.HighThreshold() = 2*params.LowThreshold();	// Note: high threshold is used by post-processing 
	params.Alpha() = 0.001f;
	params.MaxModes() = 3;

	Algorithms::BackgroundSubtraction::GrimsonGMM bgs;
	bgs.Initalize(params);
	*/

	/*
	Algorithms::BackgroundSubtraction::ZivkovicParams params;
	params.SetFrameSize(width, height);
	params.LowThreshold() = 5.0f*5.0f;
	params.HighThreshold() = 3*params.LowThreshold();	// Note: high threshold is used by post-processing 
	params.Alpha() = 0.001f;
	params.MaxModes() = 3;

	Algorithms::BackgroundSubtraction::ZivkovicAGMM bgs;
	bgs.Initalize(params);
	*/

	/*
	Algorithms::BackgroundSubtraction::MeanParams params;
	params.SetFrameSize(width, height);
	params.LowThreshold() = 3*30*30;
	params.HighThreshold() = 2*params.LowThreshold();	// Note: high threshold is used by post-processing 
	params.Alpha() = 1e-6f;
	params.LearningFrames() = 30;

	Algorithms::BackgroundSubtraction::MeanBGS bgs;
	bgs.Initalize(params);
	*/

	/*
	Algorithms::BackgroundSubtraction::WrenParams params;
	params.SetFrameSize(width, height);
	params.LowThreshold() = 3.5f*3.5f;
	params.HighThreshold() = 2*params.LowThreshold();	// Note: high threshold is used by post-processing 
	params.Alpha() = 0.005f;
	params.LearningFrames() = 30;

	Algorithms::BackgroundSubtraction::WrenGA bgs;
	bgs.Initalize(params);
	*/

	/*
	Algorithms::BackgroundSubtraction::PratiParams params;
	params.SetFrameSize(width, height);
	params.LowThreshold() = 30;
	params.HighThreshold() = 2*params.LowThreshold();	// Note: high threshold is used by post-processing 
	params.SamplingRate() = 5;
	params.HistorySize() = 16;
	params.Weight() = 5;

	Algorithms::BackgroundSubtraction::PratiMediodBGS bgs;
	bgs.Initalize(params);
	*/

	/*
	Algorithms::BackgroundSubtraction::EigenbackgroundParams params;
	params.SetFrameSize(width, height);
	params.LowThreshold() = 15*15;
	params.HighThreshold() = 2*params.LowThreshold();	// Note: high threshold is used by post-processing 
	params.HistorySize() = 100;
	params.EmbeddedDim() = 20;

	Algorithms::BackgroundSubtraction::Eigenbackground bgs;
	bgs.Initalize(params);
	*/


	IplImage* tempImg = cvCreateImage(cvSize(width, height), IPL_DEPTH_8U, 1);
	IplImage* BS = cvCreateImage(cvSize(width, height), IPL_DEPTH_8U, 1);
	cvNamedWindow("original", 1);
	cvNamedWindow("BS", 1);
	cvNamedWindow("finalBS", 1);


	bgs.InitModel(firstFrame); 
	string outPreName = createPreName(outFolder, inName, nView);
	string outFileName;
	// perform background subtraction of each frame 
	for(int i = beginFrame; i <= nFrame; ++i)
	{
		fileName = createFileName(preName, i);
		outFileName = createFileName(outPreName, i);

		frame_data = cvLoadImage(fileName.c_str(), 1);

		// perform background subtraction
		bgs.Subtract(i, frame_data, low_threshold_mask, high_threshold_mask);
		if (cvWaitKey(30) == 27) return;
		// save results
		
		//cvWriteFrame(writerAvi, low_threshold_mask.Ptr());
		cvCopy(low_threshold_mask.Ptr(), tempImg);
		cvCopy(low_threshold_mask.Ptr(), BS);
		find_connected_components(tempImg, 1, 10, NULL, NULL, NULL);
		
		
		cvShowImage("original", frame_data.Ptr());
		cvShowImage("BS", BS);
		cvShowImage("finalBS", tempImg);
		//cvSaveImage(outFileName.c_str(), BS);
		// update background subtraction
		low_threshold_mask.Clear();	// disable conditional updating
		bgs.Update(i, frame_data, low_threshold_mask);

		frame_data.ReleaseMemory(true);
	}
	cvDestroyAllWindows();
}
