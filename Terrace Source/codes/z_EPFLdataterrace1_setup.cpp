#include "z_EPFLdataterrace1_setup.h"
#include "calibration\z_cameraModel.h"

#define nCam 4


#define IMG_WITDH 720
#define IMG_HEIGHT 576
#define nfx 576
#define ncx 576
#define dpx 2.3000000000e-02
#define dpy 2.3000000000e-02
#define dx 2.3000000000e-02
#define dy 2.3000000000e-02


double focal[nCam] = {20.161920, 19.529144, 19.903218, 20.047015};
double kappa1[nCam] = {5.720865e-04, 5.184242e-04, 3.511557e-04, 4.347668e-04};
double cx[nCam] = {366.514507, 360.228130, 355.506436, 349.154019};
double cy[nCam] = {305.832552, 255.166919, 241.205640, 245.786168};
double sx[nCam] = {1, 1, 1, 1};
//     
double rx[nCam] = {1.9007833770e+00, 1.9347282363e+00, -1.8289537286e+00, -1.8418460467e+00};
double ry[nCam] = {4.9730769727e-01, -7.0418616982e-01, 3.7748154985e-01, -4.6728290805e-01};
double rz[nCam] = {1.8415452559e-01, -2.3783238362e-01, 3.0218614321e+00, -3.0205552749e+00};
double tx[nCam] = {-4.8441913843e+03, -65.433635, 1.9782813424e+03, 4.6737509054e+03};
double ty[nCam] = {5.5109448682e+02, 1594.811988, -9.4027627332e+02, -2.5743341287e+01};
double tz[nCam] = {4.9667438357e+03, 2113.640844, 1.2397750058e+04, 8.4155952460e+03};

CameraModel* camera[nCam];

void initCamera()
{
	for (int i = 0; i < nCam; i++)
	{
		camera[i] = new CameraModel();
		camera[i]->setGeometry(IMG_WITDH, IMG_HEIGHT, ncx, nfx, dx, dy, dpx, dpy);
		camera[i]->setIntrinsic(focal[i], kappa1[i], cx[i], cy[i], sx[i]);
		camera[i]->setExtrinsic(tx[i], ty[i], tz[i], rx[i], ry[i], rz[i]);
		camera[i]->internalInit();
	}
}
void setPositionEPFL2(IplImage* view2, int X, int Y, int down)
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




void test()
{

	initCamera();
	IplImage* img0 = cvLoadImage("D:\\EPFLterrace\\img-v0-f4497.png", 1);
	IplImage* img1 = cvLoadImage("D:\\EPFLterrace\\img-v1-f4497.png", 1);
	IplImage* img2 = cvLoadImage("D:\\EPFLterrace\\img-v2-f4497.png", 1);
	IplImage* img3 = cvLoadImage("D:\\EPFLterrace\\img-v3-f4497.png", 1);
	cvNamedWindow("test0", 1);
	cvNamedWindow("test1", 1);
	cvNamedWindow("test2", 1);
	cvNamedWindow("test3", 1);
	
	int x,y;
	double xu, yu;
	double inPoint[3];
	
	inverseMatrixa();
	double M, N, X, Y;
	int pos = 450;
	grid_to_tva(914, 30, 44, -500, -1500, 7500, 11000, M, N);   //doi tu vi tri grid sang vi tri topview
	inverseHormoEPFLa(M, N, 20, X, Y, 0*2);

	inPoint[0] = 3500;
	inPoint[1] = 7500;
	inPoint[2] = 20;

	


    for (inPoint[0] = -500; inPoint[0] <= 7000; inPoint[0] += 250)
		for (inPoint[1] = -1500; inPoint[1] <= 9500; inPoint[1] += 250)
		{
			
			//CvRNG a = 0;
			//inPoint[0] = 2000;//cvRandReal(&a);
			//inPoint[1] = 9500;
			inPoint[2] = 1800;
			camera[0]->worldToImage(inPoint[0], inPoint[1], inPoint[2], xu, yu);
			x = cvRound(xu/2);
			y = cvRound(yu/2);
			//cout<<inPoint[0]<<"      "<<inPoint[1]<<"          "<<x<<"            "<<y<<"\n";
			setPositionEPFL2(img0, x, y, 0);
			cvShowImage("test0", img0);

			camera[1]->worldToImage(inPoint[0], inPoint[1], inPoint[2], xu, yu);
			x = cvRound(xu/2);
			y = cvRound(yu/2);
			//cout<<inPoint[0]<<"      "<<inPoint[1]<<"          "<<x<<"            "<<y<<"\n";
			setPositionEPFL2(img1, x, y, 0);
			cvShowImage("test1", img1);

			camera[2]->worldToImage(inPoint[0], inPoint[1], inPoint[2], xu, yu);
			x = cvRound(xu/2);
			y = cvRound(yu/2);
			//cout<<inPoint[0]<<"      "<<inPoint[1]<<"          "<<x<<"            "<<y<<"\n";
			setPositionEPFL2(img2, x, y, 0);
			cvShowImage("test2", img2);

			camera[3]->worldToImage(inPoint[0], inPoint[1], inPoint[2], xu, yu);
			x = cvRound(xu/2);
			y = cvRound(yu/2);
			//cout<<inPoint[0]<<"      "<<inPoint[1]<<"          "<<x<<"            "<<y<<"\n";
			setPositionEPFL2(img3, x, y, 0);
			cvShowImage("test3", img3);
		}
	
	
	
	while(true)
	{
		if (cvWaitKey(30) == 27) return;


		cvShowImage("test0", img0);
		cvShowImage("test1", img1);
		cvShowImage("test2", img2);
		cvShowImage("test3", img3);
	}
	cvReleaseImage(&img0);
	cvReleaseImage(&img1);
	cvReleaseImage(&img2);
	cvReleaseImage(&img3);
	cvDestroyAllWindows();

}






















//even: for the ground plane
//odd: for the top plane
//EPFLCaliba*ImageView = WorldView;
double EPFLCaliba[NMatrix][3][3]={
	{{ -1.6688907435, -6.9502305710,	940.69592392565},	 {1.1984806153,	-10.7495778320,	868.29873467315},	{0.0004069210,	-0.0209324057,	0.42949125235}},
	{{0.053299,	1.247713,	13.803214},	 {-0.092242,	1.196431,	38.620552},	{ -0.000080,	0.003919,	0.164282}},
	{{0.6174778372,	-0.4836875683,	147.00510919005},	 {0.5798503075,	3.8204849039,	-386.096405131},	{0.0000000001,	0.0077222239,	-0.01593391935}},
	{{0.081202,	0.089186,	14.746465},	 {0.070703,	1.091119,	-8.872128},	{ 0.000001,	0.003361,	0.097146}},
	{{-0.2717592338,	1.0286363982,	-17.6643219215},	 {-0.1373600672,	-0.3326731339,	161.0109069274},	{0.0000600052,	0.0030858398,	-0.04195162855}},
	{{0,	0,	0},	 {0,	0,	-0},	{ 0,	0,	0}},
	{{-0.3286861858,	0.1142963200,	130.25528281945},	 {0.1809954834,	-0.2059386455,	125.0260427323},	{0.0000693641,	0.0040168154,	-0.08284534995}},
	{{0,	0,	0},	 {0,	0,	-0},	{ 0,	0,	0}}
};

//ImageView = inverseEPFLCaliba*WorldView;
double inverseEPFLCaliba[NMatrix][3][3];

void HormoEPFLa(double x, double y, double z, double&u, double&v, int icam)
{
	double	x1 = x* EPFLCaliba[icam][0][0] + y * EPFLCaliba[icam][0][1] + z* EPFLCaliba[icam][0][2];
	double	y1 = x* EPFLCaliba[icam][1][0] + y * EPFLCaliba[icam][1][1] + z* EPFLCaliba[icam][1][2];
	double	z1 = x* EPFLCaliba[icam][2][0] + y * EPFLCaliba[icam][2][1] + z* EPFLCaliba[icam][2][2];
	u=x1/z1;
	v=y1/z1;
}
void inverseHormoEPFLa(double x, double y, double z, double&u, double&v, int icam)
{
	double	x1 = x* inverseEPFLCaliba[icam][0][0] + y * inverseEPFLCaliba[icam][0][1] + z* inverseEPFLCaliba[icam][0][2];
	double	y1 = x* inverseEPFLCaliba[icam][1][0] + y * inverseEPFLCaliba[icam][1][1] + z* inverseEPFLCaliba[icam][1][2];
	double	z1 = x* inverseEPFLCaliba[icam][2][0] + y * inverseEPFLCaliba[icam][2][1] + z* inverseEPFLCaliba[icam][2][2];
	u=x1/z1;
	v=y1/z1;
}
void inverseMatrixa()
{
	double B[NMatrix][3][3];
	for (int k = 0; k < NMatrix; k++)
	{
		double detA = EPFLCaliba[k][0][0]*(EPFLCaliba[k][1][1]*EPFLCaliba[k][2][2] - EPFLCaliba[k][2][1]*EPFLCaliba[k][1][2])
						- EPFLCaliba[k][0][1]*(EPFLCaliba[k][1][0]*EPFLCaliba[k][2][2] - EPFLCaliba[k][2][0]*EPFLCaliba[k][1][2])
						+ EPFLCaliba[k][0][2]*(EPFLCaliba[k][1][0]*EPFLCaliba[k][2][1] - EPFLCaliba[k][2][0]*EPFLCaliba[k][1][1]);
		B[k][0][0] = EPFLCaliba[k][1][1]*EPFLCaliba[k][2][2] - EPFLCaliba[k][2][1]*EPFLCaliba[k][1][2];
		B[k][0][1] = - (EPFLCaliba[k][1][0]*EPFLCaliba[k][2][2] - EPFLCaliba[k][2][0]*EPFLCaliba[k][1][2]);
		B[k][0][2] = EPFLCaliba[k][1][0]*EPFLCaliba[k][2][1] - EPFLCaliba[k][2][0]*EPFLCaliba[k][1][1];
		B[k][1][0] = - (EPFLCaliba[k][0][1]*EPFLCaliba[k][2][2] - EPFLCaliba[k][2][1]*EPFLCaliba[k][0][2]);
		B[k][1][1] = EPFLCaliba[k][0][0]*EPFLCaliba[k][2][2] - EPFLCaliba[k][2][0]*EPFLCaliba[k][0][2];
		B[k][1][2] = - (EPFLCaliba[k][0][0]*EPFLCaliba[k][2][1] - EPFLCaliba[k][2][0]*EPFLCaliba[k][0][1]);
		B[k][2][0] = EPFLCaliba[k][0][1]*EPFLCaliba[k][1][2] - EPFLCaliba[k][1][1]*EPFLCaliba[k][0][2];
		B[k][2][1] = - (EPFLCaliba[k][0][0]*EPFLCaliba[k][1][2] - EPFLCaliba[k][1][0]*EPFLCaliba[k][0][2]);
		B[k][2][2] = EPFLCaliba[k][0][0]*EPFLCaliba[k][1][1] - EPFLCaliba[k][1][0]*EPFLCaliba[k][0][1];
		for (int i = 0; i < 3; i++)
			for (int j = 0; j < 3; j++)
				inverseEPFLCaliba[k][i][j] = B[k][j][i]/detA;
	}
}

//from grid position to world position
void grid_to_tva(int pos, int grid_width, int grid_height, 
                float tv_origin_x, float tv_origin_y, float tv_width,
                float tv_height, double &tv_x, double &tv_y) 
{

  tv_x = ( (pos % grid_width)) * (tv_width / grid_width) + tv_origin_x;
  tv_y = ( (pos / grid_width)) * (tv_height / grid_height) + tv_origin_y;
}

void grid_to_topview(int x, int y,int grid_width, int grid_height, 
                float tv_origin_x, float tv_origin_y, float tv_width,
                float tv_height, double &tv_x, double &tv_y) 
{

	tv_x = (x) * (tv_width / grid_width) + tv_origin_x;
	tv_y = (y) * (tv_height / grid_height) + tv_origin_y;
}

void setPositionEPFLa(IplImage* view2, int X, int Y, int down)
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

Rectangle* findRectanglea(int pos, IplImage* img, int icam)
{
	double tv_x, tv_y;
	grid_to_tva(pos, 60, 88, -500, -1500, 7500, 11000, tv_x, tv_y);
	double X[10], Y[10];


	/*inverseHormoEPFLa(tv_x + 25, tv_y, 1, X[0], Y[0], icam*2);		
	inverseHormoEPFLa(tv_x - 25, tv_y, 1, X[1], Y[1], icam*2);
	inverseHormoEPFLa(tv_x, tv_y + 25, 1, X[2], Y[2], icam*2);
	inverseHormoEPFLa(tv_x, tv_y - 25, 1, X[3], Y[3], icam*2);
	inverseHormoEPFLa(tv_x, tv_y, 1, X[4], Y[4], icam*2);*/

	camera[icam]->worldToImage(tv_x + 250, tv_y, 0, X[0], Y[0]);		
	camera[icam]->worldToImage(tv_x - 250, tv_y, 0, X[1], Y[1]);
	camera[icam]->worldToImage(tv_x, tv_y + 250, 0, X[2], Y[2]);
	camera[icam]->worldToImage(tv_x, tv_y - 250, 0, X[3], Y[3]);
	camera[icam]->worldToImage(tv_x, tv_y, 0, X[4], Y[4]);


	camera[icam]->worldToImage(tv_x + 250, tv_y, 2000, X[5], Y[5]);		
	camera[icam]->worldToImage(tv_x - 250, tv_y, 2000, X[6], Y[6]);
	camera[icam]->worldToImage(tv_x, tv_y + 250, 2000, X[7], Y[7]);
	camera[icam]->worldToImage(tv_x, tv_y - 250, 2000, X[8], Y[8]);
	camera[icam]->worldToImage(tv_x, tv_y, 2000, X[9], Y[9]);

	/*
	if (icam <= 1)
	{
		inverseHormoEPFLa(tv_x + 25, tv_y, 1, X[5], Y[5], icam*2 + 1);		
		inverseHormoEPFLa(tv_x - 25, tv_y, 1, X[6], Y[6], icam*2 + 1);
		inverseHormoEPFLa(tv_x, tv_y + 25, 1, X[7], Y[7], icam*2 + 1);
		inverseHormoEPFLa(tv_x, tv_y - 25, 1, X[8], Y[8], icam*2 + 1);
		inverseHormoEPFLa(tv_x, tv_y, 1, X[9], Y[9], icam*2 + 1);
	}
	else
	{
		for (int i = 0; i < 5; i++)
		{
			X[i + 5] = X[i];
			Y[i + 5] = 0;
		}
	}
	*/
	for (int i = 0; i < 10; i++)
	{
		X[i] = X[i]/2;
		Y[i] = Y[i]/2;
		//cout<<X[i]<<"     "<<Y[i]<<"\n";
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
	if (xmax - xmin < img->width/30 || xmax - xmin > img->width/2) ret->visible = false;
	if (ymax - ymin < img->height/10) ret->visible = false;
	CvPoint* point = new CvPoint[2];
	point[0].x = xmax;
	point[0].y = ymax;
	point[1].x = xmin;
	point[1].y = ymin;
	if (ret->visible == true) cvRectangle(img, point[0], point[1], CV_RGB(255, 255, 255), 1, 8, 0);
	return ret;
}

void setupEPFLa()
{

	initCamera();
	CvCapture* capture0 = cvCaptureFromFile("EPFL data\\terrace1-c0.avi");
	CvCapture* capture1 = cvCaptureFromFile("EPFL data\\terrace1-c1.avi");
	CvCapture* capture2 = cvCaptureFromFile("EPFL data\\terrace1-c2.avi");
	CvCapture* capture3 = cvCaptureFromFile("EPFL data\\terrace1-c3.avi");
	
	cvNamedWindow("test0", 1);
	cvNamedWindow("test1", 1);
	cvNamedWindow("test2", 1);
	cvNamedWindow("test3", 1);
	

	int frameRate = (int)(cvGetCaptureProperty(capture0, CV_CAP_PROP_FPS));
	int frameWidth = (int)(cvGetCaptureProperty(capture0, CV_CAP_PROP_FRAME_WIDTH));
	int frameHeight = (int)(cvGetCaptureProperty(capture0, CV_CAP_PROP_FRAME_HEIGHT));
	int beginTime = 0;
	cout<<"frame rate"<<"      "<<frameRate<<"\n";
	cout<<"frame width"<<"      "<<frameWidth<<"\n";
	cout<<"frame height"<<"      "<<frameHeight<<"\n";
	int countFrame = 0;
	double M, N;
	double X, Y;
	double X1, Y1;
	
	
	IplImage* img0 = cvCreateImage(cvSize(frameWidth, frameHeight), 8, 3);
	IplImage* img1 = cvCreateImage(cvSize(frameWidth, frameHeight), 8, 3);
	IplImage* img2 = cvCreateImage(cvSize(frameWidth, frameHeight), 8, 3);
	IplImage* img3 = cvCreateImage(cvSize(frameWidth, frameHeight), 8, 3);
	inverseMatrixa();
	cvSetCaptureProperty(capture0, CV_CAP_PROP_POS_FRAMES, beginTime*frameRate);
	cvSetCaptureProperty(capture1, CV_CAP_PROP_POS_FRAMES, beginTime*frameRate);
	cvSetCaptureProperty(capture2, CV_CAP_PROP_POS_FRAMES, beginTime*frameRate);
	cvSetCaptureProperty(capture3, CV_CAP_PROP_POS_FRAMES, beginTime*frameRate);
	int pos = 0; 
	
	
	
	int gridSize = 30*44;

	/*
	ofstream myfile;
	myfile.open ("EPFL terrace setup.txt");
	for (int icam = 0; icam < 4; icam++)
	{
		for (pos = 0; pos < gridSize; pos++)
		{
			Rectangle* a = findRectanglea(pos, img0, icam);
			if (a->visible) myfile<<"RECTANGLE "<<icam<<" "<<pos<<" "<<a->xmin<<" "<<a->ymin<<" "<<a->xmax<<" "<<a->ymax<<"\n";
			else myfile<<"RECTANGLE "<<icam<<" "<<pos<<" notvisible"<<"\n";
		}
	}
	myfile.close();
	*/
	

	while (true)
	{
		countFrame++;
		if (cvWaitKey(30) == 27) return;
		cvGrabFrame(capture0);
		img0 = cvRetrieveFrame(capture0);

		cvGrabFrame(capture1);
		img1 = cvRetrieveFrame(capture1);

		cvGrabFrame(capture2);
		img2 = cvRetrieveFrame(capture2);

		cvGrabFrame(capture3);
		img3 = cvRetrieveFrame(capture3);
		int x,y;
		//pos = 2138;
		int icam = 0;
		for (int j = 0; j < 3; j++)   //x
			for (int k = 10; k < 24; k++)  //y
		//for (pos = 0; pos < 450; pos++)
		{

			pos = k * 60 + j;
			//pos = 8*30 + 0;
			grid_to_tva(pos, 60, 88, -500, -1500, 7500, 11000, M, N);   //doi tu vi tri grid sang vi tri topview
			//findRectanglea(pos, img, icam);  //icam : 0, 1, 2, 3
			//inverseHormoEPFLa(M, N, 22, X, Y, 0*2);
			camera[0]->worldToImage(M, N, 1800, X, Y);
			setPositionEPFLa(img0, cvRound(X/2), cvRound(Y/2), 0);

			//inverseHormoEPFLa(M, N, 22, X, Y, 1*2);
			camera[1]->worldToImage(M, N, 1800, X, Y);
			setPositionEPFLa(img1, cvRound(X/2), cvRound(Y/2), 0);
			findRectangle_z(j,k, img1, 1, 2000);

			//inverseHormoEPFLa(M, N, 22, X, Y, 2*2);
			camera[2]->worldToImage(M, N, 2000, X, Y);
			setPositionEPFLa(img2, cvRound(X/2), cvRound(Y/2), 0);
			camera[2]->worldToImage(M, N, 0, X, Y);
			setPositionEPFLa(img2, cvRound(X/2), cvRound(Y/2),1);
			//findRectanglea(pos, img2, 2);
			findRectangle_z(j,k, img2, 2, 2000);

			//inverseHormoEPFLa(M, N, 22, X, Y, 3*2);
			camera[3]->worldToImage(M, N, 1800, X, Y);
			setPositionEPFLa(img3, cvRound(X/2), cvRound(Y/2), 0);

			/*
			grid_to_tva(340, 30, 44, -500, -1500, 7500, 11000, M, N);   //doi tu vi tri grid sang vi tri topview
			//findRectanglea(pos, img, icam);  //icam : 0, 1, 2, 3
			inverseHormoEPFLa(M, N, 20, X, Y, icam*2);
			setPositionEPFLa(img, cvRound(X), cvRound(Y), 1);

			grid_to_tva(370, 30, 44, -500, -1500, 7500, 11000, M, N);   //doi tu vi tri grid sang vi tri topview
			//findRectanglea(pos, img, icam);  //icam : 0, 1, 2, 3
			inverseHormoEPFLa(M, N, 20, X, Y, icam*2);
			setPositionEPFLa(img, cvRound(X), cvRound(Y), 1);

			grid_to_tva(400, 30, 44, -500, -1500, 7500, 11000, M, N);   //doi tu vi tri grid sang vi tri topview
			//findRectanglea(pos, img, icam);  //icam : 0, 1, 2, 3
			inverseHormoEPFLa(M, N, 20, X, Y, icam*2);
			setPositionEPFLa(img, cvRound(X), cvRound(Y), 1);

			grid_to_tva(311, 30, 44, -500, -1500, 7500, 11000, M, N);   //doi tu vi tri grid sang vi tri topview
			//findRectanglea(pos, img, icam);  //icam : 0, 1, 2, 3
			inverseHormoEPFLa(M, N, 20, X, Y, icam*2);
			setPositionEPFLa(img, cvRound(X), cvRound(Y), 0);

			grid_to_tva(312, 30, 44, -500, -1500, 7500, 11000, M, N);   //doi tu vi tri grid sang vi tri topview
			//findRectanglea(pos, img, icam);  //icam : 0, 1, 2, 3
			inverseHormoEPFLa(M, N, 20, X, Y, icam*2);
			setPositionEPFLa(img, cvRound(X), cvRound(Y), 0);

			grid_to_tva(313, 30, 44, -500, -1500, 7500, 11000, M, N);   //doi tu vi tri grid sang vi tri topview
			//findRectanglea(pos, img, icam);  //icam : 0, 1, 2, 3
			inverseHormoEPFLa(M, N, 20, X, Y, icam*2);
			setPositionEPFLa(img, cvRound(X), cvRound(Y), 0);
			*/
		}

		/*
		//grid_to_tv(pos, 56, 56, 0, 0, 358, 360, M, N);
		inverseHormoEPFLa(M + 250, N, 1, X1, Y1, icam*2);
		
		setPositionEPFLa(img, cvRound(X1), cvRound(Y1), 0);

		inverseHormoEPFLa(M - 250, N, 1, X1, Y1, icam*2);
		setPositionEPFLa(img, cvRound(X1), cvRound(Y1), 0);

		inverseHormoEPFLa(M, N + 250, 1, X1, Y1, icam*2);
		setPositionEPFLa(img, cvRound(X1), cvRound(Y1), 1);

		inverseHormoEPFLa(M, N - 250, 1, X1, Y1, icam*2);
		setPositionEPFLa(img, cvRound(X1), cvRound(Y1), 1);
		*/
		cvShowImage("test0", img0);
		cvShowImage("test1", img1);
		cvShowImage("test2", img2);
		cvShowImage("test3", img3);
		
		if (countFrame > 4000)  break;
		
	}
	//cvReleaseImage(&img);
	cvReleaseCapture(&capture0);
	cvReleaseCapture(&capture1);
	cvReleaseCapture(&capture2);
	cvReleaseCapture(&capture3);
	cvDestroyAllWindows();
	
}


Rectangle* findRectangle_z(int x, int y, IplImage* img, int icam, int height)
{
	double tv_x, tv_y;
	grid_to_topview(x, y, 60, 88, -500, -1500, 7500, 11000, tv_x, tv_y);
	double X[10], Y[10];


	/*inverseHormoEPFLa(tv_x + 25, tv_y, 1, X[0], Y[0], icam*2);		
	inverseHormoEPFLa(tv_x - 25, tv_y, 1, X[1], Y[1], icam*2);
	inverseHormoEPFLa(tv_x, tv_y + 25, 1, X[2], Y[2], icam*2);
	inverseHormoEPFLa(tv_x, tv_y - 25, 1, X[3], Y[3], icam*2);
	inverseHormoEPFLa(tv_x, tv_y, 1, X[4], Y[4], icam*2);*/

	camera[icam]->worldToImage(tv_x + 250, tv_y, 0, X[0], Y[0]);		
	camera[icam]->worldToImage(tv_x - 250, tv_y, 0, X[1], Y[1]);
	camera[icam]->worldToImage(tv_x, tv_y + 250, 0, X[2], Y[2]);
	camera[icam]->worldToImage(tv_x, tv_y - 250, 0, X[3], Y[3]);
	camera[icam]->worldToImage(tv_x, tv_y, 0, X[4], Y[4]);


	camera[icam]->worldToImage(tv_x + 250, tv_y, height, X[5], Y[5]);		
	camera[icam]->worldToImage(tv_x - 250, tv_y, height, X[6], Y[6]);
	camera[icam]->worldToImage(tv_x, tv_y + 250, height, X[7], Y[7]);
	camera[icam]->worldToImage(tv_x, tv_y - 250, height, X[8], Y[8]);
	camera[icam]->worldToImage(tv_x, tv_y, height, X[9], Y[9]);

	/*
	if (icam <= 1)
	{
		inverseHormoEPFLa(tv_x + 25, tv_y, 1, X[5], Y[5], icam*2 + 1);		
		inverseHormoEPFLa(tv_x - 25, tv_y, 1, X[6], Y[6], icam*2 + 1);
		inverseHormoEPFLa(tv_x, tv_y + 25, 1, X[7], Y[7], icam*2 + 1);
		inverseHormoEPFLa(tv_x, tv_y - 25, 1, X[8], Y[8], icam*2 + 1);
		inverseHormoEPFLa(tv_x, tv_y, 1, X[9], Y[9], icam*2 + 1);
	}
	else
	{
		for (int i = 0; i < 5; i++)
		{
			X[i + 5] = X[i];
			Y[i + 5] = 0;
		}
	}
	*/
	for (int i = 0; i < 10; i++)
	{
		X[i] = X[i]/2;
		Y[i] = Y[i]/2;
		//cout<<X[i]<<"     "<<Y[i]<<"\n";
	}
	double xmin, ymin, xmax, ymax;
	//xmin = img->width - 1;
	//ymin = img->height - 1;
	//xmax = 0;
	//ymax = 0;
	xmin = X[0];
	ymin = Y[0];
	xmax = X[0];
	ymax = Y[0];
	for (int k = 0; k < 10; k++)
	{
		//if (X[k] >= 0 && X[k] < img->width - 1 && Y[k] >=0 && Y[k] < img->height - 1)
		//{
			if (X[k] < xmin) xmin = X[k];
			if (Y[k] < ymin) ymin = Y[k];
			if (X[k] > xmax) xmax = X[k];
			if (Y[k] > ymax) ymax = Y[k];
		//}
	}
	int realWidth, realHeight;
	realWidth = xmax - xmin;
	realHeight = ymax - ymin;
	if (xmin < 0) xmin = 0;
	if (xmax > img->width - 1) xmax = img->width - 1;
	if (ymin < 0) ymin = 0;
	if (ymax > img->height - 1) ymax = img->height - 1;
	Rectangle* ret = new Rectangle();
	ret->visible = true;
	ret->xmax = cvRound(xmax);
	ret->xmin = cvRound(xmin);
	ret->ymax = cvRound(ymax);
	ret->ymin = cvRound(ymin);
	if (xmax - xmin < img->width/30 || xmax - xmin > img->width/2) ret->visible = false;
	if (ymax - ymin < img->height/10) ret->visible = false;
	CvPoint* point = new CvPoint[2];
	point[0].x = xmax;
	point[0].y = ymax;
	point[1].x = xmin;
	point[1].y = ymin;
	ret->prob = (xmax - xmin)*(ymax - ymin)/(realWidth*realHeight);
	//if (ret->visible == true) cvRectangle(img, point[0], point[1], CV_RGB(255, 255, 255), 1, 8, 0);
	return ret;
}

void markPosition(int posx, int posy, IplImage* img, int icam)
{
	double tv_x, tv_y;
	grid_to_topview(posx, posy, 60, 88, -500, -1500, 7500, 11000, tv_x, tv_y);
	double x,y;
	camera[icam]->worldToImage(tv_x, tv_y, 0, x, y);
	x = x/2;
	y = y/2;
	setPositionEPFLa(img, x, y, 0);
}