#include "z_ProjectionModel.h"


ProjectionModel::ProjectionModel()
{
	initTerraceCamera();
}

ProjectionModel::~ProjectionModel()
{
}

void ProjectionModel::initTerraceCamera()
{
	for (int i = 0; i < ter_nCam; i++)
	{
		terCamera[i] = new CameraModel();
		terCamera[i]->setGeometry(ter_imgWidth, ter_imgHeight, ter_ncx, ter_nfx, ter_dx, ter_dy, ter_dpx, ter_dpy);
		terCamera[i]->setIntrinsic(ter_focal[i], ter_kappa1[i], ter_cx[i], ter_cy[i], ter_sx[i]);
		terCamera[i]->setExtrinsic(ter_tx[i], ter_ty[i], ter_tz[i], ter_rx[i], ter_ry[i], ter_rz[i]);
		terCamera[i]->internalInit();
	}
}

CvPoint2D64f ProjectionModel::terGridToTv(CvPoint gridPos)
{
	CvPoint2D64f tvPos;
	tvPos.x = (gridPos.x) * (ter_topViewWidth / ter_gridWidth) + ter_topViewOriginX;
	tvPos.y = (gridPos.y) * (ter_topViewHeight / ter_gridHeight) + ter_topViewOriginY;
	return tvPos;
}

CvPoint2D64f ProjectionModel::terWorldToImage(CvPoint3D64f worldPos, int iCam)
{
	CvPoint2D64f imgPos = cvPoint2D64f(0, 0);
	terCamera[iCam]->worldToImage(worldPos.x, worldPos.y, worldPos.z, imgPos.x, imgPos.y);
	return imgPos;
}

CvPoint3D64f ProjectionModel::terWorldToCamera(CvPoint3D64f worldPos, int iCam)
{
	CvPoint3D64f camPos = cvPoint3D64f(0, 0, 0);
	terCamera[iCam]->worldToCameraCoord(worldPos.x, worldPos.y, worldPos.z, camPos.x, camPos.y, camPos.z);
	return camPos;
}

CvPoint3D64f ProjectionModel::terCameraToWorld(CvPoint3D64f camPos, int iCam)
{
	CvPoint3D64f worldPos = cvPoint3D64f(0, 0, 0);
	terCamera[iCam]->cameraToWorldCoord(camPos.x, camPos.y, camPos.z, worldPos.x, worldPos.y, worldPos.z);
	return worldPos;
}


bool ProjectionModel::simpleModelProjection(CvPoint position, int height, int width, int iCam, CvRect& originalRect, CvRect &rect)
{
	int minWidth, maxWidth;
	if (iCam == 2 || iCam == 3)
	{
		minWidth = 3;
		maxWidth = ter_RealWidth - 3;
	}
	else
	{
		minWidth = 0;
		maxWidth = ter_RealWidth;
	}
	rect = cvRect(0, 0, 0, 0);
	CvPoint2D64f tvPos = terGridToTv(position);
	double distance = width/2;
	
	CvPoint2D64f imgPos[10];
	imgPos[0] = terWorldToImage(cvPoint3D64f(tvPos.x, tvPos.y, 0), iCam);
	imgPos[1] = terWorldToImage(cvPoint3D64f(tvPos.x - distance, tvPos.y, 0), iCam);
	imgPos[2] = terWorldToImage(cvPoint3D64f(tvPos.x + distance, tvPos.y, 0), iCam);
	imgPos[3] = terWorldToImage(cvPoint3D64f(tvPos.x, tvPos.y - distance, 0), iCam);
	imgPos[4] = terWorldToImage(cvPoint3D64f(tvPos.x, tvPos.y + distance, 0), iCam);
	imgPos[5] = terWorldToImage(cvPoint3D64f(tvPos.x, tvPos.y, height), iCam);
	imgPos[6] = terWorldToImage(cvPoint3D64f(tvPos.x - distance, tvPos.y, height), iCam);
	imgPos[7] = terWorldToImage(cvPoint3D64f(tvPos.x + distance, tvPos.y, height), iCam);
	imgPos[8] = terWorldToImage(cvPoint3D64f(tvPos.x, tvPos.y - distance, height), iCam);
	imgPos[9] = terWorldToImage(cvPoint3D64f(tvPos.x, tvPos.y + distance, height), iCam);
	for (int i = 0; i < 10; i++)
	{
		imgPos[i].x = imgPos[i].x/2;
		imgPos[i].y = imgPos[i].y/2;
	}

	double xmin, ymin, xmax, ymax;
	xmax = imgPos[0].x;
	ymax = imgPos[0].y;
	xmin = imgPos[0].x;
	ymin = imgPos[0].y;
	
	for (int k = 0; k < 10; k++)
	{
		if (imgPos[k].x < xmin) xmin = imgPos[k].x;
		if (imgPos[k].y < ymin) ymin = imgPos[k].y;
		if (imgPos[k].x > xmax) xmax = imgPos[k].x;
		if (imgPos[k].y > ymax) ymax = imgPos[k].y;
	}
	originalRect = cvRect(xmin, ymin, xmax - xmin, ymax - ymin);
	int checkWidth = xmax - xmin;
	int checkHeight = ymax - ymin;
	if (xmin < minWidth) xmin = minWidth;
	if (xmax >= maxWidth) xmax = maxWidth - 1;
	if (ymin < 0) ymin = 0;
	if (ymax >= ter_RealHeight) ymax = ter_RealHeight - 1;
	rect = cvRect(xmin, ymin, xmax - xmin, ymax - ymin);
	//if (rect.width <= checkWidth/3 || rect.height <= checkHeight/3) return false;
	if (rect.width <= checkWidth/5 || rect.height <= checkHeight/5) return false;
	return true;
}

int ProjectionModel::checkingOcclusion(CvPoint obj1, CvPoint obj2, int iCam)
{
	CvPoint2D64f tvPos1 = terGridToTv(obj1);
	CvPoint2D64f tvPos2 = terGridToTv(obj2);
	CvPoint3D64f worldPos1 = cvPoint3D64f(tvPos1.x, tvPos1.y, 0);
	CvPoint3D64f worldPos2 = cvPoint3D64f(tvPos2.x, tvPos2.y, 0);


	CvPoint3D64f cameraPos1 = terWorldToCamera(worldPos1, iCam);
	CvPoint3D64f cameraPos2 = terWorldToCamera(worldPos2, iCam);

	double distance1 = std::sqrtf(cameraPos1.x*cameraPos1.x + cameraPos1.y*cameraPos1.y + cameraPos1.z*cameraPos1.z);
	double distance2 = std::sqrtf(cameraPos2.x*cameraPos2.x + cameraPos2.y*cameraPos2.y + cameraPos2.z*cameraPos2.z);

	if (distance1 > distance2) return 2;
	else return 1;
}