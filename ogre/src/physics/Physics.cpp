
#include "Physics.h"

#include "../mesh/StaticMeshProcesser.h"

#include "PhysicsVehicle.h"


Physics::Physics(StaticMeshProcesser * meshProesser)
    : mMeshProesser(meshProesser)
{}

Physics::~Physics()
{}

void Physics::timeStep()
{
    for(vehicles::iterator i = mVehicles.begin(), j = mVehicles.end(); i != j; ++i)
    {
        (*i).second->timeStep();
    }
}

void Physics::addVehicle(const InitialVehicleSetup& initialVehicleSetup, const PSBaseVehicle * vehiclePtr,
                        Ogre::SceneNode *wheelNodes[InitialVehicleSetup::mWheelsAmount], Ogre::SceneNode *chassis)
{
    vehicles::const_iterator found = mVehicles.find(vehiclePtr);

    if(found == mVehicles.end())
    {
        CommonIncludes::shared_ptr<PhysicsVehicle> vehicle = CommonIncludes::shared_ptr<PhysicsVehicle>(new PhysicsVehicle(this, mMeshProesser, initialVehicleSetup, wheelNodes, chassis));
        mVehicles.insert(std::make_pair(vehiclePtr, vehicle));
    }
}

void Physics::removeVehicle(const PSBaseVehicle * vehiclePtr)
{
    vehicles::const_iterator found = mVehicles.find(vehiclePtr);
    mVehicles.erase(found);
}
