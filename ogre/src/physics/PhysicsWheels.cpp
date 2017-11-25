
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

void PhysicsWheels::init(const Ogre::Vector3& chassisPos, Ogre::SceneNode *wheelNodes[InitialVehicleSetup::mWheelsAmount])
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

    for(int q = 0; q < InitialVehicleSetup::mRoofsAmount; ++q)
    {
        mWheelsSuspensionGlobal[q] = chassisPos;
        mWheelsSuspensionGlobalPrev[q] = chassisPos;
        mSuspensionHeight[q] = mInitialVehicleSetup.mSuspensionDataWheel[q].x;
        mSuspensionHeightPrev[q] = mSuspensionHeight[q];
        mSteering[q] = mInitialVehicleSetup.mSuspensionDataWheel[q].y;
        mVelocity[q] = mInitialVehicleSetup.mSuspensionDataWheel[q].z;
    }
}

void PhysicsWheels::initStep(const Ogre::Vector3& chassisPos, const Ogre::Quaternion& chassisRot)
{
    Ogre::Matrix3 rotMatrix;
    chassisRot.ToRotationMatrix(rotMatrix);
    Ogre::Vector3 rotAxisY = rotMatrix.GetColumn(1);

    for(int q = 0; q < InitialVehicleSetup::mRoofsAmount; ++q)
    {
        mGlobalPos[q] = chassisPos + chassisRot * (mInitialVehicleSetup.mConnectionPointWheel[q] + mInitialVehicleSetup.mCOG);

        Ogre::Real maxTravel = mInitialVehicleSetup.mMaxTravel * 0.5f;
        Ogre::Vector3 rotVal = maxTravel * rotAxisY;

        mWheelsSuspensionPointGlobal[q] = mGlobalPos[q] - rotVal;
        mWheelsSuspensionPoint2Global[q] = mWheelsSuspensionPointGlobal[q] - rotVal;
        mWheelsSuspensionPoint[q] = mWheelsSuspensionPoint2Global[q] - chassisPos;

        mWheelsSuspensionGlobalPrev[q] = mWheelsSuspensionGlobal[q];
        mSuspensionHeightPrev[q] = mSuspensionHeight[q];
    }

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
    if(impulseRot.length() == 0.0f)
    {
        for(int q = 0; q < InitialVehicleSetup::mRoofsAmount; ++q)
        {
            mWheelsImpulseLinear[q] = impulseLinear;
        }
    }
    else
    {
        Ogre::Real force = mInitialVehicleSetup.mChassisMass * recipMomentProj;

        for(int q = 0; q < InitialVehicleSetup::mRoofsAmount; ++q)
        {
            Ogre::Vector3 roofRot = carRot * mInitialVehicleSetup.mRoofPos[q];
            Ogre::Vector3 tangent = vehicle.findTangent(normalisedImpulseRot, roofRot);
            if(tangent.x != 0.0f || tangent.y != 0.0f || tangent.z != 0.0f)
            {
                mWheelsImpulseLinear[q] = tangent.crossProduct(impulseRotPrev) * force + impulseLinear;
            }
            else
            {
                mWheelsImpulseLinear[q] = impulseLinear;
            }
        }
    }
}

void PhysicsWheels::process(const Ogre::SceneNode& chassis, PhysicsVehicle& vehicle)
{
}