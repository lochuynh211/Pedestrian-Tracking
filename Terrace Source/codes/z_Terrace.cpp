#include "z_Terrace.h"


///////////////////////////////////////////////
//init all the terrace 
///////////////////////////////////////////////
Terrace::Terrace(int begin, int number)
{

	
	capture[0] =  cvCaptureFromFile(ter_video0);
	capture[1] =  cvCaptureFromFile(ter_video1);
	capture[2] =  cvCaptureFromFile(ter_video2);
	capture[3] =  cvCaptureFromFile(ter_video3);

	frameRate = (int)(cvGetCaptureProperty(capture[0], CV_CAP_PROP_FPS));
	frameWidth = (int)(cvGetCaptureProperty(capture[0], CV_CAP_PROP_FRAME_WIDTH));
	frameHeight = (int)(cvGetCaptureProperty(capture[0], CV_CAP_PROP_FRAME_HEIGHT));
	maxFrame = (int)(cvGetCaptureProperty(capture[0], CV_CAP_PROP_FRAME_COUNT));
	if (nFrame > maxFrame) nFrame = maxFrame;

	beginTime = begin;
	nFrame = number;
	beginFrame = beginTime*frameRate;
	countFrame = 0;
	identityToAdd = 0;

	initTerrace();
}

void Terrace::initTerrace()
{
	//background init
	for (int iCam = 0; iCam < nCam; iCam++)
	{
		cvSetCaptureProperty(capture[iCam], CV_CAP_PROP_POS_FRAMES, 0);
		cvGrabFrame(capture[iCam]);
		img[iCam] = cvRetrieveFrame(capture[iCam]);
		cvSetCaptureProperty(capture[iCam], CV_CAP_PROP_POS_FRAMES, beginFrame);
	}
	//init background subtraction model
	bsModel = new BackgroundModel(img);

	//init projection model
	projModel = new ProjectionModel();

	//init random function
	rndFunc = new RandomFunction();

	//file read init
	for (int iCam = 0; iCam < nCam; iCam++)
	{
		preName[iCam] = createPreName("C:\\data\\EPFLterrace", "img", iCam);
		preNameBS[iCam] = createPreName("C:\\data\\EPFLterraceBS3", "img", iCam);
		preNameSave[iCam] = createPreName("C:\\data\\EPFLterraceResult", "img", iCam);
		fileName[iCam] = createFileName(preName[iCam], beginFrame);
		fileNameBS[iCam] = createFileName(preNameBS[iCam], beginFrame);
		img[iCam] = cvLoadImage(fileName[iCam].c_str(), 1);
		imgBS[iCam] = cvLoadImage(fileNameBS[iCam].c_str(), 0);
		imgBS2[iCam] = cvLoadImage(fileNameBS[iCam].c_str(), 0);
	}
	preNameLog = createPreName("C:\\data\\Log", "log", 0);

	imgWidth = img[0]->width;
	imgHeight = img[0]->height;

	//init variable
	nSample = 600;  //initialization for number of sample
	
	//init entrance
	entranceNumberPosition = (ENTRANCE_X_MAX - ENTRANCE_X_MIN + 1)*(ENTRANCE_Y_MAX - ENTRANCE_Y_MIN + 1);
	entranceStepPos = ENTRANCE_Y_MAX - ENTRANCE_Y_MIN + 1;
	entranceGridPosition.resize(entranceNumberPosition);
	//entranceProb.resize(entranceNumberPosition);
	for (int i = 0; i < entranceNumberPosition; i++)
	{
		entranceGridPosition.at(i) = entrancePosToGrigPos(i);
	}
	priorTemporalForUpdate = 0;
	priorTemporalForDeath = 0;
}


///////////////////////////////////////////////
//main process
///////////////////////////////////////////////
void Terrace::process(int type)
{
	cvNamedWindow("img1", 1);
	cvNamedWindow("img2", 1);
	cvNamedWindow("img0", 1);
	cvNamedWindow("img3", 1);
	cvMoveWindow("img3", 0, 0);
	cvMoveWindow("img2", 400, 0);
	cvMoveWindow("img1", 0, 400);
	cvMoveWindow("img0", 400, 400);
	for (int iCam = 0; iCam < nCam; iCam++)
	{
		colorImg[iCam] = cvCreateImage(cvSize(this->imgWidth, imgHeight), 8, 3);
		//ellipseMaskTmp[iCam] = cvCreateImage(cvSize(this->imgWidth, imgHeight), 8, 1);
		ellipseMask[iCam] = cvCreateImage(cvSize(this->imgWidth, imgHeight), 8, 1);
	}


	logFile.open(logFileName);
	countFrame = 1992;
	int checkFrame = 0;
	while(true)
	{
		countFrame++;
		if (cvWaitKey(10) == 27 || countFrame > nFrame) break;
		for (int iCam = 0; iCam < nCam; iCam++)
		{
			if (type == 1)
			{	
				cvReleaseImage(&img[iCam]);
				cvReleaseImage(&imgBS[iCam]);
				cvReleaseImage(&imgBS2[iCam]);
				fileName[iCam] = createFileName(preName[iCam], countFrame);  //cho nay can sua lai cho hop li, fai la beginFrame
				fileNameBS[iCam] = createFileName(preNameBS[iCam], countFrame); //cho nay can sua lai cho hop li, fai la beginFrame
				fileNameSave[iCam] = createFileName(preNameSave[iCam], countFrame); //cho nay can sua lai cho hop li, fai la beginFrame
				img[iCam] = cvLoadImage(fileName[iCam].c_str(), 1);
				imgBS[iCam] = cvLoadImage(fileNameBS[iCam].c_str(), 0);
				imgBS2[iCam] = cvLoadImage(fileNameBS[iCam].c_str(), 0);
			}
			else
			{
				cvReleaseImage(&imgBS[iCam]);
				cvGrabFrame(capture[iCam]);
				img[iCam] = cvRetrieveFrame(capture[iCam]);
				imgBS[iCam] = bsModel->makeBS(img[iCam], iCam, countFrame);
				imgBS2[iCam] = cvLoadImage(fileNameBS[iCam].c_str(), 0);
			}
		}

		
		if (countFrame == 1993) readResult(countFrame - 1);
		BSpixel.clear();
		for (int iCam = 0; iCam < nCam; iCam++)
		{
			BSpixel.push_back(countForegroundInImage(imgBS[iCam]));
		}
		//make new BS using for entrace
		for (int iCam = 0; iCam < nCam; iCam++)
		{
			for (unsigned int i = 0; i < objectList.size(); i++)
			{
				if (objectList[i].visible[iCam]) cvRectangleR(imgBS2[iCam], objectList[i].rect[iCam] ,cvScalarAll(0), CV_FILLED);
			}
		}

		for (unsigned int i = 0; i < objectList.size(); i++)
		{
			objectList.at(i).setFirstEntrance(false);
		}
		if (objectList.size() > 1 && objectList[1].humanHeight() != 1800) objectList[1].setHumanHeight(1800, projModel);

	
		objectListSample.clear();
		objectListSamplePosterior.clear();
		objectListSamplePrior.clear();
		objectListSampleBSLikelihood.clear();
		objectListSampleColorLikelihood.clear();
		objectListSample.push_back(objectList);
		
		//viewIntersectVect.clear();
		objectListSamplePosterior.push_back(posteriorFuncInit(objectListSample[0]));   //save the posterior for config
		//viewIntersectVect.push_back(preViewIntersect);
		currentConfig = 0;

		


		makeHypothesisUpdate();
		stepMoveManagement();	
		entranceProbAnalysis();
		//entranceProbPrint();

		for (unsigned int i = 0; i < hypothesisAdd.size(); i++)
		{
			hypothesisAdd[i].makeEllipseMaskTmp(imgWidth, imgHeight);
		}

		
		//for each step
		for (int i = 0; i < nSample; i++)
		{
			switch(rndFunc->stepChainGet())
			{
				case STEP_BIRTH:
					birthStep();
					break;
				case STEP_DEATH:
					deathStep();
					break;
				case STEP_UPDATE:
					updateStep();
					break;
			}
		}
		
		
		int result = maxListOfSapmleList();
		objectList = objectListSample.at(result);
		if (hypothesisAdd.size() > 0) 
			cout<<"xxx\n";
		for (unsigned int i = 0; i < hypothesisAdd.size(); i++)
		{
			if (!searchObjectInList(hypothesisAdd[i], objectList))
			{
				hypothesisAdd[i].releaseEllipseMaskTmp();
			}
		}
		int k;
		for (unsigned int i = 0; i < objectList.size(); i++)
		{
			k = checkingHumanModel(objectList[i]);
			if (k == 0 && (objectList[i].existedNFrame() > 10 || !isAtEntrance(objectList[i].gridPosition())) && (objectList[i].getIdentity() == -1))
			{
				objectList[i].setIdentity(identityToAdd);
				identityToAdd++;
			}
			objectList[i].increaseExistedNFrame();

			if (k == 0 && !isAtEntrance(objectList[i].gridPosition()) && searchIdentity(mainColorObjList, objectList[i].getIdentity()) == -1)
			{
				idRememberVector.push_back(objectList[i].getIdentity());
				cout<<"aaaadddddddddd identity to vector\n";
			}
		}
		updateColorVector(objectList);
		printResult(result);

		showResult(imgBS[2], 2);
		showResult(imgBS[1], 1);
		showResult(imgBS[0], 0);
		showResult(imgBS[3], 3);
		
		for (int iCam = 0; iCam < nCam; iCam++)
		{
			cvSaveImage(fileNameSave[iCam].c_str(), imgBS[iCam]);
		}
		
		cvShowImage("img2", imgBS[2]);	
		cvShowImage("img1", imgBS[1]);	
		cvShowImage("img0", imgBS[0]);	
		cvShowImage("img3", imgBS[3]);	
		//if (cvWaitKey(10000) == 'q') continue;

	}
	logFile.close();
	cvDestroyAllWindows();
	for (int iCam = 0; iCam < nCam; iCam++)
	{
		cvReleaseCapture(&capture[iCam]);
	}

}



///////////////////////////////////////////////
//make the probabilities at entrance
///////////////////////////////////////////////
CvPoint Terrace::entrancePosToGrigPos(int entrancePos)
{
	int x = entrancePos/entranceStepPos + ENTRANCE_X_MIN;
	int y = entrancePos%entranceStepPos + ENTRANCE_Y_MIN;
	return cvPoint(x, y);
}
void  Terrace::entranceProbPrint()
{

	/*
	int pos;
	for (int i = 0; i < nSample/2; i++)
	{
		pos = entranceReferencePos.at(rndFunc->entranceChainGet());
		cout<<"pos:"<<pos<<"    "<<pos<<"\n";
		cout<<"prob at "<<entranceGridPosition.at(pos).x<<" and "<<entranceGridPosition.at(pos).y<<" are "<<entranceProb.at(pos)<<"\n";
	}
	*/
}
void  Terrace::entranceProbAnalysis()
{
	HumanModel obj;
	
	hypothesisAdd.clear();
	hypothesisAddIndex = 0;
	if (entranceProb.size() > 0)
	{
		for (int pos = 0; pos < entranceNumberPosition; pos++)
		{
			entranceProb[pos].clear();
		}
		entranceProb.clear();
	}
	vector<int> countView;
	vector<double> tempVect;
	double temp;
	double sizeOfRect = 0;
	double sumAllEntranceProb = 0;
	int count = 0;
	for (int pos = 0; pos < entranceNumberPosition; pos++)
	{
		obj.setGridPosition(entranceGridPosition.at(pos), projModel);
		count = 0;
		tempVect.clear();
		for (int iCam = 0; iCam < nCam; iCam++)
		{
			temp = countForegroundInHumanModel(imgBS2[iCam], obj, iCam, sizeOfRect);
			if (temp != -1 ) temp = temp*2/sizeOfRect;
			tempVect.push_back(temp);
			if (temp > 0.2) count++;
		}
		entranceProb.push_back(tempVect);
		countView.push_back(count);
	}

	//filter too small probabilities
	double sum;
	for (int pos = 0; pos < entranceNumberPosition; pos++)
	{
		sum = 0;
		for (int iCam = 0; iCam < nCam; iCam++)
		{
			if (entranceProb[pos][iCam] != -1) sum += entranceProb[pos][iCam];
		}
		bool check = false;
		if (countView[pos] == 2)
		{
			if (entranceProb[pos][2] > 0.2 && entranceProb[pos][1] > 0.5) check = true;
		}
		else if (countView[pos] == 1)
		{
			if (entranceProb[pos][1] > 0.4) check = true;
			if (entranceProb[pos][2] > 0.4) check = true;
		}
		if (check)
		{
			obj.setGridPosition(entranceGridPosition.at(pos), projModel);
			if (checkingHumanModel2(obj) == 0)
			{
				hypothesisAdd.push_back(obj);
			}
		}	
	}
}


///////////////////////////////////
//Image processing function///////
/////////////////////////////////

void Terrace::new_makeBinaryProjectionImageFromHumanModel(IplImage* img, HumanModel obj, int iCam)
{
	CvRect rect;
	bool visible; 
	visible = obj.visible[iCam];
	rect = obj.rect[iCam];
	
	if (visible)
	{
		int xMax = rect.x + rect.width;
		int yMax = rect.y + rect.height;
		uchar* ptr;
		for (int y = rect.y; y <= yMax; y++)
		{ 
			ptr = (uchar*)(img->imageData + y * img->widthStep);
			for (int x = rect.x; x <= xMax; x++)
			{
				ptr[x]++;
			}
		}
	}
}
IplImage* Terrace::new_makeBinaryProjectionImageFromList(vector<HumanModel> objList, int iCam)
{
	IplImage* img = cvCreateImage(cvSize(imgWidth, imgHeight), 8, 1);
	cvZero(img);
	for (unsigned int i = 0; i < objList.size(); i++)
	{
		new_makeBinaryProjectionImageFromHumanModel(img, objList.at(i), iCam);
	}
	return img;
}


double Terrace::countForegroundInHumanModel(IplImage* img, HumanModel obj, int iCam, double& sizeOfRect)
{
	double count = 0;
	CvRect rect;
	bool visible; 
	visible = obj.visible[iCam];
	rect = obj.rect[iCam];
	if (!visible) 
	{
		sizeOfRect = 0;
		return -1;
	}
	sizeOfRect = rect.width*rect.height;
	int xMax = rect.x + rect.width;
	int yMax = rect.y + rect.height;
	uchar* ptr;
	for (int y = rect.y; y < yMax; y++)
	{ 
		ptr = (uchar*)(img->imageData + y * img->widthStep);
		for (int x = rect.x; x < xMax; x++)
		{
			if (ptr[x] != 0) count++;
		}
	}
	return count;
}
double Terrace::countForegroundInImage(IplImage* img)
{
	
	double count = 0;
	uchar* ptr;
	for (int y = 0; y < imgHeight; y++)
	{ 
		ptr = (uchar*)(img->imageData + y * img->widthStep);
		for (int x = 0; x < imgWidth; x++)
		{
			if (ptr[x] != 0) 
			{
				count++;
			}
		}
	}
	return count;
}
double Terrace::countForegroundInImage2(IplImage* img)
{
	
	double count = 0;
	uchar* ptr;
	for (int y = 0; y < imgHeight; y++)
	{ 
		ptr = (uchar*)(img->imageData + y * img->widthStep);
		for (int x = 0; x < imgWidth; x++)
		{
			if (ptr[x] != 0) 
			{
				count+=ptr[x];
			}
		}
	}
	return count;
}

double Terrace::compare2BSImage(IplImage* img1, IplImage* img2)  //BS vs Hypo
{
	double count = 0;
	uchar* ptr1;
	uchar* ptr2;
	for (int y = 0; y < imgHeight; y++)
	{ 
		ptr1 = (uchar*)(img1->imageData + y * img1->widthStep);
		ptr2 = (uchar*)(img2->imageData + y * img2->widthStep);
		for (int x = 0; x < imgWidth; x++)
		{
			//use with the new make hypothesis image
			if (ptr1[x] != 0 && ptr2[x] == 0)
			{
				count += 8;  //2   1-0
			}
			else if (ptr1[x] == 0 && ptr2[x] != 0)
			{
				count += 2*ptr2[x];  // 0-1
			}
			else if (ptr1[x] != 0 && ptr2[x] != 0)
			{
				count += 1 - ptr2[x];
			}
		}
	}
	return count;
	
}

///////////////////////////////////
//Other useful function///////
/////////////////////////////////

int Terrace::searchListInSapmleList(vector<HumanModel>& objList)
{
	for (unsigned int i = 0; i < objectListSample.size(); i++)
	{
		if (this->objectListSample.at(i).size() == objList.size())
		{
			if (compare2ObjectList(objectListSample[i], objList)) return i;
		}
	}
	return -1;
}

bool Terrace::compare2ObjectList(vector<HumanModel>& objList1, vector<HumanModel>& objList2)
{
	/*
	bool check = true;
	while (check)
	{
		if (objList1.size() == 0) return true;
		check = false;
		HumanModel obj1 = objList1.at(0);
		for (int i = 0; i < objList2.size(); i++)
		{
			if (objList2.at(i) == obj1)
			{
				check = true;
				objList2.erase(objList2.begin() + i);
				objList1.erase(objList1.begin());
				break;
			}
		}
		if (!check) 
		{
			return false;
		}
	}
	return false;
	*/
	for (unsigned int i = 0; i < objList1.size(); i++)
	{
		if (!(objList1[i] == objList2[i])) return false;
	}
	return true;
}

bool Terrace::searchObjectInList(HumanModel obj, vector<HumanModel> objList)
{
	for (unsigned int i = 0; i < objList.size(); i++)
	{
		if (obj == objList.at(i)) return true;
	}
	return false; 
}
int Terrace::maxListOfSapmleList()
{
	long double max = 10000;
	int index = -1;
	for (unsigned int i = 0; i < objectListSample.size(); i++)
	{
		//colorObjectList
		if (objectListSamplePosterior.at(i) < max) 
		{
			max = objectListSamplePosterior.at(i);
			index = i;
		}
	}
	return index;
}
void Terrace::showResult(IplImage* img, int iCam)
{
	HumanModel obj;
	CvRect rect;
	bool visible;
	for (unsigned int i = 0; i < objectList.size(); i++)
	{
		obj = objectList.at(i);
		visible = obj.visible[iCam];
		rect = obj.rect[iCam];
		if (visible) cvRectangleR(img, rect, CV_RGB(255, 255, 255), 1, 8, 0);
	}
}
void Terrace::printResult(int index)
{
	cout<<"\nnFrame "<<countFrame<<"\n\n";
	cout<<"prior "<<objectListSamplePrior[index]<<"\n";
	cout<<"BSLikelihood "<<objectListSampleBSLikelihood[index]<<"\n";
	cout<<"ColorLikelihood "<<objectListSampleColorLikelihood[index]<<"\n";
	bool check = true;
	if (check)
	{
		
		logFile<<"\nnFrame "<<countFrame<<"\n\n";
		logFile<<"prior "<<objectListSamplePrior[index]<<"\n";
		logFile<<"BSLikelihood "<<objectListSampleBSLikelihood[index]<<"\n";
		logFile<<"ColorLikelihood "<<objectListSampleColorLikelihood[index]<<"\n";

		logFile<<"objectList\n";
		for (unsigned int i = 0; i < objectList.size(); i++)
		{
			logFile<<"object "<<i<<" id "<<objectList[i].getIdentity()<<" x= "<<objectList[i].gridPosition().x<<" y= "<<objectList[i].gridPosition().y<<" nFrame= "<<objectList[i].existedNFrame()<<"\n";
			/*
			for (int iCam = 0; iCam < nCam; iCam++)
			{
				logFile<<"visible"<<iCam<<" "<<objectList[i].visible[iCam]<<"\n";
				if (objectList[i].visible[iCam])
				{
					logFile<<"rectx "<<objectList[i].rect[iCam].x<<" ";
					logFile<<"recty "<<objectList[i].rect[iCam].y<<" ";
					logFile<<"rectw "<<objectList[i].rect[iCam].width<<" ";
					logFile<<"recth "<<objectList[i].rect[iCam].height<<"\n";

					logFile<<"originalrectx "<<objectList[i].originalRect[iCam].x<<" ";
					logFile<<"originalrecty "<<objectList[i].originalRect[iCam].y<<" ";
					logFile<<"originalrectw "<<objectList[i].originalRect[iCam].width<<" ";
					logFile<<"originalrecth "<<objectList[i].originalRect[iCam].height<<"\n";
				}
			}
			*/
		}
		logFile<<"endObjectList\n";
		logFile<<"colorObjectList\n";
		for (unsigned int i = 0; i < mainColorObjList.size(); i++)
		{
			mainColorObjList[i].printColor(logFile);
		}
		logFile<<"endColorObjectList\n";
	}
}
	



/////////////////////////////////////////////////////////
//MCMC Step movement function///////////////////////////
////////////////////////////////////////////////////////
void Terrace::stepMoveManagement()
{
	vector<double> stepProb;
	if (objectList.size() <= 3)
	{
		nSample = 100 + objectList.size()*100;
		stepProb.push_back(0.1); //birth
		stepProb.push_back(0.1); //death
		stepProb.push_back(0.8); //update
	}
	else
	{
		nSample = 100 + objectList.size()*100;
		stepProb.push_back(0.1); //birth
		stepProb.push_back(0.1); //death
		stepProb.push_back(0.8); //update
	}
	rndFunc->stepChainCreator(stepProb, nSample);
}

void Terrace::makeHypothesisUpdate()
{
	hypothesisUpdate.clear();
	for (unsigned int i = 0; i < hypothesisUpdate.size(); i++)
	{
		hypothesisUpdate[i].clear();
	}
	hypothesisUpdateIndex.clear();
	for (unsigned int i = 0; i < objectList.size(); i++)
	{
		HumanModel obj = objectList[i];
		vector<CvPoint> tempVector;
		int oldX = obj.gridPosition().x;
		int oldY = obj.gridPosition().y;
		int newX, newY;
		for (int x = -STEP_MOVE; x <= STEP_MOVE; x++)
		{
			newX = oldX + x;
			if (newX < 0 || newX >= ter_gridWidth) continue;
			for (int y = -STEP_MOVE; y <= STEP_MOVE; y++) 
			{
				newY = oldY + y;
				if (newY < 0 || newY >= ter_gridHeight) continue;
				obj.setGridPosition(cvPoint(newX, newY), projModel);
				if (checkingHumanModel(obj) == 0)
				{
					tempVector.push_back(cvPoint(newX, newY));
				}
			}
		}
		hypothesisUpdate.push_back(tempVector);
		hypothesisUpdateIndex.push_back(0);
	}
}

bool Terrace::birthStep()
{

	if (hypothesisAdd.size() > 0) 
	{
		//cout<<"bith step\n";
		vector<HumanModel> newConfig = objectListSample.at(currentConfig);
		if (hypothesisAddIndex >= hypothesisAdd.size()) hypothesisAddIndex = 0;
		if (!searchObjectInList(hypothesisAdd[hypothesisAddIndex], newConfig)) 
			newConfig.push_back( hypothesisAdd[hypothesisAddIndex]);    //add 1 data moi 

		bool check = MCMCStep(newConfig, MCMC_BIRTH_ACCEPT_RATIO);
		
		hypothesisAddIndex++;
		return true;
	}
	return false;
}

bool Terrace::deathStep()
{
	vector<HumanModel> newConfig = objectListSample.at(currentConfig);
	if (newConfig.size() == 0) return false;
	else
	{
		int indexToDelete;
		indexToDelete = rndFunc->deleteObjectMovement(newConfig.size());
		if (!isAtEntrance(newConfig[indexToDelete].gridPosition()) && newConfig[indexToDelete].existedNFrame() > 10) return false;
		if (newConfig[indexToDelete].isFirstEntrance()) 
			cout<<"check   "<<newConfig[indexToDelete].gridPosition().x<<"     "<<newConfig[indexToDelete].gridPosition().y<<"\n";
		newConfig.erase(newConfig.begin() + indexToDelete);
		bool check = MCMCStep(newConfig, MCMC_DEATH_ACCEPT_RATIO); //check xem co xoa phan tu va accept new config hay khong
		return true;
	}
}

bool Terrace::updateStep()
{
	static int stepstep = 0;
	priorTemporalForUpdate = 0;
	vector<HumanModel> newConfig = objectListSample.at(currentConfig);
	if (newConfig.size() == 0) return false;
	else
	{
		int indexToUpdate;
		CvPoint newPosition; 
		CvPoint oldPosition;
	again:
		indexToUpdate = rndFunc->updateObjectMovement(newConfig.size());
		if (newConfig[indexToUpdate].isFirstEntrance())
		{
			if (newConfig.size() > 1) 
			{
				goto again;
			}
			else return false;
		}

		if (stepstep < 2)
		{
			if (hypothesisUpdate[indexToUpdate].size() > 0)
			{
				int rnd = rndFunc->updateObjectMovement(hypothesisUpdate[indexToUpdate].size());
				newConfig[indexToUpdate].changeGridPosition(hypothesisUpdate[indexToUpdate][rnd], projModel);
			}
			else
			{
				//cout<<"rand\n";
				oldPosition = objectListSample.at(0).at(indexToUpdate).gridPosition();
				rndFunc->updateLocation(oldPosition, oldPosition, newPosition, cvPoint(ter_gridWidth, ter_gridHeight));
				newConfig[indexToUpdate].changeGridPosition(newPosition, projModel);
			}
			stepstep++;
		}
		else
		{
			stepstep = 0;
			//cout<<"rand\n";
			oldPosition = objectListSample.at(0).at(indexToUpdate).gridPosition();
			rndFunc->updateLocation(oldPosition, oldPosition, newPosition, cvPoint(ter_gridWidth, ter_gridHeight));
			newConfig[indexToUpdate].changeGridPosition(newPosition, projModel);
		}

		priorTemporalForUpdate = priorTemporal(newConfig, indexToUpdate)/2; //check the movement compare to old movement
		int saveCurrentConfig = currentConfig;
		bool check = MCMCStep(newConfig, MCMC_UPDATE_ACCEPT_RATIO);
		objectListSample[saveCurrentConfig][indexToUpdate].ellipseStatus = newConfig[indexToUpdate].ellipseStatus;
		
		priorTemporalForUpdate = 0;
		
		return true;

	}
}

int Terrace::checkingHumanModelEachView(HumanModel obj, int iCam)
{
	CvRect originalRect, rect;
	bool visible;
	visible = obj.visible[iCam];
	rect = obj.rect[iCam];
	originalRect = obj.originalRect[iCam];

	int left, right, bottom;
	left = right = bottom = -1;
	int xMax = rect.x + rect.width;
	int yMax = rect.y + rect.height;
	if (visible)
	{
		uchar* ptr;
		for (int x = rect.x; x <= xMax; x++)
		{
			for (int y = rect.y; y <= yMax; y++)
			{ 
				ptr = (uchar*)(imgBS[iCam]->imageData + y * imgBS[iCam]->widthStep);
				if (ptr[x] != 0)
				{
					left = x;
					break;
				}
			}
			if (left != -1) break;
		}
		for (int x = xMax; x >= rect.x; x--)
		{
			for (int y = rect.y; y <= yMax; y++)
			{ 
				ptr = (uchar*)(imgBS[iCam]->imageData + y * imgBS[iCam]->widthStep);
				if (ptr[x] != 0)
				{
					right = x;
					break;
				}
			}
			if (right != -1) break;
		}
		for (int y = yMax; y >= rect.y; y--)
		{
			ptr = (uchar*)(imgBS[iCam]->imageData + y * imgBS[iCam]->widthStep);
			for (int x = rect.x; x <= xMax; x++)
			{ 
				if (ptr[x] != 0)
				{
					bottom = y;
					break;
				}
			}
			if (bottom != -1) break;
		}
		if (originalRect.x < 0) 
		{
			if (left - rect.x > 2) 
			{
				return 1;
			}
			
		}
		else
		{
			if ((left - rect.x)*3 > originalRect.width) 
			{
				return 2;
			}
		}
		if (originalRect.x + originalRect.width > imgWidth)
		{
			if (xMax - right > 2) 
			{
				return 3;
			}
		}
		else
		{
			if ((xMax - right)*3 > originalRect.width) 
			{
				return 4;
			}
		}
		if (originalRect.y + originalRect.height > imgHeight)
		{
			if (yMax - bottom > 2)
			{
				return 5;
			}
		}
		else
		{
			if ((yMax - bottom)*30 > originalRect.height)
			{
				return 6;
			}
		}
		return 0;
	}
	else return 0;
}
int Terrace::checkingHumanModel(HumanModel obj)
{
	int k = 0;
	for (int iCam = 0; iCam < nCam; iCam++)
	{
		k = k + checkingHumanModelEachView(obj, iCam);
		if (k != 0) 
		{
			return iCam*10 + k;
		}
	}
	return k;
}
int Terrace::checkingHumanModelEachView2(HumanModel obj, int iCam)
{
	CvRect originalRect, rect;
	bool visible;
	visible = obj.visible[iCam];
	rect = obj.rect[iCam];
	originalRect = obj.originalRect[iCam];

	int left, right, bottom;
	left = right = bottom = -1;
	int xMax = rect.x + rect.width;
	int yMax = rect.y + rect.height;
	if (visible)
	{
		uchar* ptr;
		for (int x = rect.x; x <= xMax; x++)
		{
			for (int y = rect.y; y <= yMax; y++)
			{ 
				ptr = (uchar*)(imgBS2[iCam]->imageData + y * imgBS2[iCam]->widthStep);
				if (ptr[x] != 0)
				{
					left = x;
					break;
				}
			}
			if (left != -1) break;
		}
		for (int x = xMax; x >= rect.x; x--)
		{
			for (int y = rect.y; y <= yMax; y++)
			{ 
				ptr = (uchar*)(imgBS2[iCam]->imageData + y * imgBS2[iCam]->widthStep);
				if (ptr[x] != 0)
				{
					right = x;
					break;
				}
			}
			if (right != -1) break;
		}
		for (int y = yMax; y >= rect.y; y--)
		{
			ptr = (uchar*)(imgBS2[iCam]->imageData + y * imgBS2[iCam]->widthStep);
			for (int x = rect.x; x <= xMax; x++)
			{ 
				if (ptr[x] != 0)
				{
					bottom = y;
					break;
				}
			}
			if (bottom != -1) break;
		}
		if (originalRect.x < 0) 
		{
			if (left - rect.x > 2) 
			{
				return 1;
			}
			
		}
		else
		{
			if ((left - rect.x)*3 > originalRect.width) 
			{
				return 2;
			}
		}
		if (originalRect.x + originalRect.width > imgWidth)
		{
			if (xMax - right > 2) 
			{
				return 3;
			}
		}
		else
		{
			if ((xMax - right)*3 > originalRect.width) 
			{
				return 4;
			}
		}
		if (originalRect.y + originalRect.height > imgHeight)
		{
			if (yMax - bottom > 2)
			{
				return 5;
			}
		}
		else
		{
			if ((yMax - bottom)*30 > originalRect.height)
			{
				return 6;
			}
		}
		return 0;
	}
	else return 0;
}
int Terrace::checkingHumanModel2(HumanModel obj)
{
	int k = 0;
	for (int iCam = 0; iCam < nCam; iCam++)
	{
		if (iCam == 2) continue;
		k = k + checkingHumanModelEachView2(obj, iCam);
		if (k != 0) 
		{
			return iCam*10 + k;
		}
	}
	return k;
}
bool Terrace::MCMCStep(vector<HumanModel>& newConfig, double constant)
{
	int index = this->searchListInSapmleList(newConfig); //check if this config existed
	long double newConfigPosterior;
	if (index == -1) 
	{
		newConfigPosterior = posteriorFunc(newConfig);
	}
	else 
	{
		newConfigPosterior = objectListSamplePosterior.at(index);
		//viewIntersect = viewIntersectVect[index];
	}
	
	long double currentConfigPosterior = objectListSamplePosterior.at(currentConfig);
	
	long double ratio;
	
	if (newConfigPosterior == 0) //case 1-0
	{
		if (currentConfigPosterior != 0)
		{
			return false;
		}
	}
	else
	{
		if (currentConfigPosterior == 0) //case 0-1 
		{
			if (index != -1)
			{
				currentConfig = index;
				return false;
			}
			else
			{
				objectListSample.push_back(newConfig);
				objectListSamplePosterior.push_back(newConfigPosterior);
				currentConfig = objectListSamplePosterior.size() - 1;

				objectListSamplePrior.push_back(prior);
				objectListSampleBSLikelihood.push_back(likelihood);
				objectListSampleColorLikelihood.push_back(colorLikelihood);
				return true;
			}
		}
		else  //case 1-1
		{
			ratio = 1 - std::log(newConfigPosterior/currentConfigPosterior);
			//double ratio2 = 1 - std::log(viewIntersect/preViewIntersect);
			//cout<<"ratio2 "<<ratio2<<"\n";
			if (ratio < constant)
			{
				/*
				if (index == -1) 
				{
					objectListSample.push_back(newConfig); //van add nhung ko thay doi current config
					objectListSamplePosterior.push_back(newConfigPosterior);
					objectListSamplePrior.push_back(prior);
					objectListSampleBSLikelihood.push_back(likelihood);
					objectListSampleColorLikelihood.push_back(colorLikelihood);
				}
				*/
				return false;
			}
			else
			{
				//preViewIntersect = viewIntersect;
				if (index != -1)
				{
					currentConfig = index;
					return false;
				}
				else
				{
					objectListSample.push_back(newConfig);
					objectListSamplePosterior.push_back(newConfigPosterior);
					currentConfig = objectListSamplePosterior.size() - 1;
					objectListSamplePrior.push_back(prior);
					objectListSampleBSLikelihood.push_back(likelihood);
					objectListSampleColorLikelihood.push_back(colorLikelihood);
					//viewIntersectVect.push_back(viewIntersect);
					return true;
				}
			}
		}
	}
	return false;
}

/////////////////////////////////////////////////////////
////////////////prior function///////////////////////////
////////////////////////////////////////////////////////
double Terrace::priorFunc(vector<HumanModel> objList)
{
	double result = 0;
	double temp = 0;

	temp = comfortableZone(objList);
	result += temp;

	//temp = overlapPrior(objList);
	//result += temp;

	temp = priorTemporalForUpdate;
	//result += temp;

	temp = priorTemporalForDeath;
	//result += temp;

	return result;
}



double Terrace::comfortableZone(vector<HumanModel> objList)
{
	double result = 0; 
	int k = objList.size() - 1;
	for (int i = 0; i < k; i++)
	{
		HumanModel obj1 = objList.at(i);
		for (unsigned int j = i + 1; j < objList.size(); j++)
		{
			HumanModel obj2 = objList.at(j);
			if (abs(obj1.gridPosition().x - obj2.gridPosition().x) <= COMFORTABLE_ZONE_DIST && abs(obj1.gridPosition().y - obj2.gridPosition().y) <= COMFORTABLE_ZONE_DIST)
			{
				result += COMFORTABLE_ZONE_CONST;
			}
		}
	}
	return result;
}



double Terrace::priorTemporal(vector<HumanModel> objList, int index) //movement prior, gassian distribution
{
	double cov = 2;
	double result = 0; 

	/*
	if (index >= orderForPriorTemporal.size()) return result;

	HumanModel preObj = objectList.at(orderForPriorTemporal.at(index));
	HumanModel curObj = objList.at(index);
	int moveX = abs(preObj.gridPosition().x - curObj.gridPosition().x);
	int moveY = abs(preObj.gridPosition().y - curObj.gridPosition().y);
	result = pow(moveX - 0, 2.0)/(2* pow(cov, 2.0));
	result += pow(moveY - 0, 2.0)/(2* pow(cov, 2.0));
	result = exp(-result);
	result = 1 - result;
	//cout<<"move x "<<moveX<<" move y "<<moveY<<" result "<<result<<"\n";
	*/
	return result; 
}



//////////////////////////////////////////////////////////////
////////////////likelihood function///////////////////////////
/////////////////////////////////////////////////////////////

double Terrace::likelihoodFunc(vector<HumanModel> objList, IplImage* imgHypo[nCam])
{
	double result = 0; 
	result += likelihoodFromBS(objList, imgHypo);
	result += ghostDetection(objList);
	//result += humanDistributionInRect(objList);
	return result;
}
double Terrace::likelihoodFromBS(vector<HumanModel> objList, IplImage* imgHypo[nCam])
{
	vector<double> viewLikelihood;

	double result = 0;
	double temp;
	for (int iCam = 0; iCam < nCam; iCam++)
	{
		double scaleVar = scale.at(iCam);
		temp = compare2BSImage(imgBS[iCam], imgHypo[iCam]);
		if (scaleVar == 0)
		{
			if (temp != 0)
			{
				viewLikelihood.push_back(temp/1000);
			}
			else viewLikelihood.push_back(0);
		}
		else viewLikelihood.push_back(temp/scaleVar);
	}

	//double min = viewLikelihood[0];
	//double max = viewLikelihood[0];
	for (int iCam = 0; iCam < nCam; iCam++)
	{
		result += viewLikelihood.at(iCam);
		//if (viewLikelihood[iCam] < min) min = viewLikelihood[iCam];
		//if (viewLikelihood[iCam] > max) max = viewLikelihood[iCam];
	}
	//viewIntersect = max - min;

	result += ghostDetection(objList);
	return result;
}
double Terrace::likelihoodFromBSInit(vector<HumanModel> objList, IplImage* imgHypo[nCam])
{
	vector<double> viewLikelihood;
	double result = 0;
	double temp;
	for (int iCam = 0; iCam < nCam; iCam++)
	{
		double scaleVar = scale.at(iCam);//*0.5;
		temp = compare2BSImage(imgBS[iCam], imgHypo[iCam]);
		if (scaleVar == 0)
		{
			if (temp != 0)
			{
				viewLikelihood.push_back(temp/1000);
			}
			else viewLikelihood.push_back(0);
		}
		else viewLikelihood.push_back(temp/scaleVar);
	}
	//double min = viewLikelihood[0];
	//double max = viewLikelihood[0];
	for (int iCam = 0; iCam < nCam; iCam++)
	{
		result += viewLikelihood.at(iCam);
		//if (viewLikelihood[iCam] < min) min = viewLikelihood[iCam];
		//if (viewLikelihood[iCam] > max) max = viewLikelihood[iCam];
	}
	//preViewIntersect = max - min;

	result += ghostDetection(objList);
	return result;
}
double Terrace::ghostDetection(vector<HumanModel> objList)
{
	HumanModel obj;
	double result = 0; 
	for (unsigned int i = 0; i < objList.size(); i++)
	{
		obj = objList.at(i);
		for (int iCam = 0; iCam < nCam; iCam++)
		{
			if (ghostDetectionByHumanModel(obj, iCam) < 0.1)
			{
				result++;
				break;
			}
		}
	}
	return result*GHOST_DETECTION_CONST;
}
double  Terrace::ghostDetectionByHumanModel(HumanModel obj, int iCam)
{
	double rectSize = 0;
	double temp =  countForegroundInHumanModel(imgBS[iCam], obj, iCam, rectSize);
	if (temp == -1) return 1; //neu ko ton tai tren image, ko tinh
	else
	{
		temp = temp/rectSize;
	}
	return temp;
}






//////////////////////////////////////////////////////////////
////////////////a posterior///////////////////////////
/////////////////////////////////////////////////////////////
long double Terrace::posteriorFunc(vector<HumanModel>& objList)
{
	scale.clear();
	IplImage* imgHypo[nCam];
	for (int iCam = 0; iCam < nCam; iCam++)
	{
		imgHypo[iCam] = new_makeBinaryProjectionImageFromList(objList, iCam);
		double temp = countForegroundInImage(imgHypo[iCam]);
		//temp = (temp/BSpixel[iCam])*temp;
		//temp = (BSpixel[iCam]/temp)*BSpixel[iCam];
		scale.push_back(temp);
	}

	//getOverlapVector(objList);
	//occlusionScaleAllView(objList);

	prior = priorFunc(objList);
	likelihood = likelihoodFunc(objList, imgHypo)/2;
	colorLikelihood = compareToMainColorObjList(objList);

	for (int iCam = 0; iCam < nCam; iCam++)
	{
		cvReleaseImage(&imgHypo[iCam]);
	}

	return prior+likelihood+colorLikelihood;
}
long double Terrace::posteriorFuncInit(vector<HumanModel>& objList)
{
	scale.clear();
	IplImage* imgHypo[nCam];
	for (int iCam = 0; iCam < nCam; iCam++)
	{
		imgHypo[iCam] = new_makeBinaryProjectionImageFromList(objList, iCam);
		double temp = countForegroundInImage(imgHypo[iCam]);
		//temp = (temp/BSpixel[iCam])*temp;
		//temp = (BSpixel[iCam]/temp)*BSpixel[iCam];
		scale.push_back(temp);
	}

	//getOverlapVector(objList);
	//occlusionScaleAllView(objList);

	prior = priorFunc(objList);
	likelihood = likelihoodFromBSInit(objList, imgHypo)/2;
	colorLikelihood = compareToMainColorObjList(objList);

	objectListSamplePrior.push_back(prior);
	objectListSampleBSLikelihood.push_back(likelihood);
	objectListSampleColorLikelihood.push_back(colorLikelihood);

	for (int iCam = 0; iCam < nCam; iCam++)
	{
		cvReleaseImage(&imgHypo[iCam]);
	}

	return prior+likelihood+colorLikelihood;
}

////////////////////////////using for prior function


vector<string> Terrace::split(const string& s, const string& delim, const bool keep_empty = true) {
    vector<string> result;
    if (delim.empty()) {
        result.push_back(s);
        return result;
    }
    string::const_iterator substart = s.begin(), subend;
    while (true) {
        subend = search(substart, s.end(), delim.begin(), delim.end());
        string temp(substart, subend);
        if (keep_empty || !temp.empty()) {
            result.push_back(temp);
        }
        if (subend == s.end()) {
            break;
        }
        substart = subend + delim.size();
    }
    return result;
}





void Terrace::testColor()
{
	IplImage* tmpImg = NULL;
	cvNamedWindow("img1", 1);
	cvNamedWindow("img2", 1);
	cvNamedWindow("img0", 1);
	cvNamedWindow("img3", 1);
	cvMoveWindow("img3", 0, 0);
	cvMoveWindow("img2", 400, 0);
	cvMoveWindow("img1", 0, 400);
	cvMoveWindow("img0", 400, 400);
	countFrame = 60;//300;//960;//110;//864;
	ifstream inStream("C:\\data\\log.txt");
	string line;
	getline(inStream, line);
	int countFrame2;
	unsigned int nObject;
	vector<string> str;
	
	
	for (int iCam = 0; iCam < nCam; iCam++)
	{
		colorImg[iCam] = cvCreateImage(cvSize(this->imgWidth, imgHeight), 8, 3);
		ellipseMaskTmp[iCam] = cvCreateImage(cvSize(this->imgWidth, imgHeight), 8, 1);
		ellipseMask[iCam] = cvCreateImage(cvSize(this->imgWidth, imgHeight), 8, 1);
		tmpImg =  cvCreateImage(cvSize(this->imgWidth, imgHeight), 8, 3);
	}
	
	while(true)
	{
		countFrame++;
		cout<<"\n";
		cout<<"Frame "<<countFrame<<"\n";
		cout<<"\n";
		if (cvWaitKey(10) == 27 || countFrame > nFrame) break;
		
		do
		{
			getline(inStream, line);
			countFrame2 = atoi(line.c_str());
		}
		while(countFrame2 != countFrame);
		
		for (int iCam = 0; iCam < nCam; iCam++)
		{
				cvReleaseImage(&img[iCam]);
				cvReleaseImage(&imgBS[iCam]);
				cvReleaseImage(&imgBS2[iCam]);
				fileName[iCam] = createFileName(preName[iCam], countFrame);  //cho nay can sua lai cho hop li, fai la beginFrame
				fileNameBS[iCam] = createFileName(preNameBS[iCam], countFrame); //cho nay can sua lai cho hop li, fai la beginFrame
				img[iCam] = cvLoadImage(fileName[iCam].c_str(), 1);
				imgBS[iCam] = cvLoadImage(fileNameBS[iCam].c_str(), 0);
		}
		
		do
		{
			getline(inStream, line);
			str = split(line, " ");
		}
		while (str[0] != "configSize");
		nObject = atoi(str[2].c_str());
		cout<<"number of object      "<<nObject<<"\n";
		for (unsigned int i = 0; i < objectList.size(); i++)
		{
			getline(inStream, line);
			str = split(line, " ");
			if (str[0] == "object")
			{
				int x = atoi(str[5].c_str());
				int y = atoi(str[8].c_str());
				objectList.at(i).setGridPosition(cvPoint(x, y), projModel);
			}
		}
		for (unsigned int i = objectList.size(); i < nObject; i++)
		{
			getline(inStream, line);
			str = split(line, " ");
			if (str[0] == "object")
			{
				int x = atoi(str[5].c_str());
				int y = atoi(str[8].c_str());
				HumanModel obj;
				obj.setGridPosition(cvPoint(x, y), projModel);
				objectList.push_back(obj);
			}
		}
		if (objectList.size() > nObject)
		{
			while (objectList.size() > nObject)
			{
				objectList.pop_back();
			}
		}
		if (objectList.size() >= 2)
		{
			objectList[1].setHumanHeight(1800, projModel);
		}


		cvCopy(colorImg[1], tmpImg, 0);
		for (int iCam = 0; iCam < nCam; iCam++)
		{
			cvZero(colorImg[iCam]);
			ready[iCam] = false;
		}
		
		makeEclipseMask(objectList);
		for (int iCam = 0; iCam < nCam; iCam++)
		{
			drawcolor(iCam);
		}
		

		showResult(imgBS[2], 2);
		showResult(imgBS[1], 1);
		showResult(imgBS[0], 0);
		showResult(imgBS[3], 3);
		

		int view1 = 1;
		int view2 = 0;
		static bool checking = false;
		if (checking)
		{
			if (ready[view1])
			{

				tempColorObjList = colorHypothesis(objectList);

				CvHistogram* hist2 = calculateHistogram(view1);
				int numrows = 8*8*8;
				//Create matrices to store the signature in
				sig2 = cvCreateMat(numrows, 4, CV_32FC1); //sigs are of type float.
				//Fill signatures for the two histograms
				for( int r = 0; r < 8; r++ ) 
				{
					for( int g = 0; g < 8; g++ ) 
					{	
						for( int b = 0; b < 8; b++ ) 
						{
							//cout<<bin_val<<"    ";
							float bin_val = cvQueryHistValue_3D( hist2, r, g, b );
							cvSet2D(sig2,r*64 + g*8 + b,0,cvScalar(bin_val)); //bin value
							cvSet2D(sig2,r*64 + g*8 + b,1,cvScalar(r)); //Coord 1
							cvSet2D(sig2,r*64 + g*8 + b,2,cvScalar(g)); //Coord 2
							cvSet2D(sig2,r*64 + g*8 + b,3,cvScalar(b)); //Coord 2
						}
						//cout<<"\n";
					}
					//cout<<"\n";
				}
		
				cout<<compare2ColorHypothesis(mainColorObjList, tempColorObjList, view1)<<"     result     "<<cvCompareHist(hist1, hist2,CV_COMP_BHATTACHARYYA)<<"            "<<cvCalcEMD2(sig1,sig2,CV_DIST_L2)<<"\n";
				cvReleaseHist(&hist1);
				//cvReleaseHist(&hist2);
				cvReleaseMat(&sig1);
				//cvReleaseMat(&sig2);
				hist1 = hist2;
				sig1 = sig2;
				releaseVectorMemory(mainColorObjList);
				mainColorObjList = tempColorObjList;	
			}
			else
			{
				releaseVectorMemory(mainColorObjList);
				checking = false;
			}
		}
		else
		{
			if (ready[view1])
			{
				mainColorObjList = colorHypothesis(objectList);

				hist1 = calculateHistogram(view1);
				int numrows = 8*8*8;
				//Create matrices to store the signature in
				//
				sig1 = cvCreateMat(numrows, 4, CV_32FC1); //1 count + 2 coords = 3
				//Fill signatures for the two histograms
				//
				for( int r = 0; r < 8; r++ ) 
				{
					for( int g = 0; g < 8; g++ ) 
					{	
						for( int b = 0; b < 8; b++ ) 
						{
							//cout<<bin_val<<"    ";
							float bin_val = cvQueryHistValue_3D( hist1, r, g, b);
							cvSet2D(sig1,r*64 + g*8 + b,0,cvScalar(bin_val)); //bin value
							cvSet2D(sig1,r*64 + g*8 + b,1,cvScalar(r)); //Coord 1
							cvSet2D(sig1,r*64 + g*8 + b,2,cvScalar(g)); //Coord 2
							cvSet2D(sig1,r*64 + g*8 + b,3,cvScalar(b)); //Coord 2
						}
					}
				}
				checking = true;
			}
		}
		/*
		if (ready[view1] && ready[view2])
		{
			CvHistogram* hist1 = calculateHistogram(view1);
			CvHistogram* hist2 = calculateHistogram(view2);
			CvMat*sig1, *sig2;
			int numrows = 8*8*8;
			//Create matrices to store the signature in
			//
			sig1 = cvCreateMat(numrows, 4, CV_32FC1); //1 count + 2 coords = 3
			sig2 = cvCreateMat(numrows, 4, CV_32FC1); //sigs are of type float.
			//Fill signatures for the two histograms
			//
			for( int r = 0; r < 8; r++ ) 
			{
				for( int g = 0; g < 8; g++ ) 
				{	
					for( int b = 0; b < 8; b++ ) 
					{
						//cout<<bin_val<<"    ";
						float bin_val = cvQueryHistValue_3D( hist1, r, g, b);
						cvSet2D(sig1,r*64 + g*8 + b,0,cvScalar(bin_val)); //bin value
						cvSet2D(sig1,r*64 + g*8 + b,1,cvScalar(r)); //Coord 1
						cvSet2D(sig1,r*64 + g*8 + b,2,cvScalar(g)); //Coord 2
						cvSet2D(sig1,r*64 + g*8 + b,3,cvScalar(b)); //Coord 2
						bin_val = cvQueryHistValue_3D( hist2, r, g, b );
						cvSet2D(sig2,r*64 + g*8 + b,0,cvScalar(bin_val)); //bin value
						cvSet2D(sig2,r*64 + g*8 + b,1,cvScalar(r)); //Coord 1
						cvSet2D(sig2,r*64 + g*8 + b,2,cvScalar(g)); //Coord 2
						cvSet2D(sig2,r*64 + g*8 + b,3,cvScalar(b)); //Coord 2
					}
					//cout<<"\n";
				}
				//cout<<"\n";
			}
		
			cout<<"result     "<<cvCompareHist(hist1, hist2,CV_COMP_BHATTACHARYYA)<<"            "<<cvCalcEMD2(sig1,sig2,CV_DIST_L2)<<"\n";
			cvReleaseHist(&hist1);
			cvReleaseHist(&hist2);
			cvReleaseMat(&sig1);
			cvReleaseMat(&sig2);
		}
		*/

		//cvShowImage("img2", imgBS[2]);	
		//cvShowImage("img1", imgBS[1]);	
		//cvShowImage("img0", imgBS[0]);	
		//cvShowImage("img3", imgBS[3]);	

		cvShowImage("img2", colorImg[2]);	
		cvShowImage("img1", colorImg[1]);	
		//cvShowImage("img0", colorImg[0]);	
		cvShowImage("img0", tmpImg);	
		cvShowImage("img3", colorImg[3]);	
		

		/*
		cvShowImage("img2", ellipseMask[2]);	
		cvShowImage("img1", ellipseMask[1]);	
		cvShowImage("img0", ellipseMask[0]);	
		cvShowImage("img3", ellipseMask[3]);	
		*/
		if (cvWaitKey(10000) == 'q') continue;
		

	}
	inStream.close();
	cvDestroyAllWindows();
	for (int iCam = 0; iCam < nCam; iCam++)
	{
		cvReleaseCapture(&capture[iCam]);
	}
}
void Terrace::testColor2()
{
	cvNamedWindow("img1", 1);
	cvNamedWindow("img2", 1);
	cvNamedWindow("img0", 1);
	cvNamedWindow("img3", 1);
	cvMoveWindow("img3", 0, 0);
	cvMoveWindow("img2", 400, 0);
	cvMoveWindow("img1", 0, 400);
	cvMoveWindow("img0", 400, 400);
	countFrame = 60;//300;//960;//110;//864;
	ifstream inStream("C:\\data\\log.txt");
	string line;
	getline(inStream, line);
	int countFrame2;
	unsigned int nObject;
	vector<string> str;
	
	
	for (int iCam = 0; iCam < nCam; iCam++)
	{
		colorImg[iCam] = cvCreateImage(cvSize(this->imgWidth, imgHeight), 8, 3);
		ellipseMaskTmp[iCam] = cvCreateImage(cvSize(this->imgWidth, imgHeight), 8, 1);
		ellipseMask[iCam] = cvCreateImage(cvSize(this->imgWidth, imgHeight), 8, 1);
	}
	
	while(true)
	{
		countFrame++;
		cout<<"\n";
		cout<<"Frame "<<countFrame<<"\n";
		cout<<"\n";
		if (cvWaitKey(10) == 27 || countFrame > nFrame) break;
		
		do
		{
			getline(inStream, line);
			countFrame2 = atoi(line.c_str());
		}
		while(countFrame2 != countFrame);
		
		for (int iCam = 0; iCam < nCam; iCam++)
		{
				cvReleaseImage(&img[iCam]);
				cvReleaseImage(&imgBS[iCam]);
				cvReleaseImage(&imgBS2[iCam]);
				fileName[iCam] = createFileName(preName[iCam], countFrame);  //cho nay can sua lai cho hop li, fai la beginFrame
				fileNameBS[iCam] = createFileName(preNameBS[iCam], countFrame); //cho nay can sua lai cho hop li, fai la beginFrame
				img[iCam] = cvLoadImage(fileName[iCam].c_str(), 1);
				imgBS[iCam] = cvLoadImage(fileNameBS[iCam].c_str(), 0);
		}
		
		do
		{
			getline(inStream, line);
			str = split(line, " ");
		}
		while (str[0] != "configSize");
		nObject = atoi(str[2].c_str());
		cout<<"number of object      "<<nObject<<"\n";
		for (unsigned int i = 0; i < objectList.size(); i++)
		{
			getline(inStream, line);
			str = split(line, " ");
			if (str[0] == "object")
			{
				int x = atoi(str[5].c_str());
				int y = atoi(str[8].c_str());
				objectList[i].setGridPosition(cvPoint(x, y), projModel);
			}
		}
		for (unsigned int i = objectList.size(); i < nObject; i++)
		{
			getline(inStream, line);
			str = split(line, " ");
			if (str[0] == "object")
			{
				int x = atoi(str[5].c_str());
				int y = atoi(str[8].c_str());
				HumanModel obj;
				obj.setGridPosition(cvPoint(x, y), projModel);
				objectList.push_back(obj);
			}
		}
		if (objectList.size() > nObject)
		{
			while (objectList.size() > nObject)
			{
				objectList.pop_back();
			}
		}
		if (objectList.size() >= 2)
		{
			objectList[1].setHumanHeight(1800, projModel);
		}

		//compareToMainColorObjList(objectList);
	
		int k;
		for (unsigned int i = 0; i < objectList.size(); i++)
		{
			k = checkingHumanModel(objectList[i]);
			if (k != 0) cout<<"aaaaaaaaaaaaaaaaaaaaaaaa      object   "<<i<<"                  falseeeeeeeeeeeeeee \n";
			if (k == 0 && (objectList[i].existedNFrame() > 10 || !isAtEntrance(objectList[i].gridPosition())) && (objectList[i].getIdentity() == -1))
			{
				objectList[i].setIdentity(identityToAdd);
				//idRememberVector.push_back(identityToAdd);
				identityToAdd++;
				cout<<"bbbbbb    "<<identityToAdd<<"\n";
			}
			objectList[i].increaseExistedNFrame();
			cout<<"aaaaaaa     "<<i<<"    "<<objectList[i].existedNFrame()<<"\n";
			if (k == 0 && !isAtEntrance(objectList[i].gridPosition()) && searchIdentity(mainColorObjList, objectList[i].getIdentity()) == -1)
			{
				idRememberVector.push_back(objectList[i].getIdentity());
				cout<<"aaaadddddddddd identity to vector\n";
			}
		}
		updateColorVector(objectList);

		for (int iCam = 0; iCam < nCam; iCam++)
		{
			cvZero(colorImg[iCam]);
			drawcolor(iCam);
		}
		
		showResult(imgBS[2], 2);
		showResult(imgBS[1], 1);
		showResult(imgBS[0], 0);
		showResult(imgBS[3], 3);

		/*
		cvShowImage("img2", imgBS[2]);	
		cvShowImage("img1", imgBS[1]);	
		cvShowImage("img0", imgBS[0]);	
		cvShowImage("img3", imgBS[3]);
			*/

		cvShowImage("img2", colorImg[2]);	
		cvShowImage("img1", colorImg[1]);	
		cvShowImage("img0", colorImg[0]);	
		cvShowImage("img3", colorImg[3]);	

		if (cvWaitKey(10000) == 'q') continue;

	}
	inStream.close();
	cvDestroyAllWindows();
	for (int iCam = 0; iCam < nCam; iCam++)
	{
		cvReleaseCapture(&capture[iCam]);
	}
}


void Terrace::drawcolor(HumanModel obj)
{
	for (int iCam = 0; iCam < nCam; iCam++)
	{
		CvRect rect;
		bool visible;
		visible = obj.visible[iCam];
		rect = obj.rect[iCam];

		if (visible)
		{
			int xMax = rect.x + rect.width;
			int yMax = rect.y + rect.height*2/3;
			uchar* ptr;
			uchar* ptr2;
			for (int y = rect.y; y <= yMax; y++)
			{ 
				ptr = (uchar*)(img[iCam]->imageData + y * img[iCam]->widthStep);
				ptr2 = (uchar*)(colorImg[iCam]->imageData + y * colorImg[iCam]->widthStep);
				for (int x = rect.x*3; x <= xMax*3; x += 3)
				{
					ptr2[x] = ptr[x];
					ptr2[x + 1] = ptr[x + 1];
					ptr2[x + 2] = ptr[x + 2];
				}
			}
		}
	}
}
void Terrace::drawcolor(int iCam)
{
	uchar* ptrOriginal;
	uchar* ptrColor;
	uchar* ptrMask;
	for (int y = 0; y < imgHeight; y++)
	{
		ptrOriginal = (uchar*)(img[iCam]->imageData + y * img[iCam]->widthStep);
		ptrColor = (uchar*)(colorImg[iCam]->imageData + y * colorImg[iCam]->widthStep);
		ptrMask = (uchar*)(ellipseMask[iCam]->imageData + y * ellipseMask[iCam]->widthStep);
		for (int x = 0; x < imgWidth; x++)
		{
			int colorX = x*3;
			if (ptrMask[x] != 0)
			{
				ptrColor[colorX] = ptrOriginal[colorX];
				ptrColor[colorX + 1] = ptrOriginal[colorX + 1];
				ptrColor[colorX + 2] = ptrOriginal[colorX + 2];
			}
		}
	}
}
void Terrace::makeEclipseMask(vector<HumanModel>& objList)
{
	initOcclusionVector(objList);
	prepareEclipseMaskTmp(objList);
	for (int iCam = 0; iCam < nCam; iCam++)
	{
		makeEclipseMaskEachView(objList, iCam);
	}
}
void Terrace::makeEclipseMaskEachView(vector<HumanModel>& objList, int iCam)
{
	cvZero(ellipseMask[iCam]);
	CvRect rect;
	for (unsigned int i = 0; i < objList.size(); i++)
	{
		bool visible = objList[i].visible[iCam];
		if (visible)
		{
			rect = objList[i].rect[iCam];
			ellipseMaskTmp[iCam] = objList[i].ellipseMaskTmp[iCam];
			int xMax = rect.x + rect.width;
			int yMax = rect.y + rect.height;
			uchar* ptrTmp;
			uchar* ptr;
			for (int y = rect.y; y <= yMax; y++)
			{ 
				ptrTmp = (uchar*)(ellipseMaskTmp[iCam]->imageData + y * ellipseMaskTmp[iCam]->widthStep);
				ptr = (uchar*)(ellipseMask[iCam]->imageData + y * ellipseMask[iCam]->widthStep);
				for (int x = rect.x; x <= xMax; x++)
				{
					if (ptrTmp[x] != 0)
					{
						if (ptr[x] == 0)
						{
							ptr[x] = ptrTmp[x] + i;
						}
						else
						{
							int k = ptr[x]%10;
							if (occlusionVector[iCam][i] < occlusionVector[iCam][k])
							{
								ptr[x] = ptrTmp[x] + i;
							}
						}
					}
				}
			}
		}
	}
}
bool Terrace::makeEclipseMaskTmp(HumanModel& obj, int iCam, CvRect& rect)
{

	cvZero(ellipseMaskTmp[iCam]);
	CvRect originalRect, newRect;
	bool visible;
	
	visible = obj.visible[iCam];
	rect = obj.rect[iCam];
	originalRect = obj.originalRect[iCam];

	if (visible)
	{
		if (rect.x == originalRect.x && rect.y == originalRect.y && rect.width == originalRect.width && rect.height == originalRect.height)
		{
			obj.setFullView(iCam);
		}
		CvPoint center;
		center.x = originalRect.x + originalRect.width/2;
		center.y = originalRect.y + originalRect.height/3;
		CvSize axes;
		axes.width = originalRect.width/2;
		axes.height = originalRect.height/3;

		obj.setEcclipseArea(iCam, axes.width, axes.height);
		
		cvEllipse(ellipseMaskTmp[iCam], center, axes, 0, 0, -90, cvScalarAll(10), CV_FILLED); 
		cvEllipse(ellipseMaskTmp[iCam], center, axes, 0, 0, 90, cvScalarAll(20), CV_FILLED); 
		cvEllipse(ellipseMaskTmp[iCam], center, axes, 0, 90, 180, cvScalarAll(30), CV_FILLED); 
		cvEllipse(ellipseMaskTmp[iCam], center, axes, 0, 180, 270, cvScalarAll(40), CV_FILLED); 
		axes.width = axes.width/2;
		axes.height = axes.height/2;
		cvEllipse(ellipseMaskTmp[iCam], center, axes, 0, 0, -90, cvScalarAll(50), CV_FILLED); 
		cvEllipse(ellipseMaskTmp[iCam], center, axes, 0, 0, 90, cvScalarAll(60), CV_FILLED); 
		cvEllipse(ellipseMaskTmp[iCam], center, axes, 0, 90, 180, cvScalarAll(70), CV_FILLED); 
		cvEllipse(ellipseMaskTmp[iCam], center, axes, 0, 180, 270, cvScalarAll(80), CV_FILLED); 

		newRect.x = originalRect.x;
		newRect.y = originalRect.y + originalRect.height*2/3;
		newRect.width = originalRect.width;
		newRect.height = originalRect.height/3;
		ready[iCam] = true;
	}
	return visible;
	
}
void Terrace::prepareEclipseMaskTmp(vector<HumanModel>& objList)
{
	for (unsigned int i = 0; i < objList.size(); i++)
	{
		if ((objList[i].ellipseStatus) == -1)
		{
			objList[i].makeEllipseMaskTmp(imgWidth, imgHeight);
		}
		else if ((objList[i].ellipseStatus) == 0)
		{
			objList[i].updateEllipseMaskTmp();
		}
	}
}
int Terrace::checkingOcclusion(HumanModel obj1, HumanModel obj2, int iCam)
{
	return projModel->checkingOcclusion(obj1.gridPosition(), obj2.gridPosition(), iCam);
}
void Terrace::initOcclusionVector(vector<HumanModel> objList)
{
	int numberObject = objList.size();
	for (int iCam = 0; iCam < nCam; iCam++)
	{
		occlusionVector[iCam].clear();
		for (int i = 0; i < numberObject; i++)
		{
			occlusionVector[iCam].push_back(0);
		}
		for (int i = 1; i < numberObject; i++)
		{
			for (int j = 0; j < i; j++)
			{
				int result = checkingOcclusion(objList[j], objList[i], iCam);
				if (result == 1) 
				{
					if (occlusionVector[iCam][i] <= occlusionVector[iCam][j]) occlusionVector[iCam][i] = occlusionVector[iCam][j] + 1;
				}
				else occlusionVector[iCam][j]++;
			}
		}
	}
}

CvHistogram* Terrace::calculateHistogram(int iCam)
{
	IplImage* hsv = cvCreateImage( cvGetSize(colorImg[iCam]), 8, 3 );
	cvCvtColor( colorImg[iCam], hsv, CV_BGR2HSV );

	IplImage* r_plane = cvCreateImage( cvGetSize(colorImg[iCam]), 8, 1 );
	IplImage* g_plane = cvCreateImage( cvGetSize(colorImg[iCam]), 8, 1 );
	IplImage* b_plane = cvCreateImage( cvGetSize(colorImg[iCam]), 8, 1 );
	IplImage* planes[] = { r_plane, g_plane, b_plane };
	
	cvCvtPixToPlane( img[iCam], r_plane, g_plane, b_plane, 0 );
	
	int r_bins = 8, g_bins = 8, b_bins = 8;
	CvHistogram* hist;
	{
	int hist_size[] = { r_bins, g_bins, b_bins};
	float r_ranges[] = { 0, 255 }; // hue is [0,180]
	float g_ranges[] = { 0, 255 };
	float b_ranges[] = { 0, 255 };
	float* ranges[] = { r_ranges, g_ranges, b_ranges };
	hist = cvCreateHist(
	3,
	hist_size,
	CV_HIST_ARRAY,
	ranges,
	1
	);
	}
	//cvCalcHist( planes, hist, 0, ellipseMask[iCam] ); //Compute histogram
	
	
	uchar* ptr, *rPtr, *gPtr, *bPtr;
	cvClearHist(hist);
	for (int y = 0; y < imgHeight; y++)
	{ 
		ptr = (uchar*)(ellipseMask[iCam]->imageData + y * ellipseMask[iCam]->widthStep);
		rPtr = (uchar*)(r_plane->imageData + y * r_plane->widthStep);
		gPtr = (uchar*)(g_plane->imageData + y * g_plane->widthStep);
		bPtr = (uchar*)(b_plane->imageData + y * b_plane->widthStep);
		for (int x = 0; x < imgWidth; x++)
		{
			if (ptr[x] != 0)
			{
				float* bin = cvGetHistValue_3D(hist, rPtr[x]/32, gPtr[x]/32, bPtr[x]/32);
				*bin = *bin + 1;
			}
		}
	}
	
	cvNormalizeHist( hist, 1.0 ); //Normalize it
	
	for( int r = 0; r < r_bins; r++ ) 
	{
		for( int g = 0; g < g_bins; g++ ) 
		{	
			for( int b = 0; b < g_bins; b++ ) 
			{
				float bin_val = cvQueryHistValue_3D( hist, r, g, b);
			}
		}
	}
	cvReleaseImage(&r_plane);
	cvReleaseImage(&g_plane);
	cvReleaseImage(&b_plane);
	cvReleaseImage(&hsv);
	return hist;
}
vector<HumanModel> Terrace::colorHypothesis(vector<HumanModel> objList)
{
	vector<HumanModel> vect;
	for (unsigned int i = 0; i < objList.size(); i++)
	{
		vect.push_back(objList[i]);
	}
	for (unsigned int i = 0; i < vect.size(); i++)
	{
		vect[i].allocateMemory();
	}
	int count;
	for (unsigned int iCam = 0; iCam < nCam; iCam++)
	{
		count = 0;
		IplImage* r_plane = cvCreateImage( cvGetSize(colorImg[iCam]), 8, 1 );
		IplImage* g_plane = cvCreateImage( cvGetSize(colorImg[iCam]), 8, 1 );
		IplImage* b_plane = cvCreateImage( cvGetSize(colorImg[iCam]), 8, 1 );
		IplImage* planes[] = { r_plane, g_plane, b_plane };
		cvCvtPixToPlane( img[iCam], r_plane, g_plane, b_plane, 0 );

		uchar* ptr, *rPtr, *gPtr, *bPtr;
		for (int y = 0; y < imgHeight; y++)
		{ 
			ptr = (uchar*)(ellipseMask[iCam]->imageData + y * ellipseMask[iCam]->widthStep);
			rPtr = (uchar*)(r_plane->imageData + y * r_plane->widthStep);
			gPtr = (uchar*)(g_plane->imageData + y * g_plane->widthStep);
			bPtr = (uchar*)(b_plane->imageData + y * b_plane->widthStep);
			for (int x = 0; x < imgWidth; x++)
			{
				if (ptr[x] != 0)
				{
					count++;
					int objIndex = ptr[x]%10;
					int partIndex = ptr[x]/10;
					vect[objIndex].setBin(iCam, partIndex, rPtr[x], gPtr[x], bPtr[x]);
				}
			}
		}

		cvReleaseImage(&r_plane);
		cvReleaseImage(&g_plane);
		cvReleaseImage(&b_plane);
	}
	return vect;
}
double Terrace::compare2ColorHypothesis(vector<HumanModel> objSrc, vector<HumanModel> objDes, int iCam)
{
	double result = 0;
	int count = 0;
	int nObject;
	if (objDes.size() > objSrc.size()) nObject = objSrc.size();
	else nObject = objDes.size();
	for (int i = 0; i < nObject; i++)
	{
		for (int iPart = 0; iPart < 7; iPart++)
		{
			if (objSrc[i].checkingVisisble(iCam, iPart) > 0.5 && objDes[i].checkingVisisble(iCam, iPart) > 0.5 && objSrc[i].getFullView(iCam))
			{
				double tmp = cvCompareHist(objSrc[i].getPreviousHist(iCam, iPart), objDes[i].getPreviousHist(iCam, iPart), CV_COMP_BHATTACHARYYA);
				result += tmp;
				count++;
				cout<<"part "<<iPart<<"     "<<tmp<<"\n";
			}
		}
		
	}
	if (count > 0) return result/count;
	else return -1;
}
void Terrace::releaseVectorMemory(vector<HumanModel>& objList)
{
	for (unsigned int i = 0; i < objList.size(); i++)
	{
		objList[i].releaseMemory();
	}
	objList.clear();
}

int Terrace::searchIdentity(vector<HumanModel> objList, int id)
{
	for (unsigned int i = 0; i < objList.size(); i++)
	{
		if (objList[i].getIdentity() == id) return i;
	}
	return -1;
}
bool Terrace::isAtEntrance(CvPoint pos)
{
	if (pos.x >= ENTRANCE_X_MIN && pos.x <= ENTRANCE_X_MAX && pos.y >= ENTRANCE_Y_MIN && pos.y <= ENTRANCE_Y_MAX) return true;
	return false;
}

void Terrace::updateColorVector(vector<HumanModel>& objList)
{
	bool checkingMakeColor = false;
	int k;
	for (unsigned int i = 0; i < objList.size(); i++)
	{
		k = searchIdentity(mainColorObjList, objList[i].getIdentity());
		if (k != -1)
		{
			if (!checkingMakeColor)
			{
				makeEclipseMask(objList);
				tempColorObjList = colorHypothesis(objList);
				checkingMakeColor = true;
			}
			mainColorObjList[k].updateModel(tempColorObjList[i]);
		}
	}
	for (unsigned int i = 0; i < idRememberVector.size(); i++)
	{
		if (!checkingMakeColor)
		{
			makeEclipseMask(objList);
			tempColorObjList = colorHypothesis(objList);
			checkingMakeColor = true;
		}
		
		k = searchIdentity(tempColorObjList, idRememberVector[i]);
		if (tempColorObjList[k].countNumberFullView() >= 3)
		{
			mainColorObjList.push_back(tempColorObjList[k]);
			tempColorObjList.erase(tempColorObjList.begin() + k);//ko xoa dc
			idRememberVector.erase(idRememberVector.begin() + i);
			i--;
			cout<<"addddddddddddddddddddddddddddd colorrrrrrrrrrrrrrrrrrrrrr\n";
		}
	}
	idRememberVector.clear();
	releaseVectorMemory(tempColorObjList);

}
double Terrace::compareToMainColorObjList(vector<HumanModel>& objList)
{
	bool checkingMakeColor = false;
	int k;
	double result = 0;
	double count = 0;
	if (mainColorObjList.size() == 0) return 0;
	for (unsigned int i = 0; i < objList.size(); i++)
	{
		k = searchIdentity(mainColorObjList, objList[i].getIdentity());
		if (k != -1)
		{
			if (!checkingMakeColor)
			{
				makeEclipseMask(objList);
				tempColorObjList = colorHypothesis(objList);
				checkingMakeColor = true;
			}
			double temp = mainColorObjList[k].compareHist(tempColorObjList[i]);
			if (temp!=0) count++;
			result += temp;
		}
	}
	releaseVectorMemory(tempColorObjList);
	if (mainColorObjList.size() != 0 && result == 0) return 20;
	if (count != 0) return result/count;
	else return result;
}

void Terrace::readResult(int countFrame)
{
	//countFrame = 63;
	ifstream inStream("C:\\data\\Log60.txt");
	inStream.seekg (0, ios::end);
	int length = inStream.tellg();
	inStream.seekg(length - 3000000, ios::beg);
	//inStream.
	string line;
	getline(inStream, line);
	int nFrame;
	vector<string> str;
		
	do
	{
		getline(inStream, line);
		str = split(line, " ");
		if (str.size() == 2 && str[0] == "nFrame") cout<<atoi(str[1].c_str())<<"\n";
	}
	while(str.size() != 2 || str[0] != "nFrame" || atoi(str[1].c_str()) != countFrame);
	
	getline(inStream, line);
	getline(inStream, line);
	getline(inStream, line);
	getline(inStream, line);

	getline(inStream, line);
	if (line == "objectList")
	{
		getline(inStream, line);  //da get line ke tiep roi
		while (line != "endObjectList")
		{
			HumanModel obj;
			str = split(line, " ");
			int id =  atoi(str[3].c_str());
			int x = atoi(str[5].c_str());
			int y = atoi(str[7].c_str());
			int n = atoi(str[9].c_str());
			obj.setGridPosition(cvPoint(x, y), projModel);
			obj.setIdentity(id);
			obj.setNFrame(n);
			objectList.push_back(obj);
			getline(inStream, line);
			//for (int i = 0; i <= 12; i++) getline(inStream, line);
		}
	}
	getline(inStream, line);
	if (line == "colorObjectList")
	{
		getline(inStream, line);  //da get line ke tiep roi
		while (line != "endColorObjectList")
		{
			HumanModel obj;

			str = split(line, " ");
			int id = atoi(str[1].c_str());

			int k = searchIdentity(objectList, id);
			obj.setGridPosition(cvPoint(objectList[k].gridPosition().x, objectList[k].gridPosition().y), projModel);
			obj.setIdentity(id);
			obj.allocateMemory();

			for (int iCam = 0; iCam < nCam; iCam++)
			{
				getline(inStream, line);
				str = split(line, " ");
				int fullView = atoi(str[1].c_str());
				int learningRate = atoi(str[3].c_str());

				if (fullView == 1) obj.fullView[iCam] = true;
				else obj.fullView[iCam] = false;
				obj.learningRate[iCam] = learningRate;
				if (fullView == 0) continue;
				for (int iPart = 0; iPart < nPart; iPart++)
				{
					getline(inStream, line);
					getline(inStream, line);
					str = split(line, " ");  //color here
					for (int i = 0; i < 256; i++)
					{
						obj.readHist(atoi(str[i].c_str()), i, iCam, iPart);
					}
				}
			}

			mainColorObjList.push_back(obj);
			getline(inStream, line);
		}
	}	
	inStream.close();
	identityToAdd = -1;
	for (unsigned int i = 0; i < objectList.size(); i++)
	{
		if (objectList[i].getIdentity() > identityToAdd) identityToAdd = objectList[i].getIdentity();
	}
	identityToAdd++;
}
void Terrace::showLogFile()
{
	cvNamedWindow("img1", 1);
	cvNamedWindow("img2", 1);
	cvNamedWindow("img0", 1);
	cvNamedWindow("img3", 1);
	cvMoveWindow("img3", 0, 0);
	cvMoveWindow("img2", 400, 0);
	cvMoveWindow("img1", 0, 400);
	cvMoveWindow("img0", 400, 400);
	
	ifstream inStream("C:\\data\\Log60.txt");
	string line;
	int nFrame;
	vector<string> str;
	while(true)
	{
		getline(inStream, line); //space line
		cout<<line<<"\n";
		getline(inStream, line); //frame line
		cout<<line<<"\n";
		str = split(line, " ");
		countFrame = atoi(str[1].c_str());
		//get image
		for (int iCam = 0; iCam < nCam; iCam++)
		{
				cvReleaseImage(&img[iCam]);
				cvReleaseImage(&imgBS[iCam]);
				cvReleaseImage(&imgBS2[iCam]);
				fileName[iCam] = createFileName(preName[iCam], countFrame);  //cho nay can sua lai cho hop li, fai la beginFrame
				fileNameBS[iCam] = createFileName(preNameBS[iCam], countFrame); //cho nay can sua lai cho hop li, fai la beginFrame
				img[iCam] = cvLoadImage(fileName[iCam].c_str(), 1);
				imgBS[iCam] = cvLoadImage(fileNameBS[iCam].c_str(), 0);
		}
		
		getline(inStream, line);
		cout<<line<<"\n";
		getline(inStream, line);
		cout<<line<<"\n";
		getline(inStream, line);
		cout<<line<<"\n";
		getline(inStream, line);
		cout<<line<<"\n";
	
		//get information
		objectList.clear();
		getline(inStream, line);
		cout<<line<<"\n";
		if (line == "objectList")
		{
			getline(inStream, line);  //da get line ke tiep roi
			while (line != "endObjectList")
			{
				cout<<line<<"\n";
				HumanModel obj;
				str = split(line, " ");
				int id =  atoi(str[3].c_str());
				int x = atoi(str[5].c_str());
				int y = atoi(str[7].c_str());
				int n = atoi(str[9].c_str());
				obj.setGridPosition(cvPoint(x, y), projModel);
				obj.setIdentity(id);
				obj.setNFrame(n);
				objectList.push_back(obj);
				getline(inStream, line);
			}
		}
		getline(inStream, line);
		if (line == "colorObjectList")
		{
			getline(inStream, line);  //da get line ke tiep roi
			while (line != "endColorObjectList")
			{
				//learning color histogram
				/*
				HumanModel obj;
				str = split(line, " ");
				int id = atoi(str[1].c_str());
				int k = searchIdentity(objectList, id);
				obj.setGridPosition(cvPoint(objectList[k].gridPosition().x, objectList[k].gridPosition().y), projModel);
				obj.setIdentity(id);
				obj.allocateMemory();
				*/
				for (int iCam = 0; iCam < nCam; iCam++)
				{
					getline(inStream, line);
					str = split(line, " ");
					int fullView = atoi(str[1].c_str());
					int learningRate = atoi(str[3].c_str());

					//if (fullView == 1) obj.fullView[iCam] = true;
					//else obj.fullView[iCam] = false;
					//obj.learningRate[iCam] = learningRate;
					if (fullView == 0) continue;
					for (int iPart = 0; iPart < nPart; iPart++)
					{
						getline(inStream, line);
						getline(inStream, line);
						//str = split(line, " ");  //color here
						//for (int i = 0; i < 256; i++)
						//{
							//obj.readHist(atoi(str[i].c_str()), i, iCam, iPart);
						//}
					}
				}

				//mainColorObjList.push_back(obj);
				getline(inStream, line);
			}
		}
		for (unsigned int i = 0; i < objectList.size(); i++)
		{
			if (i == 1) objectList[i].setHumanHeight(1800, projModel);
			showColorImage(objectList[i]);
		}
		cvShowImage("img2", img[2]);	
		cvShowImage("img1", img[1]);	
		cvShowImage("img0", img[0]);	
		cvShowImage("img3", img[3]);	

		char key = cvWaitKey(10);
		if (key == 'n') continue;
		else if (key == 'q') break;
		

	}
	inStream.close();
	cvDestroyAllWindows();
	for (int iCam = 0; iCam < nCam; iCam++)
	{
		cvReleaseCapture(&capture[iCam]);
	}
}
CvScalar Terrace::getTheColor(int id)
{
	switch(id)
	{
		case -1:
			return cvScalar(255, 0, 0);
		case 0:
			return cvScalar(0, 255, 0);
		case 1:
			return cvScalar(0, 0, 255);
		case 2:
			return cvScalar(255, 255, 0);
		case 3:
			return cvScalar(255, 0, 255);
		case 4:
			return cvScalar(0, 255, 255);
		case 5:
			return cvScalar(255, 255, 255);
		case 6:
			return cvScalar(0, 0, 0);
		case 7:
			return cvScalar(100, 100, 100);
	}
}
void Terrace::showColorImage(HumanModel obj)
{
	for (int iCam = 0; iCam < nCam; iCam++)
	{
		CvRect rect;
		bool visible; 
		visible = obj.visible[iCam];
		rect = obj.rect[iCam];
		if (visible)
		{
			cvRectangleR(img[iCam], rect, getTheColor(obj.getIdentity()), 2, 8, 0);
		}
	}
}