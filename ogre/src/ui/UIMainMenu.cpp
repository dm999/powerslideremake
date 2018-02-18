
#include "UIMainMenu.h"

#include "../gamelogic/GameModeSwitcher.h"

#include "../listeners/LoaderListener.h"

#include "../tools/OgreTools.h"
#include "../tools/Conversions.h"
#include "../tools/Randomizer.h"

#include "../customs/CustomTrayManager.h"

#include "../loaders/TextureLoader.h"

#include "../gamemodes/MenuMode.h"

#include "../BaseApp.h"

#ifndef NO_MULTIPLAYER
    #include "../multiplayer/MultiplayerRoomInfo.h"
#endif

UIMainMenu::UIMainMenu(const ModeContext& modeContext, MenuMode * menuMode, SinglePlayerMenuStates state)
    : UIMainMenuLabels(modeContext),
    mIsInStartingGrid(false),
    mMenuMode(menuMode),
    mCurrentState(state),
    mEditBox(10)
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
    mEditBox.setBackgroundMaterial("Test/CustomBackgroundBlackTransparent");
    mEditBox.init(screenAdaptionRelative, getMainBackground(), Ogre::Vector4(320.0f, 200.0f, 250.0f, 42.0f), 86.0f);
    mEditBoxIP.loadBackground(gameState.getPFLoaderGameshell(), "session.bmp");
    mEditBoxIP.init(screenAdaptionRelative, getMainBackground(), Ogre::Vector4(320.0f, 175.0f, 170.0f, 18.0f), 36.0f);
    mEditBoxIP.setCharType(UIEditBox::IP);
    mEditBoxIP.setText("78.47.85.155");//d.polubotko: FIXME

    mEditBoxUserName.setBackgroundMaterial(mEditBoxIP.getMaterialName());
    mEditBoxUserName.init(screenAdaptionRelative, getMainBackground(), Ogre::Vector4(320.0f, 250.0f, 170.0f, 18.0f), 36.0f);
    mEditBoxUserName.setText("DM");//d.polubotko: FIXME

    mEditBoxRoomName.setBackgroundMaterial(mEditBoxIP.getMaterialName());
    mEditBoxRoomName.init(screenAdaptionRelative, getMainBackground(), Ogre::Vector4(320.0f, 300.0f, 170.0f, 18.0f), 36.0f);
    mEditBoxRoomName.setText("Powerslide!");//d.polubotko: FIXME

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

    mEditBox.frameStarted(evt);
    mEditBoxIP.frameStarted(evt);
    mEditBoxUserName.frameStarted(evt);
    mEditBoxRoomName.frameStarted(evt);
}

void UIMainMenu::keyUp(MyGUI::KeyCode _key, wchar_t _char)
{
    mEditBox.keyUp(_key, _char);
    mEditBoxIP.keyUp(_key, _char);
    mEditBoxUserName.keyUp(_key, _char);
    mEditBoxRoomName.keyUp(_key, _char);

    if(mEditBoxIP.isActive())
        setColorMultiIP(mInactiveLabel);

    if(mEditBoxUserName.isActive())
        setColorMultiUserName(mInactiveLabel);

    if(mEditBoxRoomName.isActive())
        setColorMultiRoomName(mInactiveLabel);
}

void UIMainMenu::mousePressed(const Ogre::Vector2& pos)
{
    UIBaseMenu::mousePressed(pos);
    mEditBox.mouseReleased(pos);
    mEditBoxIP.mouseReleased(pos);
    mEditBoxUserName.mouseReleased(pos);
    mEditBoxRoomName.mouseReleased(pos);
}

void UIMainMenu::mouseReleased(const Ogre::Vector2& pos)
{
    UIMainMenuLabels::mouseReleased(pos);
}

void UIMainMenu::mouseMoved(const Ogre::Vector2& pos)
{
    UIMainMenuLabels::mouseMoved(pos);
}

bool UIMainMenu::isExitSubmenu()const
{
    return mCurrentState == State_ExitGame;
}

void UIMainMenu::setExitSubmenu()
{
    switchState(State_ExitGame);
}

void UIMainMenu::setTopmostSubmenu()
{
    switchState(State_SingleMulti);
}

void UIMainMenu::destroy(CustomTrayManager* trayMgr)
{
    UIBase::destroy(trayMgr);
    mEditBox.destroy(trayMgr);
    mEditBoxIP.destroy(trayMgr);
    mEditBoxUserName.destroy(trayMgr);
    mEditBoxRoomName.destroy(trayMgr);
}

void UIMainMenu::panelHit(Ogre::PanelOverlayElement* panel)
{

    if(panel != NULL && panel->getName() == "Mode")
        switchState(State_SingleMulti);

    if(panel != NULL && panel->getName() == "Track")
        switchState(State_Track);

    if(panel != NULL && panel->getName() == "Car")
        switchState(State_Car);

    if(panel != NULL && panel->getName() == "Option")
        switchState(State_Options);

    if(panel != NULL && panel->getName() == "Race")
        switchState(State_StartingGrid);

    if(panel != NULL && panel->getName() == "Exit")
    {
        mModeContext.mBaseApp->setShutdown(true);
    }
}

void UIMainMenu::startRace()
{
    mModeContext.getGameModeSwitcher()->switchMode(ModeRaceSingle);
}

finishBoard_v UIMainMenu::prepareFinishBoard()const
{
    finishBoard_v ret;

    LapController lapController = mModeContext.getLapController();
    size_t lap = mModeContext.getGameState().getPlayerCar().getLapUtils().getCurrentLap() - 1;

    bool isPlayerFinishRace = false;

    if(lap == mModeContext.getGameState().getLapsCount())//race finished
    {
        isPlayerFinishRace = true;
    }

    for(size_t q = 0; q < mModeContext.getGameState().getAICount(); ++q)
    {
        ret.push_back(finishBoardElement(
            isPlayerFinishRace ? lapController.getTotalPosition(q + 1) : q, 
            false, 
            isPlayerFinishRace ? mModeContext.getGameState().getAICar(q).getLapUtils().getTotalTime() : 0.0f,
            mModeContext.getGameState().getAICar(q).getLapUtils().getBestLapTime(),
            mModeContext.getGameState().getAICar(q).getCharacterName()));
    }

    //player
    ret.push_back(finishBoardElement(
        isPlayerFinishRace ? lapController.getTotalPosition(0) : mModeContext.getGameState().getAICount(), 
        true, 
        isPlayerFinishRace ? mModeContext.getGameState().getPlayerCar().getLapUtils().getTotalTime() : 0.0f,
        mModeContext.getGameState().getPlayerCar().getLapUtils().getBestLapTime(),
        mModeContext.getGameState().getPlayerCar().getCharacterName()
        ));

    if(isPlayerFinishRace)
    {
        std::sort(ret.begin(), ret.end());

        finishBoard_v::const_iterator i = std::find_if(ret.begin(), ret.end(), finishBoardElement::findPlayer);
        size_t playerIndexAfterSort = i - ret.begin();
        for(size_t q = playerIndexAfterSort + 1; q < ret.size(); ++q)
        {
            ret[q].mRaceTime = ret[q - 1].mRaceTime + Randomizer::GetInstance().GetRandomFloat(1.0f, 10.0f);
            if(ret[q].mBestLapTime == 0.0f)
            {
                if(mModeContext.getGameState().getLapsCount() == 1)
                    ret[q].mBestLapTime = ret[q].mRaceTime;
                else
                    ret[q].mBestLapTime = ret[q - 1].mBestLapTime + Randomizer::GetInstance().GetRandomFloat(1.0f, 2.0f);
            }
        }
    }
    else
    {
        for(size_t q = 0; q < ret.size() - 1; ++q)
        {
            if(ret[q].mBestLapTime == 0.0f)
            {
                const STRRacetimes& raceTimes = mModeContext.getGameState().getSTRRacetimes();
                Ogre::Vector2 lapTime = raceTimes.getArray2Value(mModeContext.getGameState().getTrackName() + " times", ret[q].mCharName + " lap times");
                
                float normalLapTime = lapTime.x;
                
                if(mModeContext.getGameState().getAIStrength() == Easy)
                    normalLapTime = lapTime.y;

                ret[q].mBestLapTime = normalLapTime + Randomizer::GetInstance().GetRandomFloat(normalLapTime / 20.0f, normalLapTime / 10.0f);
                ret[q].mRaceTime = mModeContext.getGameState().getLapsCount() * (normalLapTime + Randomizer::GetInstance().GetRandomFloat(normalLapTime / 10.0f, normalLapTime / 5.0f));
            }
            else
            {
                ret[q].mRaceTime = mModeContext.getGameState().getLapsCount() * ret[q].mBestLapTime + Randomizer::GetInstance().GetRandomFloat(ret[q].mBestLapTime / 20.0f, ret[q].mBestLapTime / 10.0f);
            }
        }

        std::sort(ret.begin(), ret.end() - 1, finishBoardElement::sortByRaceTime);

        for(size_t q = 0; q < ret.size() - 1; ++q)
            ret[q].mPos = q;
    }

    return ret;
}

void UIMainMenu::switchState(const SinglePlayerMenuStates& state)
{
    hideAll();

    switch(state)
    {
    case State_SingleMulti:
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
        //mEditBox.show();
        mEditBoxIP.show();
        mEditBoxUserName.show();
        mEditBoxRoomName.show();
        break;

    case State_MultiConnect:
        mIsInStartingGrid = false;
        setWindowTitle("Game Mode");
        showModeMulti();
        showMultiIPLabels();
        showBackgrounds();
        //mEditBox.show();
        mEditBoxIP.show();
        mEditBoxUserName.show();
        mEditBoxRoomName.show();
        connectToServer();
        break;

    case State_MultiCreateRoom:
        createRoom();
        break;

    case State_MultiJoinRoom:
        joinRoom();
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
        showStartingGridTimer();
        break;

    case State_Podium:
        mIsInStartingGrid = false;
        showBackgroundFirstSecondThird();
        {
            finishBoard_v finishBoard = prepareFinishBoard();
            showBackgroundCharacterSmallPodium(finishBoard);
            showPodiumLabels(finishBoard);
        }
        setWindowTitle("Podium");
        break;

    case State_ExitGame:
        mIsInStartingGrid = false;
        showBackgroundExitSign();
        showGameExitLabels();
        setWindowTitle("");
        break;
    }

    mCurrentState = state;
}

void UIMainMenu::hideAll()
{
    hideAllBackgrounds();
    hideAllLabels();
    mEditBox.hide();
    mEditBoxIP.hide();
    mEditBoxUserName.hide();
    mEditBoxRoomName.hide();
    mRoomsTable->hide();
}

void UIMainMenu::connectToServer()
{
#ifndef NO_MULTIPLAYER
    mEditBoxIP.setColor(Ogre::ColourValue::White);
    mRoomsTable->hide();

    std::string ip = mEditBoxIP.getText().asUTF8();
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
            mEditBoxIP.setColor(Ogre::ColourValue::Green);
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
            mEditBoxIP.setColor(Ogre::ColourValue::Red);
        }
    }
#endif
}

void UIMainMenu::createRoom()
{
    std::string serverIP = mEditBoxIP.getText().asUTF8();
    std::string roomName = mEditBoxRoomName.getText().asUTF8();
    std::string userName = mEditBoxUserName.getText().asUTF8();

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
    std::string serverIP = mEditBoxIP.getText().asUTF8();
    std::string roomName = mRoomsTable->getSelectedItem();
    std::string userName = mEditBoxUserName.getText().asUTF8();

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