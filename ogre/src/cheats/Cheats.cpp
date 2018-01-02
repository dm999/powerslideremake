
#include "Cheats.h"

#include "../physics/Physics.h"

#include "../cheats/CheatBurn.h"
#include "../cheats/CheatBomb.h"

Cheats::Cheats(StaticMeshProcesser * meshProesser, Ogre::SceneManager* sceneManager, Physics* world)
    : mWorld(world)
{
    mCheatBurns.reset(new CheatBurns(meshProesser, sceneManager));
    mCheatBombs.reset(new CheatBombs(meshProesser, sceneManager));
    mWorld->addListener(mCheatBurns.get());
    mWorld->addListener(mCheatBombs.get());
}

Cheats::~Cheats()
{
    mWorld->removeListener(mCheatBurns.get());
    mWorld->removeListener(mCheatBombs.get());
}

void Cheats::createBurnByPlayer(PhysicsVehicle * vehicle)
{
    mCheatBurns->createBurnByPlayer(vehicle);
}

void Cheats::createBombByPlayer(PhysicsVehicle * vehicle)
{
    mCheatBombs->createBombByPlayer(vehicle);
}