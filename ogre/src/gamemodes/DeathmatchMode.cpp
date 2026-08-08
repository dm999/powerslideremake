#include "DeathmatchMode.h"

#include "../listeners/LoaderListener.h"
#include "../physics/PhysicsVehicle.h"
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
    (void)vehicle;

    if(mAliveCars > 0)
        --mAliveCars;

    Ogre::LogManager::getSingleton().logMessage(Ogre::LML_NORMAL,
        "[DeathmatchMode::carDead]: car eliminated, " + Conversions::DMToString(mAliveCars) + " cars remaining");

    //all AI eliminated — only the player is left alive. End the session the same
    //way a single race ends on the final lap: snap to the fixed finish camera
    //(auto-tracking the player), hide the rear-view mirror, show the finish sign
    //and start the after-finish timer. setRaceFinished(true) lets the existing
    //GameModeSwitcher teardown (ModeRaceDeathmatch → ModeMenu / State_Podium)
    //run after its 10s countdown, so no switcher changes are needed here.
    //The lap-time/hiscore block from BaseRaceMode::onLapFinished is intentionally
    //omitted — lap times aren't meaningful for deathmatch; the finish board is
    //already prepared for deathmatch in GameModeSwitcher.
    if(mAliveCars <= 1)
    {
        GameState& gameState = mModeContext.getGameState();
        if(!gameState.getRaceFinished())
        {
            gameState.resetAfterFinishTimer();
            gameState.getPlayerCar().setDisableMouse(false);
            mCamera->setPosition(gameState.getSTRPowerslide().getFinishCameraPos(gameState.getTrackName()));
            if(mUIRace.get())
            {
                mUIRace->setRearViewMirrorPanelShow(false);
                mUIRace->setVisibleFinishSign(true, mLapController.getTotalPosition(0));
            }
            gameState.setRaceFinished(true);

            Ogre::LogManager::getSingleton().logMessage(Ogre::LML_NORMAL,
                "[DeathmatchMode::carDead]: all AI eliminated, session finishing");
        }
    }
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
