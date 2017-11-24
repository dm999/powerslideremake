#ifndef PHYSICSBODY_H
#define PHYSICSBODY_H

#include "../includes/OgreInclude.h"
#include "../includes/CommonIncludes.h"

#include "InitialVehicleSetup.h"

class btSphereShape;
class btCollisionObject;

class Physics;
class StaticMeshProcesser;
class PhysicsVehicle;

class PhysicsBody
{
public:
    PhysicsBody(const InitialVehicleSetup& initialVehicleSetup,
        Physics* physics,
        const StaticMeshProcesser* const meshProcesser);

    ~PhysicsBody();

    void init();
    void initStep();
    void process(const Ogre::SceneNode& chassis, PhysicsVehicle& vehicle);
    void reposition(const Ogre::Vector3& posDiff);
    void rerotation(const Ogre::Vector3& chassisPos, const Ogre::Quaternion& rot);

private:

    Physics* mPhysics;
    const StaticMeshProcesser* const mMeshProcesser;

    const InitialVehicleSetup& mInitialVehicleSetup;

    CommonIncludes::shared_ptr<btSphereShape> mBodyShape;
    CommonIncludes::shared_ptr<btCollisionObject> mBody;

    Ogre::Vector3 mBodyGlobal;
    Ogre::Vector3 mBodyGlobalPrev;
};

#endif