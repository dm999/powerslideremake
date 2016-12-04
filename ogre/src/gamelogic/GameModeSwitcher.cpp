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

    mContext.mTrayMgr->setListener(this);

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

void GameModeSwitcher::buttonHit(OgreBites::Button* button)
{
    if (button->getName() == "SinglePlayer")
    {
        switchMode(ModeRaceSingle);
    }

    if (button->getName() == "MultiPlayer")
    {
        switchMode(ModeRaceMulti);
    }
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
    bool modeRace = mGameMode == ModeRaceSingle || mGameMode == ModeRaceMulti;

    const unsigned long afterFinishTimeThreshold = 10000; // ms
    bool raceOverAndReadyToQuit =   modeRace                                &&
                                    mContext.mGameState.getRaceFinished()   &&
                                    mContext.mGameState.getAfterFinishTimerTime() > afterFinishTimeThreshold;

    if(mIsSwitchMode || raceOverAndReadyToQuit)
    {
        clear();

        if(modeRace && mIsSwitchMode || raceOverAndReadyToQuit)
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

            mGameMode = mGameModeNext;

            mContext.mTrayMgr->hideCursor();

            if(mGameMode == ModeRaceSingle)
                mPlayerMode.reset(new SinglePlayerMode(mContext));

            if(mGameMode == ModeRaceMulti)
                mPlayerMode.reset(new MultiPlayerMode(mContext));


            mPlayerMode->initData();
        }

    }
}

void GameModeSwitcher::switchMode(GameMode nextMode)
{
    mGameModeNext = nextMode;
    mIsSwitchMode = true;
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