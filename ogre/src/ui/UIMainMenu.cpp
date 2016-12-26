#include "../pcheader.h"

#include "UIMainMenu.h"

#include "MyGUI.h"

#include "../gamelogic/GameModeSwitcher.h"

#include "../tools/OgreTools.h"

#include "../customs/CustomTrayManager.h"

#include "../loaders/TextureLoader.h"

#include "../multiplayer/MultiplayerRoomInfo.h"

UIMainMenu::UIMainMenu(const ModeContext& modeContext)
: mModeContext(modeContext)
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
        mWidgetUserName->setCaption("DM");
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
        mModeContext.getGameModeSwitcher()->switchMode(ModeRaceSingle);

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
            mModeContext.mGameState.setMultiplayerMaster(false);
            mModeContext.mGameState.setMultiplayerServerIP(mWidgetIP->getCaption());

            std::string roomName = mWidgetRooms->getItemNameAt(mWidgetRooms->getIndexSelected());
            roomName = roomName.substr(7, roomName.size());
            mModeContext.mGameState.setMultiplayerRoomName(roomName);

            mModeContext.mGameState.setMultiplayerUserName(mWidgetUserName->getCaption());
            mModeContext.getGameModeSwitcher()->switchMode(ModeMenuMulti);
        }
    }

    if(senderButton->getCaption() == "Multi Host")
    {
        mModeContext.mGameState.setMultiplayerMaster(true);
        mModeContext.mGameState.setMultiplayerServerIP(mWidgetIP->getCaption());
        mModeContext.mGameState.setMultiplayerRoomName(mWidgetRoom->getCaption());
        mModeContext.mGameState.setMultiplayerUserName(mWidgetUserName->getCaption());
        mModeContext.getGameModeSwitcher()->switchMode(ModeMenuMulti);
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