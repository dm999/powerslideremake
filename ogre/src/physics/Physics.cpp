
#include "Physics.h"

#include "../mesh/StaticMeshProcesser.h"

#include "PhysicsVehicle.h"


Physics::Physics(StaticMeshProcesser * meshProesser)
    : 
    mTimeStep(0.0f),
    mMeshProesser(meshProesser)
{}

Physics::~Physics()
{}

void Physics::timeStep(Ogre::Real timeStep, size_t maxSubSteps, Ogre::Real fixedTimeStep)
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
            internalTimeStep();
        }
    }
}

void Physics::internalTimeStep()
{
    for (vehicles::iterator i = mVehicles.begin(), j = mVehicles.end(); i != j; ++i)
    {
        (*i).second->timeStep();
    }
}

void Physics::addVehicle(InitialVehicleSetup& initialVehicleSetup, const PSBaseVehicle * vehiclePtr,
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
