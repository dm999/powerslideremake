#ifndef PHYSICSROOFS_H
#define PHYSICSROOFS_H

#include "../includes/OgreInclude.h"

#include "InitialVehicleSetup.h"

class btSphereShape;
class btCollisionObject;

class Physics;
class StaticMeshProcesser;
class PhysicsVehicle;

class PhysicsRoofs
{
public:
    PhysicsRoofs(const InitialVehicleSetup& initialVehicleSetup,
        Physics* physics,
        const StaticMeshProcesser* const meshProcesser);

    ~PhysicsRoofs();

    void init();

    void initStep();
    void calcImpulses(const Ogre::Vector3& impulseRot, const Ogre::Vector3& impulseRotPrev, const Ogre::Vector3& normalisedImpulseRot,
                        const Ogre::Vector3& impulseLinear,
                        Ogre::Real recipMomentProj);
    bool process(PhysicsVehicle& vehicle);

private:

    Physics* mPhysics;
    const StaticMeshProcesser* const mMeshProcesser;

    const InitialVehicleSetup& mInitialVehicleSetup;

    Ogre::Vector3 mRoofImpulseLinear[InitialVehicleSetup::mRoofsAmount];//RR, RL, FR, FL
    Ogre::Vector3 mRoofGlobal[InitialVehicleSetup::mRoofsAmount];//RR, RL, FR, FL
    Ogre::Vector3 mRoofGlobalPrev[InitialVehicleSetup::mRoofsAmount];//RR, RL, FR, FL
};

#endif