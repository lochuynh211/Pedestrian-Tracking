#include "z_video_converter.h"


//file format of output is: "fileName-v-f.png" 
//type = 0 : original video
//type = 1 : have to cut the image size (all view in one video)
void imageFromVideo(char* filename, char* outFolder, char* outName, int nView, int beginTime, int nFrame, bool type)
{
	CvCapture* capture = cvCaptureFromFile(filename);
	int countFrame = 0;
	string preName = "";
	
	IplImage* videoImg;
	

	int frameRate = (int)(cvGetCaptureProperty(capture, CV_CAP_PROP_FPS));
	int frameWidth = (int)(cvGetCaptureProperty(capture, CV_CAP_PROP_FRAME_WIDTH));
	int frameHeight = (int)(cvGetCaptureProperty(capture, CV_CAP_PROP_FRAME_HEIGHT));
	int maxFrame = (int)(cvGetCaptureProperty(capture, CV_CAP_PROP_FRAME_COUNT));
	if (nFrame > maxFrame) nFrame = maxFrame;
	cout<<"frame rate"<<"      "<<frameRate<<"\n";
	cout<<"frame width"<<"      "<<frameWidth<<"\n";
	cout<<"frame height"<<"      "<<frameHeight<<"\n";
	
	cvSetCaptureProperty(capture, CV_CAP_PROP_POS_FRAMES, beginTime*frameRate);
	
	
	preName += outFolder;
	preName += outName;
	preName += "-v";
	
    char* tmp1 = new char[2];
    itoa(nView,tmp1,10);
	preName += tmp1;
	preName += "-f";

	char* tmp2 = new char[6];
    string tmpString;
	
	if (type == 0)
	{
		while (countFrame < nFrame)
		{
			itoa(countFrame,tmp2,10);
			tmpString = preName;
			tmpString += tmp2;
			tmpString += ".png";
			countFrame++;
			cvGrabFrame(capture);
			videoImg = cvRetrieveFrame(capture);
			cvSaveImage(tmpString.c_str(), videoImg);
		}
		cvReleaseCapture(&capture);
	}
	else if (type == 1)
	{
		IplImage* tmpImg = cvCreateImage(cvSize(800, 600), 8,3);
		while (countFrame < nFrame)
		{
			itoa(countFrame,tmp2,10);
			tmpString = preName;
			tmpString += tmp2;
			tmpString += ".png";
			countFrame++;
			cvGrabFrame(capture);
			videoImg = cvRetrieveFrame(capture);
			tmpImg->imageData = &videoImg->imageData[nView*600*frameWidth*3];
			cvSaveImage(tmpString.c_str(), tmpImg);
		}
		cvReleaseCapture(&capture);
	}
}

void videoFromImage(char* filename, char* inFolder, char* inName, int nView, int beginFrame, int nFrame, double frameRate)
{
	IplImage* img;
	string preName = "";	
	preName += inFolder;
	preName += inName;
	preName += "-v";
    char* tmp1 = new char[2];
    itoa(nView,tmp1,10);
	preName += tmp1;
	preName += "-f";

	char* tmp2 = new char[6];
    string tmpString;
	
	//get image size from the first frame
	itoa(beginFrame,tmp2,10);
	tmpString = preName;
	tmpString += tmp2;
	tmpString += ".png";
	img = cvLoadImage(tmpString.c_str(), 1);
	CvVideoWriter* write = cvCreateVideoWriter(filename, 0, frameRate, cvSize(img->width, img->height), 1);
	
	for (int countFrame = beginFrame; countFrame < nFrame; countFrame++)
	{
			itoa(countFrame,tmp2,10);
			tmpString = preName;
			tmpString += tmp2;
			tmpString += ".png";
			img = cvLoadImage(tmpString.c_str(), 1);
			cvWriteFrame(write, img);
			cout<<countFrame<<"\n";
	}
	cvReleaseVideoWriter(&write);
}

string createPreName(char* inFolder, char* inName, int nView)
{
	string preName = "";	
	preName += inFolder;
	preName += "\\";
	preName += inName;
	preName += "-v";
    char* tmp1 = new char[2];
    itoa(nView,tmp1,10);
	preName += tmp1;
	preName += "-f";
	return preName;
}
string createFileName(string preName, int countFrame)
{
	char* tmp2 = new char[6];
    string tmpString;
	itoa(countFrame,tmp2,10);
	tmpString = preName;
	//tmpString += "\\";
	tmpString += tmp2;
	tmpString += ".png";
	return tmpString;
}
string createFileNameLog(string preName, int countFrame)
{
	char* tmp2 = new char[6];
    string tmpString;
	itoa(countFrame,tmp2,10);
	tmpString = preName;
	//tmpString += "\\";
	tmpString += tmp2;
	tmpString += ".txt";
	return tmpString;
}