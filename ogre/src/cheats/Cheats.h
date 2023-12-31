#ifndef CHEATS_H
#define CHEATS_H

#include "../includes/CommonIncludes.h"

namespace Ogre{
    class SceneManager;
}

class StaticMeshProcesser;

class Physics;
class PhysicsVehicle;
class CheatBurns;
class CheatBombs;
class SoundsProcesser;

class Cheats
{
public:
    Cheats(StaticMeshProcesser * meshProesser, Ogre::SceneManager* sceneManager, Physics* world, bool isFog
#ifndef NO_OPENAL
        , SoundsProcesser * soundProcesser
#endif
        );
    ~Cheats();

    void createBurnByPlayer(PhysicsVehicle * vehicle);
    void createBombByPlayer(PhysicsVehicle * vehicle);

private:

    Physics* mWorld;

    CommonIncludes::shared_ptr<CheatBurns> mCheatBurns;
    CommonIncludes::shared_ptr<CheatBombs> mCheatBombs;
};

#endif
