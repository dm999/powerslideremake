#include "../pcheader.h"

#include "GameModeSwitcher.h"

#include "../gamemodes/MenuMode.h"
#include "../gamemodes/MenuMultiMode.h"
#include "../gamemodes/SinglePlayerMode.h"
#include "../gamemodes/MultiPlayerMode.h"

#include "../customs/CustomTrayManager.h"

#include "../multiplayer/MultiplayerController.h"

GameModeSwitcher::GameModeSwitcher(const ModeContext& modeContext)
    : mContext(modeContext),
    mGameMode(ModeMenu), mIsSwitchMode(false)
{

    mContext.setGameModeSwitcher(this);

    mMenuMode.reset(new MenuMode(mContext));
    mMenuMode->initData();
}

GameModeSwitcher::~GameModeSwitcher()
{
    clear();
}

void GameModeSwitcher::frameStarted(const Ogre::FrameEvent &evt)
{
    if(mMenuMode.get())
        mMenuMode->frameStarted(evt);

    if(mMenuMultiMode.get())
        mMenuMultiMode->frameStarted(evt);

    if(mPlayerMode.get())
        mPlayerMode->frameStarted(evt);
}

void GameModeSwitcher::frameRenderingQueued(const Ogre::FrameEvent &evt)
{
    if(mMenuMode.get())
        mMenuMode->frameRenderingQueued(evt);

    if(mMenuMultiMode.get())
        mMenuMultiMode->frameRenderingQueued(evt);

    if(mPlayerMode.get())
        mPlayerMode->frameRenderingQueued(evt);
}

/**
 * Main switchng logic goes here
 */
void GameModeSwitcher::frameEnded()
{
    bool modeRace = mGameMode == ModeRaceSingle || mGameMode == ModeRaceMulti;
    bool modeRaceNext = mGameModeNext == ModeRaceSingle || mGameModeNext == ModeRaceMulti;

    //exit on time
    const unsigned long afterFinishTimeThreshold = 10000; // ms
    bool raceOverAndReadyToQuit =   modeRace                                &&
                                    mContext.mGameState.getRaceFinished()   &&
                                    mContext.mGameState.getAfterFinishTimerTime() > afterFinishTimeThreshold;

    if(mIsSwitchMode || raceOverAndReadyToQuit)
    {
        Ogre::LogManager::getSingleton().logMessage(Ogre::LML_NORMAL, "[GameModeSwitcher::frameEnded]: game mode switching started [" + Conversions::DMToString(mGameMode) + "-" + Conversions::DMToString(mGameModeNext) + "]");

        MultiplayerSessionStartInfo multiplayerSessionStartInfo;
        CommonIncludes::shared_ptr<MultiplayerController> controller;

        //store multiplayer controller (switch from multiplayer race to main menu multi)
        if(mGameMode == ModeRaceMulti && mGameModeNext == ModeMenuMulti || raceOverAndReadyToQuit)
        {
            if(mPlayerMode.get())
            {
                MultiPlayerMode * mode = static_cast<MultiPlayerMode *>(mPlayerMode.get());
                controller = mode->getMultiplayerController();
                assert(controller.get());
                controller->setEvents(NULL);
                controller->clearSession();
            }
        }

        //store multiplayer controller (switch from main menu multi to multiplayer race)
        if(mGameMode == ModeMenuMulti && mGameModeNext == ModeRaceMulti)
        {
            if(mMenuMultiMode.get())
            {
                controller = mMenuMultiMode->getMultiplayerController();
                assert(controller.get());
                controller->setEvents(NULL);
                multiplayerSessionStartInfo = mMenuMultiMode->getMultiplayerSessionStartInfo();
            }
        }

        //clean all modes
        clear();

        //from race to main menu (single or multi)
        if(modeRace && mIsSwitchMode || raceOverAndReadyToQuit)
        {
            mIsSwitchMode = false;

            //from race to single main menu
            if(mGameMode == ModeRaceSingle)
            {
                mGameMode = ModeMenu;

                //mContext.mTrayMgr->showCursor();

                mMenuMode.reset(new MenuMode(mContext));
                mMenuMode->initData();
            }

            //from race to multi main menu
            if(mGameMode == ModeRaceMulti)
            {
                mGameMode = ModeMenuMulti;

                //mContext.mTrayMgr->showCursor();

                mMenuMultiMode.reset(new MenuMultiMode(mContext, controller));
                mMenuMultiMode->initData();
                controller->switchedToMainMenu();
            }
        }

        //from main menu single to race
        if(mGameMode == ModeMenu && mIsSwitchMode && mGameModeNext == ModeRaceSingle)
        {
            mIsSwitchMode = false;

            mGameMode = mGameModeNext;

            //mContext.mTrayMgr->hideCursor();

            mPlayerMode.reset(new SinglePlayerMode(mContext));
            mPlayerMode->initData();
        }

        //from main menu multi to race
        if(mGameMode == ModeMenuMulti && mIsSwitchMode && mGameModeNext == ModeRaceMulti)
        {
            mIsSwitchMode = false;

            mGameMode = mGameModeNext;

            //mContext.mTrayMgr->hideCursor();

            mPlayerMode.reset(new MultiPlayerMode(mContext, controller));
            mPlayerMode->initData();
            static_cast<MultiPlayerMode *>(mPlayerMode.get())->prepareDataForSession(multiplayerSessionStartInfo);
        }

        //from main menu multi to main menu single
        if(mGameMode == ModeMenuMulti && mIsSwitchMode && mGameModeNext == ModeMenu)
        {
            mIsSwitchMode = false;

            mGameMode = mGameModeNext;

            //mContext.mTrayMgr->hideCursor();

            mMenuMode.reset(new MenuMode(mContext));
            mMenuMode->initData();
        }

        //from main menu single to multi main menu
        if(mGameMode == ModeMenu && mIsSwitchMode && mGameModeNext == ModeMenuMulti)
        {
            mIsSwitchMode = false;

            mGameMode = mGameModeNext;

            mMenuMultiMode.reset(new MenuMultiMode(mContext));
            mMenuMultiMode->initData();
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

#if defined(__ANDROID__)
void GameModeSwitcher::reloadTextures()
{
    if(mMenuMode.get())
        mMenuMode->reloadTextures();

    if(mMenuMultiMode.get())
        mMenuMultiMode->reloadTextures();

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

    if(mMenuMultiMode.get())
        mMenuMultiMode->clearData();
    mMenuMultiMode.reset();

    if(mPlayerMode.get())
        mPlayerMode->clearData();
    mPlayerMode.reset();
}