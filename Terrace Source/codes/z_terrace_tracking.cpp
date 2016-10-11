#include "z_terrace_tracking.h"


z_terrace_tracking::z_terrace_tracking()
{

	beginFrame = 0;
	countFrame = beginFrame;
	nFrame = 1000;

	for (int iCam = 0; iCam < nCAM; iCam++)
	{
		preName[iCam] = createPreName("C:\\data\\EPFLterrace", "img", iCam);
		preNameBS[iCam] = createPreName("C:\\data\\EPFLterraceBS", "img", iCam);
		fileName[iCam] = createFileName(preName[iCam], beginFrame);
		fileNameBS[iCam] = createFileName(preNameBS[iCam], beginFrame);
		img[iCam] = cvLoadImage(fileName[iCam].c_str(), 1);
		imgBS[iCam] = cvLoadImage(fileNameBS[iCam].c_str(), 0);
		imgSynt[iCam] = cvCreateImage(cvSize(imgBS[iCam]->width, imgBS[iCam]->height), imgBS[iCam]->depth, imgBS[iCam]->nChannels);
	}
	std::uniform_real_distribution<double> rnd_add_f1(0.015, 0.2);
	std::uniform_int_distribution<> rnd_add_f2(0, 29);
	std::uniform_int_distribution<> rnd_update_f(0, 24);
	std::uniform_int_distribution<> rnd_step_f(0, 1);
	

	std::random_device rd;
    std::mt19937 engine(rd()); // knuth_b fails in MSVC2010, but compiles in GCC
	rnd_add1 = std::bind(rnd_add_f1, engine);
	rnd_add2 = std::bind(rnd_add_f2, engine);
	rnd_update = std::bind(rnd_update_f, engine);
	rnd_step = std::bind(rnd_step_f, engine);
	

	nObject = 0;
	coundAddSample = 0;
	initCamera();
}
// uniform random number generator
double z_terrace_tracking::uniform_random() {
   
    return (double) rand() / (double) RAND_MAX;
   
}

// Gaussian random number generator
double z_terrace_tracking::gaussian_random() {
   
    static int next_gaussian = 0;
    static double saved_gaussian_value;
   
    double fac, rsq, v1, v2;
   
    if(next_gaussian == 0) {
       
        do {
            v1 = 2.0 * uniform_random() - 1.0;
            v2 = 2.0 * uniform_random() - 1.0;
            rsq = v1 * v1 + v2 * v2;
        }
        while(rsq >= 1.0 || rsq == 0.0);
        fac = sqrt(-2.0 * log(rsq) / rsq);
        saved_gaussian_value = v1 * fac;
        next_gaussian = 1;
        return v2 * fac;
    }
    else {
        next_gaussian = 0;
        return saved_gaussian_value;
    }
}

double z_terrace_tracking::normal_distribution(double mean, double standardDeviation, double state) {
   
    double variance = standardDeviation * standardDeviation;
   
    return exp(-0.5 * (state - mean) * (state - mean) / variance ) / sqrt(2 * PI * variance);
}

z_terrace_tracking::~z_terrace_tracking()
{
}

void z_terrace_tracking::process()
{
	

	//cvNamedWindow("img0", 1);
	cvNamedWindow("img1", 1);
	cvNamedWindow("img2", 1);
	//cvNamedWindow("img3", 1);

	//cvNamedWindow("imgBS0", 1);
	cvNamedWindow("imgBS1", 1);
	cvNamedWindow("imgBS2", 1);
	//cvNamedWindow("imgBS3", 1);

	cvNamedWindow("imgsynt0", 1);
	//cvNamedWindow("imgsynt1", 1);
	//cvNamedWindow("imgsynt2", 1);
	//cvNamedWindow("imgsynt3", 1);
	
	

	for (int i = 0; i < 1000; i++)
	{
		//cout<<rnd_step()<<"      "<<rnd_update()<<"     "<<rnd_add1()<<"     "<<rnd_add2()<<"\n";
	}
	nObjectSample[0] = nObject;
	double prior1 = prior(0);
	double like1 = likelyhoodFunction(0);
	double post1 = prior1*like1;
	probAcceptance[0] = post1;

	while(true)
	{
		countFrame++;
		cout<<countFrame<<"\n";
		if (cvWaitKey(1) == 27 || countFrame > nFrame) return;
		for (int iCam = 0; iCam < nCAM; iCam++)
		{
			cvReleaseImage(&img[iCam]);
			cvReleaseImage(&imgBS[iCam]);
			fileName[iCam] = createFileName(preName[iCam], countFrame);
			fileNameBS[iCam] = createFileName(preNameBS[iCam], countFrame);
			img[iCam] = cvLoadImage(fileName[iCam].c_str(), 1);
			imgBS[iCam] = cvLoadImage(fileNameBS[iCam].c_str(), 0);
		}
		makeEntraceProb();
		

		if (maxEntraceProb > 0.015 && maxEntraceProb < 0.3)
		{
			//add(0);
			//cout<<maxEntraceProb<<"ssssssssssssssssssssssssssssssssssssssssssssssssssssssss\n";
			Rectangle* rect = findRectangle_z(maxEntraceProbPosX, maxEntraceProbPosY, img[2], 2, HEIGHT_AVERAGE);
			CvPoint* point = new CvPoint[2];
			point[0].x = rect->xmax;
			point[0].y = rect->ymax;
			point[1].x = rect->xmin;
			point[1].y = rect->ymin;
			cvRectangle(img[2], point[0], point[1], CV_RGB(0, 0, 255), 1, 8, 0);
			int stepPos = ENTRANCE_Y_MAX - ENTRANCE_Y_MIN + 1;
			for (int i = 0; i < 300; i++) markPosition(addPosSample[i]/stepPos + ENTRANCE_X_MIN, addPosSample[i]%stepPos + ENTRANCE_Y_MIN, img[2], 2);
			//markPosition(maxEntraceProbPosX, maxEntraceProbPosY, img[2], 2);

			if (countFrame == 80) 
				cout<<countFrame<<"\n";
			Rectangle* rect2 = findRectangle_z(maxEntraceProbPosX, maxEntraceProbPosY, img[1], 1, HEIGHT_AVERAGE);
			CvPoint* point2 = new CvPoint[2];
			point2[0].x = rect2->xmax;
			point2[0].y = rect2->ymax;
			point2[1].x = rect2->xmin;
			point2[1].y = rect2->ymin;
			
			cvRectangle(img[1], point2[0], point2[1], CV_RGB(0, 0, 255), 1, 8, 0);
			markPosition(maxEntraceProbPosX, maxEntraceProbPosY, img[1], 1);
		}
		
		//initSyntImg(imgSynt[2]);
		//Rectangle* rect = findRectangle_z(0, 0, img[2], 2, HEIGHT_AVERAGE);
		//makeSyntImgFromRect(imgSynt[2], rect);
		//cvShowImage("img0", img[0]);
		cvShowImage("img1", img[1]);
		cvShowImage("img2", img[2]);
		//cvShowImage("img3", img[3]);

		//cvShowImage("imgBS0", imgBS[0]);
		cvShowImage("imgBS1", imgBS[1]);
		cvShowImage("imgBS2", imgBS[2]);
		//cvShowImage("imgBS3", imgBS[3]);

		cvShowImage("imgSynt0", imgSynt[2]);
	}
	cvDestroyAllWindows();
}

void z_terrace_tracking::initSyntImg(IplImage* img)
{
	cvZero(img);
}
void z_terrace_tracking::makeSyntImgFromRect(IplImage* img, Rectangle* rect)
{
	cvRectangleR(img, cvRect(rect->xmin, rect->ymin, rect->xmax - rect->xmin, rect->ymax - rect->ymin),cvScalarAll(255), CV_FILLED);  
}
int z_terrace_tracking::and_2_images(IplImage* img_BS, IplImage* img_Synt)
{
	int count = 0;
	int size = img_BS->height*img_BS->width;
	for (int i = 0; i < size; i++)
	{
		if (img_BS->imageData[i] != 0 && img_Synt->imageData[i] != 0) count++;
	}
	return count;
}
void z_terrace_tracking::makeEntraceProb()
{
	Rectangle* rect;
	sum_entrance_prob = 0;
	double count1[nCAM]; 
	double count2[nCAM]; 
	double count;
	for (int x = ENTRANCE_X_MIN; x <= ENTRANCE_X_MAX; x++)
		for (int y = ENTRANCE_Y_MIN; y <= ENTRANCE_Y_MAX; y++)
		{
			count = 0;
			for (int iView = 1; iView < 3; iView++)
			{
				count1[iView] = 0;
				count2[iView] = 0;
				initSyntImg(imgSynt[iView]);
				//x = 0; y = 16;
				rect = findRectangle_z(x, y, img[iView], iView, HEIGHT_AVERAGE); 
				if (rect->visible == true)
				{
					count2[iView] = (rect->xmax - rect->xmin)*(rect->ymax - rect->ymin);
					makeSyntImgFromRect(imgSynt[iView], rect);
					count1[iView] = and_2_images(imgBS[iView], imgSynt[iView]);
					//cout<<x<<"  "<<y<<"   "<<iView<<"   "<<count1[iView]<<"   "<<count2[iView]<<"\n";
					count += (rect->prob*count1[iView]/count2[iView]);
					//cout<<x<<"  "<<y<<"   "<<count<<"\n";
				}
			}
			entrance_prob[x - ENTRANCE_X_MIN][y - ENTRANCE_Y_MIN] = count;
			sum_entrance_prob += count;
		}
	if (sum_entrance_prob != 0)
	{
		for (int x = ENTRANCE_X_MIN; x <= ENTRANCE_X_MAX; x++)
			for (int y = ENTRANCE_Y_MIN; y <= ENTRANCE_Y_MAX; y++)
			{
				entrance_prob[x - ENTRANCE_X_MIN][y - ENTRANCE_Y_MIN] = entrance_prob[x - ENTRANCE_X_MIN][y - ENTRANCE_Y_MIN]/sum_entrance_prob;
				//cout<<entrance_prob[x - ENTRANCE_X_MIN][y - ENTRANCE_Y_MIN]<<"\n";
			}
	}
	maxEntraceProb = 0;
	maxEntraceProbPosX = maxEntraceProbPosY = -1;
	for (int x = ENTRANCE_X_MIN; x <= ENTRANCE_X_MAX; x++)
		for (int y = ENTRANCE_Y_MIN; y <= ENTRANCE_Y_MAX; y++)
		{
			if (maxEntraceProb < entrance_prob[x - ENTRANCE_X_MIN][y - ENTRANCE_Y_MIN])
			{
				maxEntraceProb = entrance_prob[x - ENTRANCE_X_MIN][y - ENTRANCE_Y_MIN];
				maxEntraceProbPosX = x;
				maxEntraceProbPosY = y;
			}
		}
	if (maxEntraceProb > 0.015 && maxEntraceProb < 0.3)
	{
		int stepPos = ENTRANCE_Y_MAX - ENTRANCE_Y_MIN + 1;
		double rnd1;
		int rnd2;
		for (int i = 0; i < 300; i++)
		{
			while (true)
			{
				rnd1 = rnd_add1();
			
				rnd2 = rnd_add2();
				//cout<<rnd2<<"    "<<rnd1<<"\n";
				if (rnd1 <= entrance_prob[rnd2/stepPos][rnd2%stepPos])
				{
					addPosSample[i] = rnd2;
					//cout<<rnd2<<"    "<<rnd1<<"\n";
					break;
				}
			}
		}
	}
	
	
}
void z_terrace_tracking::createSyntImg(int sampleIndex)
{
	Rectangle* rect;
	int numberObject = nObjectSample[sampleIndex];
	for (int iCam = 0; iCam < nCAM; iCam++)
	{
		initSyntImg(imgSynt[iCam]);
		for (int i = 0; i < numberObject; i++)
		{
			rect = findRectangle_z(objectSample[sampleIndex][i].posx, objectSample[sampleIndex][i].posy, img[iCam], iCam, HEIGHT_AVERAGE); 
			if (rect->visible == true)
			{
				makeSyntImgFromRect(imgSynt[iCam], rect);
			}
		}
	}
}
int z_terrace_tracking::compareImage(IplImage* img1, IplImage* img2)
{
	int count = 0;
	int size = img1->height*img1->width;
	for (int i = 0; i < size; i++)
	{
		if (img1->imageData[i] != img2->imageData[i]) count++;
	}
	return count;
}
long int z_terrace_tracking::similarityCalculation()
{
	long int result = 0;
	for (int iCam = 0; iCam < nCAM; iCam++)
	{
		result += compareImage(imgBS[iCam], imgSynt[iCam]);
	}
	return result;
}
int z_terrace_tracking::ghostCalculation(int sampleIndex)
{
	Rectangle* rect;
	double count1, count2;
	int count = 0;
	int numberObject = nObjectSample[sampleIndex];
	for (int objectIndex = 0; objectIndex < numberObject; objectIndex++)
	{
		for (int iCam = 0; iCam < nCAM; iCam++)
		{
			count1 = count2 = 0;    
			rect = findRectangle_z(objectSample[sampleIndex][objectIndex].posx, objectSample[sampleIndex][objectIndex].posy, img[iCam], iCam, HEIGHT_AVERAGE); 
			if (rect->visible == true)
			{
				for (int j = rect->ymin; j <= rect->ymax; j++)
				{
					int step = j*imgBS[iCam]->width; 
					for (int i = rect->xmin; i <= rect->xmax; i++)
					{
						if (imgBS[iCam]->imageData[step + i] != 0) count1++;
					}
				}
				count2 = (rect->xmax - rect->xmin)*(rect->ymax - rect->ymin);
				if (count1/count2 < 0.1) 
				{
					count++;
					break;
				}
			}
		}
	}
	return count;
}
double z_terrace_tracking::likelyhoodFunction(int sampleIndex)
{
	double result = 0;
	createSyntImg(sampleIndex);
	long int temp1 = similarityCalculation();
	int temp2 = ghostCalculation(sampleIndex);
	result = -temp1/10000-1000*temp2;
	result = exp(result);
	return result;
}
long int z_terrace_tracking::overlapCost(int sampleIndex)
{
	long int sumD = 0;
	Rectangle *rect, *rect2;
	int numberObject = nObjectSample[sampleIndex];
	int minD;
	int count = 0;
	IplImage* imgTmp = cvCreateImage(cvSize(imgBS[0]->width, imgBS[0]->height), imgBS[0]->depth, imgBS[0]->nChannels);
	for (int objectIndex = 0; objectIndex < numberObject; objectIndex++)
	{
		minD = 0;
		for (int iCam = 0; iCam < nCAM; iCam++)
		{ 
			initSyntImg(imgTmp);
			rect = findRectangle_z(objectSample[sampleIndex][objectIndex].posx, objectSample[sampleIndex][objectIndex].posy, img[iCam], iCam, HEIGHT_AVERAGE); 
			if (rect->visible == true)
			{
				for (int objectIndex2 = 0; objectIndex2 < numberObject && objectIndex2 != objectIndex; objectIndex2++)
				{
					rect2 = findRectangle_z(objectSample[sampleIndex][objectIndex2].posx, objectSample[sampleIndex][objectIndex2].posy, img[iCam], iCam, HEIGHT_AVERAGE); 
					if (rect2->visible == true)
					{
						makeSyntImgFromRect(imgTmp, rect2);
					}
				}
				count = 0;
				for (int j = rect->ymin; j <= rect->ymax; j++)
				{
					int step = j*imgTmp->width;
					for (int i = rect->xmin; i <= rect->xmax; i++)
					{
						if (imgTmp->imageData[step + i] != 0) count++;
					}
				}
				if(count > minD) minD = count;
			}
		}
		sumD += minD; 
	}
	return sumD;
}
double z_terrace_tracking::prior(int sampleIndex)
{
	int numberObject = nObjectSample[sampleIndex];
	int count = 0;
	for (int objectIndex = 0; objectIndex < numberObject; objectIndex++)
	{
		for (int objectIndex2 = 0; objectIndex2 < numberObject && objectIndex2 != objectIndex; objectIndex2++)
		{
			int x1 = objectSample[sampleIndex][objectIndex].posx;
			int y1 = objectSample[sampleIndex][objectIndex].posy;
			int x2 = objectSample[sampleIndex][objectIndex2].posx;
			int y2 = objectSample[sampleIndex][objectIndex2].posy;
			if (abs(x1 - x2) <= 1 && abs(y1 - y2) <= 1)
			{
				count += 10;
			}
		}
	}
	long int sumD = overlapCost(sampleIndex);
	double prior = sumD + count + 100*numberObject;
	return exp(-prior);
}
double z_terrace_tracking::posterior(int sampleIndex)
{
	double prior1 = prior(sampleIndex);
	double like1 = likelyhoodFunction(sampleIndex);
	double post1 = prior1*like1;
	return post1;
}

void z_terrace_tracking::add(int sampleIndex)
{
	//add sample for adding step
	
}
vector<string> split(const string& s, const string& delim, const bool keep_empty = true) {
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

void z_terrace_tracking::process2()
{
	cvNamedWindow("img0", 1);
	cvNamedWindow("img1", 1);
	cvNamedWindow("img2", 1);
	cvNamedWindow("img3", 1);


	double prob[1320];  
	char* temp2 = new char[100];
	char* b2 = new char[100];
	for (int f = 0; f < 1500; f++)
	{
		if (cvWaitKey(1) == 27 || countFrame > nFrame) return;
		
		
		string a2 = "D:\\EPFLtmpprob\\";
		a2 += "proba-f";
		itoa(f,temp2,10);
		a2 += temp2;
		a2 += ".txt";
		strcpy(b2, a2.c_str());
		ifstream result(b2);
		string pro;
		for (int i = 0; i < 1320; i++)
		{
			getline(result, pro);
			prob[i] = atof(split(pro, " ")[1].c_str());
			//cout<<prob[i];
		}
		result.close();

		for (int iCam = 0; iCam < nCAM; iCam++)
		{
			cvReleaseImage(&img[iCam]);
			fileName[iCam] = createFileName(preName[iCam], f);
			img[iCam] = cvLoadImage(fileName[iCam].c_str(), 1);
		}
		int posx ,posy;
		for (int pos = 0; pos < 1320; pos++)
		{
			if (prob[pos] > 0.99)
			{
				posx = pos%30*2;
				posy = pos/30*2;
				for (int icam = 0; icam < 4; icam++)
				{
					//cout<<pos<<"\n";
					Rectangle* rect = findRectangle_z(posx, posy, img[icam], icam, HEIGHT_AVERAGE);
					CvPoint* point = new CvPoint[2];
					point[0].x = rect->xmax;
					point[0].y = rect->ymax;
					point[1].x = rect->xmin;
					point[1].y = rect->ymin;
					if (rect->visible) cvRectangle(img[icam], point[0], point[1], CV_RGB(0, 0, 255), 1, 8, 0);
				
				}
			}
		}
		cvShowImage("img0", img[0]);
		cvShowImage("img1", img[1]);
		cvShowImage("img2", img[2]);
		cvShowImage("img3", img[3]);
	}
	cvDestroyAllWindows();
}