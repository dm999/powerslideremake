
#include "GameModeSwitcher.h"

#include "../gamemodes/MenuMode.h"
#include "../gamemodes/MenuMultiMode.h"
#include "../gamemodes/SinglePlayerMode.h"
#ifndef NO_MULTIPLAYER
    #include "../gamemodes/MultiPlayerMode.h"
#endif

#include "../customs/CustomTrayManager.h"

#ifndef NO_MULTIPLAYER
    #include "../multiplayer/MultiplayerController.h"
#endif

#include "../InputHandler.h"

GameModeSwitcher::GameModeSwitcher(const ModeContext& modeContext)
    : mContext(modeContext),
    mGameMode(ModeMenu), mIsSwitchMode(false),
    mIsInitialLoadPassed(false),
    mIsLoadPassed(true),
    mUIBackground(mContext, modeContext.getGameState().getPFLoaderGameshell(), "data/gameshell", "em.bmp")
{

    mContext.setGameModeSwitcher(this);

    mMenuMode.reset(new MenuMode(mContext, State_SingleMulti));
    mUIBackground.show();
    mUILoader.reset(new UIBackgroundLoaderProgressTracks(mContext, modeContext.getGameState().getPFLoaderData(), "data/misc/loading", "background.tga", 381.0f, 399.0f, 85.0f, 555.0f));
    mUIUnloader.reset(new UIBackgroundLoaderProgress(mContext, modeContext.getGameState().getPFLoaderData(), "data/misc/loading/interface", "background.tga", 414.0f, 430.0f, 65.0f, 570.0f));
    mMenuMode->initData(this);
    mUIBackground.hide();
    mMenuMode->initCamera();

    mIsInitialLoadPassed = true;
}

GameModeSwitcher::~GameModeSwitcher()
{
    clear();
}

void GameModeSwitcher::frameStarted(const Ogre::FrameEvent &evt)
{
    if(mMenuMode.get())
        mMenuMode->frameStarted(evt);

#ifndef NO_MULTIPLAYER
    if(mMenuMultiMode.get())
        mMenuMultiMode->frameStarted(evt);
#endif

    if(mPlayerMode.get())
        mPlayerMode->frameStarted(evt);
}

void GameModeSwitcher::frameRenderingQueued(const Ogre::FrameEvent &evt)
{
    if(mMenuMode.get())
        mMenuMode->frameRenderingQueued(evt);

#ifndef NO_MULTIPLAYER
    if(mMenuMultiMode.get())
        mMenuMultiMode->frameRenderingQueued(evt);
#endif

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

#ifndef NO_MULTIPLAYER
        MultiplayerSessionStartInfo multiplayerSessionStartInfo;
        CommonIncludes::shared_ptr<MultiplayerController> controller;

        //store multiplayer controller (switch from multiplayer race to main menu multi)
        if(mGameMode == ModeRaceMulti && mGameModeNext == ModeMenuMulti || raceOverAndReadyToQuit)
        {
            if(mPlayerMode.get() && mGameMode == ModeRaceMulti)
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
#endif

        //extract lap data after race
        if(mGameMode == ModeRaceSingle && mGameModeNext == ModeMenu || raceOverAndReadyToQuit)
        {
            mContext.setLapController(mPlayerMode->getLapController());
        }
#ifndef NO_MULTIPLAYER
        if(mGameMode == ModeRaceMulti && mGameModeNext == ModeMenuMulti || raceOverAndReadyToQuit)
        {
            mContext.setLapController(mPlayerMode->getLapController());
        }
#endif

        //clear all modes
        clear();

        //race -> main menu (single | multi)
        if(modeRace && mIsSwitchMode || raceOverAndReadyToQuit)
        {
            mIsSwitchMode = false;

            //race -> single main menu
            if(mGameMode == ModeRaceSingle)
            {
                mGameMode = ModeMenu;

                //mContext.mTrayMgr->showCursor();

                mMenuMode.reset(new MenuMode(mContext, State_Podium));
                mIsLoadPassed = false;
                mUIUnloader->show();
                mMenuMode->initData(this);
                mUIUnloader->hide();
                mIsLoadPassed = true;
                mMenuMode->initCamera();
            }

#ifndef NO_MULTIPLAYER
            //race -> multi main menu
            if(mGameMode == ModeRaceMulti)
            {
                mGameMode = ModeMenuMulti;

                //mContext.mTrayMgr->showCursor();

                mMenuMultiMode.reset(new MenuMultiMode(mContext, controller));
                mIsLoadPassed = false;
                mUIUnloader->show();
                mMenuMultiMode->initData(this);
                mUIUnloader->hide();
                mIsLoadPassed = true;
                mMenuMultiMode->initCamera();
            }
#endif
        }

        //main menu single -> race
        if(mGameMode == ModeMenu && mIsSwitchMode && mGameModeNext == ModeRaceSingle)
        {
            mIsSwitchMode = false;

            mGameMode = mGameModeNext;

            //mContext.mTrayMgr->hideCursor();

            mPlayerMode.reset(new SinglePlayerMode(mContext));
            mIsLoadPassed = false;
            mUILoader->show(mContext.getGameState().getTrackNameAsOriginal(), true, mContext.getGameState().getAIStrength());
            mPlayerMode->initData(this);
            mUILoader->hide();
            mIsLoadPassed = true;
            mPlayerMode->initCamera();
        }

#ifndef NO_MULTIPLAYER
        //main menu multi -> race multi
        if(mGameMode == ModeMenuMulti && mIsSwitchMode && mGameModeNext == ModeRaceMulti)
        {
            mIsSwitchMode = false;

            mGameMode = mGameModeNext;

            //mContext.mTrayMgr->hideCursor();

            mPlayerMode.reset(new MultiPlayerMode(mContext, controller));
            mIsLoadPassed = false;
            mUILoader->show(mContext.getGameState().getTrackNameAsOriginal(), true, mContext.getGameState().getAIStrength());
            mPlayerMode->initData(this);
            mUILoader->hide();
            mIsLoadPassed = true;
            mPlayerMode->initCamera();
            static_cast<MultiPlayerMode *>(mPlayerMode.get())->prepareDataForSession(multiplayerSessionStartInfo);
        }

        //main menu multi -> main menu single
        if(mGameMode == ModeMenuMulti && mIsSwitchMode && mGameModeNext == ModeMenu)
        {
            mIsSwitchMode = false;

            mGameMode = mGameModeNext;

            //mContext.mTrayMgr->hideCursor();

            mMenuMode.reset(new MenuMode(mContext));
            mMenuMode->initData(this);
            mMenuMode->initCamera();
        }

        //main menu single -> multi main menu
        if(mGameMode == ModeMenu && mIsSwitchMode && mGameModeNext == ModeMenuMulti)
        {
            mIsSwitchMode = false;

            mGameMode = mGameModeNext;

            mMenuMultiMode.reset(new MenuMultiMode(mContext));
            mMenuMultiMode->initData(this);
            mMenuMultiMode->initCamera();

            //unable to enter to lobby
            //recreate main menu single
            if(!mMenuMultiMode->getIsLobbyEntered())
            {
                clear();
                mGameMode = ModeMenu;
                mMenuMode.reset(new MenuMode(mContext));
                mMenuMode->initData(this);
                mMenuMode->initCamera();
            }
        }
#endif
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

void GameModeSwitcher::tabPressed()
{
#ifndef NO_MULTIPLAYER
    if(mGameMode == ModeRaceMulti)
    {
        static_cast<MultiPlayerMode *>(mPlayerMode.get())->tabPressed();
    }
#endif
}

void GameModeSwitcher::keyUp(MyGUI::KeyCode _key, wchar_t _char )
{
    if(mMenuMode.get())
        mMenuMode->keyUp(_key, _char);

#ifndef NO_MULTIPLAYER
    if(mMenuMultiMode.get())
        mMenuMultiMode->keyUp(_key, _char);
#endif
}

void GameModeSwitcher::mousePressed(const Ogre::Vector2& pos)
{
    if(mMenuMode.get())
        mMenuMode->mousePressed(pos);

#ifndef NO_MULTIPLAYER
    if(mMenuMultiMode.get())
        mMenuMultiMode->mousePressed(pos);
#endif
}

void GameModeSwitcher::mouseReleased(const Ogre::Vector2& pos)
{
    if(mMenuMode.get())
        mMenuMode->mouseReleased(pos);

#ifndef NO_MULTIPLAYER
    if(mMenuMultiMode.get())
        mMenuMultiMode->mouseReleased(pos);
#endif
}

void GameModeSwitcher::mouseMoved(const Ogre::Vector2& pos)
{
    if(mMenuMode.get())
        mMenuMode->mouseMoved(pos);

#ifndef NO_MULTIPLAYER
    if(mMenuMultiMode.get())
        mMenuMultiMode->mouseMoved(pos);
#endif
}

#if defined(__ANDROID__)
void GameModeSwitcher::reloadTextures()
{
    if(mMenuMode.get())
        mMenuMode->reloadTextures();

#ifndef NO_MULTIPLAYER
    if(mMenuMultiMode.get())
        mMenuMultiMode->reloadTextures();
#endif

    if(mPlayerMode.get())
        mPlayerMode->reloadTextures();
}
#endif

void GameModeSwitcher::processCollision(btManifoldPoint& cp, const btCollisionObjectWrapper* colObj0Wrap, const btCollisionObjectWrapper* colObj1Wrap, int triIndex)
{
    if(mPlayerMode.get())
        mPlayerMode->processCollision(cp, colObj0Wrap, colObj1Wrap, triIndex);
}

bool GameModeSwitcher::isExitSubmenu()const
{
    bool ret = true;

    if(mMenuMode.get())
        ret = mMenuMode->isExitSubmenu();

#ifndef NO_MULTIPLAYER
    if(mMenuMultiMode.get())
        ret = mMenuMultiMode->isExitSubmenu();
#endif

    return ret;
}

void GameModeSwitcher::setExitSubmenu()
{
    if(mMenuMode.get())
        mMenuMode->setExitSubmenu();

#ifndef NO_MULTIPLAYER
    if(mMenuMultiMode.get())
        mMenuMultiMode->setExitSubmenu();
#endif
}

void GameModeSwitcher::setTopmostSubmenu()
{
    if(mMenuMode.get())
        mMenuMode->setTopmostSubmenu();

#ifndef NO_MULTIPLAYER
    if(mMenuMultiMode.get())
        mMenuMultiMode->setTopmostSubmenu();
#endif
}

void GameModeSwitcher::clear()
{
    if(mMenuMode.get())
        mMenuMode->clearData();
    mMenuMode.reset();

#ifndef NO_MULTIPLAYER
    if(mMenuMultiMode.get())
        mMenuMultiMode->clearData();
    mMenuMultiMode.reset();
#endif

    if(mPlayerMode.get())
        mPlayerMode->clearData();
    mPlayerMode.reset();
}

void GameModeSwitcher::loadState(float percent, const std::string& info)
{
    //consume input events
    if(mIsInitialLoadPassed)
        mContext.mInputHandler->capture();

    if(mGameMode == ModeRaceSingle || mGameMode == ModeRaceMulti)
    {
        mUILoader->setPercent(percent, info);
    }

    if(mGameMode == ModeMenu || mGameMode == ModeMenuMulti)
    {
        if(mIsInitialLoadPassed)
            mUIUnloader->setPercent(percent, info);
    }
}