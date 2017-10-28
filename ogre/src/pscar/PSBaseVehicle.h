#ifndef PSBASEVEHICLE_H
#define PSBASEVEHICLE_H

#include "../includes/OgreInclude.h"
#include "../includes/OgreBulletInclude.h"
#include "../includes/CommonIncludes.h"

class CustomRigidBody;
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

    Ogre::Vector3 mInitialImpulseLinear;
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

    void initPhysicalModel(OgreBulletDynamics::DynamicsWorld * world, 
        Ogre::SceneNode* modelNode, Ogre::SceneNode *wheelNodes[4], 
        const InitialVehicleSetup& initialVehicleSetup);

    /**
     * Could be slow due to total removal from world
     * http://www.bulletphysics.org/Bullet/phpBB3/viewtopic.php?f=9&t=4517
     */
    void repositionVehicle(const Ogre::Vector3& chassisPos, const Ogre::Quaternion& chassisRot, Ogre::SceneNode* modelNode, Ogre::SceneNode *wheelNodes[4]);

    OgreBulletDynamics::DynamicsWorld * mWorld;

    CommonIncludes::shared_ptr<CustomRigidBody> mCarChassis;
    CommonIncludes::shared_ptr<CustomRigidBodyWheel> mCarWheelFrontL;
    CommonIncludes::shared_ptr<CustomRigidBodyWheel> mCarWheelFrontR;
    CommonIncludes::shared_ptr<CustomRigidBodyWheel> mCarWheelBackL;
    CommonIncludes::shared_ptr<CustomRigidBodyWheel> mCarWheelBackR;

    CommonIncludes::shared_ptr<OgreBulletDynamics::SixDofSpring2Constraint> mSixDofSpringFrontL;
    CommonIncludes::shared_ptr<OgreBulletDynamics::SixDofSpring2Constraint> mSixDofSpringFrontR;
    CommonIncludes::shared_ptr<OgreBulletDynamics::SixDofSpring2Constraint> mSixDofSpringBackL;
    CommonIncludes::shared_ptr<OgreBulletDynamics::SixDofSpring2Constraint> mSixDofSpringBackR;

    InitialVehicleSetup mInitialVehicleSetup;

    static Ogre::NameGenerator nameGenNodes;

private:

    void removeFromWorld();
    void addToWorld(Ogre::SceneNode* modelNode, Ogre::SceneNode *wheelNodes[4]);

    void addRigidsToWorld(Ogre::SceneNode* modelNode, Ogre::SceneNode *wheelNodes[4]);
    void addSpringsToWorld();

    CommonIncludes::shared_ptr<OgreBulletCollisions::CompoundCollisionShape> mCompoundShape;
    CommonIncludes::shared_ptr<OgreBulletCollisions::SphereCollisionShape> mRoofBackRShape;
    CommonIncludes::shared_ptr<OgreBulletCollisions::SphereCollisionShape> mRoofBackLShape;
    CommonIncludes::shared_ptr<OgreBulletCollisions::SphereCollisionShape> mRoofFrontRShape;
    CommonIncludes::shared_ptr<OgreBulletCollisions::SphereCollisionShape> mRoofFrontLShape;
    CommonIncludes::shared_ptr<OgreBulletCollisions::SphereCollisionShape> mChassisBodyShape;
    CommonIncludes::shared_ptr<OgreBulletCollisions::BoxCollisionShape> mChassisBodyShapeBox;

    CommonIncludes::shared_ptr<OgreBulletCollisions::SphereCollisionShape> mWheelShapeFront;
    CommonIncludes::shared_ptr<OgreBulletCollisions::SphereCollisionShape> mWheelShapeBack;

};

#endif