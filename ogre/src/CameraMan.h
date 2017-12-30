#ifndef CAMERAMAN_H
#define CAMERAMAN_H

#include "includes/OgreInclude.h"

#include "Enums.h"

struct InitialVehicleSetup;

class StaticMeshProcesser;

/*all calculations in left hand - like original data*/
class CameraMan
{
public :
    CameraMan(Ogre::Camera* cam, StaticMeshProcesser& staticMeshProcesser);

    virtual ~CameraMan() {}

    void setYawPitchDist(const InitialVehicleSetup& initialVehicleSetup, const Ogre::Quaternion& carRot,
        Ogre::Light * globalLight, Ogre::Light * shadowLight);

    void setRearCamera(Ogre::Camera* cam){mRearCamera = cam;}

    Ogre::Camera* getCamera(){return mCamera;}

    void setCameraPositionType(const CameraPositions& type);
    CameraPositions getCameraPositionType()const{return mCamPositonType;}

private :

    void recalcCamParams(const InitialVehicleSetup& initialVehicleSetup);

    bool compareCamParams(Ogre::Vector2 paramA, Ogre::Vector2 paramB, Ogre::Vector2 paramC, Ogre::Vector2 paramD, Ogre::Vector2& res) const;

    Ogre::Vector3 mCameraOffset;
    Ogre::Vector2 mCamParam2D;
    Ogre::Vector3 mCamParam3D;
    Ogre::Real mCamParam;
    Ogre::Real mCamAngle;

    Ogre::Real mCamRotZ_y;
    Ogre::Vector3 mCamRotZ;
    Ogre::Vector3 mCamShift;

    Ogre::Vector3 mCamRot[3];

    bool mCamTypeSwitched;

    Ogre::Camera* mCamera;
    Ogre::Camera* mRearCamera;

    StaticMeshProcesser& mStaticMeshProcesser;

    CameraPositions mCamPositonType;
};

#endif