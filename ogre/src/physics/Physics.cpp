
#include "Physics.h"

#include "../mesh/StaticMeshProcesser.h"

#include "PhysicsVehicle.h"
#include "PhysicsVehicleAI.h"


Physics::Physics(StaticMeshProcesser * meshProesser)
    : 
    mMeshProesser(meshProesser),
    mTimeStep(-1),
    mTimeStep2(0)
{}

Physics::~Physics()
{}

void Physics::timeStep(const GameState& gameState)
{
    Ogre::Real timeFromStartSeconds = mTimer.getMilliseconds() / 1000.0f;

    mTimeStep2 = static_cast<Ogre::int32>(timeFromStartSeconds * 60.0f + 1.0f);

    if(mTimeStep2 - mTimeStep > 15 || mTimeStep2 < mTimeStep)
        mTimeStep = mTimeStep2 - 15;

    if(mTimeStep < mTimeStep2)
    {
        do
        {
            internalTimeStep(gameState);
            ++mTimeStep;
        }while(mTimeStep < mTimeStep2);
    }
}

void Physics::internalTimeStep(const GameState& gameState)
{
    if(gameState.getRaceStarted() && !gameState.isGamePaused())
    {
        for (vehicles::iterator i = mVehicles.begin(), j = mVehicles.end(); i != j; ++i)
        {
            (*i).second->timeStep(gameState);
        }
    }

    if(!gameState.getRaceStarted() && !gameState.isGamePaused())
    {
        for (vehicles::iterator i = mVehicles.begin(), j = mVehicles.end(); i != j; ++i)
        {
            (*i).second->processEngineIdle();
        }
    }

}

PhysicsVehicle* Physics::addVehicle(InitialVehicleSetup& initialVehicleSetup, PSBaseVehicle * vehiclePtr,
                        Ogre::SceneNode *wheelNodes[InitialVehicleSetup::mWheelsAmount], Ogre::SceneNode *chassis,
                        bool isAI)
{
    PhysicsVehicle* ret = NULL;

    vehicles::const_iterator found = mVehicles.find(vehiclePtr);

    if(found == mVehicles.end())
    {
        CommonIncludes::shared_ptr<PhysicsVehicle> vehicle;
        if(isAI)
            vehicle = CommonIncludes::shared_ptr<PhysicsVehicle>(new PhysicsVehicleAI(this, mMeshProesser, initialVehicleSetup, wheelNodes, chassis, static_cast<PSAICar*>(vehiclePtr)));
        else
            vehicle = CommonIncludes::shared_ptr<PhysicsVehicle>(new PhysicsVehicle(this, mMeshProesser, initialVehicleSetup, wheelNodes, chassis));
        ret = vehicle.get();
        mVehicles.insert(std::make_pair(vehiclePtr, vehicle));
    }

    return ret;
}

void Physics::removeVehicle(const PSBaseVehicle * vehiclePtr)
{
    vehicles::const_iterator found = mVehicles.find(vehiclePtr);
    mVehicles.erase(found);
}

const PhysicsVehicle * Physics::getVehicle(const PSBaseVehicle * vehiclePtr) const
{
    const PhysicsVehicle * ret = NULL;
    vehicles::const_iterator found = mVehicles.find(vehiclePtr);
    if(found != mVehicles.end())
        ret = (*found).second.get();
    return ret;
}

PhysicsVehicle * Physics::getVehicle(const PSBaseVehicle * vehiclePtr)
{
    PhysicsVehicle * ret = NULL;
    vehicles::const_iterator found = mVehicles.find(vehiclePtr);
    if(found != mVehicles.end())
        ret = (*found).second.get();
    return ret;
}
