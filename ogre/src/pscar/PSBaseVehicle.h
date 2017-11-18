#ifndef PSBASEVEHICLE_H
#define PSBASEVEHICLE_H

#include "../includes/OgreInclude.h"
#include "../includes/CommonIncludes.h"
#include "../includes/BulletInclude.h"

#include "../physics/InitialVehicleSetup.h"

class CustomRigidBody;
class CustomRigidBodyWheel;
class Physics;


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

    static const int mWheelsAmount = 4;

    void initPhysicalModel(Physics * world, 
        Ogre::SceneNode* modelNode, Ogre::SceneNode *wheelNodes[mWheelsAmount], 
        const InitialVehicleSetup& initialVehicleSetup);

    /**
     * Could be slow due to total removal from world
     * http://www.bulletphysics.org/Bullet/phpBB3/viewtopic.php?f=9&t=4517
     */
    void repositionVehicle(const Ogre::Vector3& chassisPos, const Ogre::Quaternion& chassisRot, Ogre::SceneNode* modelNode, Ogre::SceneNode *wheelNodes[4]);

    Physics * mWorld;
#if 0
    CommonIncludes::shared_ptr<CustomRigidBody> mCarChassis;
    CommonIncludes::shared_ptr<CustomRigidBodyWheel> mCarWheelFrontL;
    CommonIncludes::shared_ptr<CustomRigidBodyWheel> mCarWheelFrontR;
    CommonIncludes::shared_ptr<CustomRigidBodyWheel> mCarWheelBackL;
    CommonIncludes::shared_ptr<CustomRigidBodyWheel> mCarWheelBackR;

    CommonIncludes::shared_ptr<OgreBulletDynamics::SixDofSpring2Constraint> mSixDofSpringFrontL;
    CommonIncludes::shared_ptr<OgreBulletDynamics::SixDofSpring2Constraint> mSixDofSpringFrontR;
    CommonIncludes::shared_ptr<OgreBulletDynamics::SixDofSpring2Constraint> mSixDofSpringBackL;
    CommonIncludes::shared_ptr<OgreBulletDynamics::SixDofSpring2Constraint> mSixDofSpringBackR;
#endif
    InitialVehicleSetup mInitialVehicleSetup;

    static Ogre::NameGenerator nameGenNodes;

private:

    void removeFromWorld();
    void addToWorld(Ogre::SceneNode* modelNode, Ogre::SceneNode *wheelNodes[4]);

    void addRigidsToWorld(Ogre::SceneNode* modelNode, Ogre::SceneNode *wheelNodes[4]);
    void addSpringsToWorld();
#if 0
    CommonIncludes::shared_ptr<OgreBulletCollisions::CompoundCollisionShape> mCompoundShape;
    CommonIncludes::shared_ptr<OgreBulletCollisions::SphereCollisionShape> mRoofBackRShape;
    CommonIncludes::shared_ptr<OgreBulletCollisions::SphereCollisionShape> mRoofBackLShape;
    CommonIncludes::shared_ptr<OgreBulletCollisions::SphereCollisionShape> mRoofFrontRShape;
    CommonIncludes::shared_ptr<OgreBulletCollisions::SphereCollisionShape> mRoofFrontLShape;
    CommonIncludes::shared_ptr<OgreBulletCollisions::SphereCollisionShape> mChassisBodyShape;
    CommonIncludes::shared_ptr<OgreBulletCollisions::BoxCollisionShape> mChassisBodyShapeBox;

    CommonIncludes::shared_ptr<OgreBulletCollisions::SphereCollisionShape> mWheelShapeFront;
    CommonIncludes::shared_ptr<OgreBulletCollisions::SphereCollisionShape> mWheelShapeBack;
#endif
};

#endif