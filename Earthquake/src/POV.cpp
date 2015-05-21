#include "POV.h"
#include "cinder/gl/gl.h"
#include "cinder/CinderMath.h"

POV::POV( ci::app::App *aApp, ci::Vec3f aEye, ci::Vec3f aCenter ){
	mApp = aApp;
	
	mAspectRatio	= mApp->getWindowAspectRatio();
	mFOV			= 60.0f;
	
	mEye			= aEye;
	mCenter			= aCenter;
	
	mPhi			= 0.0f;
	mPhiDest		= 0.0f;
    mTheta			= 0.0f;
	mThetaDest		= 0.0f;
	mDist			= 500.0f;
	mDistDest		= 500.0f;
}

void POV::update()
{
	//mPhiDest += 0.01f;
	//mEye.y = sin( mPhiDest * 0.1f + 1.5707963f ) * 250.0f;
	
	mPhi -= ( mPhi - mPhiDest ) * 0.1f;
	mTheta -= ( mTheta - mThetaDest ) * 0.1f;
	mDist -= ( mDist - mDistDest ) * 0.1f;
	
	mEye.x = cos( mTheta ) * sin( mPhi ) * mDist;
	mEye.z = cos( mTheta ) * cos( mPhi ) * mDist;
	mEye.y = sin( mTheta ) * mDist;
    
	mEyeNormal = mEye - mCenter;
	mEyeNormal.normalize();
	
	mCam.lookAt( mEye, mCenter );
	mCam.setPerspective( mFOV, mApp->getWindowAspectRatio(), 1, 20000 );
	ci::gl::setMatrices( mCam );
}

void POV::adjustAngle( float phiDelta, float pitch )
{
	mPhiDest += phiDelta;
	mEye.y = pitch;
}

void POV::adjustAngles( float phiDelta, float thetaDelta )
{
	mPhiDest += phiDelta;
	mThetaDest += thetaDelta;
}

void POV::adjustDist( float distDelta )
{
	mDistDest += distDelta;
}

void POV::incDist( float distFactor )
{
    //std::cout<<"    +++ "<<mDistDest<<" : "<<distFactor<<std::endl;
	mDistDest += mDistDest * distFactor;
}

void POV::setEye( ci::Vec3f v )
{
	mEye = v;
}

void POV::setCenter( ci::Vec3f v )
{
	mCenter = v;
}