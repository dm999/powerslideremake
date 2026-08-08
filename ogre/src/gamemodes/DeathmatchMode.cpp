#include "DeathmatchMode.h"

#include "../listeners/LoaderListener.h"
#include "../physics/PhysicsVehicle.h"
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
