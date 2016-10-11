#ifndef Z_PROJECTIONMODEL_H
#define Z_PROJECTIONMODEL_H


#include <iostream>
#include "calibration\z_cameraModel.h"
#include <opencv2\highgui\highgui.hpp>

using namespace std;
using namespace cv;


//terrace
#define ter_nCam 4
const int ter_imgWidth = 720;
const int ter_imgHeight = 576;
const int ter_RealWidth = 360;
const int ter_RealHeight = 288;
const double ter_nfx = 576;
const double ter_ncx = 576;
const double ter_dpx = 2.3000000000e-02;
const double ter_dpy = 2.3000000000e-02;
const double ter_dx = 2.3000000000e-02;
const double ter_dy = 2.3000000000e-02;
const double ter_focal[ter_nCam] = {20.161920, 19.529144, 19.903218, 20.047015};
const double ter_kappa1[ter_nCam] = {5.720865e-04, 5.184242e-04, 3.511557e-04, 4.347668e-04};
const double ter_cx[ter_nCam] = {366.514507, 360.228130, 355.506436, 349.154019};
const double ter_cy[ter_nCam] = {305.832552, 255.166919, 241.205640, 245.786168};
const double ter_sx[ter_nCam] = {1, 1, 1, 1};   
const double ter_rx[ter_nCam] = {1.9007833770e+00, 1.9347282363e+00, -1.8289537286e+00, -1.8418460467e+00};
const double ter_ry[ter_nCam] = {4.9730769727e-01, -7.0418616982e-01, 3.7748154985e-01, -4.6728290805e-01};
const double ter_rz[ter_nCam] = {1.8415452559e-01, -2.3783238362e-01, 3.0218614321e+00, -3.0205552749e+00};
const double ter_tx[ter_nCam] = {-4.8441913843e+03, -65.433635, 1.9782813424e+03, 4.6737509054e+03};
const double ter_ty[ter_nCam] = {5.5109448682e+02, 1594.811988, -9.4027627332e+02, -2.5743341287e+01};
const double ter_tz[ter_nCam] = {4.9667438357e+03, 2113.640844, 1.2397750058e+04, 8.4155952460e+03};
const int ter_gridWidth = 60;
const int ter_gridHeight = 88;
const double ter_topViewOriginX = -500;
const double ter_topViewOriginY = -1500;
const double ter_topViewWidth = 7500;
const double ter_topViewHeight = 11000;

class ProjectionModel
{
	private:

		//terrace variable
		CameraModel* terCamera[ter_nCam];
		//terrace function
		void initTerraceCamera();
		CvPoint2D64f terGridToTv(CvPoint gridPos);
		
		

	public:
		ProjectionModel();
		~ProjectionModel();

		//projection function
		CvPoint2D64f terWorldToImage(CvPoint3D64f worldPos, int iCam);
		CvPoint3D64f terWorldToCamera(CvPoint3D64f worldPos, int iCam);
		CvPoint3D64f terCameraToWorld(CvPoint3D64f worldPos, int iCam);

		//terrace funciton
		bool simpleModelProjection(CvPoint position, int height, int width, int iCam, CvRect& originalRect, CvRect &rect);
		int checkingOcclusion(CvPoint obj1, CvPoint obj2, int iCam);
};

#endif