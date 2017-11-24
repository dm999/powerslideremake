#ifndef PHYSICSWHEELS_H
#define PHYSICSWHEELS_H

#include "../includes/OgreInclude.h"
#include "../includes/CommonIncludes.h"

#include "InitialVehicleSetup.h"

class btSphereShape;
class btCollisionObject;

class Physics;
class StaticMeshProcesser;
class PhysicsVehicle;

class PhysicsWheels
{
public:
    PhysicsWheels(const InitialVehicleSetup& initialVehicleSetup,
        Physics* physics,
        const StaticMeshProcesser* const meshProcesser);

    ~PhysicsWheels();

    void init(Ogre::SceneNode *wheelNodes[InitialVehicleSetup::mWheelsAmount]);

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

    Ogre::SceneNode *mWheelNodes[InitialVehicleSetup::mWheelsAmount];//RR, RL, FR, FL

    CommonIncludes::shared_ptr<btSphereShape> mWheelShape[InitialVehicleSetup::mWheelsAmount];//RR, RL, FR, FL
    CommonIncludes::shared_ptr<btCollisionObject> mWheel[InitialVehicleSetup::mWheelsAmount];//RR, RL, FR, FL
};


#endif