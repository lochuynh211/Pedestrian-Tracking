/***************************************************************************
 *   cameraModel.h     - description
 *
 *   This program is part of the ETISEO project.
 *
 *   See http://www.etiseo.net  http://www.silogic.fr    
 *
 *   (C) Silogic - ETISEO Consortium
 ***************************************************************************/
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include <fstream>
#include <opencv2\highgui\highgui.hpp>

using namespace std;
using namespace cv;


#ifndef _CAMERA_MODEL_OBJECT_H_
#define _CAMERA_MODEL_OBJECT_H_


	
 //!  A root class handling camera model
 class CameraModel
 {
    public:
      
	  //! Constructor
      CameraModel();
      //! Destructor
      virtual ~CameraModel();
	  
	  void setGeometry(int width, int height, double ncx, double nfx, 
	  					double dx, double dy, double dpx, double dpy);
		
	  void setIntrinsic(double focal, double kappa1, double cx, double cy, double sx);
	  
	  void setExtrinsic(double tx, double ty, double tz, double rx, double ry, double rz);
	   
 
	  //! Coordinate manipulation
	  //! from image coordinate to world coordinate
	  bool imageToWorld(double Xi, double Yi, double Zw, double& Xw, double &Yw);
	  
	  //! from world coordinate to image coordinate
	  bool worldToImage(double Xw, double Yw, double Zw, double& Xi, double& Yi);
	  
	  //! convert from undistorted to distorted image
	  bool undistortedToDistortedImageCoord (double Xfu, double Yfu, double& Xfd, double& Yfd);
	  //! convert from distorted to undistorted image
	  bool distortedToUndistortedImageCoord (double Xfd, double Yfd, double& Xfu, double& Yfu);
	  
	  //! from world coordinate to camera coordinate
	  bool worldToCameraCoord (double xw, double yw, double zw, double& xc, double& yc, double& zc);
	  //! from camera coordinate to world coordinate
	  bool cameraToWorldCoord (double xc, double yc, double zc, double& xw, double& yw, double& zw);

	  virtual void internalInit();

	protected:
	
		
		
		//! Coordinate manipulation, intermediate transformation :
		//! convert from distorted to undistorted sensor plane coordinates 
		void distortedToUndistortedSensorCoord (double Xd, double Yd, double& Xu, double& Yu);
		//! convert from undistorted to distorted sensor plane coordinates
		void undistortedToDistortedSensorCoord (double Xu, double Yu, double& Xd, double& Yd);
		
	private:
		
		
		// geometry
		int				mImgWidth;
		int				mImgHeight;
		double			mNcx;
		double			mNfx;
		double			mDx;
		double			mDy;
		double			mDpx;
		double			mDpy;

		// intrinsic 
		double			mFocal;
		double			mKappa1;
		double			mCx;
		double			mCy;
		double			mSx;
 
		// extrinsic 
		double			mTx;
		double			mTy;
		double			mTz;
		double			mRx;
		double			mRy;
		double			mRz;
		
		// for computation
		double			mR11;
		double			mR12;
		double			mR13;
		double			mR21;
		double			mR22;
		double			mR23;
		double			mR31;
		double			mR32;
		double			mR33;
		
		//camera position
		double			mCposx;
		double			mCposy;
		double			mCposz;
		
 };

#endif
