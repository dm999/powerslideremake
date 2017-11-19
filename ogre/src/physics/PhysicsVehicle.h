#ifndef PHYSICSVEHICLE_H
#define PHYSICSVEHICLE_H

#include "../includes/OgreInclude.h"
#include "../includes/CommonIncludes.h"

#include "InitialVehicleSetup.h"

class btSphereShape;
class btCollisionObject;

class Physics;

class PhysicsVehicle
{
public:

    static const int mWheelsAmount = 4;

    PhysicsVehicle(Physics* physics, const InitialVehicleSetup& initialVehicleSetup, Ogre::SceneNode *wheelNodes[mWheelsAmount], Ogre::SceneNode *chassis);
    virtual ~PhysicsVehicle();

    void timeStep();

protected:

    Physics* mPhysics;

    Ogre::Vector3 mImpulseLinear;
    Ogre::Vector3 mImpulseLinearInc;
    Ogre::Vector3 mImpulseRot;
    Ogre::Vector3 mImpulseRotInc;

    Ogre::Real mVehicleVelocityMod;

    Ogre::SceneNode *mWheelNodes[mWheelsAmount];
    Ogre::SceneNode *mChassis;

    CommonIncludes::shared_ptr<btSphereShape> mWheelShapeRL;
    CommonIncludes::shared_ptr<btSphereShape> mWheelShapeRR;
    CommonIncludes::shared_ptr<btSphereShape> mWheelShapeFL;
    CommonIncludes::shared_ptr<btSphereShape> mWheelShapeFR;

    CommonIncludes::shared_ptr<btCollisionObject> mWheelRL;
    CommonIncludes::shared_ptr<btCollisionObject> mWheelRR;
    CommonIncludes::shared_ptr<btCollisionObject> mWheelFL;
    CommonIncludes::shared_ptr<btCollisionObject> mWheelFR;

    CommonIncludes::shared_ptr<btSphereShape> mBodyShape;

    CommonIncludes::shared_ptr<btCollisionObject> mBody;

private:

    void reposition(const Ogre::Vector3& posDiff);

    void integrateLinear();
    void integrateRot();

    Ogre::Real momentOfInertiaProj(const Ogre::Vector3& axis)const;

    const InitialVehicleSetup& mInitialVehicleSetup;
};

#endif