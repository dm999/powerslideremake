
#include "UIMainMenu.h"

#include "../gamelogic/GameModeSwitcher.h"

#include "../listeners/LoaderListener.h"

#include "../tools/OgreTools.h"
#include "../tools/Conversions.h"

#include "../customs/CustomTrayManager.h"

#include "../loaders/TextureLoader.h"

#include "../gamemodes/MenuMode.h"

#include "../BaseApp.h"

#ifndef NO_MULTIPLAYER
    #include "../multiplayer/MultiplayerRoomInfo.h"
#endif

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

#if 0

    {
        Ogre::Vector4 posAI = screenAdaptionRelative * Ogre::Vector4(540.0f, 50.0f, 30.0f, 12.0f);
        mWidgetAICount = gui->createWidget<MyGUI::ComboBox>("ComboBox", posAI.x, posAI.y, posAI.z, posAI.w, MyGUI::Align::Default, "Middle");
        for(size_t q = 3; q <= 11; ++q)
            mWidgetAICount->addItem(Conversions::DMToString(q));

        if(mModeContext.getGameState().getAICount() < 3)
            mModeContext.getGameState().setAICount(3);
        mWidgetAICount->setIndexSelected(mModeContext.getGameState().getAICount() - 3);
        mWidgetAICount->setEditReadOnly(true);
        mWidgetAICount->eventComboChangePosition += MyGUI::newDelegate(this, &UIMainMenu::processChangeComboBox);
    }

#ifndef NO_MULTIPLAYER
    {
        Ogre::Vector4 posJoin = screenAdaptionRelative * Ogre::Vector4(320.0f, 80.0f, 40.0f, 12.0f);
        MyGUI::ButtonPtr widgetJoin = gui->createWidget<MyGUI::Button>("Button", posJoin.x, posJoin.y, posJoin.z, posJoin.w, MyGUI::Align::Default, "Middle");
        widgetJoin->setCaption("Multi Join");
        widgetJoin->eventMouseButtonClick += MyGUI::newDelegate(this, &UIMainMenu::processButtonClick);

        Ogre::Vector4 posHost = screenAdaptionRelative * Ogre::Vector4(380.0f, 80.0f, 40.0f, 12.0f);
        MyGUI::ButtonPtr widgetHost = gui->createWidget<MyGUI::Button>("Button", posHost.x, posHost.y, posHost.z, posHost.w, MyGUI::Align::Default, "Middle");
        widgetHost->setCaption("Multi Host");
        widgetHost->eventMouseButtonClick += MyGUI::newDelegate(this, &UIMainMenu::processButtonClick);

        Ogre::Vector4 posIP = screenAdaptionRelative * Ogre::Vector4(320.0f, 100.0f, 100.0f, 12.0f);
        mWidgetIP = gui->createWidget<MyGUI::EditBox>("EditBox", posIP.x, posIP.y, posIP.z, posIP.w, MyGUI::Align::Default, "Middle");
        mWidgetIP->setCaption("78.47.85.155");
        mWidgetIP->eventKeyButtonPressed += MyGUI::newDelegate(this, &UIMainMenu::processKeyPress);
        mWidgetIP->setColour(MyGUI::Colour(0.0f, 0.0f, 0.0f));
        mWidgetIP->setTextColour(MyGUI::Colour(1.0f, 1.0f, 1.0f));

        Ogre::Vector4 posRoom = screenAdaptionRelative * Ogre::Vector4(320.0f, 120.0f, 100.0f, 12.0f);
        mWidgetRoom = gui->createWidget<MyGUI::EditBox>("EditBox", posRoom.x, posRoom.y, posRoom.z, posRoom.w, MyGUI::Align::Default, "Middle");
        mWidgetRoom->setCaption("Powerslide");
        mWidgetRoom->setColour(MyGUI::Colour(0.0f, 0.0f, 0.0f));
        mWidgetRoom->setTextColour(MyGUI::Colour(1.0f, 1.0f, 1.0f));

        Ogre::Vector4 posUserName = screenAdaptionRelative * Ogre::Vector4(320.0f, 140.0f, 100.0f, 12.0f);
        mWidgetUserName = gui->createWidget<MyGUI::EditBox>("EditBox", posUserName.x, posUserName.y, posUserName.z, posUserName.w, MyGUI::Align::Default, "Middle");
        mWidgetUserName->setCaption(mModeContext.getGameState().getMultiplayerUserName());
        mWidgetUserName->setColour(MyGUI::Colour(0.0f, 0.0f, 0.0f));
        mWidgetUserName->setTextColour(MyGUI::Colour(1.0f, 1.0f, 1.0f));

        Ogre::Vector4 posRooms = screenAdaptionRelative * Ogre::Vector4(320.0f, 160.0f, 90.0f, 50.0f);
        mWidgetRooms = gui->createWidget<MyGUI::ListBox>("ListBox", posRooms.x, posRooms.y, posRooms.z, posRooms.w, MyGUI::Align::Default, "Middle");
        mWidgetRooms->setColour(MyGUI::Colour(0.0f, 0.0f, 0.0f));
        mWidgetRooms->eventListSelectAccept += MyGUI::newDelegate(this, &UIMainMenu::processItemSelected);

        Ogre::Vector4 posRoomPlayers = screenAdaptionRelative * Ogre::Vector4(420.0f, 160.0f, 90.0f, 50.0f);
        mWidgetRoomPlayers = gui->createWidget<MyGUI::ListBox>("ListBox", posRoomPlayers.x, posRoomPlayers.y, posRooms.z, posRooms.w, MyGUI::Align::Default, "Middle");
        mWidgetRoomPlayers->setColour(MyGUI::Colour(0.0f, 0.0f, 0.0f));
    }
#endif
#endif

    createCommonMaterials();
    createBackgroundMaterials();


    createBackgroundUI(screenAdaptionRelative, trayMgr);//init mMainBackground for further usage
    createLabels(screenAdaptionRelative);
    createControls(screenAdaptionRelative, getMainBackground());

    //mEditBox.loadBackground(gameState.getPFLoaderGameshell(), "session.bmp");
    //mEditBox.setBackgroundMaterial("Test/CustomBackgroundBlackTransparent");
    mEditBoxUserName.loadBackground(gameState.getPFLoaderGameshell(), "session.bmp");
    mEditBoxUserName.init(screenAdaptionRelative, getMainBackground(), Ogre::Vector4(20.0f, 82.0f, 170.0f, 18.0f), 36.0f);
    mEditBoxUserName.setText(mModeContext.getGameState().getPlayerName());

    mEditBoxMultiIP.setBackgroundMaterial(mEditBoxUserName.getMaterialName());
    mEditBoxMultiIP.init(screenAdaptionRelative, getMainBackground(), Ogre::Vector4(320.0f, 175.0f, 170.0f, 18.0f), 36.0f);
    mEditBoxMultiIP.setCharType(UIEditBox::IP);
    mEditBoxMultiIP.setText("78.47.85.155");//d.polubotko: FIXME

    mEditBoxMultiUserName.setBackgroundMaterial(mEditBoxMultiIP.getMaterialName());
    mEditBoxMultiUserName.init(screenAdaptionRelative, getMainBackground(), Ogre::Vector4(320.0f, 250.0f, 170.0f, 18.0f), 36.0f);
    mEditBoxMultiUserName.setText(mModeContext.getGameState().getPlayerName());

    mEditBoxMultiRoomName.setBackgroundMaterial(mEditBoxMultiIP.getMaterialName());
    mEditBoxMultiRoomName.init(screenAdaptionRelative, getMainBackground(), Ogre::Vector4(320.0f, 300.0f, 170.0f, 18.0f), 36.0f);
    mEditBoxMultiRoomName.setText("Powerslide!");//d.polubotko: FIXME

    {
        Ogre::Vector4 roomsTablePos = screenAdaptionRelative * Ogre::Vector4(500.0f, 180.0f, 140.0f, 0.0f);
        mRoomsTable = createSelectMenu(trayMgr, OgreBites::TL_NONE, "roomsTable", "Rooms", roomsTablePos.z, 20, Ogre::StringVector());
        mRoomsTable->getOverlayElement()->setLeft(roomsTablePos.x);
        mRoomsTable->getOverlayElement()->setTop(roomsTablePos.y);
        mRoomsTable->hide();
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
}

void UIMainMenu::keyUp(MyGUI::KeyCode _key, wchar_t _char)
{
    mEditBoxUserName.keyUp(_key, _char);
    mEditBoxMultiIP.keyUp(_key, _char);
    mEditBoxMultiUserName.keyUp(_key, _char);
    mEditBoxMultiRoomName.keyUp(_key, _char);

    if(mEditBoxMultiIP.isActive())
        setColorMultiIP(mInactiveLabel);

    if(mEditBoxMultiUserName.isActive())
        setColorMultiUserName(mInactiveLabel);

    if(mEditBoxMultiRoomName.isActive())
        setColorMultiRoomName(mInactiveLabel);
}

void UIMainMenu::mousePressed(const Ogre::Vector2& pos)
{
    UIMainMenuLabels::mousePressed(pos);

    mEditBoxUserName.mouseReleased(pos);
    mEditBoxMultiIP.mouseReleased(pos);
    mEditBoxMultiUserName.mouseReleased(pos);
    mEditBoxMultiRoomName.mouseReleased(pos);

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

void UIMainMenu::mouseReleased(const Ogre::Vector2& pos)
{
    SinglePlayerMenuStates prevState = mCurrentState;

    UIMainMenuLabels::mouseReleased(pos);

    if(mCurrentState == State_Options_Trophies && mCurrentState == prevState)
    {
        setMainBackgroundMaterial("Test/MainBackground");
        
        for(size_t q = 0; q < mControlsCount; ++q)
        {
            setControlShow(q, true);
        }

        switchState(State_Options);
    }

    if(mCurrentState == State_FinishChampionship)
    {
        setMainBackgroundMaterial("Test/MainBackground");
        
        for(size_t q = 0; q < mControlsCount; ++q)
        {
            setControlShow(q, true);
        }

        mModeContext.getGameModeSwitcher()->switchMode(ModeMenu);
        setTopmostSubmenu();
    }
}

void UIMainMenu::mouseMoved(const Ogre::Vector2& pos)
{
    UIMainMenuLabels::mouseMoved(pos);
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
                if(!mModeContext.getGameState().getChampionship().isFinished(mModeContext))
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
    std::string userName = mEditBoxUserName.getText().asUTF8();
    mModeContext.getGameState().setPlayerName(userName);
    mModeContext.getGameState().loadPlayerData();
    mModeContext.getGameState().savePlayerData();//save selected player
    switchState(State_SingleMulti);
    selectMode();
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
        setWindowTitle("Game Mode");
        showModeMulti();
        showMultiIPLabels();
        showBackgrounds();
        mEditBoxMultiUserName.setText(mModeContext.getGameState().getPlayerName());
        mEditBoxMultiIP.show();
        mEditBoxMultiUserName.show();
        mEditBoxMultiRoomName.show();
        break;

    case State_MultiConnect:
        mIsInStartingGrid = false;
        setWindowTitle("Game Mode");
        showModeMulti();
        showMultiIPLabels();
        showBackgrounds();
        mEditBoxMultiIP.show();
        mEditBoxMultiUserName.show();
        mEditBoxMultiRoomName.show();
        connectToServer();
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
        break;

    case State_Options:
        mIsInStartingGrid = false;
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
        showOptionLabels();
        showOptionNameLabels();
        mEditBoxUserName.setText(mModeContext.getGameState().getPlayerName());
        mEditBoxUserName.show();
        break;

    case State_Options_Trophies:
        mIsInStartingGrid = false;
        setMainBackgroundMaterial("Test/TrophiesBackground");
        for(size_t q = 0; q < mControlsCount; ++q)
        {
            setControlShow(q, false);
        }
        setWindowTitle("");
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
    mRoomsTable->hide();
}

void UIMainMenu::connectToServer()
{
#ifndef NO_MULTIPLAYER
    mEditBoxMultiIP.setColor(Ogre::ColourValue::White);
    mRoomsTable->hide();

    std::string ip = mEditBoxMultiIP.getText().asUTF8();
    if(!ip.empty())
    {
        std::vector<std::string> rooms;
        std::vector<std::string> roomsDesc;
        std::vector<std::pair<size_t, size_t> > playersInServerRooms;
        bool isConnected = false;
        
        try{
            isConnected = MultiplayerRoomInfo().getRoomsList(ip, mModeContext.mGameState.getMultiplayerServerPort(), rooms, roomsDesc, playersInServerRooms);
        }catch(...){
        }

        if(isConnected)
        {
            mEditBoxMultiIP.setColor(Ogre::ColourValue::Green);
            mRoomsTable->clearItems();
            mRoomsTable->show();

            for(size_t q = 0; q < rooms.size(); ++q)
            {
                if((playersInServerRooms[q].first + playersInServerRooms[q].second) < 12)
                {
                    mRoomsTable->addItem(rooms[q]);
                    //mWidgetRooms->addItem("#00FF00" + rooms[q]);
                    //mWidgetRoomPlayers->addItem("#00FF00" + Conversions::DMToString(playersInServerRooms[q].first) + " " + Conversions::DMToString(playersInServerRooms[q].second) + " " + roomsDesc[q]);
                }
                else
                {
                    mRoomsTable->addItem(rooms[q]);
                    //mWidgetRooms->addItem("#FF0000" + rooms[q]);
                    //mWidgetRoomPlayers->addItem("#FF0000" + Conversions::DMToString(playersInServerRooms[q].first) + " " + Conversions::DMToString(playersInServerRooms[q].second) + " " + roomsDesc[q]);
                }
            }
        }
        else
        {
            mEditBoxMultiIP.setColor(Ogre::ColourValue::Red);
        }
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
    std::string roomName = mRoomsTable->getSelectedItem();
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
        //mWidgetUserName->setColour(MyGUI::Colour(1.0f, 0.0f, 0.0f));
    }
}