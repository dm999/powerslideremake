#include "../pcheader.h"

#include "PSMultiplayerCar.h"

#include "../customs/CustomRigidBodyWheel.h"

#include "../customs/MovableText.h"

PSMultiplayerCar::PSMultiplayerCar() :
    mCurrentLap(0), mLapPosition(0.0f)
{
}

void PSMultiplayerCar::initModel(    lua_State * pipeline, 
                            const GameState& gameState,
                            Ogre::SceneManager* sceneMgr, Ogre::SceneNode* mainNode,
                            CameraMan * cameraMan,
                            ModelsPool* modelsPool,
                            OgreBulletDynamics::DynamicsWorld * world,
                            const std::string& characterName,
                            const Ogre::Matrix4& transform,
                            bool isPossesCamera,
                            const std::string& humanName, bool isHuman)
{
    mLastTimeOfUpdate = 0;

    PSAICar::initModel(pipeline, gameState, sceneMgr, mainNode, cameraMan, modelsPool, world, characterName, transform, isPossesCamera);

    if(isHuman)
    {
        mPlayerTitle.reset(new Ogre::MovableText("Title", humanName, "SdkTrays/Caption", 5.0f));
        mPlayerTitle->setTextAlignment(Ogre::MovableText::H_CENTER, Ogre::MovableText::V_ABOVE);
        mPlayerTitle->setLocalTranslation(Ogre::Vector3(0.0f, 10.0f, 0.0f));
        mModelNode->attachObject(mPlayerTitle.get());
    }
}

void PSMultiplayerCar::clear()
{
    PSAICar::clear();

    mPlayerTitle.reset();
}

void PSMultiplayerCar::setModelPosition(const Ogre::Vector3& pos, const Ogre::Quaternion& rot, const Ogre::Vector3& wheel0, const Ogre::Vector3& wheel1, const Ogre::Vector3& wheel2, const Ogre::Vector3& wheel3)
{
    if(mModelNode)
    {
        mModelNode->setPosition(pos);
        mModelNode->setOrientation(rot);

        mWheelNodes[0]->setPosition(wheel0);
        mWheelNodes[1]->setPosition(wheel1);
        mWheelNodes[2]->setPosition(wheel2);
        mWheelNodes[3]->setPosition(wheel3);

        mWheelNodes[0]->setOrientation(rot);
        mWheelNodes[1]->setOrientation(rot);
        mWheelNodes[2]->setOrientation(rot);
        mWheelNodes[3]->setOrientation(rot);



        mCarChassis->setCenterOfMass(pos, rot);

        mCarWheelBackR->setCenterOfMass (wheel0, rot);
        mCarWheelBackL->setCenterOfMass (wheel1, rot);
        mCarWheelFrontR->setCenterOfMass(wheel2, rot);
        mCarWheelFrontL->setCenterOfMass(wheel3, rot);
    }
}

void PSMultiplayerCar::setModelRotationOnly(const Ogre::Quaternion& rot)
{
    if(mModelNode)
    {
        mModelNode->setOrientation(rot);

        mWheelNodes[0]->setOrientation(rot);
        mWheelNodes[1]->setOrientation(rot);
        mWheelNodes[2]->setOrientation(rot);
        mWheelNodes[3]->setOrientation(rot);


        mCarChassis->setCenterOfMass(mCarChassis->getCenterOfMassPosition(), rot);

        mCarWheelBackR->setCenterOfMass (mCarWheelBackR->getCenterOfMassPosition(), rot);
        mCarWheelBackL->setCenterOfMass (mCarWheelBackL->getCenterOfMassPosition(), rot);
        mCarWheelFrontR->setCenterOfMass(mCarWheelFrontR->getCenterOfMassPosition(), rot);
        mCarWheelFrontL->setCenterOfMass(mCarWheelFrontL->getCenterOfMassPosition(), rot);
    }
}

void PSMultiplayerCar::setModelVelocity(const Ogre::Vector3& linear, const Ogre::Vector3& angular)
{
    if(mModelNode)
    {
        mCarChassis->setLinearVelocity(linear);
        mCarChassis->setAngularVelocity(angular);
    }
}

void PSMultiplayerCar::removeFromScene(Ogre::SceneManager* sceneMgr)
{
    //remove bullet objects
    removeFromPhysicsSimulation();

    //remove title
    mPlayerTitle.reset();

    //remove ogre objects
    if(mModelNode)
    {
        
        if(Ogre::SceneNode * parent = mModelNode->getParentSceneNode())
        {
            parent->removeAndDestroyChild(mModelNode->getName());
        }
    }

    for(size_t q = 0; q < 4; ++q)
    {
        if(mWheelNodes[q])
        {
            if(Ogre::SceneNode * parent = mWheelNodes[q]->getParentSceneNode())
            {
                parent->removeAndDestroyChild(mWheelNodes[q]->getName());
            }
        }

        if(mWheelEntitys[q])
        {
            sceneMgr->destroyEntity(mWheelEntitys[q]);
        }
    }

    if(mModelEntity[0])
        sceneMgr->destroyEntity(mModelEntity[0]);

    if(mParticleNodeWheelBackL)
    {
        if(Ogre::SceneNode * parent = mParticleNodeWheelBackL->getParentSceneNode())
        {
            parent->removeAndDestroyChild(mParticleNodeWheelBackL->getName());
        }
    }

    if(mParticleNodeWheelBackR)
    {
        if(Ogre::SceneNode * parent = mParticleNodeWheelBackR->getParentSceneNode())
        {
            parent->removeAndDestroyChild(mParticleNodeWheelBackR->getName());
        }
    }

    if(mWheelBackLParticle)
    {
        sceneMgr->destroyParticleSystem(mWheelBackLParticle);
    }

    if(mWheelBackRParticle)
    {
        sceneMgr->destroyParticleSystem(mWheelBackRParticle);
    }


    //other clearing
    clear();
    deinitSounds();
}

void PSMultiplayerCar::removeFromPhysicsSimulation()
{
    mWorld->removeConstraint(mSixDofSpringFrontL);
    mWorld->removeConstraint(mSixDofSpringFrontR);
    mWorld->removeConstraint(mSixDofSpringBackL);
    mWorld->removeConstraint(mSixDofSpringBackR);
    mWorld->removeRigidBody(mCarWheelFrontL);
    mWorld->removeRigidBody(mCarWheelFrontR);
    mWorld->removeRigidBody(mCarWheelBackL);
    mWorld->removeRigidBody(mCarWheelBackR);
    mWorld->removeRigidBody(mCarChassis);
    
}