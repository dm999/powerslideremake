#ifndef CAMERAMAN_H
#define CAMERAMAN_H

#include "includes/OgreInclude.h"
#include "includes/OISInclude.h"
#include "SdkCameraMan.h"

class BaseApp;

namespace OgreBulletDynamics
{
    class RigidBody;
    class DynamicsWorld;
}

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
    CameraMan(Ogre::Camera* cam, OgreBulletDynamics::DynamicsWorld * world, Ogre::SceneManager* sceneMgr);

    virtual ~CameraMan() {}

    virtual void setYawPitchDist(const Ogre::Quaternion& carRot, const Ogre::Vector3& carPos, Ogre::Real lateralVelocity, Ogre::SceneNode * const target);

    void setRearCamera(Ogre::Camera* cam){mRearCamera = cam;}

    Ogre::Camera* getCamera(){return mCamera;}

    void setCameraPositionType(const CameraPositions& type){mCamPositonType = type;}
    CameraPositions getCameraPositionType()const{return mCamPositonType;}

private :
    OgreBulletDynamics::DynamicsWorld * mWorld;

    bool checkRayInBetween(const Ogre::Vector3& From, const Ogre::Vector3& To, Ogre::Vector3& collisionPoint, Ogre::Vector3& collisionNormal)const;

    Ogre::Camera* mCamera;
    Ogre::Camera* mRearCamera;

    CameraPositions mCamPositonType;
};

#endif