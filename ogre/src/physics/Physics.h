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

    void setCollisionSplinesDD(const PSSpline& collisionDD){mCollisionDD = collisionDD;}
    void setCollisionSplinesVV(const PSSpline& collisionVV){mCollisionVV = collisionVV;}
    void setCollisionSplinesDDV(const PSSpline& collisionDDV){mCollisionDDV = collisionDDV;}
    void setCollisionSplinesVDV(const PSSpline& collisionVDV){mCollisionVDV = collisionVDV;}

    Ogre::int32 getAfterStartCounter () const {return mAfterStartCounter;}

    void onCarDead(PhysicsVehicle* vehicle);

private:

    void internalTimeStep(GameState& gameState);

    void processCarsCollisions(PhysicsVehicle* vehicle, GameState& gameState);
    void processCollisionsImpulseWeighter(PhysicsVehicle* vehicle);

    Ogre::Timer mTimer;

    Ogre::int32 mTimeStep;
    Ogre::int32 mTimeStep2;

    Ogre::int32 mAfterStartCounter;

    StaticMeshProcesser * mMeshProesser;

    typedef std::map<PSBaseVehicle *, CommonIncludes::shared_ptr<PhysicsVehicle> > vehicles;
    vehicles mVehicles;

    PSSpline mCollisionDD;
    PSSpline mCollisionVV;
    PSSpline mCollisionDDV;
    PSSpline mCollisionVDV;

    typedef std::vector<PhysicsListener *> physicsListener;
    physicsListener mListeners;
};

#endif
