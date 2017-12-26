#ifndef CAMERAMAN_H
#define CAMERAMAN_H

#include "includes/OgreInclude.h"

#include "tools/LinearController.h"

#include "Enums.h"

struct InitialVehicleSetup;

/*all calculations in left hand - like original data*/
class CameraMan
{
public :
    CameraMan(Ogre::Camera* cam);

    virtual ~CameraMan() {}

    void setYawPitchDist(const InitialVehicleSetup& initialVehicleSetup, const Ogre::Quaternion& carRot, const Ogre::Vector3& carPos, Ogre::Real lateralVelocity, Ogre::Real alignedVelocity);

    void setRearCamera(Ogre::Camera* cam){mRearCamera = cam;}

    Ogre::Camera* getCamera(){return mCamera;}

    void setCameraPositionType(const CameraPositions& type);
    CameraPositions getCameraPositionType()const{return mCamPositonType;}

private :

    void recalcCamParams(const InitialVehicleSetup& initialVehicleSetup);

    Ogre::Vector3 mCameraOffset;
    Ogre::Vector2 mCamParam2D;
    Ogre::Vector3 mCamParam3D;
    Ogre::Real mCamParam;
    Ogre::Real mCamAngle;

    bool mCamTypeSwitched;

    Ogre::Camera* mCamera;
    Ogre::Camera* mRearCamera;

    CameraPositions mCamPositonType;

    LinearController<float> mSpeedToFOV;
};

#endif