#include "z_HumanModel.h"

HumanModel::HumanModel()
{
	gridPos = cvPoint(-1,-1);
	height = HEIGHT_AVERAGE;
	
	setHumanStatus(STATE_NEW);
	setHumanWidth(WIDTH_AVERAGE);
	
	nFrame = 0;
	prob = 0;
	firstEntrance = true;
	id = -1;
	for (int iCam = 0; iCam < nCam; iCam++)
	{
		ecclipseArea[iCam] = 0;
		fullView[iCam] = false;
		learningRate[iCam] = 0;
	}
	ellipseStatus = -1;
}
HumanModel::HumanModel(CvPoint newPos, int h, int w, int state)
{
	gridPos = cvPoint(newPos.x,newPos.y);
	height = h;
	
	setHumanStatus(state);
	setHumanWidth(w);
	
	nFrame = 0;
	prob = 0;
	firstEntrance = true;
	id = -1;
	for (int iCam = 0; iCam < nCam; iCam++)
	{
		ecclipseArea[iCam] = 0;
		fullView[iCam] = false;
		learningRate[iCam] = 0;
	}
	ellipseStatus = -1;
}

HumanModel::~HumanModel()
{
}

bool HumanModel::operator== (HumanModel obj)
{
	if (gridPos.x != obj.gridPosition().x || gridPos.y != obj.gridPosition().y || height != obj.humanHeight() || width != obj.humanWidth() || firstEntrance != obj.isFirstEntrance() || id != obj.getIdentity())// || nFrame != obj.existedNFrame() || id != obj.getIdentity())
	{	
		return false;
	}
	return true;
}
void HumanModel::setGridPosition(CvPoint newPos, ProjectionModel* projModel)
{
	gridPos = newPos;
	for (int iCam = 0; iCam < nCam; iCam++)
	{
		visible[iCam] = projModel->simpleModelProjection(gridPos, height, width, iCam, originalRect[iCam], rect[iCam]);
	}
}
void HumanModel::changeGridPosition(CvPoint newPos, ProjectionModel* projModel)
{
	gridPos = newPos;
	for (int iCam = 0; iCam < nCam; iCam++)
	{
		visible[iCam] = projModel->simpleModelProjection(gridPos, height, width, iCam, originalRect[iCam], rect[iCam]);
	}
	if (ellipseStatus != -1) ellipseStatus = 0;

}
void HumanModel::setHumanHeight(int h, ProjectionModel* projModel)
{
	height = h;
	for (int iCam = 0; iCam < nCam; iCam++)
	{
		visible[iCam] = projModel->simpleModelProjection(gridPos, height, width, iCam, originalRect[iCam], rect[iCam]);
	}
}


void HumanModel::allocateMemory()
{
	for (int iCam = 0; iCam < nCam; iCam++)
	{
		for (int iPart = 0; iPart < nPart; iPart++)
		{
			previousHist[iCam][iPart] = cvCreateHist(3, histSize, CV_HIST_ARRAY, histRanges, 1);
			cvClearHist(previousHist[iCam][iPart]);
			visibleArea[iCam][iPart] = 0;
		}
		visible[iCam] = 0;
	}
}
void HumanModel::releaseMemory()
{
	for (int iCam = 0; iCam < nCam; iCam++)
	{
		for (int iPart = 0; iPart < nPart; iPart++)
		{
			cvReleaseHist(&previousHist[iCam][iPart]);
			for (int i = 0; i < learnedHist[iCam][iPart].size(); i++)
			{
				cvReleaseHist(&learnedHist[iCam][iPart][i]);
			}
			learnedHist[iCam][iPart].clear();
		}
	}
}
void HumanModel::setEcclipseArea(int iCam, int width, int height)
{
	ecclipseArea[iCam] = 3.14159*width*height;
}
void HumanModel::setBin(int iCam, int partIndex, int R, int G, int B)
{
	float* bin;
	if (partIndex <= 4)
	{
		bin = cvGetHistValue_3D(previousHist[iCam][partIndex], R/32, G/32, B/32);
		*bin = *bin + 1;
		visibleArea[iCam][partIndex]++;
		//if (partIndex == 0) cout<<"kkkkkk";

		bin = cvGetHistValue_3D(previousHist[iCam][6], R/32, G/32, B/32);
		*bin = *bin + 1;
		visibleArea[iCam][6]++;
	}
	else if (partIndex > 4)
	{
		bin = cvGetHistValue_3D(previousHist[iCam][partIndex - 4], R/32, G/32, B/32);
		*bin = *bin + 1;
		visibleArea[iCam][partIndex - 4]++;


		bin = cvGetHistValue_3D(previousHist[iCam][5], R/32, G/32, B/32);
		*bin = *bin + 1;
		visibleArea[iCam][5]++;
	}
	
	bin = cvGetHistValue_3D(previousHist[iCam][0], R/32, G/32, B/32);
	*bin = *bin + 1;
	visibleArea[iCam][0]++;
}
double HumanModel::checkingVisisble(int iCam, int iPart)
{
	if (ecclipseArea[iCam] == 0) return 0;
	if (iPart == 0)
	{
		return visibleArea[iCam][iPart]/ecclipseArea[iCam];
	}
	else if (iPart == 5)
	{
		return 4*visibleArea[iCam][iPart]/ecclipseArea[iCam];
	}
	else if (iPart == 6)
	{
		double temp = visibleArea[iCam][iPart]/(ecclipseArea[iCam]*0.75);
		return visibleArea[iCam][iPart]/(ecclipseArea[iCam]*0.75);
	}
	else if (iPart <= 4)
	{
		return 4*visibleArea[iCam][iPart]/ecclipseArea[iCam];
	}
}
CvHistogram* HumanModel::getPreviousHist(int iCam, int iPart)
{
	return previousHist[iCam][iPart];
}
int HumanModel::countNumberFullView()
{
	int count = 0;
	for (int iCam = 0; iCam < nCam; iCam++)
	{
		if (fullView[iCam])
		{
			if (visibleArea[iCam][0]/ecclipseArea[iCam] > 0.85)
			{
				count++;
			}
		}
	}
	return count;
}

void HumanModel::updateModel(HumanModel obj)
{
	for (int iCam = 0; iCam < nCam; iCam++)
	{
		if (!fullView[iCam] && obj.getFullView(iCam) && (obj.visibleArea[iCam][0]/obj.ecclipseArea[iCam] > 0.85))  //copy full
		{
			cout<<"new color update\n";
			for (int iPart = 0; iPart < nPart; iPart++)
			{
				cvCopyHist(obj.getPreviousHist(iCam, iPart), &previousHist[iCam][iPart]);
			}
			fullView[iCam] = true;
			learningRate[iCam]++;
		}
		else if (obj.getFullView(iCam) && (obj.visibleArea[iCam][0]/obj.ecclipseArea[iCam] > 0.85))   
		{
			if (learningRate[iCam] == 0)
			{
				cout<<"old color update\n";
				for (int iPart = 0; iPart < nPart; iPart++)
				{
					cvCopyHist(obj.getPreviousHist(iCam, iPart), &previousHist[iCam][iPart]);
				}
				learningRate[iCam]++;
			}
			else
			{
				learningRate[iCam]++;
				if (learningRate[iCam] >= 15) learningRate[iCam] = 0;
			}
		}
	}
}
double HumanModel::compareHist(HumanModel obj)
{
	double result = 0;
	double count = 0;
	//int countPart0 = 0;
	for (int iCam = 0; iCam < nCam; iCam++)
	{
		if (fullView[iCam])
		{
			for (int iPart = 0; iPart < nPart; iPart++)
			{
				if (obj.checkingVisisble(iCam, iPart) > 0.85)
				{
					double temp = cvCompareHist(obj.getPreviousHist(iCam, iPart), previousHist[iCam][iPart], CV_COMP_BHATTACHARYYA);
					if (iPart == 5) 
					{
						temp = temp*2;
						count += 2;
						//countPart0++;
					}
					else if (iPart != 0) 
					{
						temp = temp*0.5;
						count += 0.5;
					}
					else 
					{
						count++;
					}
					result += temp;
				}
			}
		}
	}
	
	if (count != 0) result = result/count;
	//if (countPart0 == 0) result += 10;
	return result;
}
void HumanModel::printColor(ofstream& logFile)
{
	logFile<<"id "<<id<<"\n";
	float* bin;
	for (int iCam = 0; iCam < nCam; iCam++)
	{
		logFile<<"fullView"<<iCam<<" "<<fullView[iCam]<<" learningRate "<<learningRate[iCam]<<"\n";
		if (fullView[iCam])
		{
			for (int iPart = 0; iPart < nPart; iPart++)
			{
				logFile<<"iPart "<<iPart<<"\n";
				for (int R = 0; R < 8; R++)
				{
					for (int G = 0; G < 8; G++)
					{
						
						for (int B = 0; B < 8; B++)
						{
							bin = cvGetHistValue_3D(previousHist[iCam][iPart], R, G, B);
							logFile<<*bin<<" ";
						}
						
					}
				}
				logFile<<"\n";
			}
		}
	}
}


void HumanModel::makeEllipseMaskTmp(int imgWidth, int imgHeight)
{
	cout<<"make new ellipse\n";
	if (isFirstEntrance()) cout<<"check make   "<<gridPos.x<<"   "<<gridPos.y<<"\n";
	for (unsigned int iCam = 0; iCam < nCam; iCam++)
	{
		ellipseMaskTmp[iCam] = cvCreateImage(cvSize(imgWidth, imgHeight), 8, 1);
		cvZero(ellipseMaskTmp[iCam]);
		CvRect originalRect, newRect,rect;
		bool visible;
	
		visible = this->visible[iCam];
		rect = this->rect[iCam];
		originalRect = this->originalRect[iCam];
		if (visible)
		{
			fullView[iCam] = false;
			ecclipseArea[iCam] = 0;
			if (rect.x == originalRect.x && rect.y == originalRect.y && rect.width == originalRect.width && rect.height == originalRect.height)
			{
				setFullView(iCam);
			}
			CvPoint center;
			center.x = originalRect.x + originalRect.width/2;
			center.y = originalRect.y + originalRect.height/3;
			CvSize axes;
			axes.width = originalRect.width/2;
			axes.height = originalRect.height/3;

			setEcclipseArea(iCam, axes.width, axes.height);
		
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
		}
	}
	ellipseStatus = 0;
}
void HumanModel::updateEllipseMaskTmp()
{
	//cout<<"update new ellipse\n";

	if (isFirstEntrance()) return;
	for (unsigned int iCam = 0; iCam < nCam; iCam++)
	{
		cvZero(ellipseMaskTmp[iCam]);
		CvRect originalRect, newRect,rect;
		bool visible;
	
		visible = this->visible[iCam];
		rect = this->rect[iCam];
		originalRect = this->originalRect[iCam];
		if (visible)
		{
			fullView[iCam] = false;
			ecclipseArea[iCam] = 0;
			if (rect.x == originalRect.x && rect.y == originalRect.y && rect.width == originalRect.width && rect.height == originalRect.height)
			{
				setFullView(iCam);
			}
			CvPoint center;
			center.x = originalRect.x + originalRect.width/2;
			center.y = originalRect.y + originalRect.height/3;
			CvSize axes;
			axes.width = originalRect.width/2;
			axes.height = originalRect.height/3;

			setEcclipseArea(iCam, axes.width, axes.height);
		
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
		}
	}
	ellipseStatus = 1;
}
void HumanModel::releaseEllipseMaskTmp()
{
	for (int iCam = 0; iCam < nCam; iCam++)
	{
		if (ellipseStatus != -1) cvReleaseImage(&ellipseMaskTmp[iCam]);
	}
}

void HumanModel::readHist(int value, int pos, int iCam, int iPart)
{
	float* bin = cvGetHistValue_3D(previousHist[iCam][iPart], pos/64, (pos%64)/8, (pos%64)%8);
	*bin = value;
}