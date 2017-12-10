#ifndef INITIALVEHICLESETUP_H
#define INITIALVEHICLESETUP_H

#include "../tools/PSSpline.h"
#include "TerrainData.h"

struct InitialVehicleSetup
{
    Ogre::Vector3 mCOG;

    Ogre::Matrix4 mTrackPosition;
    Ogre::Vector3 mInitialImpulseLinear;
    Ogre::Vector3 mInitialImpulseLinearInc;
    Ogre::Vector3 mInitialImpulseRot;
    Ogre::Vector3 mInitialImpulseRotInc;

    Ogre::Vector3 mChassisPos;
    Ogre::Quaternion mChassisRot;

    static const int mWheelsAmount = 4;
    Ogre::Real mWheelRadius[mWheelsAmount];//RR, RL, FR, FL
    Ogre::Vector3 mConnectionPointWheel[mWheelsAmount];//RR, RL, FR, FL
    Ogre::Vector3 mSuspensionDataWheel[mWheelsAmount];//RR, RL, FR, FL; x - susp height, y - steering, z - velocity

    static const int mRoofsAmount = 4;
    float mRoofRadius[mRoofsAmount];//RR, RL, FR, FL
    Ogre::Vector3 mRoofPos[mRoofsAmount];//RR, RL, FR, FL

    Ogre::Vector3 mCoreBase;

    float mBodyRadius;
    Ogre::Vector3 mBodyBasePos;

    Ogre::Real mAirDensityTranslation;
    Ogre::Real mAirDensityRot;

    Ogre::Real mChassisMass;
    Ogre::Real mChassisInvMass;

    Ogre::Vector3 mMomentOfInertia;

    Ogre::Real mGravityForce;

    PSSpline mWheelUnderGroundDD;
    PSSpline mWheelUnderGroundVV;
    PSSpline mWheelUnderGroundDDV;
    PSSpline mWheelUnderGroundVDV;

    static const int mVelocitySplinesAmoint = TerrainData::mTerrainsAmount;
    PSSpline mVelocitySpline[mVelocitySplinesAmoint];

    static const int mGearCount = 4;
    Ogre::Real mEngineIdleRevsStart;
    Ogre::Real mEngineIdleRevsEnd;
    Ogre::Real mGearRatioMain;
    Ogre::Real mGearRatio[mGearCount];
    Ogre::Real mChangeUp[mGearCount];
    Ogre::Real mChangeDown[mGearCount];

    Ogre::Real mMaxTravel;
    Ogre::Real mFrontSuspension;
    Ogre::Real mRisingDamp;

    Ogre::Real mFrontBreak;
    Ogre::Real mBackBreak;

    Ogre::Real mTransmissionDrag;

    Ogre::Real mDiffSlip;

    Ogre::Real mFrontWheelDrive;

    Ogre::Real mAccelerationFactor;

    PSSpline mPower;
};


#endif