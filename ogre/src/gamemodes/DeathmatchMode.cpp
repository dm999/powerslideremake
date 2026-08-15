#include "DeathmatchMode.h"

#include "../listeners/LoaderListener.h"
#include "../physics/PhysicsVehicle.h"
#include "../physics/Physics.h"
#include "../cheats/Cheats.h"
#include "../ui/UIRace.h"
#include "../tools/Conversions.h"

#include "OgreLogManager.h"

DeathmatchMode::DeathmatchMode(const ModeContext& modeContext)
    : SinglePlayerMode(modeContext),
    mAliveCars(0)
{
}

void DeathmatchMode::initData(LoaderListener* loaderListener)
{
    //enable the deathmatch engine hooks (Phase 1) before the world is built, so
    //collision damage / dead-tick / AI-throttle-by-life are active from the first step.
    mModeContext.getGameState().setDeathmatch(true);

    BaseRaceMode::initData(loaderListener);

    //bomb/burn are constructed during BaseRaceMode::initData; tell them to drain
    //life on hit so the weapons can actually eliminate cars in deathmatch.
    if(mCheats.get())
        mCheats->setDeathmatch(true);

    mAliveCars = mLapController.getTotalCars();

    //reset per-AI elimination/finish times for this race (-1 == still alive/didn't finish).
    const size_t aiCount = mModeContext.getGameState().getAICountInRace();
    mEliminationTimes.assign(aiCount, -1.0f);
    mFinishTimes.assign(aiCount, -1.0f);
}

void DeathmatchMode::clearData()
{
    //drop the flag so any subsequent mode (single / championship / time trial / multi)
    //runs with life inert, exactly as on master.
    mModeContext.getGameState().setDeathmatch(false);

    BaseRaceMode::clearData();
}

void DeathmatchMode::carDead(PhysicsVehicle* vehicle)
{
    GameState& gameState = mModeContext.getGameState();

    //capture this AI's elimination time on the race clock (player total lap
    //time + current lap time = seconds since race start). The player can never
    //die in deathmatch, so vehicle is always an AI; find its index by matching
    //the PhysicsVehicle pointer against each AI's vehicle.
    const Ogre::Real raceClock = gameState.getPlayerCar().getLapUtils().getTotalTime()
                                    + gameState.getPlayerCar().getLapUtils().getLapTime();

    const size_t aiCount = gameState.getAICountInRace();
    for(size_t q = 0; q < aiCount && q < mEliminationTimes.size(); ++q)
    {
        if(mEliminationTimes[q] < 0.0f &&
            mWorld->getVehicle(&gameState.getAICar(q)) == vehicle)
        {
            mEliminationTimes[q] = raceClock;
            break;
        }
    }

    if(mAliveCars > 0)
        --mAliveCars;

    Ogre::LogManager::getSingleton().logMessage(Ogre::LML_NORMAL,
        "[DeathmatchMode::carDead]: car eliminated, " + Conversions::DMToString(mAliveCars) + " cars remaining");

    //all AI eliminated — only the player is left alive. End the session via the
    //shared finish sequence (finish camera, mirror, finish sign, stats, race
    //finished). setRaceFinished(true) lets the existing GameModeSwitcher teardown
    //(ModeRaceDeathmatch → ModeMenu / State_DeathmatchStats) run after its 10s
    //countdown, so no switcher changes are needed here.
    if(mAliveCars <= 1)
    {
        if(!gameState.getRaceFinished())
        {
            //capture the statistics from the live race clock before the finish
            //sequence freezes session state.
            fillDeathmatchResults(true);
            finishDeathmatchSession();

            Ogre::LogManager::getSingleton().logMessage(Ogre::LML_NORMAL,
                "[DeathmatchMode::carDead]: all AI eliminated, session finishing");
        }
    }
}

void DeathmatchMode::computeDeathmatchScore()
{
    GameState& gameState = mModeContext.getGameState();
    const size_t aiCount = gameState.getAICountInRace();

    size_t eliminated = 0;
    size_t injured = 0;

    for(size_t q = 0; q < aiCount; ++q)
    {
        const Ogre::Real life = mWorld->getVehicle(&gameState.getAICar(q))->getLife();
        if(life <= 0.0f)
            ++eliminated;
        else if(life < 1.0f)
            ++injured;
    }

    mDeathmatchEliminatedCount = eliminated;
    mDeathmatchInjuredCount = injured;
    mDeathmatchScore = eliminated * 100 + injured * 25;
}

void DeathmatchMode::fillDeathmatchResults(bool isPlayerFinished)
{
    //compute massacre score/statistics (eliminated/injured counts, score)
    //before freezing the result rows. Safe to call in all deathmatch modes
    //(the counts stay 0 in non-massacre where the base members are ignored).
    computeDeathmatchScore();

    GameState& gameState = mModeContext.getGameState();

    //race clock = seconds since race start. At an AI death (mid-lap) this is
    //totalTime + the in-progress lapTime; at lap-finish the lap timer has just
    //reset to 0, so it collapses to totalTime. Either way it is the elapsed
    //race time at this instant.
    const Ogre::Real raceClock = gameState.getPlayerCar().getLapUtils().getTotalTime()
                                    + gameState.getPlayerCar().getLapUtils().getLapTime();

    const size_t aiCount = gameState.getAICountInRace();

    //freeze the statistics for the post-race screen: the player's total race
    //time, plus each AI's elimination time (or survived flag). Stored on the
    //mode itself (mDeathmatchResults, inherited from BaseRaceMode);
    //GameModeSwitcher copies it into its own ModeContext on teardown, since
    //each mode holds a private copy of ModeContext.
    //
    //DNF: if the session ended because all alive AI finished their laps before
    //the player crossed the final finish line, the player didn't actually
    //complete the race — show "DNF" in the time column instead of the race
    //clock. When all AI were eliminated (carDead path), the player wins by
    //default regardless of lap progress, so the race clock stands.

    bool anyAISurvived = false;
    for(size_t q = 0; q < aiCount; ++q)
    {
        if(q >= mEliminationTimes.size() || mEliminationTimes[q] < 0.0f)
        {
            anyAISurvived = true;
            break;
        }
    }
    const Ogre::Real playerTime = (!isPlayerFinished && anyAISurvived) ? -1.0f : raceClock;

    deathmatchResultVec results;
    results.reserve(aiCount + 1);
    results.push_back(DeathmatchResultRow(
        gameState.getPlayerName(), true, playerTime, true));
    for(size_t q = 0; q < aiCount; ++q)
    {
        const bool survived = (q >= mEliminationTimes.size() || mEliminationTimes[q] < 0.0f);
        //survived AI show their individual finish time (when they completed all laps),
        //or the race clock at session end if they didn't finish; eliminated AI show
        //the race clock at the moment they died.
        Ogre::Real aiTime = raceClock;
        if(survived && q < mFinishTimes.size() && mFinishTimes[q] >= 0.0f)
            aiTime = mFinishTimes[q];
        else if(!survived)
            aiTime = mEliminationTimes[q];
        results.push_back(DeathmatchResultRow(
            gameState.getAICar(q).getCharacterName(), false,
            aiTime, survived));
    }
    mDeathmatchResults = results;
}

void DeathmatchMode::onLapFinished()
{
    //let the single-race finish sequence run (finish camera, sign, hiscores,
    //setRaceFinished(true)) — deathmatch reuses it verbatim.
    SinglePlayerMode::onLapFinished();

    //if that lap was the final one, the race is now finished; capture the
    //statistics so the post-race screen has rows to show. Without this, a
    //deathmatch ended by the player finishing the last lap (AI still alive)
    //would render an empty stats table.
    if(mModeContext.getGameState().getRaceFinished())
        fillDeathmatchResults(true);
}

void DeathmatchMode::finishDeathmatchSession()
{
    //see header for the contract. Idempotent — callers guard on getRaceFinished(),
    //but the guard here keeps the helper safe to call directly too.
    GameState& gameState = mModeContext.getGameState();
    if(gameState.getRaceFinished())
        return;

    gameState.resetAfterFinishTimer();
    gameState.getPlayerCar().setDisableMouse(false);
    mCamera->setPosition(gameState.getSTRPowerslide().getFinishCameraPos(gameState.getTrackName()));
    if(mUIRace.get())
    {
        mUIRace->setRearViewMirrorPanelShow(false);
        mUIRace->setVisibleFinishSign(true, 1);
    }

    gameState.setRaceFinished(true);
}

void DeathmatchMode::customFrameRenderingQueuedDo2DUI()
{
    //A deathmatch session can also end without the field being cleared: if every
    //still-alive AI has completed all its laps, there is nothing left to race for,
    //so end the session the same way carDead does. AI lap tracking runs every
    //physics step (BaseRaceMode::timeStepBefore) and advances mCurrentLap past
    //getLapsCount() once an AI crosses the line on its final lap — the same
    //threshold the player's finish uses (lap == getLapsCount() where lap =
    //getCurrentLap() - 1). Eliminated AI (life <= 0) are skipped so a cleared
    //field doesn't trip this on stale lap values; the all-eliminated case is
    //already handled in carDead.
    GameState& gameState = mModeContext.getGameState();
    if(gameState.getRaceStarted() && !gameState.getRaceFinished() && gameState.getAICountInRace() > 0)
    {
        const size_t lapsCount = gameState.getLapsCount();
        const size_t aiCount = gameState.getAICountInRace();

        bool allAliveAIFinished = true;
        bool anyAlive = false;
        for(size_t q = 0; q < aiCount; ++q)
        {
            if(mWorld->getVehicle(&gameState.getAICar(q))->getLife() <= 0.0f)
                continue;

            anyAlive = true;

            //Check if this AI has finished all laps
            if(gameState.getAICar(q).getLapUtils().getCurrentLap() > lapsCount)
            {
                //AI finished all laps — capture its finish time if not already recorded.
                //Each AI may finish at a different moment, so track individually.
                if(mFinishTimes[q] < 0.0f)
                {
                    mFinishTimes[q] = gameState.getAICar(q).getLapUtils().getTotalTime()
                                        + gameState.getAICar(q).getLapUtils().getLapTime();
                }
            }
            else
            {
                //AI hasn't finished yet — session can't end, but keep checking other AI
                //so we capture finish times for any that have already crossed the line.
                allAliveAIFinished = false;
            }
        }

        //only finish when at least one AI is still alive and every such AI has
        //completed all laps — otherwise a fully-eliminated field would end here
        //too (carDead already owns that path).
        if(anyAlive && allAliveAIFinished)
        {
            //capture the statistics from the live race clock before the finish
            //sequence freezes session state.
            fillDeathmatchResults(false);
            finishDeathmatchSession();

            Ogre::LogManager::getSingleton().logMessage(Ogre::LML_NORMAL,
                "[DeathmatchMode]: all alive AI finished their laps, session finishing");
        }
    }

    //massacre mode: check the 10-minute countdown timer. The countdown is derived
    //from the player's race clock (elapsed seconds from GO). When it hits 0,
    //end the session the same way as the other end conditions, computing stats
    //from the live field. Only applies to massacre; regular deathmatch has no
    //time limit.
    if(!gameState.getRaceFinished() && gameState.isMassacreEnabled())
    {
        const Ogre::Real raceClock = gameState.getPlayerCar().getLapUtils().getTotalTime()
                                        + gameState.getPlayerCar().getLapUtils().getLapTime();
        if(raceClock >= GameState::mMassacreTimeLimit)
        {
            fillDeathmatchResults(false);
            finishDeathmatchSession();

            Ogre::LogManager::getSingleton().logMessage(Ogre::LML_NORMAL,
                "[DeathmatchMode]: massacre time limit expired, session finishing");
        }
    }

    //base writes the original grid size into the position HUD; override the total
    //with the live survivor count so "pos / total" reflects cars still in play.
    if(mUIRace.get())
    {
        mUIRace->setCarPos(
            mLapController.getTotalPosition(0),
            mAliveCars);
    }
}
