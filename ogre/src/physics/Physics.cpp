
#include "Physics.h"

#include "../mesh/StaticMeshProcesser.h"

#include "PhysicsVehicle.h"
#include "PhysicsVehicleAI.h"


Physics::Physics(StaticMeshProcesser * meshProesser)
    : 
    mTimeStep(0.0f),
    mMeshProesser(meshProesser)
{}

Physics::~Physics()
{}

void Physics::timeStep(const GameState& gameState, Ogre::Real timeStep, size_t maxSubSteps, Ogre::Real fixedTimeStep)
{

    size_t numSimulationSubSteps = 0;

    mTimeStep += timeStep;
    if (mTimeStep >= fixedTimeStep)
    {
        numSimulationSubSteps = static_cast<size_t>(mTimeStep / fixedTimeStep);
        mTimeStep -= numSimulationSubSteps * fixedTimeStep;
    }

    if (numSimulationSubSteps)
    {
        size_t clampedSimulationSteps = (numSimulationSubSteps > maxSubSteps) ? maxSubSteps : numSimulationSubSteps;
        for (size_t i = 0; i < clampedSimulationSteps; i++)
        {
            internalTimeStep(gameState);
        }
    }
}

void Physics::internalTimeStep(const GameState& gameState)
{
    for (vehicles::iterator i = mVehicles.begin(), j = mVehicles.end(); i != j; ++i)
    {
        (*i).second->timeStep(gameState);
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
