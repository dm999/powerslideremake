
#include "PSAICar.h"

#include "../physics/Physics.h"
#include "../physics/PhysicsVehicle.h"
#include "../mesh/StaticMeshProcesser.h"

#include "../tools/Conversions.h"

PSAICar::PSAICar()
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
    PSControllableCar::initModel(pipeline, gameState, sceneMgr, mainNode, modelsPool, world, characterName, 
        initialVehicleSetup,
        isPossesCamera);

    mPhysicsVehicle->setVehicleType(AIVehicle);
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
    mPhysicsVehicle->setRaceStarted();
    mPhysicsVehicle->gearUp();
    mAIUtils.raceStarted();
}