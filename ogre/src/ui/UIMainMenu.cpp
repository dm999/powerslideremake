#include "../pcheader.h"

#include "UIMainMenu.h"

#include "MyGUI.h"

#include "../gamelogic/GameModeSwitcher.h"

#include "../tools/OgreTools.h"
#include "../tools/Conversions.h"

#include "../customs/CustomTrayManager.h"

#include "../loaders/TextureLoader.h"

#include "../gamemodes/MenuMode.h"

#include "../multiplayer/MultiplayerRoomInfo.h"

UIMainMenu::UIMainMenu(const ModeContext& modeContext, MenuMode * menuMode)
    : mModeContext(modeContext),
    mMenuMode(menuMode)
{}

UIMainMenu::~UIMainMenu()
{}

void UIMainMenu::loadMisc(const PFLoader& pfLoaderData, const PFLoader& pfLoaderGameshell)
{
    TextureLoader().loadChroma( pfLoaderGameshell, 
                                "data/gameshell", "cursor.bmp", 
                                "OriginalCursor", TEMP_RESOURCE_GROUP_NAME, 
                                Ogre::ColourValue(0.0f, 1.0f, 0.1f, 1.0f), 0.1f);

    TextureLoader().load( pfLoaderGameshell, 
                                "data/gameshell", "bg.bmp", 
                                "OriginalMainBackground", TEMP_RESOURCE_GROUP_NAME);

    TextureLoader().load( pfLoaderGameshell, 
                                "data/gameshell", "tmga.bmp", 
                                "OriginalBackgroundA", TEMP_RESOURCE_GROUP_NAME);

    TextureLoader().load( pfLoaderGameshell, 
                                "data/gameshell", "tmgb.bmp", 
                                "OriginalBackgroundB", TEMP_RESOURCE_GROUP_NAME);
}

void UIMainMenu::load(MyGUI::Gui* gui, const GameState& gameState)
{
    Ogre::OverlayManager& om = Ogre::OverlayManager::getSingleton(); 
    Ogre::Real viewportWidth = om.getViewportWidth(); 
    Ogre::Real viewportHeight = om.getViewportHeight(); 

    Ogre::Matrix4 screenAdaptionRelative(
        viewportWidth / 640.0f, 0.0f,                       0.0f,                   0.0f,
        0.0f,                   viewportHeight / 480.0f,    0.0f,                   0.0f,
        0.0f,                   0.0f,                       viewportWidth / 640.0f, 0.0f,
        0.0f,                   0.0f,                       0.0f,                   viewportHeight / 480.0f);

    loadMisc(gameState.getPFLoaderData(), gameState.getPFLoaderGameshell());


    {
        Ogre::Vector4 backgroundAPos = screenAdaptionRelative * Ogre::Vector4(0.0f, 0.0f, 197.0f, 328.0f);
        Ogre::Vector4 backgroundBPos = screenAdaptionRelative * Ogre::Vector4(197.0f, 0.0f, 102.0f, 217.0f);

        MyGUI::ImageBox * mainBackground = gui->createWidget<MyGUI::ImageBox>("ImageBox", 0, 0, viewportWidth, viewportHeight, MyGUI::Align::Default, "Wallpaper");
        mainBackground->setImageTexture("OriginalMainBackground");

        MyGUI::ImageBox * backgroundA = gui->createWidget<MyGUI::ImageBox>("ImageBox", backgroundAPos.x, backgroundAPos.y, backgroundAPos.z, backgroundAPos.w, MyGUI::Align::Default, "Back");
        backgroundA->setImageTexture("OriginalBackgroundA");

        MyGUI::ImageBox * backgroundB = gui->createWidget<MyGUI::ImageBox>("ImageBox", backgroundBPos.x, backgroundBPos.y, backgroundBPos.z, backgroundBPos.w, MyGUI::Align::Default, "Back");
        backgroundB->setImageTexture("OriginalBackgroundB");
    }

    {
        Ogre::Vector4 pos = screenAdaptionRelative * Ogre::Vector4(320.0f, 50.0f, 40.0f, 12.0f);

        MyGUI::ButtonPtr widget = gui->createWidget<MyGUI::Button>("Button", pos.x, pos.y, pos.z, pos.w, MyGUI::Align::Default, "Middle");
        widget->setCaption("Single");
        widget->eventMouseButtonClick += MyGUI::newDelegate(this, &UIMainMenu::processButtonClick);


        //track
        Ogre::Vector4 posTrack = screenAdaptionRelative * Ogre::Vector4(360.0f, 50.0f, 60.0f, 12.0f);
        mWidgetTrack = gui->createWidget<MyGUI::ComboBox>("ComboBox", posTrack.x, posTrack.y, posTrack.z, posTrack.w, MyGUI::Align::Default, "Middle");

        const STRPowerslide& strPowerslide = mModeContext.getGameState().getSTRPowerslide();
        std::vector<std::string> availTracks = strPowerslide.getArrayValue("", "available tracks");

        size_t itemToSelect = 0;
        for(size_t q = 0; q < availTracks.size(); ++q)
        {
            mWidgetTrack->addItem(strPowerslide.getTrackTitle(availTracks[q]));

            if(availTracks[q] == mModeContext.getGameState().getTrackNameAsOriginal())
                itemToSelect = q;
        }

        mWidgetTrack->setIndexSelected(itemToSelect);

        mWidgetTrack->setEditReadOnly(true);
        mWidgetTrack->eventComboChangePosition += MyGUI::newDelegate(this, &UIMainMenu::processChangeComboBox);


        std::string characterCar = strPowerslide.getCarFromCharacter(mModeContext.getGameState().getPlayerCar().getCharacterName());
        characterCar = strPowerslide.getBaseCarFromCar(characterCar);

        std::vector<std::string> availCars = strPowerslide.getArrayValue("", "available cars");
        std::vector<std::string> availChars = strPowerslide.getCharactersByBaseCar(characterCar);

        //car
        Ogre::Vector4 posCar = screenAdaptionRelative * Ogre::Vector4(420.0f, 50.0f, 60.0f, 12.0f);
        mWidgetCar = gui->createWidget<MyGUI::ComboBox>("ComboBox", posCar.x, posCar.y, posCar.z, posCar.w, MyGUI::Align::Default, "Middle");

        itemToSelect = 0;
        for(size_t q = 0; q < availCars.size(); ++q)
        {
            mWidgetCar->addItem(strPowerslide.getCarTitle(availCars[q]));

            if(availCars[q] == characterCar)
                itemToSelect = q;
        }
        mWidgetCar->setIndexSelected(itemToSelect);
        mWidgetCar->setEditReadOnly(true);
        mWidgetCar->eventComboChangePosition += MyGUI::newDelegate(this, &UIMainMenu::processChangeComboBox);


        //character
        Ogre::Vector4 posChar = screenAdaptionRelative * Ogre::Vector4(480.0f, 50.0f, 60.0f, 12.0f);
        mWidgetCharacter = gui->createWidget<MyGUI::ComboBox>("ComboBox", posChar.x, posChar.y, posChar.z, posChar.w, MyGUI::Align::Default, "Middle");

        itemToSelect = 0;
        for(size_t q = 0; q < availChars.size(); ++q)
        {
            mWidgetCharacter->addItem(strPowerslide.getCharacterTitle(availChars[q]));

            if(availChars[q] == mModeContext.getGameState().getPlayerCar().getCharacterName())
                itemToSelect = q;
        }
        mWidgetCharacter->setIndexSelected(itemToSelect);

        mWidgetCharacter->setEditReadOnly(true);
        mWidgetCharacter->eventComboChangePosition += MyGUI::newDelegate(this, &UIMainMenu::processChangeComboBox);



        Ogre::Vector4 posAI = screenAdaptionRelative * Ogre::Vector4(540.0f, 50.0f, 30.0f, 12.0f);
        mWidgetAICount = gui->createWidget<MyGUI::ComboBox>("ComboBox", posAI.x, posAI.y, posAI.z, posAI.w, MyGUI::Align::Default, "Middle");
        for(size_t q = 3; q <= 11; ++q)
            mWidgetAICount->addItem(Conversions::DMToString(q));

        if(mModeContext.getGameState().getAICount() < 3)
            mModeContext.getGameState().setAICount(3);
        mWidgetAICount->setIndexSelected(mModeContext.getGameState().getAICount() - 3);
        mWidgetAICount->setEditReadOnly(true);
        mWidgetAICount->eventComboChangePosition += MyGUI::newDelegate(this, &UIMainMenu::processChangeComboBox);


        Ogre::Vector4 posAIStrength = screenAdaptionRelative * Ogre::Vector4(570.0f, 50.0f, 30.0f, 12.0f);
        mWidgetAIStrength = gui->createWidget<MyGUI::ComboBox>("ComboBox", posAIStrength.x, posAIStrength.y, posAIStrength.z, posAIStrength.w, MyGUI::Align::Default, "Middle");
        mWidgetAIStrength->addItem("Novice");
        mWidgetAIStrength->addItem("Advanced");
        mWidgetAIStrength->addItem("Expert");
        mWidgetAIStrength->addItem("Insane");

        mWidgetAIStrength->setIndexSelected(mModeContext.getGameState().getAIStrength());
        mWidgetAIStrength->setEditReadOnly(true);
        mWidgetAIStrength->eventComboChangePosition += MyGUI::newDelegate(this, &UIMainMenu::processChangeComboBox);

    }

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


    //cursor
    {
        std::vector<Ogre::String> texName;
        texName.push_back("OriginalCursor");
        Ogre::MaterialPtr newMat = CloneMaterial(  "Test/Cursor", 
                            "Test/DiffuseTransparent", 
                            texName, 
                            1.0f,
                            TEMP_RESOURCE_GROUP_NAME);
        newMat->getTechnique(0)->getPass(0)->setDepthCheckEnabled(false);
        newMat->getTechnique(0)->getPass(0)->setLightingEnabled(false);
        Ogre::TextureUnitState *state = newMat->getTechnique(0)->getPass(0)->getTextureUnitState(0);
        state->setTextureAddressingMode(Ogre::TextureUnitState::TAM_CLAMP);
        state->setTextureFiltering(Ogre::FO_NONE, Ogre::FO_NONE, Ogre::FO_NONE);
    }

    //MyGUI::PointerManager::getInstance().getByName("arrow")->setResourceName();

#if 0
    //main background
    {
        std::vector<Ogre::String> texName;
        texName.push_back("OriginalMainBackground");
        Ogre::MaterialPtr newMat = CloneMaterial(  "Test/MainBackground", 
                            "Test/Diffuse", 
                            texName, 
                            1.0f,
                            TEMP_RESOURCE_GROUP_NAME);
        newMat->getTechnique(0)->getPass(0)->setDepthCheckEnabled(false);
        newMat->getTechnique(0)->getPass(0)->setLightingEnabled(false);
        Ogre::TextureUnitState *state = newMat->getTechnique(0)->getPass(0)->getTextureUnitState(0);
        state->setTextureAddressingMode(Ogre::TextureUnitState::TAM_CLAMP);
        state->setTextureFiltering(Ogre::FO_NONE, Ogre::FO_NONE, Ogre::FO_NONE);

        mMainBackground = createPanel("MainBackground", viewportWidth, viewportHeight, 0.0f, 0.0f, "Test/MainBackground");
        mMainBackground->setUV(0.0f, 0.0f, 1.0f, 1.0f);
        trayMgr->getTrayContainer(OgreBites::TL_NONE)->addChild(mMainBackground);
        //mMainBackground->hide();
    }

    //additional background (feryl)
    {
        {
            std::vector<Ogre::String> texName;
            texName.push_back("OriginalBackgroundA");
            Ogre::MaterialPtr newMat = CloneMaterial(  "Test/BackgroundA", 
                                "Test/Diffuse", 
                                texName, 
                                1.0f,
                                TEMP_RESOURCE_GROUP_NAME);
            newMat->getTechnique(0)->getPass(0)->setDepthCheckEnabled(false);
            newMat->getTechnique(0)->getPass(0)->setLightingEnabled(false);
            Ogre::TextureUnitState *state = newMat->getTechnique(0)->getPass(0)->getTextureUnitState(0);
            state->setTextureAddressingMode(Ogre::TextureUnitState::TAM_CLAMP);
            state->setTextureFiltering(Ogre::FO_NONE, Ogre::FO_NONE, Ogre::FO_NONE);
        }

        {
            std::vector<Ogre::String> texName;
            texName.push_back("OriginalBackgroundB");
            Ogre::MaterialPtr newMat = CloneMaterial(  "Test/BackgroundB", 
                                "Test/Diffuse", 
                                texName, 
                                1.0f,
                                TEMP_RESOURCE_GROUP_NAME);
            newMat->getTechnique(0)->getPass(0)->setDepthCheckEnabled(false);
            newMat->getTechnique(0)->getPass(0)->setLightingEnabled(false);
            Ogre::TextureUnitState *state = newMat->getTechnique(0)->getPass(0)->getTextureUnitState(0);
            state->setTextureAddressingMode(Ogre::TextureUnitState::TAM_CLAMP);
            state->setTextureFiltering(Ogre::FO_NONE, Ogre::FO_NONE, Ogre::FO_NONE);
        }

        Ogre::Vector4 backgroundA = screenAdaptionRelative * Ogre::Vector4(0.0f, 0.0f, 197.0f, 328.0f);
        Ogre::Vector4 backgroundB = screenAdaptionRelative * Ogre::Vector4(197.0f, 0.0f, 197.0f + 102.0f, 217.0f);

        mBackgroundA = createPanel("BackgroundA", backgroundA, "Test/BackgroundA");
        mBackgroundA->setUV(0.0f, 0.0f, 1.0f, 1.0f);
        trayMgr->getTrayContainer(OgreBites::TL_NONE)->addChild(mBackgroundA);

        mBackgroundB = createPanel("BackgroundB", backgroundB, "Test/BackgroundB");
        mBackgroundB->setUV(0.0f, 0.0f, 1.0f, 1.0f);
        trayMgr->getTrayContainer(OgreBites::TL_NONE)->addChild(mBackgroundB);
    }

    //cursor
    {
        std::vector<Ogre::String> texName;
        texName.push_back("OriginalCursor");
        Ogre::MaterialPtr newMat = CloneMaterial(  "Test/Cursor", 
                            "Test/DiffuseTransparent", 
                            texName, 
                            1.0f,
                            TEMP_RESOURCE_GROUP_NAME);
        newMat->getTechnique(0)->getPass(0)->setDepthCheckEnabled(false);
        newMat->getTechnique(0)->getPass(0)->setLightingEnabled(false);
        Ogre::TextureUnitState *state = newMat->getTechnique(0)->getPass(0)->getTextureUnitState(0);
        state->setTextureAddressingMode(Ogre::TextureUnitState::TAM_CLAMP);
        state->setTextureFiltering(Ogre::FO_NONE, Ogre::FO_NONE, Ogre::FO_NONE);
    }

    //buttons
    {
        {
            createButton(trayMgr, OgreBites::TL_NONE, "SinglePlayer", "Single", 120);
        }

        {
            OgreBites::Button* widget = createButton(trayMgr, OgreBites::TL_NONE, "MultiPlayer", "Multi", 120);
            widget->getOverlayElement()->setLeft(200.0f);
        }

        {
            OgreBites::TextBox* widget = createTextBox(trayMgr, OgreBites::TL_NONE, "RoomList", "Rooms", 120, 200);
            widget->getOverlayElement()->setLeft(200.0f);
            widget->getOverlayElement()->setTop(200.0f);
            widget->appendText("OK1\n");
            widget->appendText("OK2\n");
            widget->appendText("OK3\n");
            widget->appendText("OK4\n");
            widget->appendText("OK5\n");
            widget->appendText("OK6\n");
            widget->appendText("OK7\n");
            widget->appendText("OK8\n");
            widget->appendText("OK9\n");
            widget->appendText("OK10\n");
        }

        {
            Ogre::StringVector items;
            items.push_back("room1");
            items.push_back("room2");
            items.push_back("room3");
            items.push_back("room4");
            OgreBites::SelectMenu* widget = createSelectMenu(trayMgr, OgreBites::TL_NONE, "RoomList2", "Rooms2", 120, 20, items);
            widget->getOverlayElement()->setLeft(400.0f);
            widget->getOverlayElement()->setTop(200.0f);
        }
    }
#endif
}

#if defined(__ANDROID__)
void UIMainMenu::reloadTextures(const GameState& gameState)
{
    loadMisc(gameState.getPFLoaderData(), gameState.getPFLoaderGameshell());
}
#endif

void UIMainMenu::processButtonClick(MyGUI::Widget* sender)
{
    MyGUI::Button * senderButton = static_cast<MyGUI::Button *>(sender);

    if(senderButton->getCaption() == "Single")
    {

        std::vector<std::string> playersCharacters;
        playersCharacters.push_back(mModeContext.getGameState().getPlayerCar().getCharacterName());
        mMenuMode->recalculateCharacterNames(playersCharacters);

        mModeContext.getGameModeSwitcher()->switchMode(ModeRaceSingle);
    }

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
    if(sender == mWidgetTrack)
    {
        const STRPowerslide& strPowerslide = mModeContext.getGameState().getSTRPowerslide();
        std::vector<std::string> availTracks = strPowerslide.getArrayValue("", "available tracks");
        mModeContext.getGameState().setRaceParameters(availTracks[index], mModeContext.getGameState().getAIStrength());
    }

    if(sender == mWidgetCar)
    {
        mWidgetCharacter->deleteAllItems();

        const STRPowerslide& strPowerslide = mModeContext.getGameState().getSTRPowerslide();
        std::vector<std::string> availCars = strPowerslide.getArrayValue("", "available cars");
        std::string characterCar = availCars[index];
        std::vector<std::string> availChars = strPowerslide.getCharactersByBaseCar(characterCar);

        for(size_t q = 0; q < availChars.size(); ++q)
        {
            mWidgetCharacter->addItem(strPowerslide.getCharacterTitle(availChars[q]));
        }

        mWidgetCharacter->setIndexSelected(0);
        mModeContext.getGameState().getPlayerCar().setCharacterName(availChars[0]);
    }

    if(sender == mWidgetCharacter)
    {
        const STRPowerslide& strPowerslide = mModeContext.getGameState().getSTRPowerslide();

        std::string characterCar = strPowerslide.getCarFromCharacter(mModeContext.getGameState().getPlayerCar().getCharacterName());
        characterCar = strPowerslide.getBaseCarFromCar(characterCar);

        std::vector<std::string> availChars = strPowerslide.getCharactersByBaseCar(characterCar);

        mModeContext.getGameState().getPlayerCar().setCharacterName(availChars[index]);
    }

    if(sender == mWidgetAICount)
    {
        mModeContext.getGameState().setAICount(index + 3);
    }

    if(sender == mWidgetAIStrength)
    {
        mModeContext.getGameState().setRaceParameters(mModeContext.getGameState().getTrackName(), static_cast<AIStrength>(index), mModeContext.getGameState().getLapsCount());
    }
}