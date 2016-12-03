#include "../pcheader.h"

#include "GameModeSwitcher.h"

#include "../gamemodes/BaseMenuMode.h"
#include "../gamemodes/SinglePlayerMode.h"
#include "../gamemodes/MultiPlayerMode.h"

#include "../customs/CustomTrayManager.h"

GameModeSwitcher::GameModeSwitcher(const ModeContext& modeContext)
    : mContext(modeContext),
    mGameMode(ModeMenu), mIsSwitchMode(false)
{
#if 1
    mMenuMode.reset(new BaseMenuMode(mContext));
#else
    mPlayerMode.reset(new SinglePlayerMode(mContext));
#endif

    if(mMenuMode.get())
        mMenuMode->initData();

    if(mPlayerMode.get())
        mPlayerMode->initData();
}

GameModeSwitcher::~GameModeSwitcher()
{
    clear();
}

void GameModeSwitcher::frameStarted(const Ogre::FrameEvent &evt)
{
    if(mMenuMode.get())
        mMenuMode->frameStarted(evt);

    if(mPlayerMode.get())
        mPlayerMode->frameStarted(evt);
}

void GameModeSwitcher::frameRenderingQueued(const Ogre::FrameEvent &evt)
{
    if(mMenuMode.get())
        mMenuMode->frameRenderingQueued(evt);

    if(mPlayerMode.get())
        mPlayerMode->frameRenderingQueued(evt);
}

void GameModeSwitcher::frameEnded()
{
    const unsigned long afterFinishTimeThreshold = 10000; // ms
    bool raceOverAndReadyToQuit =   mGameMode == ModeRace                       &&
                                    mContext.mGameState.getRaceFinished()       &&
                                    mContext.mGameState.getAfterFinishTimerTime() > afterFinishTimeThreshold;

    if(mIsSwitchMode || raceOverAndReadyToQuit)
    {
        clear();

        if(mGameMode == ModeRace && mIsSwitchMode || raceOverAndReadyToQuit)
        {
            mIsSwitchMode = false;

            mGameMode = ModeMenu;

            mContext.mTrayMgr->showCursor();

            mMenuMode.reset(new BaseMenuMode(mContext));

            mMenuMode->initData();
        }

        if(mGameMode == ModeMenu && mIsSwitchMode)
        {
            mIsSwitchMode = false;

            mGameMode = ModeRace;

            mContext.mTrayMgr->hideCursor();

#if 1
            mPlayerMode.reset(new SinglePlayerMode(mContext));
#else
            mPlayerMode.reset(new MultiPlayerMode(mContext));
#endif

            mPlayerMode->initData();
        }

    }
}

void GameModeSwitcher::restartRace()
{
    if(mPlayerMode.get())
        mPlayerMode->restart();
}

void GameModeSwitcher::reloadRace()
{
    if(mPlayerMode.get())
        mPlayerMode->reload();
}

#if defined(__ANDROID__)
void GameModeSwitcher::reloadTextures()
{
    if(mMenuMode.get())
        mMenuMode->reloadTextures();

    if(mPlayerMode.get())
        mPlayerMode->reloadTextures();
}
#endif

void GameModeSwitcher::processCollision(btManifoldPoint& cp, const btCollisionObjectWrapper* colObj0Wrap, const btCollisionObjectWrapper* colObj1Wrap, int triIndex)
{
    if(mPlayerMode.get())
        mPlayerMode->processCollision(cp, colObj0Wrap, colObj1Wrap, triIndex);
}

void GameModeSwitcher::clear()
{
    if(mMenuMode.get())
        mMenuMode->clearData();
    mMenuMode.reset();

    if(mPlayerMode.get())
        mPlayerMode->clearData();
    mPlayerMode.reset();
}