
#include "UIMainMenu.h"

#include "../gamelogic/GameModeSwitcher.h"

#include "../listeners/LoaderListener.h"

#include "../tools/OgreTools.h"
#include "../tools/Conversions.h"

#include "../customs/CustomTrayManager.h"

#include "../loaders/TextureLoader.h"

#include "../gamemodes/MenuMode.h"

#include "../BaseApp.h"

#include "../multiplayer/MultiplayerRoomInfo.h"

#if defined(__ANDROID__)
    #include <android/keycodes.h>
#endif

namespace{
#if !defined(__ANDROID__)
    const Ogre::Real buttonSize = 12.0f;
    const Ogre::Real buttonLeftAdj = 0.0f;
    const Ogre::Real buttonTopAdj = 0.0f;
#else
    const Ogre::Real buttonSize = 18.0f;
    const Ogre::Real buttonLeftAdj = -3.0f;
    const Ogre::Real buttonTopAdj = -3.0f;
#endif
}

UIMainMenu::UIMainMenu(const ModeContext& modeContext, const GameMode gameMode, MenuMode * menuMode, SinglePlayerMenuStates state)
    : UIMainMenuLabels(modeContext, gameMode),
    mIsInStartingGrid(false),
    mMenuMode(menuMode),
    mCurrentState(state),
    mEditBoxUserName(20),
    mEditBoxMultiUserName(20),
    mEditBoxMultiRoomName(20)
{}

UIMainMenu::~UIMainMenu()
{}

void UIMainMenu::loadMisc(const PFLoader& pfLoaderData, const PFLoader& pfLoaderGameshell, LoaderListener* loaderListener)
{
    loadCommonTextures(pfLoaderGameshell);

    createBackgroundTextures(pfLoaderGameshell, loaderListener);

    if(loaderListener)
        loaderListener->loadState(1.0f, "all loaded");
}

void UIMainMenu::onButtonReleased(UIButton * button)
{
    UIMainMenuLabels::onButtonReleased(button);

    if(button == &mRoomsMoveTop)
    {
        if(mTableOffset > 0)
        {
            --mTableOffset;
            redrawTable();
        }
    }

    if(button == &mRoomsMoveBottom)
    {
        if(!mRoomNames.empty())
        {
            if(mTableOffset + GameState::mRaceGridCarsMax < mRoomNames.size())
            {
                ++mTableOffset;
                redrawTable();
            }
        }
    }
}

void UIMainMenu::onButtonOver(UIButton * button)
{
    UIMainMenuLabels::onButtonOver(button);
}

void UIMainMenu::load(CustomTrayManager* trayMgr, const GameState& gameState, LoaderListener* loaderListener)
{
    Ogre::OverlayManager& om = Ogre::OverlayManager::getSingleton(); 
    Ogre::Real viewportWidth = om.getViewportWidth(); 
    Ogre::Real viewportHeight = om.getViewportHeight(); 

    Ogre::Matrix4 screenAdaptionRelative(
        viewportWidth / 640.0f, 0.0f,                       0.0f,                   0.0f,
        0.0f,                   viewportHeight / 480.0f,    0.0f,                   0.0f,
        0.0f,                   0.0f,                       viewportWidth / 640.0f, 0.0f,
        0.0f,                   0.0f,                       0.0f,                   viewportHeight / 480.0f);

    loadMisc(gameState.getPFLoaderData(), gameState.getPFLoaderGameshell(), loaderListener);

    Ogre::LogManager::getSingleton().logMessage(Ogre::LML_NORMAL, "[UIMainMenu::load]: viewport [" + Conversions::DMToString(viewportWidth) + "x" + Conversions::DMToString(viewportHeight) + "]");

    createCommonMaterials();
    createBackgroundMaterials();


    createBackgroundUI(screenAdaptionRelative, trayMgr);//init mMainBackground for further usage
    createLabels(screenAdaptionRelative);
    createControls(screenAdaptionRelative, getMainBackground());

    //mEditBoxUserName.loadBackground(gameState.getPFLoaderGameshell(), "session.bmp");
    mEditBoxUserName.setBackgroundMaterial("Test/CustomBackgroundBlackTransparent");
    //mEditBoxUserName.loadBackground("OriginalEditBox");
    mEditBoxUserName.init(screenAdaptionRelative, getMainBackground(), Ogre::Vector4(205.0f, 193.0f, 260.0f, 50.0f), 106.0f);
    mEditBoxUserName.setText(mModeContext.getGameState().getPlayerName());

    //mEditBoxMultiIP.setBackgroundMaterial(mEditBoxUserName.getMaterialName());
    mEditBoxMultiIP.loadBackground("OriginalEditBox");
    mEditBoxMultiIP.init(screenAdaptionRelative, getMainBackground(), Ogre::Vector4(20.0f, 215.0f, 170.0f, 18.0f), 36.0f);
    mEditBoxMultiIP.setCharType(UIEditBox::IP);
    mEditBoxMultiIP.setText("78.47.85.155");//d.polubotko: FIXME

    mEditBoxMultiUserName.setBackgroundMaterial(mEditBoxMultiIP.getMaterialName());
    mEditBoxMultiUserName.init(screenAdaptionRelative, getMainBackground(), Ogre::Vector4(20.0f, 270.0f, 170.0f, 18.0f), 36.0f);
    mEditBoxMultiUserName.setText(mModeContext.getGameState().getPlayerName());

    mEditBoxMultiRoomName.setBackgroundMaterial(mEditBoxMultiIP.getMaterialName());
    mEditBoxMultiRoomName.init(screenAdaptionRelative, getMainBackground(), Ogre::Vector4(20.0f, 320.0f, 170.0f, 18.0f), 36.0f);
    mEditBoxMultiRoomName.setText("Powerslide!");

    {
        mMultiRoomsList.loadBackground(true);
        mMultiRoomsList.init(screenAdaptionRelative, getMainBackground(), Ogre::Vector4(330.0f, 73.0f, 310.0f, 22.0f), 36.0f);
        mMultiRoomsList.setTableOnAction(this);
    }

    {
        mMultiRoomPlayersList.loadBackground(false);
        mMultiRoomPlayersList.init(screenAdaptionRelative, getMainBackground(), Ogre::Vector4(24.0f, 58.0f, 250.0f, 22.0f), 36.0f);
    }

    {
        mRoomsMoveTop.loadBackground("OriginalButtonSBUp");
        mRoomsMoveTop.init(screenAdaptionRelative, getMainBackground(), Ogre::Vector4(600.0f + buttonLeftAdj, 50.0f + buttonTopAdj, buttonSize, buttonSize), true);
        mRoomsMoveTop.setButtonOnAction(this);

        mRoomsMoveBottom.loadBackground("OriginalButtonSBDown");
        mRoomsMoveBottom.init(screenAdaptionRelative, getMainBackground(), Ogre::Vector4(600.0f + buttonLeftAdj, 340.0f + buttonTopAdj, buttonSize, buttonSize), true);
        mRoomsMoveBottom.setButtonOnAction(this);
    }

    selectTrack(mModeContext.mGameState.getTrackNameAsOriginal());

    const STRPowerslide& strPowerslide = mModeContext.getGameState().getSTRPowerslide();
    std::string characterCar = strPowerslide.getCarFromCharacter(mModeContext.getGameState().getPlayerCar().getCharacterName());
    characterCar = strPowerslide.getBaseCarFromCar(characterCar);
    selectCar(characterCar);

    switchState(mCurrentState);

    if(mGameModeSelected == ModeMenuChampionship)
    {
        for(size_t q = 0; q < mControlsCount - 2; ++q)
            setControlActive(q, false);
    }
}

#if defined(__ANDROID__)
void UIMainMenu::reloadTextures(const GameState& gameState)
{
    loadMisc(gameState.getPFLoaderData(), gameState.getPFLoaderGameshell(), NULL);
}
#endif

void UIMainMenu::frameStarted(const Ogre::FrameEvent &evt)
{
    if(mCurrentState == State_StartingGrid)
    {
        unsigned long time = mStartingGridTimer.getMilliseconds();
        const unsigned long threshold = 5000; //ms
        setStartingGridTime(Conversions::DMToString((threshold - time + 999) / 1000));
        if(time >= threshold)
        {
            startRace();
        }
    }

    mEditBoxUserName.frameStarted(evt);
    mEditBoxMultiIP.frameStarted(evt);
    mEditBoxMultiUserName.frameStarted(evt);
    mEditBoxMultiRoomName.frameStarted(evt);

    if(mCurrentState == State_Options_Trophies_Video)
    {
        if(!mVideoPlayer.isFinished())
        {
            mVideoPlayer.frameStarted(evt);
        }
        else
        {
            if(!mVideoTitleLabel->isVisible())
            {
                std::string award = "";

                if(mVideoIndex < Championship::mEveryWinnerFruitOffset)
                {
                    award = Championship::getAwardString(mVideoIndex, mModeContext, 0);
                }

                if(mVideoIndex >= Championship::mEveryWinnerFruitOffset && mVideoIndex < Championship::mExpertCarFruitOffset)
                {
                    award = Championship::getAwardString(mVideoIndex, mModeContext, 0);
                }

                if(mVideoIndex >= Championship::mExpertCarFruitOffset && mVideoIndex < Championship::mInsaneCarFruitOffset)
                {
                    award = Championship::getAwardString(8, mModeContext, mVideoIndex - Championship::mExpertCarFruitOffset);
                }

                if(mVideoIndex >= Championship::mInsaneCarFruitOffset && mVideoIndex < Championship::mBeatEmergentGunFruitOffset)
                {
                    award = Championship::getAwardString(9, mModeContext, mVideoIndex - Championship::mInsaneCarFruitOffset);
                }

                if(mVideoIndex >= Championship::mBeatEmergentGunFruitOffset && mVideoIndex < Championship::mBrusselFruitOffset)
                {
                    award = Championship::getAwardString(10, mModeContext, 0, mVideoIndex - Championship::mBeatEmergentGunFruitOffset);
                }

                if(mVideoIndex == Championship::mBrusselFruitOffset)
                {
                    award = Championship::getAwardString(11, mModeContext, 0);
                }

                mVideoTitleLabel->getTextArea()->setCaption(award);
                mVideoTitleLabel->show();
            }
        }
    }

    if (mCurrentState == State_Options_Credits_Video)
    {
        if (!mVideoPlayer.isFinished())
        {
            mVideoPlayer.frameStarted(evt);
        }
        else
        {
            if (!mLabelCredits->isVisible())
            {
                mLabelCredits->show();
                setMainBackgroundMaterial("Test/CustomBackgroundBlack");
            }
        }
    }

    if (mCurrentState == State_Options_About_Video)
    {
        if (!mVideoPlayer.isFinished())
        {
            mVideoPlayer.frameStarted(evt);
        }
        else
        {
            if (!mLabelAbout->isVisible())
            {
                mLabelAbout->show();
                setMainBackgroundMaterial("Test/MainBackgroundInit");
            }
        }
    }
}

void UIMainMenu::keyUp(MyGUI::KeyCode _key, wchar_t _char)
{
    UIMainMenuLabels::keyUp(_key, _char);

    mEditBoxUserName.keyUp(_key, _char);
    mEditBoxMultiIP.keyUp(_key, _char);
    mEditBoxMultiUserName.keyUp(_key, _char);
    mEditBoxMultiRoomName.keyUp(_key, _char);

    if(mEditBoxMultiIP.isActive())
    {
        mEditBoxMultiIP.setColor(Ogre::ColourValue::White);
        setColorMultiIP(UILabel::mInactiveLabel);
    }

    if(mEditBoxMultiUserName.isActive())
        setColorMultiUserName(UILabel::mInactiveLabel);

    if(mEditBoxMultiRoomName.isActive())
        setColorMultiRoomName(UILabel::mInactiveLabel);

    if(mCurrentState == State_Options_Name)
    {
#if !defined(__ANDROID__)
        if(_key == MyGUI::KeyCode::Return)
        {
            onNameChange();
        }
#else
        if(_key == MyGUI::KeyCode::Enum(AKEYCODE_ENTER))
        {
            mModeContext.getBaseApp()->androidHideKeyboard();
            onNameChange();
        }
#endif
    }

    if(mCurrentState == State_Options_Trophies_Video)
    {
#if !defined(__ANDROID__)
        if(_key == MyGUI::KeyCode::Return)
        {
            switchState(State_Options_Trophies);
        }
        if(_key == MyGUI::KeyCode::Space)
        {
            mVideoTitleLabel->hide();
            mVideoPlayer.restart();
        }
#endif
    }

    if (mCurrentState == State_Options_Credits_Video|| mCurrentState == State_Options_About_Video)
    {
#if !defined(__ANDROID__)
        if (_key == MyGUI::KeyCode::Return)
        {
            switchState(State_Options);
        }
        if (_key == MyGUI::KeyCode::Space)
        {
            setMainBackgroundMaterial("Test/VideoTexture");
            mLabelCredits->hide();
            mLabelAbout->hide();
            mVideoPlayer.restart();
        }
#endif
    }
}

void UIMainMenu::mousePressed(const Ogre::Vector2& pos)
{
    UIMainMenuLabels::mousePressed(pos);

    mEditBoxUserName.mouseReleased(pos);
    mEditBoxMultiIP.mouseReleased(pos);
    mEditBoxMultiUserName.mouseReleased(pos);
    mEditBoxMultiRoomName.mouseReleased(pos);
    mMultiRoomsList.mousePressed(pos);
    mRoomsMoveTop.mousePressed(pos);
    mRoomsMoveBottom.mousePressed(pos);

#if defined(__ANDROID__)
    bool isAnyActive =  mEditBoxUserName.isActive() |
                        mEditBoxMultiIP.isActive() |
                        mEditBoxMultiUserName.isActive() |
                        mEditBoxMultiRoomName.isActive();

    if(isAnyActive)
    {
        mModeContext.getBaseApp()->androidShowKeyboard();
    }
    else
    {
        mModeContext.getBaseApp()->androidHideKeyboard();
    }
#endif
}

void UIMainMenu::mouseReleased(const Ogre::Vector2& pos, OIS::MouseButtonID id)
{
    SinglePlayerMenuStates prevState = mCurrentState;

    UIMainMenuLabels::mouseReleased(pos, id);

    mMultiRoomsList.mouseReleased(pos);
    mRoomsMoveTop.mouseReleased(pos);
    mRoomsMoveBottom.mouseReleased(pos);

    if(mCurrentState == State_Options_Trophies_Video && mCurrentState == prevState)
    {
        switchState(State_Options_Trophies);
    }

    if (mCurrentState == State_Options_Credits_Video && mCurrentState == prevState)
    {
        switchState(State_Options);
    }

    if (mCurrentState == State_Options_About_Video && mCurrentState == prevState)
    {
        switchState(State_Options);
    }
}

void UIMainMenu::mouseMoved(const Ogre::Vector2& pos)
{
    UIMainMenuLabels::mouseMoved(pos);

    mMultiRoomsList.mouseMoved(pos);
    mRoomsMoveTop.mouseMoved(pos);
    mRoomsMoveBottom.mouseMoved(pos);
}

bool UIMainMenu::isExitSubmenu()const
{
    return mCurrentState == State_ExitGame;
}

void UIMainMenu::setSubmenu(const std::string& title)
{
    mExitSubmenuTitle = title;
    switchState(State_ExitGame);
}

void UIMainMenu::setTopmostSubmenu()
{
    mGameModeSelected = ModeMenu;

    for(size_t q = 0; q < mControlsCount - 2; ++q)
        setControlActive(q, true);

    switchState(State_SingleMulti);

    selectMode();
    selectTrack(mModeContext.mGameState.getTrackNameAsOriginal());
}

void UIMainMenu::setPodiumSubmenu()
{
    switchState(State_Podium);
}

void UIMainMenu::destroy(CustomTrayManager* trayMgr)
{
    UIMainMenuLabels::destroy(trayMgr);

    mEditBoxUserName.destroy(trayMgr);
    mEditBoxMultiIP.destroy(trayMgr);
    mEditBoxMultiUserName.destroy(trayMgr);
    mEditBoxMultiRoomName.destroy(trayMgr);
    mMultiRoomsList.destroy(trayMgr);
    mMultiRoomPlayersList.destroy(trayMgr);
    mRoomsMoveTop.destroy(trayMgr);
    mRoomsMoveBottom.destroy(trayMgr);
}

void UIMainMenu::onTableReleased(UITable * table, size_t row)
{
    mMultiRoomPlayersList.clear();
    mMultiRoomPlayersList.setText(mRoomNames[row + mTableOffset], 0, Ogre::ColourValue::White);
    mMultiRoomPlayersList.setText("Host: " + mRoomsHosts[row + mTableOffset], 1, Ogre::ColourValue::Black);
    mMultiRoomPlayersList.setText("Desc: " + mRoomsDescriptions[row + mTableOffset], 2, Ogre::ColourValue::White);
    mMultiRoomPlayersList.setText("Human: " + Conversions::DMToString(mPlayersInServerRooms[row + mTableOffset].first), 3, Ogre::ColourValue::Black);
    mMultiRoomPlayersList.setText("AI: " + Conversions::DMToString(mPlayersInServerRooms[row + mTableOffset].second), 4, Ogre::ColourValue::White);
    size_t availSlots = GameState::mRaceGridCarsMax - mPlayersInServerRooms[row + mTableOffset].first - mPlayersInServerRooms[row + mTableOffset].second;
    if(availSlots > 0)
        mMultiRoomPlayersList.setText("Free Slots: " + Conversions::DMToString(availSlots), 5, Ogre::ColourValue::Black);
    else
        mMultiRoomPlayersList.setText("Free Slots: " + Conversions::DMToString(availSlots), 5, Ogre::ColourValue::Red);

    if(availSlots > 0)
        showMultiJoin();
    else
        hideMultiJoin();

    mSelectedRoomIndex = row;
}

void UIMainMenu::panelHit(Ogre::PanelOverlayElement* panel)
{

    if(panel != NULL && panel->getName() == "Mode")
    {
        switchState(State_SingleMulti);
        selectMode();
    }

    if(panel != NULL && panel->getName() == "Track")
        switchState(State_Track);

    if(panel != NULL && panel->getName() == "Car")
        switchState(State_Car);

    if(panel != NULL && panel->getName() == "Option")
        switchState(State_Options);

    if(panel != NULL && panel->getName() == "Race")
    {
        if(mGameModeSelected == ModeMenu)
        {
            if(!mModeContext.getGameState().isAITrack())
            {
                mModeContext.getGameState().setAICountInRace(0);
            }
            else
            {
                mModeContext.getGameState().setAICountInRace(mModeContext.getGameState().getAICount());
            }
            switchState(State_StartingGrid);
        }

        if(mGameModeSelected == ModeMenuTimetrial)
        {
            mModeContext.getGameState().setAICountInRace(0);
            startRace();
        }

        //first step in championship
        if(mGameModeSelected == ModeMenuChampionship && mModeContext.getGameModeSwitcher()->getMode() != ModeMenuChampionship)
        {
            mModeContext.getGameState().setAICountInRace(mModeContext.getGameState().getAICount());
            mModeContext.getGameState().getChampionship().init();
            switchState(State_StartingGrid);
        }

        //next steps in championship
        if(mGameModeSelected == ModeMenuChampionship && mModeContext.getGameModeSwitcher()->getMode() == ModeMenuChampionship)
        {
            if(mCurrentState == State_Leaderboard)
            {
                if(!mModeContext.getGameState().getChampionship().isFinished())
                {
                    switchState(State_StartingGrid);
                }
                else
                {
                    mModeContext.getGameModeSwitcher()->switchMode(ModeMenu);
                    setTopmostSubmenu();
                }
            }
            else
            {
                if(!mModeContext.getGameState().getChampionship().getIsShownLeaderboard())
                {
                    mModeContext.getGameState().getChampionship().setShownLeaderboard();
                    switchState(State_Leaderboard);
                }
                else
                {
                    switchState(State_StartingGrid);
                }
            }
        }
    }

    if(panel != NULL && panel->getName() == "Exit")
    {
        mModeContext.mBaseApp->setShutdown(true);
    }
}

void UIMainMenu::playVideo(size_t fruitIndex)
{
    mVideoIndex = fruitIndex;
    switchState(State_Options_Trophies_Video);
}

void UIMainMenu::startRace()
{
    if(mGameModeSelected == ModeMenuChampionship)
    {
        mModeContext.getGameModeSwitcher()->switchMode(ModeRaceChampionship);
    }
    else if(mGameModeSelected == ModeMenuTimetrial)
    {
        mModeContext.getGameModeSwitcher()->switchMode(ModeRaceTimetrial);
    }
    else
    {
        mModeContext.getGameModeSwitcher()->switchMode(ModeRaceSingle);
    }
}

void UIMainMenu::onNameChange()
{
    setDefaultBackground(false);

    std::string userName = mEditBoxUserName.getText().asUTF8();
    if(userName != mModeContext.getGameState().getPlayerName())
    {
        mModeContext.getGameState().setPlayerName(userName);
        mModeContext.getGameState().loadPlayerData();
        mModeContext.getGameState().savePlayerData();//save selected player
        switchState(State_SingleMulti);
        selectMode();
        selectTrack(mModeContext.mGameState.getTrackNameAsOriginal());
        const STRPowerslide& strPowerslide = mModeContext.getGameState().getSTRPowerslide();
        std::string characterCar = strPowerslide.getCarFromCharacter(mModeContext.getGameState().getPlayerCar().getCharacterName());
        characterCar = strPowerslide.getBaseCarFromCar(characterCar);
        selectCar(characterCar);
    }
    else
    {
        switchState(State_SingleMulti);
        selectMode();
    }
}

void UIMainMenu::setDefaultBackground(bool isSwitchState)
{
    setMainBackgroundMaterial("Test/MainBackground");
    
    for(size_t q = 0; q < mControlsCount; ++q)
    {
        setControlShow(q, true);
    }

    if(isSwitchState)
    {
        switchState(State_Options);
    }
}

void UIMainMenu::switchState(const SinglePlayerMenuStates& state)
{
    hideAll();

    switch(state)
    {
    case State_SingleMulti:
        mGameModeSelected = ModeMenu;
        mIsInStartingGrid = false;
        setWindowTitle("Game Mode");
        showModeSingleMulti();
        showBackgrounds();
        break;

    case State_Multi:
        mIsInStartingGrid = false;
        setWindowTitle("Rooms");
        mMultiRoomsList.show(GameState::mRaceGridCarsMax);
        mRoomsMoveTop.show();
        mRoomsMoveBottom.show();
        mMultiRoomPlayersList.show(6);
        showMultiIPLabels();
        mEditBoxMultiUserName.setText(mModeContext.getGameState().getPlayerName());
        mEditBoxMultiIP.show();
        mEditBoxMultiUserName.show();
        mEditBoxMultiRoomName.show();
        break;

    case State_MultiCreateRoom:
        createRoom();
        break;

    case State_MultiJoinRoom:
        joinRoom();
        break;

    case State_SingleType:
        mIsInStartingGrid = false;
        setWindowTitle("Game Mode");
        showModeSingle();
        showModeSingleType();
        showBackgrounds();
        break;

    case State_Difficulty:
        mIsInStartingGrid = false;
        setWindowTitle("Game Mode");
        showModeSingle();
        showModeDifficulty();
        showBackgrounds();
        break;

    case State_Track:
        mIsInStartingGrid = false;
        showBackgroundTrack();
        setWindowTitle("Select Track");
        showTrackLabels();
        setCurrentTrackLogo();
        setCurrentTrackBestTime();
        setCurrentTrackDescription();
        break;

    case State_Car:
        mIsInStartingGrid = false;
        showBackgroundCar();
        setWindowTitle("Select Car");
        showCarLabels();
        setCurrentCarLogo();
        break;

    case State_Character:
        mIsInStartingGrid = false;
        showBackgroundCharacter();
        setWindowTitle("Select Character");
        showCharacterLabels();
        setCurrentBioDescription();
        break;

    case State_Options:
        mIsInStartingGrid = false;
        setDefaultBackground(false);
        setWindowTitle("Options: Graphics");
        showOptionLabels();
        showOptionGraphicsLabels();
        break;

    case State_Options_Input:
        mIsInStartingGrid = false;
        showOptionLabels();
        showOptionInputLabels();
        break;

    case State_Options_Sound:
        mIsInStartingGrid = false;
        showOptionLabels();
        showOptionSoundLabels();
        break;

    case State_Options_Race:
        mIsInStartingGrid = false;
        showOptionLabels();
        showOptionRaceLabels();
        break;

    case State_Options_Scores:
        mIsInStartingGrid = false;
        showOptionLabels();
        showOptionHiscoreLabels();
        break;

    case State_Options_Name:
        mIsInStartingGrid = false;
        setMainBackgroundMaterial("Test/NameBackground");
        for(size_t q = 0; q < mControlsCount; ++q)
        {
            setControlShow(q, false);
        }
        setWindowTitle("");
        mEditBoxUserName.setText(mModeContext.getGameState().getPlayerName());
        mEditBoxUserName.setActive(true);
        mEditBoxUserName.show();
#if defined(__ANDROID__)
        mModeContext.getBaseApp()->androidShowKeyboard();
#endif
        break;

    case State_Options_Trophies:
        mIsInStartingGrid = false;
        setMainBackgroundMaterial("Test/TrophiesBackground");
        showBackgroundFruis(mModeContext.getGameState().getPlayerData());
        for(size_t q = 0; q < mControlsCount; ++q)
        {
            setControlShow(q, false);
        }
        showOptionTrophies();
        setWindowTitle("");
        break;

    case State_Options_Trophies_Video:
        mIsInStartingGrid = false;
        setMainBackgroundMaterial("Test/VideoTexture");
        for(size_t q = 0; q < mControlsCount; ++q)
        {
            setControlShow(q, false);
        }
        setWindowTitle("");
        switch(mVideoIndex)
        {
            case Championship::mWinnerFruitOffset + 0:
                mVideoPlayer.init(mModeContext.getGameState().getPFLoaderGameshell(), "data/gameshell", "apple.avi", "VideoTexture");
            break;

            case Championship::mWinnerFruitOffset + 1:
                mVideoPlayer.init(mModeContext.getGameState().getPFLoaderGameshell(), "data/gameshell", "banana.avi", "VideoTexture");
            break;

            case Championship::mWinnerFruitOffset + 2:
                mVideoPlayer.init(mModeContext.getGameState().getPFLoaderGameshell(), "data/gameshell", "orange.avi", "VideoTexture");
            break;

            case Championship::mWinnerFruitOffset + 3:
                mVideoPlayer.init(mModeContext.getGameState().getPFLoaderGameshell(), "data/gameshell", "sberry.avi", "VideoTexture");
            break;

            case Championship::mEveryWinnerFruitOffset + 0:
                mVideoPlayer.init(mModeContext.getGameState().getPFLoaderGameshell(), "data/gameshell", "tangelo.avi", "VideoTexture");
            break;

            case Championship::mEveryWinnerFruitOffset + 1:
                mVideoPlayer.init(mModeContext.getGameState().getPFLoaderGameshell(), "data/gameshell", "peach.avi", "VideoTexture");
            break;

            case Championship::mEveryWinnerFruitOffset + 2:
                mVideoPlayer.init(mModeContext.getGameState().getPFLoaderGameshell(), "data/gameshell", "passion.avi", "VideoTexture");
            break;

            case Championship::mEveryWinnerFruitOffset + 3:
                mVideoPlayer.init(mModeContext.getGameState().getPFLoaderGameshell(), "data/gameshell", "cherry.avi", "VideoTexture");
            break;

            case Championship::mExpertCarFruitOffset + 0:
            case Championship::mExpertCarFruitOffset + 1:
            case Championship::mExpertCarFruitOffset + 2:
            case Championship::mExpertCarFruitOffset + 3:
            case Championship::mExpertCarFruitOffset + 4:
            case Championship::mExpertCarFruitOffset + 5:
            case Championship::mExpertCarFruitOffset + 6:
                mVideoPlayer.init(mModeContext.getGameState().getPFLoaderGameshell(), "data/gameshell", "papaya.avi", "VideoTexture");
            break;

            case Championship::mInsaneCarFruitOffset + 0:
            case Championship::mInsaneCarFruitOffset + 1:
            case Championship::mInsaneCarFruitOffset + 2:
            case Championship::mInsaneCarFruitOffset + 3:
            case Championship::mInsaneCarFruitOffset + 4:
            case Championship::mInsaneCarFruitOffset + 5:
            case Championship::mInsaneCarFruitOffset + 6:
                mVideoPlayer.init(mModeContext.getGameState().getPFLoaderGameshell(), "data/gameshell", "mango.avi", "VideoTexture");
            break;

            case Championship::mBeatEmergentGunFruitOffset + 0:
            case Championship::mBeatEmergentGunFruitOffset + 1:
            case Championship::mBeatEmergentGunFruitOffset + 2:
            case Championship::mBeatEmergentGunFruitOffset + 3:
            case Championship::mBeatEmergentGunFruitOffset + 4:
            case Championship::mBeatEmergentGunFruitOffset + 5:
            case Championship::mBeatEmergentGunFruitOffset + 6:
            case Championship::mBeatEmergentGunFruitOffset + 7:
                mVideoPlayer.init(mModeContext.getGameState().getPFLoaderGameshell(), "data/gameshell", "rberry.avi", "VideoTexture");
            break;

            case Championship::mBrusselFruitOffset + 0:
                mVideoPlayer.init(mModeContext.getGameState().getPFLoaderGameshell(), "data/gameshell", "brussel.avi", "VideoTexture");
            break;

            default:
                mVideoPlayer.init(mModeContext.getGameState().getPFLoaderGameshell(), "data/gameshell", "apple.avi", "VideoTexture");
        }
        mVideoPlayer.start();
        break;

    case State_Options_Credits_Video:
        mIsInStartingGrid = false;
        setMainBackgroundMaterial("Test/VideoTexture");
        for (size_t q = 0; q < mControlsCount; ++q)
        {
            setControlShow(q, false);
        }
        setWindowTitle("");
        mVideoPlayer.init(mModeContext.getGameState().getPFLoaderGameshell(), "data/gameshell", "ratbag.avi", "VideoTexture");
        mVideoPlayer.start();
        break;

    case State_Options_About_Video:
        mIsInStartingGrid = false;
        setMainBackgroundMaterial("Test/VideoTexture");
        for (size_t q = 0; q < mControlsCount; ++q)
        {
            setControlShow(q, false);
        }
        setWindowTitle("");
        mVideoPlayer.init(mModeContext.getGameState().getPFLoaderGameshell(), "data/gameshell", "intro.avi", "VideoTexture");
        mVideoPlayer.start();
        break;


    case State_StartingGrid:
        if(mIsInStartingGrid)
        {
            mIsInStartingGrid = false;
            startRace();
            break;
        }
        mIsInStartingGrid = true;
        {
            std::vector<std::string> playersCharacters;
            playersCharacters.push_back(mModeContext.getGameState().getPlayerCar().getCharacterName());
            mMenuMode->recalculateCharacterNames(playersCharacters);
        }
        showBackgroundCharacterSmall();
        showRaceGridCharactersLabels();
        mStartingGridTimer.reset();
        setWindowTitle("Starting Grid");
        if(mGameModeSelected == ModeMenuChampionship)
        {
            setWindowTitle("Starting Grid Championship");
        }
        showStartingGridTimer();
        break;

    case State_Podium:
        mIsInStartingGrid = false;
        showBackgroundFirstSecondThird();
        {
            const finishBoardVec& finishBoard = mModeContext.getFinishBoard();
            showBackgroundCharacterSmallPodium(finishBoard);
            showPodiumLabels(finishBoard);
        }
        setWindowTitle("Podium");
        break;

    case State_Leaderboard:
        mIsInStartingGrid = false;
        {
            finishBoardVec leaderBoard = mModeContext.getGameState().getChampionship().getLeaderboard();
            showLeaderboardLabels(leaderBoard);
        }
        setWindowTitle("Leaderboard");
        break;

    case State_FinishChampionship:
        mIsInStartingGrid = false;
        setMainBackgroundMaterial("Test/EndChampionshipBackground");
        for(size_t q = 0; q < mControlsCount; ++q)
        {
            setControlShow(q, false);
        }
        
        {
            Championship& champ = mModeContext.getGameState().getChampionship();
            std::vector<size_t> fruitsIndexes = champ.getAvailableFruits();
            size_t labelsOccupied = 0;

            const STRPowerslide& strPowerslide = mModeContext.getGameState().getSTRPowerslide();
            std::string characterCar = strPowerslide.getCarFromCharacter(mModeContext.getGameState().getPlayerCar().getCharacterName());
            characterCar = strPowerslide.getBaseCarFromCar(characterCar);
            std::vector<std::string> availCars = strPowerslide.getArrayValue("", "available cars");
            size_t carIndex = std::find(availCars.begin(), availCars.end(), characterCar) - availCars.begin();

            if(fruitsIndexes[0] < Championship::mEveryWinnerFruitOffset)//unlock happened
            {
                mChampionshipResultsLabel[0]->setCaption(champ.getAwardString(fruitsIndexes[0], mModeContext, carIndex));
                mChampionshipResultsLabel[1]->setCaption(champ.getUnlockedString(fruitsIndexes[0]));
                labelsOccupied += 2;
            }

            for(size_t q = 0; q < fruitsIndexes.size(); ++q)
            {
                if(fruitsIndexes[q] >= Championship::mEveryWinnerFruitOffset && fruitsIndexes[q] < Championship::mExpertCarFruitOffset)
                {
                    mChampionshipResultsLabel[labelsOccupied++]->setCaption(champ.getAwardString(fruitsIndexes[q], mModeContext, carIndex));
                }

                if(fruitsIndexes[q] >= Championship::mExpertCarFruitOffset && fruitsIndexes[q] < Championship::mInsaneCarFruitOffset)
                {
                    mChampionshipResultsLabel[labelsOccupied++]->setCaption(champ.getAwardString(8, mModeContext, carIndex));
                }

                if(fruitsIndexes[q] >= Championship::mInsaneCarFruitOffset && fruitsIndexes[q] < Championship::mBeatEmergentGunFruitOffset)
                {
                    mChampionshipResultsLabel[labelsOccupied++]->setCaption(champ.getAwardString(9, mModeContext, carIndex));
                }

                if(fruitsIndexes[q] >= Championship::mBeatEmergentGunFruitOffset && fruitsIndexes[q] < Championship::mBrusselFruitOffset)
                {
                    mChampionshipResultsLabel[labelsOccupied++]->setCaption(champ.getAwardString(10, mModeContext, carIndex, fruitsIndexes[q] - Championship::mBeatEmergentGunFruitOffset));
                }

                if(fruitsIndexes[q] == Championship::mBrusselFruitOffset)
                {
                    mChampionshipResultsLabel[labelsOccupied++]->setCaption(champ.getAwardString(11, mModeContext, carIndex));
                }
            }
        }
        showChampionshipResultsLabels();
        setWindowTitle("");
        break;

    case State_ExitGame:
        mIsInStartingGrid = false;
        showBackgroundExitSign();
        showExitLabels(mExitSubmenuTitle);
        setWindowTitle("");
        break;
    }

    mCurrentState = state;
}

void UIMainMenu::hideAll()
{
    hideAllBackgrounds();
    hideAllLabels();
    mEditBoxUserName.hide();
    mEditBoxMultiIP.hide();
    mEditBoxMultiUserName.hide();
    mEditBoxMultiRoomName.hide();
    mMultiRoomsList.hide();
    mMultiRoomPlayersList.hide();
    mRoomsMoveTop.hide();
    mRoomsMoveBottom.hide();
}

void UIMainMenu::redrawTable()
{
    hideMultiJoin();

    mMultiRoomsList.clear();
    mMultiRoomPlayersList.clear();

    std::string userName = mEditBoxMultiUserName.getText().asUTF8();
    std::string newRoomName = mEditBoxMultiRoomName.getText().asUTF8();

    for(size_t q = mTableOffset; q < mRoomNames.size(); ++q)
    {
        if(mRoomNames[q] == newRoomName)
        {
            setColorMultiRoomName(Ogre::ColourValue::Red);
        }

        if(mRoomsHosts[q] == userName)
        {
            setColorMultiUserName(Ogre::ColourValue::Red);
        }

        //available for join
        if((mPlayersInServerRooms[q].first + mPlayersInServerRooms[q].second) < GameState::mRaceGridCarsMax)
        {
            mMultiRoomsList.setText(mRoomNames[q], q - mTableOffset, Ogre::ColourValue::Green);
        }
        else //not available for join
        {
            mMultiRoomsList.setText(mRoomNames[q], q - mTableOffset, Ogre::ColourValue::Red);
        }
    }
}

void UIMainMenu::connectToServer()
{
    mTableOffset = 0;
#if 0//emulation for test
    mRoomNames.clear();
    mRoomsDescriptions.clear();
    mRoomsHosts.clear();
    mPlayersInServerRooms.clear();
    mPlayersNamesInRooms.clear();
    for(size_t q = 0; q < 20; ++q)
    {
        mRoomNames.push_back("Pow" + Conversions::DMToString(q));
        mRoomsDescriptions.push_back("");
        mRoomsHosts.push_back("dm");
        mPlayersInServerRooms.push_back(std::make_pair(1, 2));
    }
    redrawTable();
#else
    std::string serverIP = mEditBoxMultiIP.getText().asUTF8();

    hideMultiJoin();
    setColorMultiUserName(UILabel::mInactiveLabel);
    setColorMultiRoomName(UILabel::mInactiveLabel);

    if(!serverIP.empty())
    {

        mEditBoxMultiIP.setColor(Ogre::ColourValue::White);

        mMultiRoomsList.clear();
        mMultiRoomPlayersList.clear();

        std::string ip = mEditBoxMultiIP.getText().asUTF8();
        if(!ip.empty())
        {
            bool isConnected = false;
            
            try{
                isConnected = MultiplayerRoomInfo().getRoomsList(ip, mModeContext.mGameState.getMultiplayerServerPort(), mRoomNames, mRoomsDescriptions, mRoomsHosts, mPlayersInServerRooms, mPlayersNamesInRooms);
            }catch(...){
            }

            if(isConnected)
            {
                mEditBoxMultiIP.setColor(Ogre::ColourValue::Green);

                redrawTable();
            }
            else
            {
                mEditBoxMultiIP.setColor(Ogre::ColourValue::Red);
            }
        }
    }
    else
    {
        if(serverIP.empty())
            setColorMultiIP(Ogre::ColourValue::Red);
    }
#endif
}

void UIMainMenu::createRoom()
{
    std::string serverIP = mEditBoxMultiIP.getText().asUTF8();
    std::string roomName = mEditBoxMultiRoomName.getText().asUTF8();
    std::string userName = mEditBoxMultiUserName.getText().asUTF8();

    if(!serverIP.empty() && !roomName.empty() && !userName.empty())
    {
        mModeContext.mGameState.setMultiplayerMaster(true);
        mModeContext.mGameState.setMultiplayerServerIP(serverIP);
        mModeContext.mGameState.setMultiplayerRoomName(roomName);
        mModeContext.mGameState.setMultiplayerUserName(userName);
        mModeContext.getGameModeSwitcher()->switchMode(ModeMenuMulti);
    }
    else
    {
        switchState(State_Multi);

        if(serverIP.empty())
            setColorMultiIP(Ogre::ColourValue::Red);

        if(userName.empty())
            setColorMultiUserName(Ogre::ColourValue::Red);

        if(roomName.empty())
            setColorMultiRoomName(Ogre::ColourValue::Red);
    }
}

void UIMainMenu::joinRoom()
{
    std::string serverIP = mEditBoxMultiIP.getText().asUTF8();
    std::string roomName = mMultiRoomsList.getText(mSelectedRoomIndex);
    std::string userName = mEditBoxMultiUserName.getText().asUTF8();

    if(!serverIP.empty() && !roomName.empty() && !userName.empty())
    {
        mModeContext.mGameState.setMultiplayerMaster(false);
        mModeContext.mGameState.setMultiplayerServerIP(serverIP);
        mModeContext.mGameState.setMultiplayerRoomName(roomName);
        mModeContext.mGameState.setMultiplayerUserName(userName);
        mModeContext.getGameModeSwitcher()->switchMode(ModeMenuMulti);
    }
    else
    {
        switchState(State_Multi);

        if(serverIP.empty())
        {
            setColorMultiIP(Ogre::ColourValue::Red);
        }

        if(roomName.empty())
        {
            setColorMultiRoomName(Ogre::ColourValue::Red);
        }

        if(userName.empty())
        {
            setColorMultiUserName(Ogre::ColourValue::Red);
        }
    }
}