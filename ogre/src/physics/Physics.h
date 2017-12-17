#ifndef PHYSICS_H
#define PHYSICS_H

#include <map>

#include "../includes/CommonIncludes.h"

#include "InitialVehicleSetup.h"

struct GameWorld;

struct DE2SingleBatch;
struct DE2Part;

class PSBaseVehicle;
class PhysicsVehicle;

class StaticMeshProcesser;

class GameState;

class Physics
{
public:

    Physics(StaticMeshProcesser * meshProesser);
    ~Physics();

    void timeStep(const GameState& gameState, Ogre::Real timeStep, size_t maxSubSteps = 1, Ogre::Real fixedTimeStep = 1.0f / 60.0f);

    PhysicsVehicle* addVehicle(InitialVehicleSetup& initialVehicleSetup, PSBaseVehicle * vehiclePtr, 
        Ogre::SceneNode *wheelNodes[InitialVehicleSetup::mWheelsAmount], Ogre::SceneNode *chassis,
        bool isAI
        );
    void removeVehicle(const PSBaseVehicle * vehiclePtr);
    const PhysicsVehicle * getVehicle(const PSBaseVehicle * vehiclePtr) const;
    PhysicsVehicle * getVehicle(const PSBaseVehicle * vehiclePtr);


private:

    void internalTimeStep(const GameState& gameState);

    Ogre::Real mTimeStep;

    StaticMeshProcesser * mMeshProesser;

    typedef std::map<const PSBaseVehicle *, CommonIncludes::shared_ptr<PhysicsVehicle> > vehicles;
    vehicles mVehicles;
};

#endif
