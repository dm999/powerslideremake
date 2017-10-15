
#include "PSBaseVehicle.h"

#include "../customs/CustomRigidBody.h"
#include "../customs/CustomRigidBodyWheel.h"

Ogre::NameGenerator PSBaseVehicle::nameGenNodes("Scene/Node/Vehicle/Name");

void PSBaseVehicle::initPhysicalModel(OgreBulletDynamics::DynamicsWorld * world, 
    Ogre::SceneNode* modelNode, Ogre::SceneNode *wheelNodes[4], 
    const InitialVehicleSetup& initialVehicleSetup)
{
    mWorld = world;

    mInitialVehicleSetup = initialVehicleSetup;

    addToWorld(modelNode, wheelNodes);
}

void PSBaseVehicle::repositionVehicle(const Ogre::Vector3& chassisPos, const Ogre::Quaternion& chassisRot, Ogre::SceneNode* modelNode, Ogre::SceneNode *wheelNodes[4])
{
    //http://www.bulletphysics.org/Bullet/phpBB3/viewtopic.php?f=9&t=4517

    removeFromWorld();

    mInitialVehicleSetup.mChassisPos = chassisPos;
    mInitialVehicleSetup.mChassisRot = chassisRot;

    addToWorld(modelNode, wheelNodes);
}

void PSBaseVehicle::clear()
{
    removeFromWorld();
}


void PSBaseVehicle::removeFromWorld()
{
    if(mSixDofSpringFrontL.get())
        mWorld->removeConstraint(mSixDofSpringFrontL.get());

    if(mSixDofSpringFrontR.get())
        mWorld->removeConstraint(mSixDofSpringFrontR.get());

    if(mSixDofSpringBackL.get())
        mWorld->removeConstraint(mSixDofSpringBackL.get());

    if(mSixDofSpringBackR.get())
        mWorld->removeConstraint(mSixDofSpringBackR.get());

    if(mCarChassis.get())
        mWorld->removeRigidBody(mCarChassis.get());

    if(mCarWheelBackR.get())
        mWorld->removeRigidBody(mCarWheelBackR.get());

    if(mCarWheelBackL.get())
        mWorld->removeRigidBody(mCarWheelBackL.get());

    if(mCarWheelFrontR.get())
        mWorld->removeRigidBody(mCarWheelFrontR.get());

    if(mCarWheelFrontL.get())
        mWorld->removeRigidBody(mCarWheelFrontL.get());

    mSixDofSpringFrontL.reset();
    mSixDofSpringFrontR.reset();
    mSixDofSpringBackL.reset();
    mSixDofSpringBackR.reset();

    mCarWheelFrontL.reset();
    mCarWheelFrontR.reset();
    mCarWheelBackL.reset();
    mCarWheelBackR.reset();

    mCarChassis.reset();

    mRoofBackRShape.reset();
    mRoofBackLShape.reset();
    mRoofFrontRShape.reset();
    mRoofFrontLShape.reset();
    mChassisBodyShape.reset();
    mCompoundShape.reset();

    mWheelShapeFront.reset();
    mWheelShapeBack.reset();

}

void PSBaseVehicle::addToWorld(Ogre::SceneNode* modelNode, Ogre::SceneNode *wheelNodes[4])
{
    addRigidsToWorld(modelNode, wheelNodes);
    addSpringsToWorld();
}


void PSBaseVehicle::addRigidsToWorld(Ogre::SceneNode* modelNode, Ogre::SceneNode *wheelNodes[4])
{

    mCompoundShape.reset(new OgreBulletCollisions::CompoundCollisionShape());

    {
        mRoofBackRShape.reset(new OgreBulletCollisions::SphereCollisionShape(mInitialVehicleSetup.mRoofBackRadius));
        mRoofBackLShape.reset(new OgreBulletCollisions::SphereCollisionShape(mInitialVehicleSetup.mRoofBackRadius));
        mCompoundShape->addChildShape(mRoofBackRShape.get(), mInitialVehicleSetup.mRoofBackPos);
        mCompoundShape->addChildShape(mRoofBackLShape.get(), Ogre::Vector3(-mInitialVehicleSetup.mRoofBackPos.x, mInitialVehicleSetup.mRoofBackPos.y, mInitialVehicleSetup.mRoofBackPos.z));
        
        mRoofFrontRShape.reset(new OgreBulletCollisions::SphereCollisionShape(mInitialVehicleSetup.mRoofFrontRadius));
        mRoofFrontLShape.reset(new OgreBulletCollisions::SphereCollisionShape(mInitialVehicleSetup.mRoofFrontRadius));
        mCompoundShape->addChildShape(mRoofFrontRShape.get(), mInitialVehicleSetup.mRoofFrontPos);
        mCompoundShape->addChildShape(mRoofFrontLShape.get(), Ogre::Vector3(-mInitialVehicleSetup.mRoofFrontPos.x, mInitialVehicleSetup.mRoofFrontPos.y, mInitialVehicleSetup.mRoofFrontPos.z));

        mChassisBodyShape.reset(new OgreBulletCollisions::SphereCollisionShape(mInitialVehicleSetup.mBodyRadius));
        mCompoundShape->addChildShape(mChassisBodyShape.get(), mInitialVehicleSetup.mBodyBasePos);
    }


    mCarChassis.reset(new CustomRigidBody(nameGenNodes.generate(), mWorld, mInitialVehicleSetup.mInitialImpulseLinear));
    //mCarChassis = new OgreBulletDynamics::RigidBody("carChassis", mWorld, 0, 1);//no collision with static mesh

    mCarChassis->setShape ( modelNode, 
                            mCompoundShape.get(), 
                            mInitialVehicleSetup.mChassisRestitution, 
                            mInitialVehicleSetup.mChassisFriction, 
                            1.0f,
                            Ogre::Vector3::UNIT_SCALE,
                            mInitialVehicleSetup.mChassisMass,
                            mInitialVehicleSetup.mChassisPos, 
                            mInitialVehicleSetup.mChassisRot);

    mCarChassis->getBulletRigidBody()->setCollisionFlags(mCarChassis->getBulletRigidBody()->getCollisionFlags() | btCollisionObject::CF_CUSTOM_MATERIAL_CALLBACK);

    mCarChassis->setDamping(mInitialVehicleSetup.mAirDensityLinear, mInitialVehicleSetup.mAirDensityAngular);

    mWheelShapeFront.reset(new OgreBulletCollisions::SphereCollisionShape(mInitialVehicleSetup.mWheelRadius.x));
    mWheelShapeBack.reset(new OgreBulletCollisions::SphereCollisionShape(mInitialVehicleSetup.mWheelRadius.y));

    mCarWheelFrontL.reset(new CustomRigidBodyWheel(nameGenNodes.generate(), mWorld, modelNode, mInitialVehicleSetup.mConnectionPointFLWheel, true, 1, 2));// don`t collide with other car
    mCarWheelFrontR.reset(new CustomRigidBodyWheel(nameGenNodes.generate(), mWorld, modelNode, mInitialVehicleSetup.mConnectionPointFRWheel, true, 1, 2));// don`t collide with other car
    mCarWheelBackL.reset(new CustomRigidBodyWheel(nameGenNodes.generate(), mWorld, modelNode, mInitialVehicleSetup.mConnectionPointRLWheel, false, 1, 2));// don`t collide with other car
    mCarWheelBackR.reset(new CustomRigidBodyWheel(nameGenNodes.generate(), mWorld, modelNode, mInitialVehicleSetup.mConnectionPointRRWheel, false, 1, 2));// don`t collide with other car



    mCarWheelFrontL->setShape ( wheelNodes[3], 
                                mWheelShapeFront.get(), 
                                mInitialVehicleSetup.mWheelsRestitution, 
                                mInitialVehicleSetup.mWheelsFriction, 
                                mInitialVehicleSetup.mRollingFriction,
                                mInitialVehicleSetup.mAnisotropicFriction,
                                mInitialVehicleSetup.mWheelsFMass, 
                                mInitialVehicleSetup.mChassisPos + mInitialVehicleSetup.mChassisRot * mInitialVehicleSetup.mConnectionPointFLWheel, 
                                mInitialVehicleSetup.mChassisRot);

    mCarWheelFrontR->setShape ( wheelNodes[2], 
                                mWheelShapeFront.get(), 
                                mInitialVehicleSetup.mWheelsRestitution, 
                                mInitialVehicleSetup.mWheelsFriction, 
                                mInitialVehicleSetup.mRollingFriction,
                                mInitialVehicleSetup.mAnisotropicFriction,
                                mInitialVehicleSetup.mWheelsFMass, 
                                mInitialVehicleSetup.mChassisPos + mInitialVehicleSetup.mChassisRot * mInitialVehicleSetup.mConnectionPointFRWheel, 
                                mInitialVehicleSetup.mChassisRot);

    mCarWheelBackL->setShape (  wheelNodes[1], 
                                mWheelShapeBack.get(), 
                                mInitialVehicleSetup.mWheelsRestitution, 
                                mInitialVehicleSetup.mWheelsFriction, 
                                mInitialVehicleSetup.mRollingFriction,
                                mInitialVehicleSetup.mAnisotropicFriction,
                                mInitialVehicleSetup.mWheelsRMass, 
                                mInitialVehicleSetup.mChassisPos + mInitialVehicleSetup.mChassisRot * mInitialVehicleSetup.mConnectionPointRLWheel, 
                                mInitialVehicleSetup.mChassisRot);

    mCarWheelBackR->setShape (  wheelNodes[0], 
                                mWheelShapeBack.get(), 
                                mInitialVehicleSetup.mWheelsRestitution, 
                                mInitialVehicleSetup.mWheelsFriction, 
                                mInitialVehicleSetup.mRollingFriction,
                                mInitialVehicleSetup.mAnisotropicFriction,
                                mInitialVehicleSetup.mWheelsRMass, 
                                mInitialVehicleSetup.mChassisPos + mInitialVehicleSetup.mChassisRot * mInitialVehicleSetup.mConnectionPointRRWheel, 
                                mInitialVehicleSetup.mChassisRot);

    mCarWheelFrontL->setDamping(mInitialVehicleSetup.mAirDensityLinear, mInitialVehicleSetup.mAirDensityAngular);
    mCarWheelFrontR->setDamping(mInitialVehicleSetup.mAirDensityLinear, mInitialVehicleSetup.mAirDensityAngular);
    mCarWheelBackL->setDamping(mInitialVehicleSetup.mAirDensityLinear, mInitialVehicleSetup.mAirDensityAngular);
    mCarWheelBackR->setDamping(mInitialVehicleSetup.mAirDensityLinear, mInitialVehicleSetup.mAirDensityAngular);


    mCarChassis->disableDeactivation ();
    mCarWheelFrontL->disableDeactivation ();
    mCarWheelFrontR->disableDeactivation ();
    mCarWheelBackL->disableDeactivation ();
    mCarWheelBackR->disableDeactivation ();

}

void PSBaseVehicle::addSpringsToWorld()
{

    {
        //http://bulletphysics.org/mediawiki-1.5.8/index.php/Constraints
        //http://bulletphysics.org/Bullet/phpBB3/viewtopic.php?t=8284
        //http://bulletphysics.org/Bullet/phpBB3/viewtopic.php?t=6662
        //http://bulletphysics.org/Bullet/phpBB3/viewtopic.php?f=9&t=4785&view=previous

        mSixDofSpringFrontL.reset(new OgreBulletDynamics::SixDofSpring2Constraint(mCarChassis.get(), mCarWheelFrontL.get(), mInitialVehicleSetup.mConnectionPointFLWheel, Ogre::Quaternion::IDENTITY, Ogre::Vector3::ZERO, Ogre::Quaternion::IDENTITY));
        mSixDofSpringFrontL->setLinearLowerLimit(Ogre::Vector3(0.0f, mInitialVehicleSetup.mMaxTravel, 0.0f));
        mSixDofSpringFrontL->setLinearUpperLimit(Ogre::Vector3(0.0f, 0.0f, 0.0f));
        mSixDofSpringFrontL->setAngularLowerLimit(Ogre::Vector3(1.0f, 0.0f, 0.0f));
        mSixDofSpringFrontL->setAngularUpperLimit(Ogre::Vector3(0.0f, 0.0f, 0.0f));
        mSixDofSpringFrontL->enableSpring(1, true);
        mSixDofSpringFrontL->setStiffness(1, mInitialVehicleSetup.mWheelsFSpringStiffness, mInitialVehicleSetup.mLimitSpringParamsF);
        mSixDofSpringFrontL->setDamping(1, mInitialVehicleSetup.mWheelsFSpringDamping, mInitialVehicleSetup.mLimitSpringParamsF);
        mSixDofSpringFrontL->setEquilibriumPoint(1, mInitialVehicleSetup.mMaxTravel);
        mWorld->addConstraint(mSixDofSpringFrontL.get(), true);
    }

    {
        mSixDofSpringFrontR.reset(new OgreBulletDynamics::SixDofSpring2Constraint(mCarChassis.get(), mCarWheelFrontR.get(), mInitialVehicleSetup.mConnectionPointFRWheel, Ogre::Quaternion::IDENTITY, Ogre::Vector3::ZERO, Ogre::Quaternion::IDENTITY));
        mSixDofSpringFrontR->setLinearLowerLimit(Ogre::Vector3(0.0f, mInitialVehicleSetup.mMaxTravel, 0.0f));
        mSixDofSpringFrontR->setLinearUpperLimit(Ogre::Vector3(0.0f, 0.0f, 0.0f));
        mSixDofSpringFrontR->setAngularLowerLimit(Ogre::Vector3(1.0f, 0.0f, 0.0f));
        mSixDofSpringFrontR->setAngularUpperLimit(Ogre::Vector3(0.0f, 0.0f, 0.0f));
        mSixDofSpringFrontR->enableSpring(1, true);
        mSixDofSpringFrontR->setStiffness(1, mInitialVehicleSetup.mWheelsFSpringStiffness, mInitialVehicleSetup.mLimitSpringParamsF);
        mSixDofSpringFrontR->setDamping(1, mInitialVehicleSetup.mWheelsFSpringDamping, mInitialVehicleSetup.mLimitSpringParamsF);
        mSixDofSpringFrontR->setEquilibriumPoint(1, mInitialVehicleSetup.mMaxTravel);
        mWorld->addConstraint(mSixDofSpringFrontR.get(), true);
    }

    {
        mSixDofSpringBackL.reset(new OgreBulletDynamics::SixDofSpring2Constraint(mCarChassis.get(), mCarWheelBackL.get(), mInitialVehicleSetup.mConnectionPointRLWheel, Ogre::Quaternion::IDENTITY, Ogre::Vector3::ZERO, Ogre::Quaternion::IDENTITY));
        mSixDofSpringBackL->setLinearLowerLimit(Ogre::Vector3(0.0f, mInitialVehicleSetup.mMaxTravel, 0.0f));
        mSixDofSpringBackL->setLinearUpperLimit(Ogre::Vector3(0.0f, 0.0f, 0.0f));
        mSixDofSpringBackL->setAngularLowerLimit(Ogre::Vector3(1.0f, 0.0f, 0.0f));
        mSixDofSpringBackL->setAngularUpperLimit(Ogre::Vector3(0.0f, 0.0f, 0.0f));
        mSixDofSpringBackL->enableSpring(1, true);
        mSixDofSpringBackL->setStiffness(1, mInitialVehicleSetup.mWheelsRSpringStiffness, mInitialVehicleSetup.mLimitSpringParamsR);
        mSixDofSpringBackL->setDamping(1, mInitialVehicleSetup.mWheelsRSpringDamping, mInitialVehicleSetup.mLimitSpringParamsR);
        mSixDofSpringBackL->setEquilibriumPoint(1, mInitialVehicleSetup.mMaxTravel);
        mWorld->addConstraint(mSixDofSpringBackL.get(), true);
    }

    {
        mSixDofSpringBackR.reset(new OgreBulletDynamics::SixDofSpring2Constraint(mCarChassis.get(), mCarWheelBackR.get(), mInitialVehicleSetup.mConnectionPointRRWheel, Ogre::Quaternion::IDENTITY, Ogre::Vector3::ZERO, Ogre::Quaternion::IDENTITY));
        mSixDofSpringBackR->setLinearLowerLimit(Ogre::Vector3(0.0f, mInitialVehicleSetup.mMaxTravel, 0.0f));
        mSixDofSpringBackR->setLinearUpperLimit(Ogre::Vector3(0.0f, 0.0f, 0.0f));
        mSixDofSpringBackR->setAngularLowerLimit(Ogre::Vector3(1.0f, 0.0f, 0.0f));
        mSixDofSpringBackR->setAngularUpperLimit(Ogre::Vector3(0.0f, 0.0f, 0.0f));
        mSixDofSpringBackR->enableSpring(1, true);
        mSixDofSpringBackR->setStiffness(1, mInitialVehicleSetup.mWheelsRSpringStiffness, mInitialVehicleSetup.mLimitSpringParamsR);
        mSixDofSpringBackR->setDamping(1, mInitialVehicleSetup.mWheelsRSpringDamping, mInitialVehicleSetup.mLimitSpringParamsR);
        mSixDofSpringBackR->setEquilibriumPoint(1, mInitialVehicleSetup.mMaxTravel);
        mWorld->addConstraint(mSixDofSpringBackR.get(), true);
    }
}