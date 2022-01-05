
#include "Cheats.h"

#include "../physics/Physics.h"

#include "../cheats/CheatBurn.h"
#include "../cheats/CheatBomb.h"

Cheats::Cheats(StaticMeshProcesser * meshProesser, Ogre::SceneManager* sceneManager, Physics* world, bool isFog
#ifndef NO_OPENAL
        , SoundsProcesser * soundProcesser
#endif
    )
    : mWorld(world)
{
    mCheatBurns = std::make_shared<CheatBurns>(meshProesser, sceneManager, isFog
#ifndef NO_OPENAL
        , soundProcesser
#endif
        );
    mCheatBombs = std::make_shared<CheatBombs>(meshProesser, sceneManager
#ifndef NO_OPENAL
        , soundProcesser
#endif
        );
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