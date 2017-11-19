#ifndef INITIALVEHICLESETUP_H
#define INITIALVEHICLESETUP_H

#include "../tools/PSSpline.h"

struct InitialVehicleSetup
{
    Ogre::Vector3 mChassisPos;
    Ogre::Quaternion mChassisRot;
    Ogre::Vector3 mConnectionPointRRWheel;
    Ogre::Vector3 mConnectionPointRLWheel;
    Ogre::Vector3 mConnectionPointFRWheel;
    Ogre::Vector3 mConnectionPointFLWheel;

    float mRoofBackRadius;
    Ogre::Vector3 mRoofBackPos;

    float mRoofFrontRadius;
    Ogre::Vector3 mRoofFrontPos;

    float mBodyRadius;
    Ogre::Vector3 mBodyBasePos;

    Ogre::Real mWheelRadiusFront;
    Ogre::Real mWheelRadiusBack;

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

    Ogre::Vector3 mInitialImpulseLinear;
    Ogre::Vector3 mInitialImpulseLinearInc;
    Ogre::Vector3 mInitialImpulseRot;
    Ogre::Vector3 mInitialImpulseRotInc;

    Ogre::Vector3 mMomentOfInertia;

    Ogre::Real mGravityForce;

    PSSpline mWheelUnderGroundDD;
    PSSpline mWheelUnderGroundVV;
    PSSpline mWheelUnderGroundDDV;
    PSSpline mWheelUnderGroundVDV;
};


#endif