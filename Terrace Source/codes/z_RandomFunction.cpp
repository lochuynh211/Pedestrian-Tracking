#include "z_RandomFunction.h"


RandomFunction::RandomFunction()
{
}

int RandomFunction::uniformSampling(int min, int max)
{
	std::uniform_int_distribution<> rnd_uniform(min, max);
	std::mt19937 engine(rd());
	//rnd_uniform.reset();
	std::function<int()> rnd_uniformFunc = std::bind(rnd_uniform, engine);
	return rnd_uniformFunc();
}
double RandomFunction::normalSampling(int mean, int covariance)
{
	std::normal_distribution<double> rnd_normal(mean, covariance);
	std::mt19937 engine(rd());
	//rnd_normal.reset();
	//engine.distribution().reset();
	std::function<double()> rnd_normalFunc = std::bind(rnd_normal, engine);
	return rnd_normalFunc();
}

vector<int> RandomFunction::discreteSampling(vector<double> probVect, int nSample)
{
	vector<int> sample; 
	int nElement = probVect.size();
	double probMin, probMax;
	probMin = probMax = probVect.at(0);
	for (int i = 1; i < nElement; i++)
	{
		if (probVect.at(i) < probMin)  probMin = probVect.at(i);
		if (probVect.at(i) > probMax)  probMax = probVect.at(i);
	}
	probMin = 0; 
	//probMax = 1;
	std::uniform_int_distribution<> rnd_element(0, nElement - 1);
	std::uniform_real_distribution<double> rnd_prob(probMin, probMax);
	//rnd_element.reset();
	//rnd_prob.reset();
	std::mt19937 engine1(rd());// knuth_b fails in MSVC2010, but compiles in GCC
	std::mt19937 engine2(rd());
	std::function<int()> rnd_elementFunc = std::bind(rnd_element, engine1);
	std::function<double()> rnd_probFunc = std::bind(rnd_prob, engine2);
	for (int i = 0; i < nSample; i++)
		{
			while (true)
			{
				int e = rnd_elementFunc();
				double p = rnd_probFunc();
				//cout<<e<<"    "<<p<<"\n";
				if (p <= probVect[e])
				{
					//cout<<e<<"    "<<p<<"\n";
					sample.push_back(e);
					break;
				}
			}
		}
	return sample; 
}

/////////////////////////////////////////////////////////
////////////////step chain///////////////////////////////
/////////////////////////////////////////////////////////

void RandomFunction::stepChainCreator(vector<double> probVect, int nSample)
{
	stepChain.clear();
	stepChain = discreteSampling(probVect, nSample);
	countStepChain = 0;

}
int RandomFunction::stepChainGet()
{
	int result = stepChain.at(countStepChain);
	countStepChain++;
	if (countStepChain >= stepChain.size()) countStepChain = 0;
	return result;
}

/////////////////////////////////////////////////////////
////////////////entrance chain///////////////////////////////
/////////////////////////////////////////////////////////

void RandomFunction::entranceChainCreator(vector<double> probVect, int nSample)
{
	entranceChain.clear();
	if (probVect.size() == 0) 
	{
		entranceChainReady = false;
		return;
	}
	entranceChain = discreteSampling(probVect, nSample);
	entranceChainReady = true;
	countEntranceChain = 0;
}
int RandomFunction::entranceChainGet()
{
	int result = entranceChain.at(countEntranceChain);
	countEntranceChain++;
	if (countEntranceChain >= entranceChain.size()) countEntranceChain = 0;
	return result;
}

/////////////////////////////////////////////////////////
////////////////delete object///////////////////////////////
/////////////////////////////////////////////////////////
int RandomFunction::deleteObjectMovement(int sizeOfList)
{
	return uniformSampling(0, sizeOfList - 1);
}

/////////////////////////////////////////////////////////
////////////////update object///////////////////////////////
/////////////////////////////////////////////////////////

int RandomFunction::updateObjectMovement(int sizeOfList)
{
	if (sizeOfList <= 0) return 0;
	return uniformSampling(0, sizeOfList - 1);
}

double RandomFunction::updateLocation(CvPoint currentLocation, CvPoint expectedLocation, CvPoint& newLocation, CvPoint maxValue)
{
	int meanX = expectedLocation.x;
	int meanY = expectedLocation.y;
	int covX = abs(expectedLocation.x - currentLocation.x);
	int covY = abs(expectedLocation.y - currentLocation.y);
	if (covX == 0) covX = 2;
	if (covY == 0) covY = 2;
	newLocation.x = cvRound(normalSampling(meanX, covX));
	newLocation.y = cvRound(normalSampling(meanY, covY));
	if (newLocation.x < 0) newLocation.x = 0;
	else if (newLocation.x > maxValue.x) newLocation.x = maxValue.x;
	if (newLocation.y < 0) newLocation.y = 0;
	else if (newLocation.y > maxValue.y) newLocation.y = maxValue.y;
	return 0;
}