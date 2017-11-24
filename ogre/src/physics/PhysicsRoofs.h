#ifndef PHYSICSROOFS_H
#define PHYSICSROOFS_H

#include "../includes/OgreInclude.h"
#include "../includes/CommonIncludes.h"

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

    void init(const Ogre::Vector3& chassisPos);

    void initStep();
    void calcImpulses(const Ogre::Vector3& impulseRot, const Ogre::Vector3& impulseRotPrev, const Ogre::Vector3& normalisedImpulseRot,
                        const Ogre::Vector3& impulseLinear,
                        Ogre::Real recipMomentProj,
                        const Ogre::Quaternion& carRot,
                        const PhysicsVehicle& vehicle);
    void process(const Ogre::SceneNode& chassis, PhysicsVehicle& vehicle);
    void reposition(const Ogre::Vector3& posDiff);
    void rerotation(const Ogre::Vector3& chassisPos, const Ogre::Quaternion& rot);

private:

    Physics* mPhysics;
    const StaticMeshProcesser* const mMeshProcesser;

    const InitialVehicleSetup& mInitialVehicleSetup;

    CommonIncludes::shared_ptr<btSphereShape> mRoofShape[InitialVehicleSetup::mRoofsAmount];//RR, RL, FR, FL
    CommonIncludes::shared_ptr<btCollisionObject> mRoof[InitialVehicleSetup::mRoofsAmount];//RR, RL, FR, FL

    Ogre::Vector3 mRoofImpulseLinear[InitialVehicleSetup::mRoofsAmount];//RR, RL, FR, FL
    Ogre::Vector3 mRoofGlobal[InitialVehicleSetup::mRoofsAmount];//RR, RL, FR, FL
    Ogre::Vector3 mRoofGlobalPrev[InitialVehicleSetup::mRoofsAmount];//RR, RL, FR, FL
};

#endif