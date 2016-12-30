#include "../pcheader.h"

#include "PSBaseVehicle.h"

#include "../customs/CustomRigidBodyWheel.h"

Ogre::NameGenerator PSBaseVehicle::nameGenNodes("Scene/Node/Vehicle/Name");

void PSBaseVehicle::initPhysicalModel(OgreBulletDynamics::DynamicsWorld * world, Ogre::SceneNode* modelNode, Ogre::SceneNode *wheelNodes[4], const InitialVehicleSetup& initialVehicleSetup)
{

    mInitialVehicleSetup = initialVehicleSetup;

    OgreBulletCollisions::CompoundCollisionShape* compound = new OgreBulletCollisions::CompoundCollisionShape();

    {
        OgreBulletCollisions::SphereCollisionShape* roofBackR = new OgreBulletCollisions::SphereCollisionShape(initialVehicleSetup.mRoofBackRadius);
        OgreBulletCollisions::SphereCollisionShape* roofBackL = new OgreBulletCollisions::SphereCollisionShape(initialVehicleSetup.mRoofBackRadius);
        compound->addChildShape(roofBackR, initialVehicleSetup.mRoofBackPos);
        compound->addChildShape(roofBackL, Ogre::Vector3(-initialVehicleSetup.mRoofBackPos.x, initialVehicleSetup.mRoofBackPos.y, initialVehicleSetup.mRoofBackPos.z));
        
        OgreBulletCollisions::SphereCollisionShape* roofFrontR = new OgreBulletCollisions::SphereCollisionShape(initialVehicleSetup.mRoofFrontRadius);
        OgreBulletCollisions::SphereCollisionShape* roofFrontL = new OgreBulletCollisions::SphereCollisionShape(initialVehicleSetup.mRoofFrontRadius);
        compound->addChildShape(roofFrontR, initialVehicleSetup.mRoofFrontPos);
        compound->addChildShape(roofFrontL, Ogre::Vector3(-initialVehicleSetup.mRoofFrontPos.x, initialVehicleSetup.mRoofFrontPos.y, initialVehicleSetup.mRoofFrontPos.z));

        OgreBulletCollisions::SphereCollisionShape* chassisBody = new OgreBulletCollisions::SphereCollisionShape(initialVehicleSetup.mBodyRadius);
        compound->addChildShape(chassisBody, initialVehicleSetup.mBodyBasePos);
    }


    mCarChassis = new OgreBulletDynamics::RigidBody(nameGenNodes.generate(), world);
    //mCarChassis = new OgreBulletDynamics::RigidBody("carChassis", world, 0, 1);//no collision with static mesh

    mCarChassis->setShape ( modelNode, 
                            compound, 
                            initialVehicleSetup.mChassisRestitution, 
                            initialVehicleSetup.mChassisFriction, 
                            1.0f,
                            Ogre::Vector3::UNIT_SCALE,
                            initialVehicleSetup.mChassisMass,
                            initialVehicleSetup.mChassisPos, 
                            initialVehicleSetup.mChassisRot);

    mCarChassis->getBulletRigidBody()->setCollisionFlags(mCarChassis->getBulletRigidBody()->getCollisionFlags() | btCollisionObject::CF_CUSTOM_MATERIAL_CALLBACK);

    mCarChassis->setDamping(initialVehicleSetup.mAirDensityLinear, initialVehicleSetup.mAirDensityAngular);

    OgreBulletCollisions::SphereCollisionShape* wheelShapeFront = new OgreBulletCollisions::SphereCollisionShape(initialVehicleSetup.mWheelRadius.x);
    OgreBulletCollisions::SphereCollisionShape* wheelShapeBack = new OgreBulletCollisions::SphereCollisionShape(initialVehicleSetup.mWheelRadius.y);

    mCarWheelFrontL = new CustomRigidBodyWheel(nameGenNodes.generate(), world, modelNode, initialVehicleSetup.mConnectionPointFLWheel, true, 1, 2);// don`t collide with other car
    mCarWheelFrontR = new CustomRigidBodyWheel(nameGenNodes.generate(), world, modelNode, initialVehicleSetup.mConnectionPointFRWheel, true, 1, 2);// don`t collide with other car
    mCarWheelBackL = new CustomRigidBodyWheel(nameGenNodes.generate(), world, modelNode, initialVehicleSetup.mConnectionPointRLWheel, false, 1, 2);// don`t collide with other car
    mCarWheelBackR = new CustomRigidBodyWheel(nameGenNodes.generate(), world, modelNode, initialVehicleSetup.mConnectionPointRRWheel, false, 1, 2);// don`t collide with other car



    mCarWheelFrontL->setShape ( wheelNodes[3], 
                                wheelShapeFront, 
                                initialVehicleSetup.mWheelsRestitution, 
                                initialVehicleSetup.mWheelsFriction, 
                                initialVehicleSetup.mRollingFriction,
                                initialVehicleSetup.mAnisotropicFriction,
                                initialVehicleSetup.mWheelsFMass, 
                                initialVehicleSetup.mChassisPos + initialVehicleSetup.mChassisRot * initialVehicleSetup.mConnectionPointFLWheel, 
                                initialVehicleSetup.mChassisRot);

    mCarWheelFrontR->setShape ( wheelNodes[2], 
                                wheelShapeFront, 
                                initialVehicleSetup.mWheelsRestitution, 
                                initialVehicleSetup.mWheelsFriction, 
                                initialVehicleSetup.mRollingFriction,
                                initialVehicleSetup.mAnisotropicFriction,
                                initialVehicleSetup.mWheelsFMass, 
                                initialVehicleSetup.mChassisPos + initialVehicleSetup.mChassisRot * initialVehicleSetup.mConnectionPointFRWheel, 
                                initialVehicleSetup.mChassisRot);

    mCarWheelBackL->setShape (  wheelNodes[1], 
                                wheelShapeBack, 
                                initialVehicleSetup.mWheelsRestitution, 
                                initialVehicleSetup.mWheelsFriction, 
                                initialVehicleSetup.mRollingFriction,
                                initialVehicleSetup.mAnisotropicFriction,
                                initialVehicleSetup.mWheelsRMass, 
                                initialVehicleSetup.mChassisPos + initialVehicleSetup.mChassisRot * initialVehicleSetup.mConnectionPointRLWheel, 
                                initialVehicleSetup.mChassisRot);

    mCarWheelBackR->setShape (  wheelNodes[0], 
                                wheelShapeBack, 
                                initialVehicleSetup.mWheelsRestitution, 
                                initialVehicleSetup.mWheelsFriction, 
                                initialVehicleSetup.mRollingFriction,
                                initialVehicleSetup.mAnisotropicFriction,
                                initialVehicleSetup.mWheelsRMass, 
                                initialVehicleSetup.mChassisPos + initialVehicleSetup.mChassisRot * initialVehicleSetup.mConnectionPointRRWheel, 
                                initialVehicleSetup.mChassisRot);

    mCarWheelFrontL->setDamping(initialVehicleSetup.mAirDensityLinear, initialVehicleSetup.mAirDensityAngular);
    mCarWheelFrontR->setDamping(initialVehicleSetup.mAirDensityLinear, initialVehicleSetup.mAirDensityAngular);
    mCarWheelBackL->setDamping(initialVehicleSetup.mAirDensityLinear, initialVehicleSetup.mAirDensityAngular);
    mCarWheelBackR->setDamping(initialVehicleSetup.mAirDensityLinear, initialVehicleSetup.mAirDensityAngular);


    mCarChassis->disableDeactivation ();
    mCarWheelFrontL->disableDeactivation ();
    mCarWheelFrontR->disableDeactivation ();
    mCarWheelBackL->disableDeactivation ();
    mCarWheelBackR->disableDeactivation ();

    {
        //http://bulletphysics.org/mediawiki-1.5.8/index.php/Constraints
        //http://bulletphysics.org/Bullet/phpBB3/viewtopic.php?t=8284
        //http://bulletphysics.org/Bullet/phpBB3/viewtopic.php?t=6662
        //http://bulletphysics.org/Bullet/phpBB3/viewtopic.php?f=9&t=4785&view=previous

        mSixDofSpringFrontL = new OgreBulletDynamics::SixDofSpring2Constraint(mCarChassis, mCarWheelFrontL, initialVehicleSetup.mConnectionPointFLWheel, Ogre::Quaternion::IDENTITY, Ogre::Vector3::ZERO, Ogre::Quaternion::IDENTITY);
        mSixDofSpringFrontL->setLinearLowerLimit(Ogre::Vector3(0.0f, initialVehicleSetup.mMaxTravel, 0.0f));
        mSixDofSpringFrontL->setLinearUpperLimit(Ogre::Vector3(0.0f, 0.0f, 0.0f));
        mSixDofSpringFrontL->setAngularLowerLimit(Ogre::Vector3(1.0f, 0.0f, 0.0f));
        mSixDofSpringFrontL->setAngularUpperLimit(Ogre::Vector3(0.0f, 0.0f, 0.0f));
        mSixDofSpringFrontL->enableSpring(1, true);
        mSixDofSpringFrontL->setStiffness(1, initialVehicleSetup.mWheelsFSpringStiffness, initialVehicleSetup.mLimitSpringParamsF);
        mSixDofSpringFrontL->setDamping(1, initialVehicleSetup.mWheelsFSpringDamping, initialVehicleSetup.mLimitSpringParamsF);
        mSixDofSpringFrontL->setEquilibriumPoint(1, initialVehicleSetup.mMaxTravel);
        world->addConstraint(mSixDofSpringFrontL, true);
    }

    {
        mSixDofSpringFrontR = new OgreBulletDynamics::SixDofSpring2Constraint(mCarChassis, mCarWheelFrontR, initialVehicleSetup.mConnectionPointFRWheel, Ogre::Quaternion::IDENTITY, Ogre::Vector3::ZERO, Ogre::Quaternion::IDENTITY);
        mSixDofSpringFrontR->setLinearLowerLimit(Ogre::Vector3(0.0f, initialVehicleSetup.mMaxTravel, 0.0f));
        mSixDofSpringFrontR->setLinearUpperLimit(Ogre::Vector3(0.0f, 0.0f, 0.0f));
        mSixDofSpringFrontR->setAngularLowerLimit(Ogre::Vector3(1.0f, 0.0f, 0.0f));
        mSixDofSpringFrontR->setAngularUpperLimit(Ogre::Vector3(0.0f, 0.0f, 0.0f));
        mSixDofSpringFrontR->enableSpring(1, true);
        mSixDofSpringFrontR->setStiffness(1, initialVehicleSetup.mWheelsFSpringStiffness, initialVehicleSetup.mLimitSpringParamsF);
        mSixDofSpringFrontR->setDamping(1, initialVehicleSetup.mWheelsFSpringDamping, initialVehicleSetup.mLimitSpringParamsF);
        mSixDofSpringFrontR->setEquilibriumPoint(1, initialVehicleSetup.mMaxTravel);
        world->addConstraint(mSixDofSpringFrontR, true);
    }

    {
        mSixDofSpringBackL = new OgreBulletDynamics::SixDofSpring2Constraint(mCarChassis, mCarWheelBackL, initialVehicleSetup.mConnectionPointRLWheel, Ogre::Quaternion::IDENTITY, Ogre::Vector3::ZERO, Ogre::Quaternion::IDENTITY);
        mSixDofSpringBackL->setLinearLowerLimit(Ogre::Vector3(0.0f, initialVehicleSetup.mMaxTravel, 0.0f));
        mSixDofSpringBackL->setLinearUpperLimit(Ogre::Vector3(0.0f, 0.0f, 0.0f));
        mSixDofSpringBackL->setAngularLowerLimit(Ogre::Vector3(1.0f, 0.0f, 0.0f));
        mSixDofSpringBackL->setAngularUpperLimit(Ogre::Vector3(0.0f, 0.0f, 0.0f));
        mSixDofSpringBackL->enableSpring(1, true);
        mSixDofSpringBackL->setStiffness(1, initialVehicleSetup.mWheelsRSpringStiffness, initialVehicleSetup.mLimitSpringParamsR);
        mSixDofSpringBackL->setDamping(1, initialVehicleSetup.mWheelsRSpringDamping, initialVehicleSetup.mLimitSpringParamsR);
        mSixDofSpringBackL->setEquilibriumPoint(1, initialVehicleSetup.mMaxTravel);
        world->addConstraint(mSixDofSpringBackL, true);
    }

    {
        mSixDofSpringBackR = new OgreBulletDynamics::SixDofSpring2Constraint(mCarChassis, mCarWheelBackR, initialVehicleSetup.mConnectionPointRRWheel, Ogre::Quaternion::IDENTITY, Ogre::Vector3::ZERO, Ogre::Quaternion::IDENTITY);
        mSixDofSpringBackR->setLinearLowerLimit(Ogre::Vector3(0.0f, initialVehicleSetup.mMaxTravel, 0.0f));
        mSixDofSpringBackR->setLinearUpperLimit(Ogre::Vector3(0.0f, 0.0f, 0.0f));
        mSixDofSpringBackR->setAngularLowerLimit(Ogre::Vector3(1.0f, 0.0f, 0.0f));
        mSixDofSpringBackR->setAngularUpperLimit(Ogre::Vector3(0.0f, 0.0f, 0.0f));
        mSixDofSpringBackR->enableSpring(1, true);
        mSixDofSpringBackR->setStiffness(1, initialVehicleSetup.mWheelsRSpringStiffness, initialVehicleSetup.mLimitSpringParamsR);
        mSixDofSpringBackR->setDamping(1, initialVehicleSetup.mWheelsRSpringDamping, initialVehicleSetup.mLimitSpringParamsR);
        mSixDofSpringBackR->setEquilibriumPoint(1, initialVehicleSetup.mMaxTravel);
        world->addConstraint(mSixDofSpringBackR, true);
    }
}

void PSBaseVehicle::clear()
{
}


void PSBaseVehicle::removeFromWorld()
{
}

void PSBaseVehicle::addToWorld()
{
}