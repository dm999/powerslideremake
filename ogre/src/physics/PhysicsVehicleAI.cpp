#include "PhysicsVehicleAI.h"

#include "../pscar/PSAICar.h"

PhysicsVehicleAI::PhysicsVehicleAI(Physics* physics, 
                               StaticMeshProcesser * meshProesser,
                               InitialVehicleSetup& initialVehicleSetup, 
                               Ogre::SceneNode *wheelNodes[InitialVehicleSetup::mWheelsAmount],
                               Ogre::SceneNode *chassis,
                               PSAICar * aiCar) :
    PhysicsVehicle(physics, meshProesser, initialVehicleSetup, wheelNodes, chassis)
{
    mVehicleType = AIVehicle;
    mAICar = aiCar;
}

void PhysicsVehicleAI::doAIStep(const GameState& gameState)
{
    if(mAICar)
        mAICar->performAICorrection(gameState, this);
}

void PhysicsVehicleAI::setSteering(Ogre::Real value)
{
    mSteeringOriginal = -value;
}

void PhysicsVehicleAI::setAcceleration(Ogre::Real value)
{
    mThrottle = value;
}

void PhysicsVehicleAI::setBreaks(Ogre::Real value)
{
    mBreaks = value;
}