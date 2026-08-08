#ifndef DEATHMATCHMODE_H
#define DEATHMATCHMODE_H

#include "SinglePlayerMode.h"

class PhysicsVehicle;
class LoaderListener;

//Deathmatch: a single-player race variant where AI cars have life that is
//drained by collisions (player rams them, or they mutually destroy each other).
//A car whose life hits zero explodes and is counted out, shrinking the field.
//Built on the Phase 1 engine hooks (GameState::isDeathmatch, PhysicsListener::carDead,
//PhysicsVehicle life/dead-ticks). Single player / championship / time trial / multiplayer
//are unaffected: setDeathmatch(true) is only called from here.
class DeathmatchMode : public SinglePlayerMode
{
public:

    DeathmatchMode(const ModeContext& modeContext);
    virtual ~DeathmatchMode(){}

    //BaseMode
    void initData(LoaderListener* loaderListener)override;
    void clearData()override;

    //PhysicsListener: fired once per vehicle when its life crosses zero.
    void carDead(PhysicsVehicle* vehicle)override;

protected:

    //BaseRaceMode custom hook: refresh the position HUD with the survivor count
    //(base writes the original grid size, which never shrinks).
    void customFrameRenderingQueuedDo2DUI()override;

private:

    //number of cars still alive in the field (decremented as cars die).
    size_t mAliveCars;
};

#endif
