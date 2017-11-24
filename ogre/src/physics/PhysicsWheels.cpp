
#include "PhysicsRoofs.h"

#include "Physics.h"
#include "PhysicsVehicle.h"
#include "../tools/PhysicsTools.h"
#include "../mesh/StaticMeshProcesser.h"

PhysicsWheels::PhysicsWheels(const InitialVehicleSetup& initialVehicleSetup, 
                        Physics* physics,
                        const StaticMeshProcesser* const meshProcesser)
    :   mInitialVehicleSetup(initialVehicleSetup),
        mPhysics(physics),
        mMeshProcesser(meshProcesser)
{}

PhysicsWheels::~PhysicsWheels()
{
    for(int q = 0; q < InitialVehicleSetup::mWheelsAmount; ++q)
    {
        if(mWheel[q].get())
            mPhysics->removeCollisionObject(mWheel[q].get());
    }
}

void PhysicsWheels::init(Ogre::SceneNode *wheelNodes[InitialVehicleSetup::mWheelsAmount])
{
    for(int q = 0; q < InitialVehicleSetup::mWheelsAmount; ++q)
    {
        mWheelNodes[q] = wheelNodes[q];
        mWheelShape[q] = CommonIncludes::shared_ptr<btSphereShape>(new btSphereShape(mInitialVehicleSetup.mWheelRadius[q]));
        mWheel[q] = CommonIncludes::shared_ptr<btCollisionObject>(new btCollisionObject());
        mWheel[q]->setCollisionShape(mWheelShape[q].get());
        mWheel[q]->getWorldTransform().setOrigin(PhysicsTools::convert(mInitialVehicleSetup.mChassisPos + mInitialVehicleSetup.mChassisRot * mInitialVehicleSetup.mConnectionPointWheel[q]));
        mWheel[q]->getWorldTransform().setRotation(PhysicsTools::convert(mInitialVehicleSetup.mChassisRot));

        mPhysics->addKinematicObject(mWheel[q].get());
    }
}

void PhysicsWheels::initStep()
{
}

void PhysicsWheels::reposition(const Ogre::Vector3& posDiff)
{
    for(int q = 0; q < InitialVehicleSetup::mWheelsAmount; ++q)
    {
        mWheelNodes[q]->translate(posDiff);
        if(mWheel[q].get())
            mWheel[q]->getWorldTransform().setOrigin(mWheel[q]->getWorldTransform().getOrigin() + PhysicsTools::convert(posDiff));
    }
}

void PhysicsWheels::rerotation(const Ogre::Vector3& chassisPos, const Ogre::Quaternion& rot)
{
    for(int q = 0; q < InitialVehicleSetup::mWheelsAmount; ++q)
    {
        mWheelNodes[q]->setPosition(chassisPos + rot * mInitialVehicleSetup.mConnectionPointWheel[q]);
        mWheelNodes[q]->setOrientation(rot);
        if(mWheel[q].get())
        {
            mWheel[q]->getWorldTransform().setOrigin(PhysicsTools::convert(mWheelNodes[q]->getPosition()));
            mWheel[q]->getWorldTransform().setRotation(PhysicsTools::convert(rot));
        }
    }

}

void PhysicsWheels::calcImpulses(const Ogre::Vector3& impulseRot, const Ogre::Vector3& impulseRotPrev, const Ogre::Vector3& normalisedImpulseRot,
                    const Ogre::Vector3& impulseLinear,
                    Ogre::Real recipMomentProj,
                    const Ogre::Quaternion& carRot,
                    const PhysicsVehicle& vehicle)
{
}

void PhysicsWheels::process(const Ogre::SceneNode& chassis, PhysicsVehicle& vehicle)
{
}