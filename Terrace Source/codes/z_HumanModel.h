#ifndef Z_HUMANMODEL_H
#define Z_HUMANMODEL_H

#include <iostream>
#include <cv.h>
#include <cxcore.h>
#include <highgui.h>
#include <opencv2\highgui\highgui.hpp>

#include "z_ProjectionModel.h"
         
using namespace std;
using namespace cv;

#define HEIGHT_MIN 1000
#define HEIGHT_MAX 2500
#define HEIGHT_AVERAGE 2000
#define HEIGHT_STEP 100

#define STATE_NEW 1
#define STATE_LEARNING 2
#define STATE_NORMAL 3

#define WIDTH_AVERAGE 500

#define nCam 4
#define nPart 7

const int rBins = 8;
const int gBins = 8;
const int bBins = 8;
static int histSize[3] = { rBins, gBins, bBins};
static float rRanges[3] = { 0, 255 }; // hue is [0,180]
static float gRanges[3] = { 0, 255 };
static float bRanges[3] = { 0, 255 };
static float* histRanges[3] = { rRanges, gRanges, bRanges };


class HumanModel
{
	private: 

		//simple model
		CvPoint gridPos;
		int width;
		int height;
		int status;
		
		double prob;
		
		bool firstEntrance;
		int nFrame;
		int id;

		CvHistogram* previousHist[nCam][nPart];
		vector<CvHistogram*> learnedHist[nCam][nPart];
		

	public:	
		HumanModel();
		HumanModel(CvPoint newPos, int h, int w, int state);
		~HumanModel();
		void allocateMemory();
		void releaseMemory();

		CvPoint &gridPosition() {return gridPos;}
		int &humanHeight() {return height;}
		int &humanWidth() {return width;}
		int &humanStatus() {return status;}
		int &existedNFrame() {return nFrame;}
		int &getIdentity() {return id;}
		bool &getFullView(int iCam) {return fullView[iCam];}
		bool isFirstEntrance() {return firstEntrance;}

		void setGridPosition(CvPoint newPos, ProjectionModel* projModel); 
		void changeGridPosition(CvPoint newPos, ProjectionModel* projModel); 
		void setHumanHeight(int h, ProjectionModel* projModel);
		void setHumanWidth(int w) {width = w;}
		void setHumanStatus(int state) {status = state;}
		void increaseExistedNFrame() {nFrame++;}
		void setNFrame(int n) {nFrame = n;}
		void setIdentity(int idx) {id = idx;}
		void setFullView(int iCam) {fullView[iCam] = true;}

		void setFirstEntrance(bool b) {firstEntrance = b;}

		//compare two object
		bool operator== (HumanModel obj1);
		
		void setEcclipseArea(int iCam, int width, int height);
		void setBin(int iCam, int iPart, int R, int G, int B);
		double checkingVisisble(int iCam, int iPart);
		CvHistogram* getPreviousHist(int iCam, int iPart);

		int countNumberFullView(); 
		void updateModel(HumanModel obj);
		double compareHist(HumanModel obj);



		//public variable
		CvRect rect[nCam];
		CvRect originalRect[nCam];
		bool visible[nCam];

		void printColor(ofstream& logFile);

		//ellispe variable
		int ellipseStatus;
		IplImage* ellipseMaskTmp[nCam];
		void makeEllipseMaskTmp(int imgWidth, int imgHeight);
		void updateEllipseMaskTmp();
		void releaseEllipseMaskTmp();

		double visibleArea[nCam][nPart];
		double ecclipseArea[nCam];
		bool fullView[nCam];
		int learningRate[nCam];

		void readHist(int value, int pos, int iCam, int iPart);

};


#endif