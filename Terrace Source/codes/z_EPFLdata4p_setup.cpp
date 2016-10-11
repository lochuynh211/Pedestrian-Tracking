#include "z_EPFLdata4p_setup.h"



//even: for the ground plane
//odd: for the top plane
//EPFLCalib*ImageView = WorldView;
double EPFLCalib[NMatrix][3][3]={
	{{ 0.176138, 0.647589, -63.412272},	 {-0.180912, 0.622446, -0.125533},	{ -0.000002, 0.001756, 0.102316}},
	{{0.053299,	1.247713,	13.803214},	 {-0.092242,	1.196431,	38.620552},	{ -0.000080,	0.003919,	0.164282}},
	{{0.177291,	0.004724,	31.224545},	 {0.169895,	0.661935,	-79.781865},	{-0.000028,	0.001888,	0.054634}},
	{{0.081202,	0.089186,	14.746465},	 {0.070703,	1.091119,	-8.872128},	{ 0.000001,	0.003361,	0.097146}},
	{{-0.104843,      0.099275,         50.734500},	 {0.107082,       0.102216,         7.822562},	{-0.000054,      0.001922,         -0.068053}},
	{{0,	0,	0},	 {0,	0,	-0},	{ 0,	0,	0}},
	{{-0.142865,	0.553150,	-17.395045},	 {-0.125726,	0.039770,	75.937144},	{-0.000011,	0.001780,	0.015675}},
	{{0,	0,	0},	 {0,	0,	-0},	{ 0,	0,	0}}
};

//ImageView = inverseEPFLCalib*WorldView;
double inverseEPFLCalib[NMatrix][3][3];

void HormoEPFL(double x, double y, double z, double&u, double&v, int icam)
{
	double	x1 = x* EPFLCalib[icam][0][0] + y * EPFLCalib[icam][0][1] + z* EPFLCalib[icam][0][2];
	double	y1 = x* EPFLCalib[icam][1][0] + y * EPFLCalib[icam][1][1] + z* EPFLCalib[icam][1][2];
	double	z1 = x* EPFLCalib[icam][2][0] + y * EPFLCalib[icam][2][1] + z* EPFLCalib[icam][2][2];
	u=x1/z1;
	v=y1/z1;
}
void inverseHormoEPFL(double x, double y, double z, double&u, double&v, int icam)
{
	double	x1 = x* inverseEPFLCalib[icam][0][0] + y * inverseEPFLCalib[icam][0][1] + z* inverseEPFLCalib[icam][0][2];
	double	y1 = x* inverseEPFLCalib[icam][1][0] + y * inverseEPFLCalib[icam][1][1] + z* inverseEPFLCalib[icam][1][2];
	double	z1 = x* inverseEPFLCalib[icam][2][0] + y * inverseEPFLCalib[icam][2][1] + z* inverseEPFLCalib[icam][2][2];
	u=x1/z1;
	v=y1/z1;
}
void inverseMatrix()
{
	double B[NMatrix][3][3];
	for (int k = 0; k < NMatrix; k++)
	{
		double detA = EPFLCalib[k][0][0]*(EPFLCalib[k][1][1]*EPFLCalib[k][2][2] - EPFLCalib[k][2][1]*EPFLCalib[k][1][2])
						- EPFLCalib[k][0][1]*(EPFLCalib[k][1][0]*EPFLCalib[k][2][2] - EPFLCalib[k][2][0]*EPFLCalib[k][1][2])
						+ EPFLCalib[k][0][2]*(EPFLCalib[k][1][0]*EPFLCalib[k][2][1] - EPFLCalib[k][2][0]*EPFLCalib[k][1][1]);
		B[k][0][0] = EPFLCalib[k][1][1]*EPFLCalib[k][2][2] - EPFLCalib[k][2][1]*EPFLCalib[k][1][2];
		B[k][0][1] = - (EPFLCalib[k][1][0]*EPFLCalib[k][2][2] - EPFLCalib[k][2][0]*EPFLCalib[k][1][2]);
		B[k][0][2] = EPFLCalib[k][1][0]*EPFLCalib[k][2][1] - EPFLCalib[k][2][0]*EPFLCalib[k][1][1];
		B[k][1][0] = - (EPFLCalib[k][0][1]*EPFLCalib[k][2][2] - EPFLCalib[k][2][1]*EPFLCalib[k][0][2]);
		B[k][1][1] = EPFLCalib[k][0][0]*EPFLCalib[k][2][2] - EPFLCalib[k][2][0]*EPFLCalib[k][0][2];
		B[k][1][2] = - (EPFLCalib[k][0][0]*EPFLCalib[k][2][1] - EPFLCalib[k][2][0]*EPFLCalib[k][0][1]);
		B[k][2][0] = EPFLCalib[k][0][1]*EPFLCalib[k][1][2] - EPFLCalib[k][1][1]*EPFLCalib[k][0][2];
		B[k][2][1] = - (EPFLCalib[k][0][0]*EPFLCalib[k][1][2] - EPFLCalib[k][1][0]*EPFLCalib[k][0][2]);
		B[k][2][2] = EPFLCalib[k][0][0]*EPFLCalib[k][1][1] - EPFLCalib[k][1][0]*EPFLCalib[k][0][1];
		for (int i = 0; i < 3; i++)
			for (int j = 0; j < 3; j++)
				inverseEPFLCalib[k][i][j] = B[k][j][i]/detA;
	}
}

//from grid position to world position
void grid_to_tv(int pos, int grid_width, int grid_height, 
                float tv_origin_x, float tv_origin_y, float tv_width,
                float tv_height, double &tv_x, double &tv_y) 
{

  tv_x = ( (pos % grid_width) + 0.5 ) * (tv_width / grid_width) + tv_origin_x;
  tv_y = ( (pos / grid_width) + 0.5 ) * (tv_height / grid_height) + tv_origin_y;
}

void setPositionEPFL(IplImage* view2, int X, int Y, int down)
{
	int size = 2;
	int r,g,b;
	r=g=b=0;
	if (down == 0) 
	{
		g = 255;
		b = 255;
	}
	else
	{
		r = 255;
		b = 255;
	}
	if (size <= X && X < view2->width - size && size <= Y && Y < view2->height - size) 
	{
		for (int k = -size; k <= size; k++)
			for (int h = -size; h <= size; h++)
			{
				view2->imageData[(Y+k)*view2->widthStep + (X + h)*3] = r;
				view2->imageData[(Y+k)*view2->widthStep + (X + h)*3 + 1] = g;
				view2->imageData[(Y+k)*view2->widthStep + (X + h)*3 + 2] = b;
			}
	}
}

Rectangle* findRectangle(int pos, IplImage* img, int icam)
{
	double tv_x, tv_y;
	grid_to_tv(pos, 56, 56, 0, 0, 358, 360, tv_x, tv_y);
	double X[10], Y[10];

	inverseHormoEPFL(tv_x + 25, tv_y, 1, X[0], Y[0], icam*2);		
	inverseHormoEPFL(tv_x - 25, tv_y, 1, X[1], Y[1], icam*2);
	inverseHormoEPFL(tv_x, tv_y + 25, 1, X[2], Y[2], icam*2);
	inverseHormoEPFL(tv_x, tv_y - 25, 1, X[3], Y[3], icam*2);
	inverseHormoEPFL(tv_x, tv_y, 1, X[4], Y[4], icam*2);

	if (icam <= 1)
	{
		inverseHormoEPFL(tv_x + 25, tv_y, 1, X[5], Y[5], icam*2 + 1);		
		inverseHormoEPFL(tv_x - 25, tv_y, 1, X[6], Y[6], icam*2 + 1);
		inverseHormoEPFL(tv_x, tv_y + 25, 1, X[7], Y[7], icam*2 + 1);
		inverseHormoEPFL(tv_x, tv_y - 25, 1, X[8], Y[8], icam*2 + 1);
		inverseHormoEPFL(tv_x, tv_y, 1, X[9], Y[9], icam*2 + 1);
	}
	else
	{
		for (int i = 0; i < 5; i++)
		{
			X[i + 5] = X[i];
			Y[i + 5] = 0;
		}
	}

	double xmin, ymin, xmax, ymax;
	xmin = img->width - 1;
	ymin = img->height - 1;
	xmax = 0;
	ymax = 0;
	for (int k = 0; k < 10; k++)
	{
		if (X[k] >= 0 && X[k] < img->width - 1 && Y[k] >=0 && Y[k] < img->height - 1)
		{
			if (X[k] < xmin) xmin = X[k];
			if (Y[k] < ymin) ymin = Y[k];
			if (X[k] > xmax) xmax = X[k];
			if (Y[k] > ymax) ymax = Y[k];
		}
	}
	Rectangle* ret = new Rectangle();
	ret->visible = true;
	ret->xmax = cvRound(xmax);
	ret->xmin = cvRound(xmin);
	ret->ymax = cvRound(ymax);
	ret->ymin = cvRound(ymin);
	if (xmax - xmin < img->width/10 || xmax - xmin > img->width/2) ret->visible = false;
	if (ymax - ymin < img->height/10) ret->visible = false;
	CvPoint* point = new CvPoint[2];
	point[0].x = xmax;
	point[0].y = ymax;
	point[1].x = xmin;
	point[1].y = ymin;
	if (ret->visible == true) cvRectangle(img, point[0], point[1], CV_RGB(255, 255, 255), 1, 8, 0);
	return ret;
}

void setupEPFL()
{
	CvCapture* capture = cvCaptureFromFile("EPFL data\\6p-c1.avi");
	
	cvNamedWindow("test", 1);
	

	int frameRate = (int)(cvGetCaptureProperty(capture, CV_CAP_PROP_FPS));
	int frameWidth = (int)(cvGetCaptureProperty(capture, CV_CAP_PROP_FRAME_WIDTH));
	int frameHeight = (int)(cvGetCaptureProperty(capture, CV_CAP_PROP_FRAME_HEIGHT));
	int beginTime = 0;
	cout<<"frame rate"<<"      "<<frameRate<<"\n";
	cout<<"frame width"<<"      "<<frameWidth<<"\n";
	cout<<"frame height"<<"      "<<frameHeight<<"\n";
	int countFrame = 0;
	double M, N;
	double X, Y;
	double X1, Y1;
	
	
	IplImage* img = cvCreateImage(cvSize(frameWidth, frameHeight), 8, 3);
	inverseMatrix();
	cvSetCaptureProperty(capture, CV_CAP_PROP_POS_FRAMES, beginTime*frameRate);	
	int pos = 0; 
	
	
	
	int gridSize = 56*56;

	/*
	ofstream myfile;
	myfile.open ("EPFL indoor setup.txt");
	for (int icam = 0; icam < 4; icam++)
	{
		for (pos = 0; pos < gridSize; pos++)
		{
			Rectangle* a = findRectangle(pos, img, icam);
			if (a->visible) myfile<<"RECTANGLE "<<icam<<" "<<pos<<" "<<a->xmin<<" "<<a->ymin<<" "<<a->xmax<<" "<<a->ymax<<"\n";
			else myfile<<"RECTANGLE "<<icam<<" "<<pos<<" notvisible"<<"\n";
		}
	}
	myfile.close();
	*/
	if (cvWaitKey(30) == 27) return;
	cvGrabFrame(capture);
	img = cvRetrieveFrame(capture);

	while (true)
	{
		countFrame++;
		if (cvWaitKey(30) == 27) return;
		//cvGrabFrame(capture);
		//img = cvRetrieveFrame(capture);
		//pos = 2138;
		pos = 1425;
		int icam = 1;
		for (pos = 0; pos < 56*56; pos++)
		{
			grid_to_tv(pos, 56, 56, 0, 0, 358, 360, M, N);   //doi tu vi tri grid sang vi tri topview
		    //findRectangle(pos, img, icam);  //icam : 0, 1, 2, 3
			inverseHormoEPFL(M, N, 1, X, Y, icam*2);
			setPositionEPFL(img, cvRound(X), cvRound(Y), 0);
		}
		/*
		//grid_to_tv(pos, 56, 56, 0, 0, 358, 360, M, N);
		inverseHormoEPFL(M + 25, N, 1, X1, Y1, icam*2);
		
		setPositionEPFL(img, cvRound(X1), cvRound(Y1), 0);

		inverseHormoEPFL(M - 25, N, 1, X1, Y1, icam*2);
		setPositionEPFL(img, cvRound(X1), cvRound(Y1), 0);

		inverseHormoEPFL(M, N + 25, 1, X1, Y1, icam*2);
		setPositionEPFL(img, cvRound(X1), cvRound(Y1), 1);

		inverseHormoEPFL(M, N - 25, 1, X1, Y1, icam*2);
		setPositionEPFL(img, cvRound(X1), cvRound(Y1), 1);
		*/


		cvShowImage("test", img);
		
		if (countFrame > 900)  break;
		
	}
	cvReleaseImage(&img);
	cvReleaseCapture(&capture);
	cvDestroyAllWindows();
	
}