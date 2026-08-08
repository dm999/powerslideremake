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

    //deathmatch: enable life damage on bomb/burn hits. No-op in other modes
    //(flag stays false) so single player / championship / timetrial are unaffected.
    void setDeathmatch(bool v);

private:

    Physics* mWorld;

    CommonIncludes::shared_ptr<CheatBurns> mCheatBurns;
    CommonIncludes::shared_ptr<CheatBombs> mCheatBombs;
};

#endif
