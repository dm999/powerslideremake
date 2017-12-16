#ifndef PHYSICS_H
#define PHYSICS_H

#include <map>

#include "../includes/CommonIncludes.h"

#include "InitialVehicleSetup.h"

struct GameWorld;

struct DE2SingleBatch;
struct DE2Part;

class btCollisionObject;
class btCollisionShape;
class btTriangleMesh;
class btCollisionObject;

class PSBaseVehicle;
class PhysicsVehicle;

class StaticMeshProcesser;

class Physics
{
public:

    Physics(StaticMeshProcesser * meshProesser);
    ~Physics();

    void timeStep(Ogre::Real timeStep, size_t maxSubSteps = 1, Ogre::Real fixedTimeStep = 1.0f / 60.0f);

    PhysicsVehicle* addVehicle(InitialVehicleSetup& initialVehicleSetup, const PSBaseVehicle * vehiclePtr, 
        Ogre::SceneNode *wheelNodes[InitialVehicleSetup::mWheelsAmount], Ogre::SceneNode *chassis
        );
    void removeVehicle(const PSBaseVehicle * vehiclePtr);
    const PhysicsVehicle * getVehicle(const PSBaseVehicle * vehiclePtr) const;
    PhysicsVehicle * getVehicle(const PSBaseVehicle * vehiclePtr);


private:

    void internalTimeStep();

    Ogre::Real mTimeStep;

    StaticMeshProcesser * mMeshProesser;

    typedef std::map<const PSBaseVehicle *, CommonIncludes::shared_ptr<PhysicsVehicle> > vehicles;
    vehicles mVehicles;
};

#endif
