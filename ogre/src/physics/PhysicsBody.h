#ifndef PHYSICSBODY_H
#define PHYSICSBODY_H

#include "../includes/OgreInclude.h"

#include "InitialVehicleSetup.h"

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
    bool process(PhysicsVehicle& vehicle);

private:

    Physics* mPhysics;
    const StaticMeshProcesser* const mMeshProcesser;

    const InitialVehicleSetup& mInitialVehicleSetup;

    Ogre::Vector3 mBodyGlobal;
    Ogre::Vector3 mBodyGlobalPrev;
};

#endif