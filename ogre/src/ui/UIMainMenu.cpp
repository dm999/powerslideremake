
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

UIMainMenu::UIMainMenu(const ModeContext& modeContext, MenuMode * menuMode, SinglePlayerMenuStates state)
    : UIMainMenuLabels(modeContext),
    mIsInStartingGrid(false),
    mMenuMode(menuMode),
    mCurrentState(state)
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
    trayMgr->setListener(this);

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

#if 0
void UIMainMenu::processButtonClick(MyGUI::Widget* sender)
{
    MyGUI::Button * senderButton = static_cast<MyGUI::Button *>(sender);

#ifndef NO_MULTIPLAYER
    if(senderButton->getCaption() == "Multi Join")
    {
        if(mWidgetRooms->getIndexSelected() == MyGUI::ITEM_NONE)
        {
            mWidgetRooms->deleteAllItems();
            mWidgetRoomPlayers->deleteAllItems();

            std::string ip = mWidgetIP->getCaption();
            if(!ip.empty())
            {
                std::vector<std::string> rooms;
                std::vector<std::string> roomsDesc;
                std::vector<std::pair<size_t, size_t> > playersInServerRooms;
                bool isConnected = MultiplayerRoomInfo().getRoomsList(ip, mModeContext.mGameState.getMultiplayerServerPort(), rooms, roomsDesc, playersInServerRooms);
                if(isConnected)
                {
                    for(size_t q = 0; q < rooms.size(); ++q)
                    {
                        if((playersInServerRooms[q].first + playersInServerRooms[q].second) < 12)
                        {
                            mWidgetRooms->addItem("#00FF00" + rooms[q]);
                            mWidgetRoomPlayers->addItem("#00FF00" + Conversions::DMToString(playersInServerRooms[q].first) + " " + Conversions::DMToString(playersInServerRooms[q].second) + " " + roomsDesc[q]);
                        }
                        else
                        {
                            mWidgetRooms->addItem("#FF0000" + rooms[q]);
                            mWidgetRoomPlayers->addItem("#FF0000" + Conversions::DMToString(playersInServerRooms[q].first) + " " + Conversions::DMToString(playersInServerRooms[q].second) + " " + roomsDesc[q]);
                        }
                    }
                }
                else
                {
                    mWidgetIP->setColour(MyGUI::Colour(1.0f, 0.0f, 0.0f));
                }
            }
        }
        else
        {
            if(mWidgetUserName->getCaption() != "")
            {
                mModeContext.mGameState.setMultiplayerMaster(false);
                mModeContext.mGameState.setMultiplayerServerIP(mWidgetIP->getCaption());

                //remove color data from room name
                std::string roomName = mWidgetRooms->getItemNameAt(mWidgetRooms->getIndexSelected());
                roomName = roomName.substr(7, roomName.size());
                mModeContext.mGameState.setMultiplayerRoomName(roomName);

                mModeContext.mGameState.setMultiplayerUserName(mWidgetUserName->getCaption());
                mModeContext.getGameModeSwitcher()->switchMode(ModeMenuMulti);
            }
            else
            {
                mWidgetUserName->setColour(MyGUI::Colour(1.0f, 0.0f, 0.0f));
            }
        }
    }

    if(senderButton->getCaption() == "Multi Host")
    {
        if(mWidgetUserName->getCaption() != "")
        {
            mModeContext.mGameState.setMultiplayerMaster(true);
            mModeContext.mGameState.setMultiplayerServerIP(mWidgetIP->getCaption());
            mModeContext.mGameState.setMultiplayerRoomName(mWidgetRoom->getCaption());
            mModeContext.mGameState.setMultiplayerUserName(mWidgetUserName->getCaption());
            mModeContext.getGameModeSwitcher()->switchMode(ModeMenuMulti);
        }
        else
        {
            mWidgetUserName->setColour(MyGUI::Colour(1.0f, 0.0f, 0.0f));
        }
    }
#endif
}

void UIMainMenu::processKeyPress(MyGUI::Widget* sender, MyGUI::KeyCode key, unsigned int _char)
{
    if(sender == mWidgetIP)
    {
        mWidgetIP->setColour(MyGUI::Colour(0.0f, 0.0f, 0.0f));
    }
}

void UIMainMenu::processItemSelected(MyGUI::Widget* sender, size_t index)
{
    if(sender == mWidgetRooms)
    {
    }
}

void UIMainMenu::processChangeComboBox(MyGUI::Widget* sender, size_t index)
{
    if(sender == mWidgetAICount)
    {
        mModeContext.getGameState().setAICount(index + 3);
        if(
            mModeContext.getGameState().getTrackName() == "stunt track"         ||
            mModeContext.getGameState().getTrackName() == "luge track"          ||
            mModeContext.getGameState().getTrackName() == "Foxnhound1 track"    ||
            mModeContext.getGameState().getTrackName() == "Foxnhound2 track"
            )
            mModeContext.getGameState().setAICount(0);
    }
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
}

void UIMainMenu::mousePressed(const Ogre::Vector2& pos)
{
    UIBaseMenu::mousePressed(pos);
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

void UIMainMenu::panelHit(Ogre::PanelOverlayElement* panel)
{

    if(panel != NULL && panel->getName() == "Mode")
        switchState(State_SingleMulti);

    if(panel != NULL && panel->getName() == "Track")
        switchState(State_Track);

    if(panel != NULL && panel->getName() == "Car")
        switchState(State_Car);

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

    if(lap == mModeContext.getGameState().getLapsCount())//race finished
    {
        //player
        ret.push_back(finishBoardElement(
            lapController.getTotalPosition(0), 
            true, 
            mModeContext.getGameState().getPlayerCar().getLapUtils().getTotalTime(),
            mModeContext.getGameState().getPlayerCar().getCharacterName()
            ));

        for(size_t q = 0; q < mModeContext.getGameState().getAICount(); ++q)
        {
            ret.push_back(finishBoardElement(
                lapController.getTotalPosition(q + 1), 
                false, 
                mModeContext.getGameState().getAICar(q).getLapUtils().getTotalTime(),
                mModeContext.getGameState().getAICar(q).getCharacterName()));
        }
    }
    else
    {
        //player
        ret.push_back(finishBoardElement(
            mModeContext.getGameState().getAICount(), 
            true, 
            0.0f,
            mModeContext.getGameState().getPlayerCar().getCharacterName()
            ));

        for(size_t q = 0; q < mModeContext.getGameState().getAICount(); ++q)
        {
            ret.push_back(finishBoardElement(
                q, 
                false, 
                0.0f,
                mModeContext.getGameState().getAICar(q).getCharacterName()));
        }
    }

    std::sort(ret.begin(), ret.end());

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
        showModeSingle();
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
}