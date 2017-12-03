
#include "PSAICar.h"

#include "../physics/Physics.h"
#include "../mesh/StaticMeshProcesser.h"

#include "../tools/Conversions.h"

PSAICar::PSAICar() :
    mSteeringImpulse(0.0f)
{
}

void PSAICar::initModel(    lua_State * pipeline, 
                            const GameState& gameState,
                            Ogre::SceneManager* sceneMgr, Ogre::SceneNode* mainNode,
                            ModelsPool* modelsPool,
                            Physics * world,
                            const std::string& characterName,
                            InitialVehicleSetup& initialVehicleSetup,
                            bool isPossesCamera)
{
    mSteeringImpulse = 0.0f;
    mSteeringValue = 0.0f;

    PSControllableCar::initModel(pipeline, gameState, sceneMgr, mainNode, modelsPool, world, characterName, 
        initialVehicleSetup,
        isPossesCamera);
}

void PSAICar::processInternalTick(float timeStep, bool isRaceStarted)
{
#if 0
    PSControllableCar::processInternalTick(timeStep, isRaceStarted);

    Ogre::Real spfFake = 1.5f;

    Ogre::Quaternion rot = mCarChassis->getSceneNode()->_getDerivedOrientation();

    if (mSteeringLeft)
    {
        if(checkFrontCollision())
        {
            mCarWheelFrontL->applyImpulse(rot * Ogre::Vector3(-mSteeringImpulse * spfFake, 0.0f, 0.0f), rot * Ogre::Vector3(0.0f, 0.0f, -1.0f));
            mCarWheelFrontR->applyImpulse(rot * Ogre::Vector3(-mSteeringImpulse * spfFake, 0.0f, 0.0f), rot * Ogre::Vector3(0.0f, 0.0f, -1.0f));
        }
    }
    else if (mSteeringRight)
    {
        if(checkFrontCollision())
        {
            mCarWheelFrontL->applyImpulse(rot * Ogre::Vector3(mSteeringImpulse * spfFake, 0.0f, 0.0f), rot * Ogre::Vector3(0.0f, 0.0f, -1.0f));
            mCarWheelFrontR->applyImpulse(rot * Ogre::Vector3(mSteeringImpulse * spfFake, 0.0f, 0.0f), rot * Ogre::Vector3(0.0f, 0.0f, -1.0f));
        }
    }
#endif
}

void PSAICar::adjustFrontWheelsAngle(const Ogre::FrameEvent &evt)
{
    mSteering = mSteeringMax * mSteeringValue;
}

void PSAICar::performAICorrection(const GameState& gameState, bool isRaceStarted, bool isGamePaused)
{
    mAIUtils.performAICorrection(this, gameState, isRaceStarted, isGamePaused);
}

void PSAICar::setAIData(const AIWhole& aiWhole, Ogre::SceneManager* sceneMgr, bool isDebugAI)
{
    mAIUtils.setAIData(aiWhole, sceneMgr, isDebugAI);
}

void PSAICar::raceStarted()
{
    mAIUtils.raceStarted();
}