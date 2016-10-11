#ifndef Z_TERRACE_TRACKING_H
#define Z_TERRACE_TRACKING_H

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <random>
#include <functional>
#include <map>


#include <fstream>
#include <opencv2\highgui\highgui.hpp>

//#include "rectangle.h"
#include "z_EPFLdataterrace1_setup.h"
#include "z_object_definition.h"
#include "z_video_converter.h"

using namespace std;
using namespace cv;


#define MAX_OBJECT	10 
#define nCAM		4
#define nSAMPLE		1000



#define ENTRANCE_X_MIN 0
#define ENTRANCE_Y_MIN 16
#define ENTRANCE_X_MAX 2
#define ENTRANCE_Y_MAX 25

#define PI 3.14159265

class z_terrace_tracking
{
	public:

		//for object
		z_object_definition object[MAX_OBJECT];
		z_object_definition objectSample[nSAMPLE][MAX_OBJECT];
		int nObject;
		int nObjectSample[nSAMPLE];
		double probAcceptance[nSAMPLE];


		//for file reading
		string preName[nCAM];
		string fileName[nCAM];
		string preNameBS[nCAM];
		string fileNameBS[nCAM];

		//img
		int beginFrame;
		int nFrame;
		int countFrame;
		IplImage* img[nCAM];
		IplImage* imgBS[nCAM];
		IplImage* imgSynt[nCAM];

		//for entrance
		double entrance_prob[ENTRANCE_X_MAX - ENTRANCE_X_MIN + 1][ENTRANCE_Y_MAX - ENTRANCE_Y_MIN + 1];
		double sum_entrance_prob;
		int addPosSample[300];
		int coundAddSample;
		double maxEntraceProb;
		int maxEntraceProbPosX, maxEntraceProbPosY;
		


		z_terrace_tracking();
		~z_terrace_tracking();
		void process();

		void process2();
	private:


		
		//image processing
		void initSyntImg(IplImage* img);
		void makeSyntImgFromRect(IplImage* img, Rectangle* rect);
		int and_2_images(IplImage* img_BS, IplImage* img_Synt);
		
		//probability processing
		void makeEntraceProb();

		//likelyhood function
		void createSyntImg(int sampleIndex);
		int compareImage(IplImage* img1, IplImage* img2);
		long int similarityCalculation();
		int ghostCalculation(int sampleIndex);
		double likelyhoodFunction(int sampleIndex);

		//prior function
		long int overlapCost(int sampleIndex);
		double prior(int sampleIndex);

		//posterior
		double posterior(int sampleIndex);

		//random function
		double normal_distribution(double mean, double standardDeviation, double state);
		double gaussian_random();
		double uniform_random();

		//reverse jump
		void add(int sampleIndex);
		void update(int sampleIndex);

		//random variable
		std::function<double()> rnd_add1;
		std::function<double()> rnd_add2;

		std::function<double()> rnd_update;
		std::function<double()> rnd_step; //0 : birth 1: update

};



#endif
