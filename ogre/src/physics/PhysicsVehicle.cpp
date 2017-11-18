
#include "PhysicsVehicle.h"

#include "BulletCollision/CollisionShapes/btSphereShape.h"
#include "BulletCollision/CollisionDispatch/btCollisionObject.h"

#include "Physics.h"
#include "../tools/PhysicsTools.h"

PhysicsVehicle::PhysicsVehicle(Physics* physics, const InitialVehicleSetup& initialVehicleSetup, Ogre::SceneNode *wheelNodes[mWheelsAmount], Ogre::SceneNode *chassis)
: mPhysics(physics),
    mChassis(chassis)
{
    for(int q = 0; q < mWheelsAmount; ++q)
    {
        mWheelNodes[q] = wheelNodes[q];
    }

    mWheelShapeFL = CommonIncludes::shared_ptr<btSphereShape>(new btSphereShape(initialVehicleSetup.mWheelRadiusFront));
    mWheelShapeFR = CommonIncludes::shared_ptr<btSphereShape>(new btSphereShape(initialVehicleSetup.mWheelRadiusFront));
    mWheelShapeRL = CommonIncludes::shared_ptr<btSphereShape>(new btSphereShape(initialVehicleSetup.mWheelRadiusBack));
    mWheelShapeRR = CommonIncludes::shared_ptr<btSphereShape>(new btSphereShape(initialVehicleSetup.mWheelRadiusBack));

    mWheelFL = CommonIncludes::shared_ptr<btCollisionObject>(new btCollisionObject());
    mWheelFR = CommonIncludes::shared_ptr<btCollisionObject>(new btCollisionObject());
    mWheelRL = CommonIncludes::shared_ptr<btCollisionObject>(new btCollisionObject());
    mWheelRR = CommonIncludes::shared_ptr<btCollisionObject>(new btCollisionObject());

    mWheelFL->setCollisionShape(mWheelShapeFL.get());
    mWheelFL->getWorldTransform().setOrigin(PhysicsTools::convert(initialVehicleSetup.mChassisPos + initialVehicleSetup.mChassisRot * initialVehicleSetup.mConnectionPointFLWheel));
    mWheelFL->getWorldTransform().setRotation(PhysicsTools::convert(initialVehicleSetup.mChassisRot));

    mWheelFR->setCollisionShape(mWheelShapeFR.get());
    mWheelFR->getWorldTransform().setOrigin(PhysicsTools::convert(initialVehicleSetup.mChassisPos + initialVehicleSetup.mChassisRot * initialVehicleSetup.mConnectionPointFRWheel));
    mWheelFR->getWorldTransform().setRotation(PhysicsTools::convert(initialVehicleSetup.mChassisRot));

    mWheelRL->setCollisionShape(mWheelShapeRL.get());
    mWheelRL->getWorldTransform().setOrigin(PhysicsTools::convert(initialVehicleSetup.mChassisPos + initialVehicleSetup.mChassisRot * initialVehicleSetup.mConnectionPointRLWheel));
    mWheelRL->getWorldTransform().setRotation(PhysicsTools::convert(initialVehicleSetup.mChassisRot));

    mWheelRR->setCollisionShape(mWheelShapeRR.get());
    mWheelRR->getWorldTransform().setOrigin(PhysicsTools::convert(initialVehicleSetup.mChassisPos + initialVehicleSetup.mChassisRot * initialVehicleSetup.mConnectionPointRRWheel));
    mWheelRR->getWorldTransform().setRotation(PhysicsTools::convert(initialVehicleSetup.mChassisRot));

    mPhysics->addCollisionObject(mWheelFL.get());
    mPhysics->addCollisionObject(mWheelFR.get());
    mPhysics->addCollisionObject(mWheelRL.get());
    mPhysics->addCollisionObject(mWheelRR.get());
}

PhysicsVehicle::~PhysicsVehicle()
{
    mPhysics->removeCollisionObject(mWheelFL.get());
    mPhysics->removeCollisionObject(mWheelFR.get());
    mPhysics->removeCollisionObject(mWheelRL.get());
    mPhysics->removeCollisionObject(mWheelRR.get());
}

void PhysicsVehicle::shiftPos(const Ogre::Vector3& shiftAmount)
{
    for(int q = 0; q < mWheelsAmount; ++q)
    {
        mWheelNodes[q]->translate(shiftAmount);
    }

    mChassis->translate(shiftAmount);

    mWheelFL->getWorldTransform().setOrigin(mWheelFL->getWorldTransform().getOrigin() + PhysicsTools::convert(shiftAmount));
    mWheelFR->getWorldTransform().setOrigin(mWheelFR->getWorldTransform().getOrigin() + PhysicsTools::convert(shiftAmount));
    mWheelRL->getWorldTransform().setOrigin(mWheelRL->getWorldTransform().getOrigin() + PhysicsTools::convert(shiftAmount));
    mWheelRR->getWorldTransform().setOrigin(mWheelRR->getWorldTransform().getOrigin() + PhysicsTools::convert(shiftAmount));
}