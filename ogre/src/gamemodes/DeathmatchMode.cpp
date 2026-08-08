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

    //reset per-AI elimination times for this race (-1 == still alive).
    const size_t aiCount = mModeContext.getGameState().getAICountInRace();
    mEliminationTimes.assign(aiCount, -1.0f);
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

    //all AI eliminated — only the player is left alive. End the session the same
    //way a single race ends on the final lap: snap to the fixed finish camera
    //(auto-tracking the player), hide the rear-view mirror, show the finish sign
    //and start the after-finish timer. setRaceFinished(true) lets the existing
    //GameModeSwitcher teardown (ModeRaceDeathmatch → ModeMenu / State_DeathmatchStats)
    //run after its 10s countdown, so no switcher changes are needed here.
    //The lap-time/hiscore block from BaseRaceMode::onLapFinished is intentionally
    //omitted — lap times aren't meaningful for deathmatch.
    if(mAliveCars <= 1)
    {
        if(!gameState.getRaceFinished())
        {
            gameState.resetAfterFinishTimer();
            gameState.getPlayerCar().setDisableMouse(false);
            mCamera->setPosition(gameState.getSTRPowerslide().getFinishCameraPos(gameState.getTrackName()));
            if(mUIRace.get())
            {
                mUIRace->setRearViewMirrorPanelShow(false);
                mUIRace->setVisibleFinishSign(true, 1);
            }

            //freeze the statistics for the post-race screen.
            fillDeathmatchResults();

            gameState.setRaceFinished(true);

            Ogre::LogManager::getSingleton().logMessage(Ogre::LML_NORMAL,
                "[DeathmatchMode::carDead]: all AI eliminated, session finishing");
        }
    }
}

void DeathmatchMode::fillDeathmatchResults()
{
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
    deathmatchResultVec results;
    results.reserve(aiCount + 1);
    results.push_back(DeathmatchResultRow(
        gameState.getPlayerName(), true, raceClock, true));
    for(size_t q = 0; q < aiCount; ++q)
    {
        const bool survived = (q >= mEliminationTimes.size() || mEliminationTimes[q] < 0.0f);
        results.push_back(DeathmatchResultRow(
            gameState.getAICar(q).getCharacterName(), false,
            survived ? -1.0f : mEliminationTimes[q], survived));
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
        fillDeathmatchResults();
}

void DeathmatchMode::customFrameRenderingQueuedDo2DUI()
{
    //base writes the original grid size into the position HUD; override the total
    //with the live survivor count so "pos / total" reflects cars still in play.
    if(mUIRace.get())
    {
        mUIRace->setCarPos(
            static_cast<unsigned char>(mLapController.getTotalPosition(0)),
            static_cast<unsigned char>(mAliveCars));
    }
}
