#ifndef z_video_converter_H
#define z_video_converter_H

#include <iostream>
#include <stdio.h>
#include <stdlib.h>

#include <fstream>
#include <opencv2\highgui\highgui.hpp>

using namespace std;
using namespace cv;

void imageFromVideo(char* filename, char* outFolder, char* outName, int nView, int beginTime, int nFrame, bool type);
void videoFromImage(char* filename, char* inFolder, char* inName, int nView, int beginFrame, int nFrame, double frameRate);
string createPreName(char* inFolder, char* inName, int nView);
string createFileName(string preName, int countFrame);
string createFileNameLog(string preName, int countFrame);

#endif