#ifndef DEATHMATCHMODE_H
#define DEATHMATCHMODE_H

#include "SinglePlayerMode.h"

#include <vector>

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

    //LapUtils::Events: a deathmatch session can also end by the player crossing
    //the line on the final lap (the AI need not all be dead). In that case the
    //base onLapFinished drives the finish camera/sign, but it never touches the
    //deathmatch results — so capture them here, mirroring what carDead does when
    //the last AI is eliminated.
    void onLapFinished()override;

protected:

    //BaseRaceMode custom hook: refresh the position HUD with the survivor count
    //(base writes the original grid size, which never shrinks).
    void customFrameRenderingQueuedDo2DUI()override;

private:

    //build mDeathmatchResults from the current race clock + per-AI elimination
    //times. Called from both session-end paths (carDead when the last AI dies,
    //onLapFinished when the player completes the final lap) so the post-race
    //statistics screen is populated no matter how the session ended.
    void fillDeathmatchResults();

    //number of cars still alive in the field (decremented as cars die).
    size_t mAliveCars;

    //per-AI elimination time (seconds on the race clock) captured at the moment
    //each AI dies, so the post-race statistics screen can show survival times.
    //-1.0f means the AI was still alive at session end. Indexed by AI index.
    std::vector<Ogre::Real> mEliminationTimes;
};

#endif
