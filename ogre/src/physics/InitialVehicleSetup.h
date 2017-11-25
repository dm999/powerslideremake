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

    float mBodyRadius;
    Ogre::Vector3 mBodyBasePos;

    Ogre::Vector3 mAnisotropicFriction;
    Ogre::Real mRollingFriction;

    Ogre::Real mMaxTravel;

    Ogre::Real mWheelsFSpringStiffness;
    Ogre::Real mWheelsFSpringDamping;
    Ogre::Real mWheelsRSpringStiffness;
    Ogre::Real mWheelsRSpringDamping;
    bool mLimitSpringParamsF;
    bool mLimitSpringParamsR;

    Ogre::Real mAirDensityTranslation;
    Ogre::Real mAirDensityRot;

    Ogre::Real mChassisMass;
    Ogre::Real mChassisInvMass;
    Ogre::Real mChassisRestitution;
    Ogre::Real mChassisFriction;

    Ogre::Real mWheelsFMass;
    Ogre::Real mWheelsRMass;
    Ogre::Real mWheelsRestitution;
    Ogre::Real mWheelsFriction;

    Ogre::Vector3 mMomentOfInertia;

    Ogre::Real mGravityForce;

    PSSpline mWheelUnderGroundDD;
    PSSpline mWheelUnderGroundVV;
    PSSpline mWheelUnderGroundDDV;
    PSSpline mWheelUnderGroundVDV;

    static const int mVelocitySplinesAmoint = TerrainData::mTerrainsAmount;
    PSSpline mVelocitySpline[mVelocitySplinesAmoint];
};


#endif