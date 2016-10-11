#include "z_BackgroundModel.h"

BackgroundModel::BackgroundModel(IplImage* view[])
{
	for (int iCam = 0; iCam < ter_nCam; iCam++)
	{
		terFirstFrame[iCam] = cvCreateImage(cvSize(view[iCam]->width, view[iCam]->height), view[iCam]->depth, view[iCam]->nChannels);
		cvCopy( view[iCam], terFirstFrame[iCam]);
	}
	initBackgroundModel();
}

void BackgroundModel::initBackgroundModel()
{
	int width	= (int) terFirstFrame[0]->width;
	int height = (int) terFirstFrame[0]->height;
	for (int iCam = 0; iCam < ter_nCam; iCam++)
	{
		// setup marks to hold results of low and high thresholding
		ter_low_threshold_mask[iCam] = cvCreateImage(cvSize(width, height), IPL_DEPTH_8U, 1);
		ter_low_threshold_mask[iCam].Ptr()->origin = IPL_ORIGIN_BL;
		
		ter_high_threshold_mask[iCam] = cvCreateImage(cvSize(width, height), IPL_DEPTH_8U, 1);
		ter_high_threshold_mask[iCam].Ptr()->origin = IPL_ORIGIN_BL;

		params[iCam].SetFrameSize(width, height);
		params[iCam].LowThreshold() = 50;
		params[iCam].HighThreshold() = 2*params[iCam].LowThreshold();	// Note: high threshold is used by post-processing 
		params[iCam].SamplingRate() = 7;
		params[iCam].LearningFrames() = 30;

		bgsModel[iCam].Initalize(params[iCam]);
		bgsModel[iCam].InitModel(terFirstFrame[iCam]);
	}
	tempImg = cvCreateImage(cvSize(width, height), 8, 3);
	frame_data = tempImg;

	
}

IplImage* BackgroundModel::makeBS(IplImage* img, int iCam, int nFrame)
{
	int width	= (int) terFirstFrame[iCam]->width;
	int height = (int) terFirstFrame[iCam]->height;
	IplImage* finalBS = cvCreateImage(cvSize(width, height), IPL_DEPTH_8U, 1);

	
	
	cvCopyImage(img, tempImg);
	frame_data.ReleaseMemory(false);	// AVI frame data is released by with the AVI capture device
	bgsModel[iCam].Subtract(nFrame, frame_data, ter_low_threshold_mask[iCam], ter_high_threshold_mask[iCam]);
	cvCopy(ter_low_threshold_mask[iCam].Ptr(), finalBS);
	//find_connected_components(finalBS, 1, 10, NULL, NULL, NULL);
	ter_low_threshold_mask[iCam].Clear();	// disable conditional updating
	bgsModel[iCam].Update(nFrame, frame_data, ter_low_threshold_mask[iCam]);
	frame_data.ReleaseMemory(true);
	return finalBS;
}

void BackgroundModel::find_connected_components(
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