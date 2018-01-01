
#include "Physics.h"

#include "../mesh/StaticMeshProcesser.h"

#include "PhysicsVehicle.h"
#include "PhysicsVehicleAI.h"

#include "../listeners/PhysicsListener.h"


Physics::Physics(StaticMeshProcesser * meshProesser)
    : 
    mMeshProesser(meshProesser),
    mTimeStep(-1),
    mTimeStep2(0)
{}

Physics::~Physics()
{}

void Physics::timeStep(GameState& gameState)
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

void Physics::internalTimeStep(GameState& gameState)
{
    for (physicsListener::iterator i = mListeners.begin(), j = mListeners.end(); i != j; ++i)
    {
        (*i)->timeStepBefore(this);
    }

    if(gameState.getRaceStarted() && !gameState.isGamePaused())
    {
        for (vehicles::iterator i = mVehicles.begin(), j = mVehicles.end(); i != j; ++i)
        {
            (*i).second->timeStep(gameState);
            (*i).first->processCamera(gameState);

            for (physicsListener::iterator ii = mListeners.begin(), jj = mListeners.end(); ii != jj; ++ii)
            {
                (*ii)->timeStepForVehicle((*i).second.get(), mVehicles);
            }
        }
    }

    if(!gameState.getRaceStarted() && !gameState.isGamePaused())
    {
        for (vehicles::iterator i = mVehicles.begin(), j = mVehicles.end(); i != j; ++i)
        {
            (*i).second->processEngineIdle();
            (*i).first->processCamera(gameState);

            for (physicsListener::iterator ii = mListeners.begin(), jj = mListeners.end(); ii != jj; ++ii)
            {
                (*ii)->timeStepForVehicle((*i).second.get(), mVehicles);
            }
        }
    }

    for (physicsListener::iterator i = mListeners.begin(), j = mListeners.end(); i != j; ++i)
    {
        (*i)->timeStepAfter(this);
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

void Physics::removeVehicle(PSBaseVehicle * vehiclePtr)
{
    vehicles::const_iterator found = mVehicles.find(vehiclePtr);
    mVehicles.erase(found);
}

const PhysicsVehicle * Physics::getVehicle(PSBaseVehicle * vehiclePtr) const
{
    const PhysicsVehicle * ret = NULL;
    vehicles::const_iterator found = mVehicles.find(vehiclePtr);
    if(found != mVehicles.end())
        ret = (*found).second.get();
    return ret;
}

PhysicsVehicle * Physics::getVehicle(PSBaseVehicle * vehiclePtr)
{
    PhysicsVehicle * ret = NULL;
    vehicles::const_iterator found = mVehicles.find(vehiclePtr);
    if(found != mVehicles.end())
        ret = (*found).second.get();
    return ret;
}

void Physics::addListener(PhysicsListener* listener)
{
    mListeners.push_back(listener);
}

void Physics::removeListener(PhysicsListener* listener)
{
    physicsListener::const_iterator found = std::find(mListeners.begin(), mListeners.end(), listener);
    if(found != mListeners.end())
        mListeners.erase(found);
}