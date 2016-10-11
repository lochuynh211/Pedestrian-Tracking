#ifndef Z_RandomFunction_H
#define Z_RandomFunction_H

#include <iostream>
#include <vector>
#include <math.h>
#include <random>
#include <functional>
#include <map>
#include <opencv2\highgui\highgui.hpp>
         
using namespace std;
using namespace cv;

//step move
#define STEP_BIRTH 0
#define STEP_DEATH 1
#define STEP_UPDATE 2

#define STEP_NUMBER 3



class RandomFunction
{
	private:
		std::random_device rd; //random engine

		//step move variable
		vector<int> stepChain;
		int countStepChain;

		//entrance variable
		vector<int> entranceChain;
		bool entranceChainReady;
		int countEntranceChain;
		
		//discreteSampling creator
		vector<int> discreteSampling(vector<double> probVect, int nSample);

		//return the value of uniform distribution
		int uniformSampling(int min, int max);

		//return the value of gaussian distribution
		double normalSampling(int mean, int cov);
		
		
	public:
		RandomFunction();

		//step move function
		void stepChainCreator(vector<double> probVect, int nSample);
		int stepChainGet();

		//entrance chain function
		void entranceChainCreator(vector<double> probVect, int nSample);
		int entranceChainGet();
		bool &entranceChainIsReady() {return entranceChainReady;}

		//choose delete object function
		int deleteObjectMovement(int sizeOfList);

		//for update the object properties
		int updateObjectMovement(int sizeOfList); //choose the object to update
		double updateLocation(CvPoint currentLocation, CvPoint expectedLocation, CvPoint& newLocation, CvPoint maxValue); //choose the location
		

};

#endif