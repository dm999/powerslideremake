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

    mCarEngine.setTransmissionType(trAuto);
}

void PhysicsVehicleAI::doAIStep(const GameState& gameState)
{
    if(mAICar)
        mAICar->performAICorrection(gameState, this, mInitialVehicleSetup);
}

void PhysicsVehicleAI::setSteering(Ogre::Real value)
{
    mSteeringOriginal = value;
}

void PhysicsVehicleAI::setAcceleration(Ogre::Real value)
{
    mThrottle = value;
}

void PhysicsVehicleAI::setBreaks(Ogre::Real value)
{
    mBreaks = value;
}

Ogre::Real PhysicsVehicleAI::getLinearVelocity() const
{
    return mVehicleVelocityMod;
}

void PhysicsVehicleAI::setLinearVelocity(Ogre::Real val)
{
    mVehicleVelocityMod = val;
}

Ogre::Vector3 PhysicsVehicleAI::getLinearImpulseInc() const
{
    return mImpulseLinearInc;
}

void PhysicsVehicleAI::setLinearImpulseInc(const Ogre::Vector3& val)
{
    mImpulseLinearInc = val;
}

Ogre::Real PhysicsVehicleAI::doAdjustAISteering(Ogre::Real val)const
{
    return Ogre::Math::Pow(val * 1.0526316f, 1.2f);
}

Ogre::Real PhysicsVehicleAI::doGetVelocityScale()const
{
    Ogre::Real ret = 1.0f;

    if(mAICar)
        ret = mAICar->getAIData().velocityScale;

    return ret;
}

Ogre::Real PhysicsVehicleAI::doGetThrottleScale()const
{
    Ogre::Real ret = 1.0f;

    if(mAICar)
        ret = mAICar->getAIData().aiImpulseScale;

    return ret;
}

Ogre::Real PhysicsVehicleAI::doGetTractionScale()const
{
    Ogre::Real ret = 1.0f;

    if(mAICar)
        ret = mAICar->getAIData().tractionCoeff;

    return ret;
}