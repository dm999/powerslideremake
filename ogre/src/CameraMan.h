#ifndef CAMERAMAN_H
#define CAMERAMAN_H

#include "includes/OgreInclude.h"
#include "includes/OISInclude.h"
#include "SdkCameraMan.h"

#include "tools/LinearController.h"

class BaseApp;
class Physics;

class btPairCachingGhostObject;
class btOverlappingPairCallback;

enum CameraPositions
{
    CameraPosition_Bumper,
    CameraPosition_ChassisA,
    CameraPosition_ChassisB,
    CameraPosition_ChassisC
};

class CameraMan
{
public :
    CameraMan(Ogre::Camera* cam, Physics * world, Ogre::SceneManager* sceneMgr);

    virtual ~CameraMan() {}

    virtual void setYawPitchDist(const Ogre::Quaternion& carRot, const Ogre::Vector3& carPos, Ogre::Real lateralVelocity, Ogre::Real alignedVelocity);

    void setRearCamera(Ogre::Camera* cam){mRearCamera = cam;}

    Ogre::Camera* getCamera(){return mCamera;}

    void setCameraPositionType(const CameraPositions& type){mCamPositonType = type;}
    CameraPositions getCameraPositionType()const{return mCamPositonType;}

private :
    Physics * mWorld;

    bool checkRayInBetween(const Ogre::Vector3& From, const Ogre::Vector3& To, Ogre::Vector3& collisionPoint, Ogre::Vector3& collisionNormal)const;

    Ogre::Camera* mCamera;
    Ogre::Camera* mRearCamera;

    CameraPositions mCamPositonType;

    LinearController<float> mSpeedToFOV;
};

#endif