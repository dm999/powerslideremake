#ifndef PSBASEVEHICLE_H
#define PSBASEVEHICLE_H

#include "../includes/OgreInclude.h"
#include "../includes/OgreBulletInclude.h"
#include "../includes/CommonIncludes.h"

class CustomRigidBodyWheel;

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

    Ogre::Vector2 mWheelRadius;

    Ogre::Vector3 mAnisotropicFriction;
    Ogre::Real mRollingFriction;

    Ogre::Real mMaxTravel;

    Ogre::Real mWheelsFSpringStiffness;
    Ogre::Real mWheelsFSpringDamping;
    Ogre::Real mWheelsRSpringStiffness;
    Ogre::Real mWheelsRSpringDamping;
    bool mLimitSpringParamsF;
    bool mLimitSpringParamsR;

    Ogre::Real mAirDensityLinear;
    Ogre::Real mAirDensityAngular;

    Ogre::Real mChassisMass;
    Ogre::Real mChassisRestitution;
    Ogre::Real mChassisFriction;

    Ogre::Real mWheelsFMass;
    Ogre::Real mWheelsRMass;
    Ogre::Real mWheelsRestitution;
    Ogre::Real mWheelsFriction;
};

/**
 * Class to manipulate with bullet objects
 */
class PSBaseVehicle
{
public:

    PSBaseVehicle(){}
    virtual ~PSBaseVehicle(){}

    virtual void clear();

protected:

    void initPhysicalModel(OgreBulletDynamics::DynamicsWorld * world, Ogre::SceneNode* modelNode, Ogre::SceneNode *wheelNodes[4], const InitialVehicleSetup& initialVehicleSetup);

    OgreBulletDynamics::RigidBody *mCarChassis;
    CustomRigidBodyWheel *mCarWheelFrontL;
    CustomRigidBodyWheel *mCarWheelFrontR;
    CustomRigidBodyWheel *mCarWheelBackL;
    CustomRigidBodyWheel *mCarWheelBackR;

    OgreBulletDynamics::SixDofSpring2Constraint * mSixDofSpringFrontL;
    OgreBulletDynamics::SixDofSpring2Constraint * mSixDofSpringFrontR;
    OgreBulletDynamics::SixDofSpring2Constraint * mSixDofSpringBackL;
    OgreBulletDynamics::SixDofSpring2Constraint * mSixDofSpringBackR;

    InitialVehicleSetup mInitialVehicleSetup;

    static Ogre::NameGenerator nameGenNodes;

private:

    void removeFromWorld();
    void addToWorld();

};

#endif