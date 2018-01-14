
#include "PSMultiplayerCar.h"

#include "../customs/MovableText.h"

#include "../physics/Physics.h"
#include "../physics/PhysicsVehicle.h"

PSMultiplayerCar::PSMultiplayerCar() :
    mCurrentLap(0), mLapPosition(0.0f)
{
}

void PSMultiplayerCar::initModel(    lua_State * pipeline, 
                            const GameState& gameState,
                            Ogre::SceneManager* sceneMgr, Ogre::SceneNode* mainNode,
                            ModelsPool* modelsPool,
                            Physics * world,
                            const std::string& characterName,
                            InitialVehicleSetup& initialVehicleSetup,
                            bool isPossesCamera,
                            const std::string& humanName, bool isHuman)
{
    mLastTimeOfUpdate = 0;

    PSPlayerCar::initModel(pipeline, gameState, sceneMgr, mainNode, modelsPool, world, characterName, 
        initialVehicleSetup,
        isPossesCamera);

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
    mWorld->removeListener(this);

    PSPlayerCar::clear();

    mPlayerTitle.reset();
}

void PSMultiplayerCar::setModelImpulse(const Ogre::Vector3& linear, const Ogre::Vector3& angular)
{
    if(mModelNode)
    {
        setLinearImpulse(linear);
        setAngularImpulse(angular);
    }
}

void PSMultiplayerCar::removeFromScene(Ogre::SceneManager* sceneMgr)
{
    //remove bullet objects
    clear();

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
    deinitSounds();
}

