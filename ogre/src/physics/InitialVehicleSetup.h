#ifndef INITIALVEHICLESETUP_H
#define INITIALVEHICLESETUP_H

#include "../tools/PSSpline.h"
#include "TerrainData.h"

/*most data assumed to be in right hand - like Ogre*/
struct InitialVehicleSetup
{
    Ogre::Matrix4 mTrackPosition;
    Ogre::Vector3 mInitialImpulseLinear;//left hand
    Ogre::Vector3 mInitialImpulseLinearInc;//left hand
    Ogre::Vector3 mInitialImpulseRot;//left hand
    Ogre::Vector3 mInitialImpulseRotInc;//left hand

    Ogre::Vector3 mCarGlobalPos;
    Ogre::Quaternion mCarRot;

    static const int mWheelsAmount = 4;
    Ogre::Real mWheelRadius[mWheelsAmount];//RR, RL, FR, FL
    Ogre::Vector3 mConnectionPointWheel[mWheelsAmount];//RR, RL, FR, FL
    Ogre::Vector3 mSuspensionDataWheel[mWheelsAmount];//RR, RL, FR, FL; x - susp height, y - steering, z - velocity

    static const int mRoofsAmount = 4;
    float mRoofRadius[mRoofsAmount];//RR, RL, FR, FL
    Ogre::Vector3 mRoofPos[mRoofsAmount];//RR, RL, FR, FL //left hand

    Ogre::Real mMissileAngle;

    Ogre::Real mCollisionRadius;

    Ogre::Vector3 mCOG;
    Ogre::Vector3 mCoreBase;
    Ogre::Vector3 mCockpit;

    float mBodyRadius;
    Ogre::Vector3 mBodyBasePos;//left hand

    Ogre::Real mAirDensityTranslation;
    Ogre::Real mAirDensityRot;

    Ogre::Real mChassisMass;
    Ogre::Real mChassisInvMass;

    Ogre::Vector3 mMomentOfInertia;

    Ogre::Real mGravityVelocity;

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

    Ogre::Real mVelocityScale;

    PSSpline mPower;

    PSSpline mSteering;
    Ogre::Real mTurnFinish;

    Ogre::Real mHighFrontTraction;
    Ogre::Real mLowFrontTraction;
    Ogre::Real mHighRearTraction;
    Ogre::Real mLowRearTraction;

    Ogre::Real mWheelVelocitySub;

    Ogre::Real mOversteer;

    PSSpline mSuspensionTraction;
};


#endif