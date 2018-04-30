
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
    mUIBackground(mContext, modeContext.getGameState().getPFLoaderGameshell(), "data/gameshell", "em.bmp"),
    mIsRecreate(false)
{

    mContext.setGameModeSwitcher(this);

    mMenuMode.reset(new MenuMode(mContext, ModeMenu, State_SingleMulti));
    mUIBackground.show();
    mUILoader.reset(new UIBackgroundLoaderProgressTracks(mContext, modeContext.getGameState().getPFLoaderData(), "data/misc/loading", "background.tga", 381.0f, 399.0f, 85.0f, 555.0f));
    mUILoaderChampionship.reset(new UIBackgroundLoaderProgressTracksChampionship(mContext, modeContext.getGameState().getPFLoaderData(), "data/misc/loading", "background.tga", 381.0f, 399.0f, 85.0f, 555.0f));
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
    bool modeRace = mGameMode == ModeRaceSingle || mGameMode == ModeRaceChampionship || mGameMode == ModeRaceMulti;
    bool modeRaceNext = mGameModeNext == ModeRaceSingle || mGameModeNext == ModeRaceChampionship || mGameModeNext == ModeRaceMulti;

    if(mIsRecreate)
    {
        ++mRecreateFramesCount;
        if(mRecreateFramesCount > 1)//mRecreateFramesCount - to make sure OverlayManager changed
        {
            mIsInitialLoadPassed = false;//to disable unloader progress
            clear();
            mGameMode = ModeMenu;
            mMenuMode.reset(new MenuMode(mContext, ModeMenu, State_SingleMulti));
            mMenuMode->initData(this);
            mMenuMode->initCamera();
            mIsInitialLoadPassed = true;
            mIsRecreate = false;
            return;
        }
    }

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
                multiplayerSessionStartInfo = mode->getMultiplayerSessionStartInfo();
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
        if(mGameMode == ModeRaceSingle && mGameModeNext == ModeMenu || raceOverAndReadyToQuit && mGameMode == ModeRaceSingle)
        {
            mContext.setLapController(mPlayerMode->getLapController());
        }
        //extract lap data after championship race
        if(mGameMode == ModeRaceChampionship && mGameModeNext == ModeMenuChampionship || raceOverAndReadyToQuit && mGameMode == ModeRaceChampionship)
        {
            mContext.setLapController(mPlayerMode->getLapController());
            mContext.getGameState().getChampionship().trackFinished(mContext);
        }
#ifndef NO_MULTIPLAYER
        if(mGameMode == ModeRaceMulti && mGameModeNext == ModeMenuMulti || raceOverAndReadyToQuit && mGameMode == ModeRaceMulti)
        {
            mContext.setLapController(mPlayerMode->getLapController());
        }
#endif

        //in case of switch from menu championship to menu keep mMenuMode
        if(
            mGameMode == ModeMenuChampionship && mGameModeNext == ModeMenu
            )
        {
        }
        else
        {
            //clear all modes
            clear();
        }

        //race -> main menu (single | multi)
        if(modeRace && mIsSwitchMode || raceOverAndReadyToQuit)
        {
            mIsSwitchMode = false;

            //race -> single main menu
            if(mGameMode == ModeRaceSingle)
            {
                mGameMode = ModeMenu;

                //mContext.mTrayMgr->showCursor();

                mMenuMode.reset(new MenuMode(mContext, ModeMenu, State_Podium));
                mIsLoadPassed = false;
                mUIUnloader->show();
                mMenuMode->initData(this);
                mUIUnloader->hide();
                mIsLoadPassed = true;
                mMenuMode->initCamera();
            }

            //championship -> single main menu (championship)
            if(mGameMode == ModeRaceChampionship)
            {
                mGameMode = ModeMenuChampionship;

                mMenuMode.reset(new MenuMode(mContext, ModeMenuChampionship, State_Podium));
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

                mMenuMultiMode.reset(new MenuMultiMode(mContext, controller, multiplayerSessionStartInfo));
                mIsLoadPassed = false;
                mUIUnloader->show();
                mMenuMultiMode->initData(this);
                mUIUnloader->hide();
                mIsLoadPassed = true;
                mMenuMultiMode->initCamera();
            }
#endif
        }

        //main menu single -> race single
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

        //main menu single (championship) -> race championship
        if((mGameMode == ModeMenu || mGameMode == ModeMenuChampionship) && mIsSwitchMode && mGameModeNext == ModeRaceChampionship)
        {
            mIsSwitchMode = false;

            mGameMode = mGameModeNext;

            size_t championshipTrack = mContext.getGameState().getChampionship().getCurrentTrack();

            const STRPowerslide& strPowerslide = mContext.getGameState().getSTRPowerslide();
            std::vector<std::string> availTracks = strPowerslide.getArrayValue("", "available tracks");
            mContext.getGameState().setRaceParameters(availTracks[championshipTrack], mContext.getGameState().getAIStrength());

            mPlayerMode.reset(new SinglePlayerMode(mContext));
            mIsLoadPassed = false;
            mUILoaderChampionship->show(championshipTrack, mContext.getGameState().getTrackNameAsOriginal(), true, mContext.getGameState().getAIStrength());
            mPlayerMode->initData(this);
            mUILoaderChampionship->hide();
            mIsLoadPassed = true;
            mPlayerMode->initCamera();
        }

        //main menu championship -> main menu single
        if(mGameMode == ModeMenuChampionship && mIsSwitchMode && mGameModeNext == ModeMenu)
        {
            mIsSwitchMode = false;

            mGameMode = mGameModeNext;

            mIsLoadPassed = false;
            mIsLoadPassed = true;
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

            mIsInitialLoadPassed = false;//to disable unloader progress
            mMenuMode.reset(new MenuMode(mContext, ModeMenu, State_SingleMulti));
            mMenuMode->initData(this);
            mMenuMode->initCamera();
            mIsInitialLoadPassed = true;
        }

        //main menu single -> multi main menu
        if(mGameMode == ModeMenu && mIsSwitchMode && mGameModeNext == ModeMenuMulti)
        {
            mIsSwitchMode = false;

            mGameMode = mGameModeNext;

            mIsInitialLoadPassed = false;//to disable unloader progress
            mMenuMultiMode.reset(new MenuMultiMode(mContext));
            mMenuMultiMode->initData(this);
            mMenuMultiMode->initCamera();

            //unable to enter to lobby
            //recreate main menu single
            if(!mMenuMultiMode->getIsLobbyEntered())
            {
                clear();
                mGameMode = ModeMenu;
                mMenuMode.reset(new MenuMode(mContext, ModeMenu, State_SingleMulti));
                mMenuMode->initData(this);
                mMenuMode->initCamera();
            }
            mIsInitialLoadPassed = true;
        }
#endif
    }
}

void GameModeSwitcher::switchMode(GameMode nextMode)
{
    mGameModeNext = nextMode;
    mIsSwitchMode = true;
}

void GameModeSwitcher::recreateMenu()
{
    mIsRecreate = true;
    mRecreateFramesCount = 0;
}

void GameModeSwitcher::restartRace()
{
    if(mPlayerMode.get())
        mPlayerMode->restart();
}

void GameModeSwitcher::createBurnByPlayer()
{
    if(mPlayerMode.get())
        mPlayerMode->createBurnByPlayer();
}

void GameModeSwitcher::createBombByPlayer()
{
    if(mPlayerMode.get())
        mPlayerMode->createBombByPlayer();
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

    if(mGameMode == ModeRaceMulti)
    {
        static_cast<MultiPlayerMode *>(mPlayerMode.get())->keyUp(_key, _char);
    }
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

    if(mPlayerMode.get())
        mPlayerMode->mousePressed(pos);
}

void GameModeSwitcher::mouseReleased(const Ogre::Vector2& pos)
{
    if(mMenuMode.get())
        mMenuMode->mouseReleased(pos);

#ifndef NO_MULTIPLAYER
    if(mMenuMultiMode.get())
        mMenuMultiMode->mouseReleased(pos);
#endif

    if(mPlayerMode.get())
        mPlayerMode->mouseReleased(pos);
}

void GameModeSwitcher::mouseMoved(const Ogre::Vector2& pos)
{
    if(mMenuMode.get())
        mMenuMode->mouseMoved(pos);

#ifndef NO_MULTIPLAYER
    if(mMenuMultiMode.get())
        mMenuMultiMode->mouseMoved(pos);
#endif

    if(mPlayerMode.get())
        mPlayerMode->mouseMoved(pos);
}

#if defined(__ANDROID__)
void GameModeSwitcher::reloadTextures()
{
    mUILoader->reloadTextures(mContext.getGameState().getPFLoaderData(), "data/misc/loading", "background.tga");
    mUILoaderChampionship->reloadTextures(mContext.getGameState().getPFLoaderData(), "data/misc/loading", "background.tga");
    mUIUnloader->reloadTextures(mContext.getGameState().getPFLoaderData(), "data/misc/loading/interface", "background.tga");

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

void GameModeSwitcher::setSubmenu(const std::string& title)
{
    if(mMenuMode.get())
        mMenuMode->setSubmenu(title);

#ifndef NO_MULTIPLAYER
    if(mMenuMultiMode.get())
        mMenuMultiMode->setSubmenu(title);
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

void GameModeSwitcher::setPodiumSubmenu()
{
    if(mMenuMode.get())
        mMenuMode->setPodiumSubmenu();
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

    if(mGameMode == ModeRaceChampionship)
    {
        mUILoaderChampionship->setPercent(percent, info);
    }

    if(mGameMode == ModeMenu || mGameMode == ModeMenuChampionship || mGameMode == ModeMenuMulti)
    {
        if(mIsInitialLoadPassed)
            mUIUnloader->setPercent(percent, info);
    }
}