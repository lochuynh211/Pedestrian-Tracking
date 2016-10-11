#ifndef z_Terrace_H
#define z_Terrace_H

#include <iostream>
#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string>

#include <fstream>
#include <opencv2\highgui\highgui.hpp>
#include "z_BackgroundModel.h"
#include "z_HumanModel.h"
#include "z_RandomFunction.h"




using namespace std;
using namespace cv;

#define nCam  4
static const char* ter_video0 = "EPFL data\\terrace1-c0.avi";
static const char* ter_video1 = "EPFL data\\terrace1-c1.avi";
static const char* ter_video2 = "EPFL data\\terrace1-c2.avi";
static const char* ter_video3 = "EPFL data\\terrace1-c3.avi";
static const char* logFileName = "C:\\data\\Log.txt";

#define ENTRANCE_X_MIN 0
#define ENTRANCE_Y_MIN 15
#define ENTRANCE_X_MAX 5
#define ENTRANCE_Y_MAX 19

#define MAX_NSAMPLE 3000

//const in stochastic process

//prior function constant
#define COMFORTABLE_ZONE_CONST	10
#define COMFORTABLE_ZONE_DIST	2  //distance to accept in comfortable zone
#define NUMBER_OF_PEOPLE_CONST	10

//likelihood constant
#define GHOST_DETECTION_CONST	10

//using for MCMC
#define MCMC_BIRTH_ACCEPT_RATIO	0.95
#define MCMC_DEATH_ACCEPT_RATIO	0.99
#define MCMC_UPDATE_ACCEPT_RATIO 0.95

#define STEP_MOVE 4


class Terrace
{
	private:

		//capture variable - using when not directly read the existed image
		CvCapture* capture[nCam];
		int frameRate;
		int frameWidth;
		int frameHeight;
		int maxFrame;
		int beginTime;
		int nFrame;
		int beginFrame;

		//file name variable - using for directly read the existed image
		string preName[nCam];
		string fileName[nCam];
		string preNameBS[nCam];
		string fileNameBS[nCam];
		string preNameSave[nCam];
		string fileNameSave[nCam];
		string preNameLog;
		string fileNameLog;
		
		//image variable
		int countFrame;
		int imgWidth, imgHeight;
		IplImage* img[nCam];
		IplImage* imgBS[nCam];
		IplImage* imgBS2[nCam];


		//background model - using with background suctraction model
		BackgroundModel* bsModel;

		//projection model
		ProjectionModel* projModel;

		//random function
		RandomFunction* rndFunc; 

		//template human model using for create the hypothesis
		//HumanModel tempHumanmodel;

		//object list management
		vector<HumanModel> objectList;
		vector<vector<HumanModel>> objectListSample;
		vector<long double> objectListSamplePosterior;
		vector<double> objectListSampleBSLikelihood;
		vector<double> objectListSamplePrior;
		vector<double> objectListSampleColorLikelihood;

		double prior;
		double likelihood;
		double colorLikelihood;


		
	    int currentConfig; 
		int nSample;  //initialization for number of sample
		int identityToAdd;

		int searchIdentity(vector<HumanModel> objList, int identity);
		bool isAtEntrance(CvPoint pos);

		//object list management function
		int searchListInSapmleList(vector<HumanModel>& objList);
		bool compare2ObjectList(vector<HumanModel>& objList1, vector<HumanModel>& objList2);
		
		bool searchObjectInList(HumanModel obj, vector<HumanModel> objList);
		int maxListOfSapmleList();
		void showResult(IplImage* img, int iCam);
		void printResult(int index);
		void readResult(int countFrame);
		void testList();

		//step move management depends on the number of people
		void stepMoveManagement();

		//make the probabilities at entrance
		int entranceNumberPosition; 
		int entranceStepPos;
		vector<CvPoint> entranceGridPosition;
		vector<vector<double>> entranceProb;
		vector<int> entranceReferencePos;         //entrance reference after filter
		vector<double> entranceReferencePosProb;  //probability

		CvPoint entrancePosToGrigPos(int entrancePos);
		void entranceProbPrint();
		void entranceProbAnalysis();
		
		
		//image processing function
		void new_makeBinaryProjectionImageFromHumanModel(IplImage* img, HumanModel obj, int iCam);
		IplImage* new_makeBinaryProjectionImageFromList(vector<HumanModel> objList, int iCam);

		double countForegroundInHumanModel(IplImage* img, HumanModel obj, int iCam, double& sizeOfRect);
		double compare2BSImage(IplImage* img1, IplImage* img2);
		double countForegroundInImage(IplImage* img);
		double countForegroundInImage2(IplImage* img);
	
			//each step process function
		bool birthStep();
		bool deathStep();
		bool updateStep();
		bool MCMCStep(vector<HumanModel>& newConfig, double constant);

		//likelihood function
		double likelihoodFunc(vector<HumanModel> objList, IplImage* imgHypo[nCam]);

		double likelihoodFromBS(vector<HumanModel> objList, IplImage* imgHypo[nCam]);
		double likelihoodFromBSInit(vector<HumanModel> objList, IplImage* imgHypo[nCam]);
		double ghostDetection(vector<HumanModel> objList);
		double ghostDetectionByHumanModel(HumanModel obj, int iCam);

		//prior function
		double priorFunc(vector<HumanModel> objList);
		double priorTemporalForUpdate;
		double priorTemporalForDeath;
		double comfortableZone(vector<HumanModel> objList);
		double priorTemporal(vector<HumanModel> objList, int index);	//when update, get the priorTemporal
		
		//a posterior
		long double posteriorFunc(vector<HumanModel>& objList);
		long double posteriorFuncInit(vector<HumanModel>& objList);

		//checking for different between view
		//double viewIntersect;
		//double preViewIntersect;
		//vector<double> viewIntersectVect;

		//terrace function
		void initTerrace();

		//using for log file

		ofstream logFile;


		vector<double> scale;
		vector<double> BSpixel;

		
		


		


		//new function for checking model
		//cho udpdate
		int checkingHumanModelEachView(HumanModel obj, int iCam);
		int checkingHumanModel(HumanModel obj);
		//cho add
		int checkingHumanModelEachView2(HumanModel obj, int iCam);
		int checkingHumanModel2(HumanModel obj);


		//new variable for data driven
		vector<vector<CvPoint>> hypothesisUpdate;
		vector<int> hypothesisUpdateIndex;
		vector<HumanModel> hypothesisAdd;
		unsigned int hypothesisAddIndex;
		vector<int> hypothesisDel;

		void makeHypothesisUpdate();
		void makeHypothesisAdd();
		void makeHypothesisDel();

		//another function
		vector<string> split(const string& s, const string& delim, const bool keep_empty);

		//using for color
		IplImage* ellipseMask[nCam];
		IplImage* ellipseMaskTmp[nCam];
		IplImage* colorImg[nCam];
		vector<int> occlusionVector[nCam];

		//create occlusion vector
		int checkingOcclusion(HumanModel obj1, HumanModel obj2, int iCam);
		void initOcclusionVector(vector<HumanModel> objList);

		//making mask
		void makeEclipseMask(vector<HumanModel>& objList);
		void makeEclipseMaskEachView(vector<HumanModel>& objList, int iCam);
		bool makeEclipseMaskTmp(HumanModel& obj, int iCam, CvRect& rect);
		void prepareEclipseMaskTmp(vector<HumanModel>& objList);

		//color vector get the final result for color data
		vector<HumanModel> mainColorObjList;
		vector<HumanModel> colorObjectList;
		vector<int> idRememberVector;
		void updateColorVector(vector<HumanModel>&  objList);
		double compareToMainColorObjList(vector<HumanModel>&  objList);

		//void printResult();

		//test color
		void drawcolor(HumanModel obj);
		void drawcolor(int iCam);
		
		

		
		

		CvHistogram* calculateHistogram(int iCam);
		bool ready[nCam];

		CvHistogram* hist1;
		CvHistogram* hist2;
		CvMat*sig1, *sig2;

		//color configuration histogram
		
		vector<HumanModel> tempColorObjList;
		vector<HumanModel> colorHypothesis(vector<HumanModel> objList);
		double compare2ColorHypothesis(vector<HumanModel> objSrc, vector<HumanModel> objDes, int iCam);
		void releaseVectorMemory(vector<HumanModel>& objList);

		//show Log file 
		CvScalar getTheColor(int id);
		void showColorImage(HumanModel obj);
	
	public:
		Terrace::Terrace(int beginTime, int nFrame);
		~Terrace();

		void process(int type); //0 cooperate with BS 

		void testProjection(int type);

		void testColor();
		void testColor2();

		void showLogFile();
};

#endif