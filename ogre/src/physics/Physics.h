#ifndef PHYSICS_H
#define PHYSICS_H

#include <map>

#include "../includes/CommonIncludes.h"

#include "../Enums.h"

#include "InitialVehicleSetup.h"

struct GameWorld;

struct DE2SingleBatch;
struct DE2Part;

class PSBaseVehicle;
class PhysicsVehicle;

class StaticMeshProcesser;

class GameState;

class PhysicsListener;

class Physics
{
public:

    Physics(StaticMeshProcesser * meshProesser);
    ~Physics();

    void timeStep(GameState& gameState);

    PhysicsVehicle* addVehicle(InitialVehicleSetup& initialVehicleSetup, PSBaseVehicle * vehiclePtr, 
        Ogre::SceneNode *wheelNodes[InitialVehicleSetup::mWheelsAmount], Ogre::SceneNode *chassis,
        InputType type,
        bool isAI
        );
    void removeVehicle(PSBaseVehicle * vehiclePtr);
    const PhysicsVehicle * getVehicle(PSBaseVehicle * vehiclePtr) const;
    PhysicsVehicle * getVehicle(PSBaseVehicle * vehiclePtr);

    void addListener(PhysicsListener* listener);
    void removeListener(PhysicsListener* listener);

private:

    void internalTimeStep(GameState& gameState);

    Ogre::Timer mTimer;

    Ogre::int32 mTimeStep;
    Ogre::int32 mTimeStep2;

    StaticMeshProcesser * mMeshProesser;

    typedef std::map<PSBaseVehicle *, CommonIncludes::shared_ptr<PhysicsVehicle> > vehicles;
    vehicles mVehicles;

    typedef std::vector<PhysicsListener *> physicsListener;
    physicsListener mListeners;
};

#endif
