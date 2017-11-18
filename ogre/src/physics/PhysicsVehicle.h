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

    void shiftPos(const Ogre::Vector3& shiftAmount);

protected:

    Physics* mPhysics;

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

private:
};

#endif