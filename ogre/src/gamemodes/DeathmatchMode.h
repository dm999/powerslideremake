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

    //Build mDeathmatchResults from the current race clock + per-AI elimination
    //times. Called from both natural session-end paths (carDead when the last AI
    //dies, onLapFinished when the player completes the final lap). Also called
    //by GameModeSwitcher at teardown when the player Esc-quits early — in that
    //case neither natural path ran, so the results would otherwise be empty and
    //the post-race statistics screen would show no table.
    void fillDeathmatchResults();

    //Shared session-end sequence: snap to the fixed finish camera (auto-tracking
    //the player), hide the rear-view mirror, show the finish sign, and setRaceFinished(true).
    //Called from every natural session-end path that does NOT go through the base
    //onLapFinished — all AI eliminated (carDead), and every still-alive AI having
    //completed all its laps (customFrameRenderingQueuedDo2DUI). The lap-time/hiscore
    //block from BaseRaceMode::onLapFinished is intentionally omitted here — lap times
    //aren't meaningful for deathmatch. Idempotent: no-op once getRaceFinished() is
    //already true. Callers MUST call fillDeathmatchResults() first so the statistics
    //are captured from the live race clock before this freezes session state.
    void finishDeathmatchSession();

protected:

    //BaseRaceMode custom hook: refresh the position HUD with the survivor count
    //(base writes the original grid size, which never shrinks).
    void customFrameRenderingQueuedDo2DUI()override;

private:

    //number of cars still alive in the field (decremented as cars die).
    size_t mAliveCars;

    //per-AI elimination time (seconds on the race clock) captured at the moment
    //each AI dies, so the post-race statistics screen can show survival times.
    //-1.0f means the AI was still alive at session end. Indexed by AI index.
    std::vector<Ogre::Real> mEliminationTimes;

    //per-AI finish time (seconds on the race clock) captured when each AI
    //completes all its laps. -1.0f means the AI didn't finish (still racing or
    //eliminated). Indexed by AI index.
    std::vector<Ogre::Real> mFinishTimes;
};

#endif
