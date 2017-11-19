
#include "PhysicsVehicle.h"

#include "BulletCollision/CollisionShapes/btSphereShape.h"
#include "BulletCollision/CollisionDispatch/btCollisionObject.h"

#include "Physics.h"
#include "../tools/PhysicsTools.h"

PhysicsVehicle::PhysicsVehicle(Physics* physics, const InitialVehicleSetup& initialVehicleSetup, Ogre::SceneNode *wheelNodes[mWheelsAmount], Ogre::SceneNode *chassis)
: mPhysics(physics),
    mChassis(chassis),
    mInitialVehicleSetup(initialVehicleSetup)
{
    for(int q = 0; q < mWheelsAmount; ++q)
    {
        mWheelNodes[q] = wheelNodes[q];
    }

    //wheels
    mWheelShapeFL = CommonIncludes::shared_ptr<btSphereShape>(new btSphereShape(mInitialVehicleSetup.mWheelRadiusFront));
    mWheelShapeFR = CommonIncludes::shared_ptr<btSphereShape>(new btSphereShape(mInitialVehicleSetup.mWheelRadiusFront));
    mWheelShapeRL = CommonIncludes::shared_ptr<btSphereShape>(new btSphereShape(mInitialVehicleSetup.mWheelRadiusBack));
    mWheelShapeRR = CommonIncludes::shared_ptr<btSphereShape>(new btSphereShape(mInitialVehicleSetup.mWheelRadiusBack));

    mWheelFL = CommonIncludes::shared_ptr<btCollisionObject>(new btCollisionObject());
    mWheelFR = CommonIncludes::shared_ptr<btCollisionObject>(new btCollisionObject());
    mWheelRL = CommonIncludes::shared_ptr<btCollisionObject>(new btCollisionObject());
    mWheelRR = CommonIncludes::shared_ptr<btCollisionObject>(new btCollisionObject());

    mWheelFL->setCollisionShape(mWheelShapeFL.get());
    mWheelFL->getWorldTransform().setOrigin(PhysicsTools::convert(mInitialVehicleSetup.mChassisPos + mInitialVehicleSetup.mChassisRot * mInitialVehicleSetup.mConnectionPointFLWheel));
    mWheelFL->getWorldTransform().setRotation(PhysicsTools::convert(mInitialVehicleSetup.mChassisRot));

    mWheelFR->setCollisionShape(mWheelShapeFR.get());
    mWheelFR->getWorldTransform().setOrigin(PhysicsTools::convert(mInitialVehicleSetup.mChassisPos + mInitialVehicleSetup.mChassisRot * mInitialVehicleSetup.mConnectionPointFRWheel));
    mWheelFR->getWorldTransform().setRotation(PhysicsTools::convert(mInitialVehicleSetup.mChassisRot));

    mWheelRL->setCollisionShape(mWheelShapeRL.get());
    mWheelRL->getWorldTransform().setOrigin(PhysicsTools::convert(mInitialVehicleSetup.mChassisPos + mInitialVehicleSetup.mChassisRot * mInitialVehicleSetup.mConnectionPointRLWheel));
    mWheelRL->getWorldTransform().setRotation(PhysicsTools::convert(mInitialVehicleSetup.mChassisRot));

    mWheelRR->setCollisionShape(mWheelShapeRR.get());
    mWheelRR->getWorldTransform().setOrigin(PhysicsTools::convert(mInitialVehicleSetup.mChassisPos + mInitialVehicleSetup.mChassisRot * mInitialVehicleSetup.mConnectionPointRRWheel));
    mWheelRR->getWorldTransform().setRotation(PhysicsTools::convert(mInitialVehicleSetup.mChassisRot));

    //mPhysics->addCollisionObject(mWheelFL.get());
    //mPhysics->addCollisionObject(mWheelFR.get());
    //mPhysics->addCollisionObject(mWheelRL.get());
    //mPhysics->addCollisionObject(mWheelRR.get());
    //wheels END

    //body
    mBodyShape = CommonIncludes::shared_ptr<btSphereShape>(new btSphereShape(mInitialVehicleSetup.mBodyRadius));
    mBody = CommonIncludes::shared_ptr<btCollisionObject>(new btCollisionObject());

    mBody->setCollisionShape(mBodyShape.get());
    mBody->getWorldTransform().setOrigin(PhysicsTools::convert(mInitialVehicleSetup.mChassisPos + mInitialVehicleSetup.mChassisRot * mInitialVehicleSetup.mBodyBasePos));
    mBody->getWorldTransform().setRotation(PhysicsTools::convert(mInitialVehicleSetup.mChassisRot));

    mPhysics->addCollisionObject(mBody.get());
    //body END

    mImpulseLinear = mInitialVehicleSetup.mInitialImpulseLinear;
    mImpulseLinearInc = mInitialVehicleSetup.mInitialImpulseLinearInc;
    mImpulseRot = mInitialVehicleSetup.mInitialImpulseRot;
    mImpulseRotInc = mInitialVehicleSetup.mInitialImpulseRotInc;
}

PhysicsVehicle::~PhysicsVehicle()
{
    //mPhysics->removeCollisionObject(mWheelFL.get());
    //mPhysics->removeCollisionObject(mWheelFR.get());
    //mPhysics->removeCollisionObject(mWheelRL.get());
    //mPhysics->removeCollisionObject(mWheelRR.get());

    mPhysics->removeCollisionObject(mBody.get());
}

void PhysicsVehicle::timeStep()
{

    integrateLinear();
    integrateRot();

    Ogre::Vector3 posDiff = mImpulseLinear * mInitialVehicleSetup.mChassisInvMass;
    reposition(posDiff);

    Ogre::Real linearImpulseMod = mImpulseLinear.length();

    mVehicleVelocityMod = linearImpulseMod * mInitialVehicleSetup.mChassisInvMass;

    if(mVehicleVelocityMod < 0.0001f) mVehicleVelocityMod = 0.0001f;

    Ogre::Real airDensTransCoeff = -1.0f * mInitialVehicleSetup.mAirDensityTranslation * linearImpulseMod + 1.0f;
    mImpulseLinear *= airDensTransCoeff;

    Ogre::Real rotImpulseMod = mImpulseRot.length();
    if(rotImpulseMod > 0.00001f)
    {
        //Ogre::Vector3 normalisedImpulseRot = mImpulseRot.normalisedCopy();
        //Ogre::Real momentProj = momentOfInertiaProj(normalisedImpulseRot);
        mImpulseRot *= mInitialVehicleSetup.mAirDensityRot;
    }


    mImpulseLinearInc.y += mInitialVehicleSetup.mChassisMass * (-mInitialVehicleSetup.mGravityForce);

    integrateLinear();
    integrateRot();

    //do stuff

    Ogre::Vector3 worldNormal;
    Ogre::Real distance = 0.0f;
    if(mPhysics->findCollision(mBody.get(), worldNormal, distance))
    {
    }

    //mImpulseLinearInc.y -= mInitialVehicleSetup.mChassisMass * (-mInitialVehicleSetup.mGravityForce);
    //mImpulseLinearInc.x += mInitialVehicleSetup.mChassisMass * mInitialVehicleSetup.mGravityForce;
    //mImpulseLinearInc.y += mInitialVehicleSetup.mChassisMass * mInitialVehicleSetup.mGravityForce;
    //mImpulseLinearInc.z += mInitialVehicleSetup.mChassisMass * mInitialVehicleSetup.mGravityForce;

}

void PhysicsVehicle::reposition(const Ogre::Vector3& posDiff)
{
    for(int q = 0; q < mWheelsAmount; ++q)
    {
        mWheelNodes[q]->translate(posDiff);
    }

    mChassis->translate(posDiff);

    mWheelFL->getWorldTransform().setOrigin(mWheelFL->getWorldTransform().getOrigin() + PhysicsTools::convert(posDiff));
    mWheelFR->getWorldTransform().setOrigin(mWheelFR->getWorldTransform().getOrigin() + PhysicsTools::convert(posDiff));
    mWheelRL->getWorldTransform().setOrigin(mWheelRL->getWorldTransform().getOrigin() + PhysicsTools::convert(posDiff));
    mWheelRR->getWorldTransform().setOrigin(mWheelRR->getWorldTransform().getOrigin() + PhysicsTools::convert(posDiff));

    mBody->getWorldTransform().setOrigin(mBody->getWorldTransform().getOrigin() + PhysicsTools::convert(posDiff));
}

void PhysicsVehicle::integrateLinear()
{
    mImpulseLinear += mImpulseLinearInc;
    mImpulseLinearInc = Ogre::Vector3::ZERO;
}

void PhysicsVehicle::integrateRot()
{
    mImpulseRot += mImpulseRotInc;
    mImpulseRotInc = Ogre::Vector3::ZERO;
}

Ogre::Real PhysicsVehicle::momentOfInertiaProj(const Ogre::Vector3& axis)const
{
    Ogre::Real ret = 0.0f;

    Ogre::Matrix3 carRot;
    mChassis->getOrientation().ToRotationMatrix(carRot);
    Ogre::Vector3 tmpRes = carRot * axis;

    ret = mInitialVehicleSetup.mMomentOfInertia.dotProduct(Ogre::Vector3(tmpRes * tmpRes));

    return ret;
}