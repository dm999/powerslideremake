
#include "GameModeSwitcher.h"

#include <memory>

#include "../gamemodes/MenuMode.h"
#include "../gamemodes/MenuMultiMode.h"
#include "../gamemodes/SinglePlayerMode.h"
#include "../gamemodes/MultiPlayerMode.h"

#include "../customs/CustomTrayManager.h"

#include "../multiplayer/MultiplayerController.h"

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

    mMenuMode = std::make_shared<MenuMode>(mContext, ModeMenu, State_Options_Name);
    mUIBackground.show();
    mUILoader = std::make_shared<UIBackgroundLoaderProgressTracks>(mContext, modeContext.getGameState().getPFLoaderData(), "data/misc/loading", "background.tga", 381.0f, 399.0f, 85.0f, 555.0f);
    mUILoaderChampionship = std::make_shared<UIBackgroundLoaderProgressTracksChampionship>(mContext, modeContext.getGameState().getPFLoaderData(), "data/misc/loading", "background.tga", 381.0f, 399.0f, 85.0f, 555.0f);
    mUIUnloader = std::make_shared<UIBackgroundLoaderProgress>(mContext, modeContext.getGameState().getPFLoaderData(), "data/misc/loading/interface", "background.tga", 414.0f, 430.0f, 65.0f, 570.0f);
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
    bool modeRace = mGameMode == ModeRaceSingle || mGameMode == ModeRaceChampionship || mGameMode == ModeRaceTimetrial ||mGameMode == ModeRaceMulti;
    bool modeRaceNext = mGameModeNext == ModeRaceSingle || mGameModeNext == ModeRaceChampionship || mGameModeNext == ModeRaceTimetrial || mGameModeNext == ModeRaceMulti;

    if(mIsRecreate)
    {
        ++mRecreateFramesCount;
        if(mRecreateFramesCount > 1)//mRecreateFramesCount - to make sure OverlayManager changed
        {
            mIsInitialLoadPassed = false;//to disable unloader progress
            clear();
            mGameMode = ModeMenu;
            mMenuMode = std::make_shared<MenuMode>(mContext, ModeMenu, State_SingleMulti);
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

        mContext.getGameState().setSpeedCheatUsed(false);
        mContext.getGameState().setStickyCheatUsed(false);

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

        //extract lap data after race
        if(mGameMode == ModeRaceSingle && mGameModeNext == ModeMenu || raceOverAndReadyToQuit && mGameMode == ModeRaceSingle)
        {
            mContext.setLapController(mPlayerMode->getLapController());
            mContext.setFinishBoard(FinishBoard::prepareFinishBoard(mContext));

            //check emergent gun
            {
                Ogre::Real bestLap = mContext.getGameState().getPlayerCar().getLapUtils().getBestLapTime();
                size_t trackIndex;
                if(Championship::checkEmergentGunBeaten(mContext, bestLap, trackIndex))
                {
                    STRPlayerSettings::PlayerData& playerData = mContext.getGameState().getPlayerData();
                    playerData.fruit[trackIndex + Championship::mBeatEmergentGunFruitOffset] = true;
                    mContext.getGameState().savePlayerData();
                }
            }
        }
        //extract lap data after championship race, save progress
        if(mGameMode == ModeRaceChampionship && mGameModeNext == ModeMenuChampionship || raceOverAndReadyToQuit && mGameMode == ModeRaceChampionship)
        {
            mContext.setLapController(mPlayerMode->getLapController());
            mContext.setFinishBoard(FinishBoard::prepareFinishBoard(mContext));

            Ogre::Real bestLap = mContext.getGameState().getPlayerCar().getLapUtils().getBestLapTime();

            mContext.getGameState().getChampionship().trackFinished(mContext, bestLap);

            if(mContext.getGameState().getChampionship().isFinished())
            {
                if(mContext.getGameState().getChampionship().isFruitsAvailable())
                {
                    std::vector<size_t> fruitsIndexes = mContext.getGameState().getChampionship().getAvailableFruits();

                    STRPlayerSettings::PlayerData& playerData = mContext.getGameState().getPlayerData();

                    if(fruitsIndexes[0] < Championship::mEveryWinnerFruitOffset)//unlock happened
                    {
                        playerData.level = static_cast<AIStrength>(Ogre::Math::Clamp(playerData.level + 1, 0, 3));
                    }

                    for(size_t q = 0; q < fruitsIndexes.size(); ++q)
                    {
                        playerData.fruit[fruitsIndexes[q]] = true;
                    }

                    mContext.getGameState().savePlayerData();
                }
            }
        }
        //update hiscores after time trial
        if(mGameMode == ModeRaceTimetrial && mGameModeNext == ModeMenuTimetrial)
        {
            mContext.getGameState().updateHiscores(
                mContext.getGameState().getPlayerCar().getCharacterName(),
                mContext.getGameState().getPlayerName(),
                mContext.getGameState().getPlayerCar().getLapUtils().getBestLapTime());
        }
        if(mGameMode == ModeRaceMulti && mGameModeNext == ModeMenuMulti || raceOverAndReadyToQuit && mGameMode == ModeRaceMulti)
        {
            mContext.setLapController(mPlayerMode->getLapController());
        }

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

                mMenuMode = std::make_shared<MenuMode>(mContext, ModeMenu, State_Podium);
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

                SinglePlayerMenuStates state = State_Podium;
                if(mContext.getGameState().getChampionship().isFinished())
                {
                    if(mContext.getGameState().getChampionship().isFruitsAvailable())
                    {
                        state = State_FinishChampionship;
                    }
                    else
                    {
                        mGameMode = ModeMenu;
                        state = State_SingleMulti;
                    }
                }

                mMenuMode = std::make_shared<MenuMode>(mContext, mGameMode, state);
                mIsLoadPassed = false;
                mUIUnloader->show();
                mMenuMode->initData(this);
                mUIUnloader->hide();
                mIsLoadPassed = true;
                mMenuMode->initCamera();
            }

            //timetrial race -> single main menu
            if(mGameMode == ModeRaceTimetrial)
            {
                mGameMode = ModeMenu;

                mMenuMode = std::make_shared<MenuMode>(mContext, ModeMenuTimetrial, State_Track);
                mIsLoadPassed = false;
                mUIUnloader->show();
                mMenuMode->initData(this);
                mUIUnloader->hide();
                mIsLoadPassed = true;
                mMenuMode->initCamera();
            }

            //race -> multi main menu
            if(mGameMode == ModeRaceMulti)
            {
                mGameMode = ModeMenuMulti;

                //mContext.mTrayMgr->showCursor();

                mMenuMultiMode = std::make_shared<MenuMultiMode>(mContext, controller, multiplayerSessionStartInfo);
                mIsLoadPassed = false;
                mUIUnloader->show();
                mMenuMultiMode->initData(this);
                mUIUnloader->hide();
                mIsLoadPassed = true;
                mMenuMultiMode->initCamera();
            }
        }

        //main menu single -> race single
        if(mGameMode == ModeMenu && mIsSwitchMode && mGameModeNext == ModeRaceSingle)
        {
            mIsSwitchMode = false;

            mGameMode = mGameModeNext;

            //mContext.mTrayMgr->hideCursor();

            mPlayerMode = std::make_shared<SinglePlayerMode>(mContext);
            mIsLoadPassed = false;
            mUILoader->show(mContext, true);
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

            mPlayerMode = std::make_shared<SinglePlayerMode>(mContext);
            mIsLoadPassed = false;
            mUILoaderChampionship->show(championshipTrack, mContext, true);
            mPlayerMode->initData(this);
            mUILoaderChampionship->hide();
            mIsLoadPassed = true;
            mPlayerMode->initCamera();
        }

        //main menu single (timetrial) -> race timetrial
        if(mGameMode == ModeMenu && mIsSwitchMode && mGameModeNext == ModeRaceTimetrial)
        {
            mIsSwitchMode = false;

            mGameMode = mGameModeNext;

            //mContext.mTrayMgr->hideCursor();

            mPlayerMode = std::make_shared<SinglePlayerMode>(mContext);
            mIsLoadPassed = false;
            mUILoader->show(mContext, false);
            mPlayerMode->initData(this);
            mUILoader->hide();
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

        //main menu multi -> race multi
        if(mGameMode == ModeMenuMulti && mIsSwitchMode && mGameModeNext == ModeRaceMulti)
        {
            mIsSwitchMode = false;

            mGameMode = mGameModeNext;

            //mContext.mTrayMgr->hideCursor();

            mPlayerMode = std::make_shared<MultiPlayerMode>(mContext, controller);
            mIsLoadPassed = false;
            mUILoader->show(mContext, true);
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

            mContext.getGameState().setRaceParameters("desert track", mContext.getGameState().getAIStrength());
            mContext.getGameState().setAICountInRace(mContext.getGameState().getAICount());

            mIsInitialLoadPassed = false;//to disable unloader progress
            mMenuMode = std::make_shared<MenuMode>(mContext, ModeMenu, State_SingleMulti);
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
            mMenuMultiMode = std::make_shared<MenuMultiMode>(mContext);
            mMenuMultiMode->initData(this);
            mMenuMultiMode->initCamera();

            //unable to enter to lobby
            //recreate main menu single
            if(!mMenuMultiMode->getIsLobbyEntered())
            {
                clear();
                mGameMode = ModeMenu;
                mMenuMode = std::make_shared<MenuMode>(mContext, ModeMenu, State_Multi);
                mMenuMode->initData(this);
                mMenuMode->initCamera();
            }
            mIsInitialLoadPassed = true;
        }
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

void GameModeSwitcher::nitroByPlayer()
{
    if(mPlayerMode.get())
        mPlayerMode->nitroByPlayer();
}

void GameModeSwitcher::cheatByPlayer(bool isEnabled)
{
    if(mPlayerMode.get())
        mPlayerMode->cheatByPlayer(isEnabled);
}

void GameModeSwitcher::tabPressed()
{
    if(mGameMode == ModeRaceMulti)
    {
        static_cast<MultiPlayerMode *>(mPlayerMode.get())->tabPressed();
    }
}

void GameModeSwitcher::keyUp(MyGUI::KeyCode _key, wchar_t _char )
{
    if(mMenuMode.get())
        mMenuMode->keyUp(_key, _char);

    if(mMenuMultiMode.get())
        mMenuMultiMode->keyUp(_key, _char);

    if(mGameMode == ModeRaceMulti)
    {
        static_cast<MultiPlayerMode *>(mPlayerMode.get())->keyUp(_key, _char);
    }
}

void GameModeSwitcher::mousePressed(const Ogre::Vector2& pos)
{
    if(mMenuMode.get())
        mMenuMode->mousePressed(pos);

    if(mMenuMultiMode.get())
        mMenuMultiMode->mousePressed(pos);

    if(mPlayerMode.get())
        mPlayerMode->mousePressed(pos);
}

void GameModeSwitcher::mouseReleased(const Ogre::Vector2& pos, OIS::MouseButtonID id)
{
    if(mMenuMode.get())
        mMenuMode->mouseReleased(pos, id);

    if(mMenuMultiMode.get())
        mMenuMultiMode->mouseReleased(pos, id);

    if(mPlayerMode.get())
        mPlayerMode->mouseReleased(pos);
}

void GameModeSwitcher::mouseMoved(const Ogre::Vector2& pos)
{
    if(mMenuMode.get())
        mMenuMode->mouseMoved(pos);

    if(mMenuMultiMode.get())
        mMenuMultiMode->mouseMoved(pos);

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

    if(mMenuMultiMode.get())
        mMenuMultiMode->reloadTextures();

    if(mPlayerMode.get())
        mPlayerMode->reloadTextures();
}
#endif

SinglePlayerMenuStates GameModeSwitcher::getSubmenuState() const
{
    SinglePlayerMenuStates state(State_SingleMulti);

    if(mMenuMode.get())
        state = mMenuMode->getSubmenuState();

    return state;
}

bool GameModeSwitcher::isExitSubmenu()const
{
    bool ret = true;

    if(mMenuMode.get())
        ret = mMenuMode->isExitSubmenu();

    if(mMenuMultiMode.get())
        ret = mMenuMultiMode->isExitSubmenu();

    return ret;
}

void GameModeSwitcher::setSubmenu(const std::string& title)
{
    if(mMenuMode.get())
        mMenuMode->setSubmenu(title);

    if(mMenuMultiMode.get())
        mMenuMultiMode->setSubmenu(title);
}

void GameModeSwitcher::setTopmostSubmenu()
{
    if(mMenuMode.get())
        mMenuMode->setTopmostSubmenu();

    if(mMenuMultiMode.get())
        mMenuMultiMode->setTopmostSubmenu();
}

void GameModeSwitcher::setDefaultBackground(bool isSwitchState)
{
    if(mMenuMode.get())
        mMenuMode->setDefaultBackground(isSwitchState);

    if(mMenuMultiMode.get())
        mMenuMultiMode->setDefaultBackground(isSwitchState);
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

    if(mMenuMultiMode.get())
        mMenuMultiMode->clearData();
    mMenuMultiMode.reset();

    if(mPlayerMode.get())
        mPlayerMode->clearData();
    mPlayerMode.reset();
}

void GameModeSwitcher::loadState(float percent, const std::string& info)
{
    //consume input events
    if(mIsInitialLoadPassed)
        mContext.mInputHandler->capture();

    if(mGameMode == ModeRaceSingle || mGameMode == ModeRaceTimetrial || mGameMode == ModeRaceMulti)
    {
        mUILoader->setPercent(percent, info);
    }

    if(mGameMode == ModeRaceChampionship)
    {
        mUILoaderChampionship->setPercent(percent, info);
    }

    if(mGameMode == ModeMenu || mGameMode == ModeMenuChampionship || mGameMode == ModeMenuTimetrial || mGameMode == ModeMenuMulti)
    {
        if(mIsInitialLoadPassed)
            mUIUnloader->setPercent(percent, info);
    }
}