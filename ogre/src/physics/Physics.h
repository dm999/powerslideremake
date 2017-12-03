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

    void timeStep();

    void addVehicle(const InitialVehicleSetup& initialVehicleSetup, const PSBaseVehicle * vehiclePtr, 
        Ogre::SceneNode *wheelNodes[InitialVehicleSetup::mWheelsAmount], Ogre::SceneNode *chassis
        );
    void removeVehicle(const PSBaseVehicle * vehiclePtr);


private:

    StaticMeshProcesser * mMeshProesser;

    typedef std::map<const PSBaseVehicle *, CommonIncludes::shared_ptr<PhysicsVehicle> > vehicles;
    vehicles mVehicles;
};

#endif
