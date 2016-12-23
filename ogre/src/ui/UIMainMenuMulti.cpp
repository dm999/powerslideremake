#include "../pcheader.h"

#include "UIMainMenuMulti.h"

#include "MyGUI.h"

#include "../tools/OgreTools.h"
#include "../tools/Conversions.h"

#include "../customs/CustomTrayManager.h"

#include "../loaders/TextureLoader.h"

#include "../gamemodes/MenuMultiMode.h"

#include "../multiplayer/MultiplayerControllerMaster.h"
#include "../multiplayer/MultiplayerControllerSlave.h"

UIMainMenuMulti::UIMainMenuMulti(const ModeContext& modeContext, MenuMultiMode * menuMultiMode)
    : mModeContext(modeContext),
    mMenuMultiMode(menuMultiMode)
{}

UIMainMenuMulti::~UIMainMenuMulti()
{}

void UIMainMenuMulti::loadMisc(const PFLoader& pfLoaderData, const PFLoader& pfLoaderGameshell)
{
    TextureLoader().loadChroma( pfLoaderGameshell, 
                                "data/gameshell", "cursor.bmp", 
                                "OriginalCursor", TEMP_RESOURCE_GROUP_NAME, 
                                Ogre::ColourValue(0.0f, 1.0f, 0.1f, 1.0f), 0.1f);

    TextureLoader().load( pfLoaderGameshell, 
                                "data/gameshell", "m_chat.bmp", 
                                "OriginalMainBackground", TEMP_RESOURCE_GROUP_NAME);
}

void UIMainMenuMulti::load(MyGUI::Gui* gui, const GameState& gameState)
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
        MyGUI::ImageBox * mainBackground = gui->createWidget<MyGUI::ImageBox>("ImageBox", 0, 0, viewportWidth, viewportHeight, MyGUI::Align::Default, "Wallpaper");
        mainBackground->setImageTexture("OriginalMainBackground");
    }

    {
        if(mModeContext.mGameState.isMultiplayerMaster())
        {
            Ogre::Vector4 posReady = screenAdaptionRelative * Ogre::Vector4(320.0f, 80.0f, 40.0f, 12.0f);
            mWidgetJoin = gui->createWidget<MyGUI::Button>("Button", posReady.x, posReady.y, posReady.z, posReady.w, MyGUI::Align::Default, "Middle");
            mWidgetJoin->setCaption("Ready");
            mWidgetJoin->eventMouseButtonClick += MyGUI::newDelegate(this, &UIMainMenuMulti::processButtonClick);

            Ogre::Vector4 posJoin = screenAdaptionRelative * Ogre::Vector4(360.0f, 80.0f, 40.0f, 12.0f);
            mWidgetStart = gui->createWidget<MyGUI::Button>("Button", posJoin.x, posJoin.y, posJoin.z, posJoin.w, MyGUI::Align::Default, "Middle");
            mWidgetStart->setCaption("Start");
            mWidgetStart->eventMouseButtonClick += MyGUI::newDelegate(this, &UIMainMenuMulti::processButtonClick);
            mWidgetStart->setEnabled(false);

            Ogre::Vector4 posTrack = screenAdaptionRelative * Ogre::Vector4(400.0f, 80.0f, 60.0f, 12.0f);
            mWidgetTrack = gui->createWidget<MyGUI::ComboBox>("ComboBox", posTrack.x, posTrack.y, posTrack.z, posTrack.w, MyGUI::Align::Default, "Middle");
            mWidgetTrack->addItem("desert track");
            mWidgetTrack->addItem("speedway track");
            mWidgetTrack->addItem("dam");
            mWidgetTrack->addItem("mountain forest track");
            mWidgetTrack->addItem("mineshaft");
            mWidgetTrack->addItem("alpine track");
            mWidgetTrack->addItem("city track");
            mWidgetTrack->addItem("nutopia track");
            mWidgetTrack->addItem("stunt track");
            mWidgetTrack->addItem("luge track");
            mWidgetTrack->addItem("Foxnhound1 track");
            mWidgetTrack->addItem("Foxnhound2 track");
            mWidgetTrack->setIndexSelected(0);
            mWidgetTrack->setEditReadOnly(true);
            mWidgetTrack->eventComboChangePosition += MyGUI::newDelegate(this, &UIMainMenuMulti::processChangeComboBox);

            Ogre::Vector4 posAI = screenAdaptionRelative * Ogre::Vector4(520.0f, 80.0f, 30.0f, 12.0f);
            mWidgetAICount = gui->createWidget<MyGUI::ComboBox>("ComboBox", posAI.x, posAI.y, posAI.z, posAI.w, MyGUI::Align::Default, "Middle");
            for(size_t q = 0; q < 11; ++q)
                mWidgetAICount->addItem(Conversions::DMToString(q));
            mWidgetAICount->setIndexSelected(1);
            mWidgetAICount->setEditReadOnly(true);
            mWidgetAICount->eventComboChangePosition += MyGUI::newDelegate(this, &UIMainMenuMulti::processChangeComboBox);

        }
        else
        {
            Ogre::Vector4 posJoin = screenAdaptionRelative * Ogre::Vector4(320.0f, 80.0f, 40.0f, 12.0f);
            mWidgetJoin = gui->createWidget<MyGUI::Button>("Button", posJoin.x, posJoin.y, posJoin.z, posJoin.w, MyGUI::Align::Default, "Middle");
            mWidgetJoin->setCaption("Ready");
            mWidgetJoin->eventMouseButtonClick += MyGUI::newDelegate(this, &UIMainMenuMulti::processButtonClick);
        }

        {
            Ogre::Vector4 posChar = screenAdaptionRelative * Ogre::Vector4(460.0f, 80.0f, 60.0f, 12.0f);
            mWidgetCharacter = gui->createWidget<MyGUI::ComboBox>("ComboBox", posChar.x, posChar.y, posChar.z, posChar.w, MyGUI::Align::Default, "Middle");
            mWidgetCharacter->addItem("frantic");
            mWidgetCharacter->addItem("cyber");
            mWidgetCharacter->addItem("radiation");
            mWidgetCharacter->addItem("zig");
            mWidgetCharacter->addItem("zag");
            mWidgetCharacter->addItem("beryl");
            mWidgetCharacter->addItem("stig");
            mWidgetCharacter->addItem("hemp");
            mWidgetCharacter->setIndexSelected(0);
            mWidgetCharacter->setEditReadOnly(true);
            mWidgetCharacter->eventComboChangePosition += MyGUI::newDelegate(this, &UIMainMenuMulti::processChangeComboBox);
        }

        {
            Ogre::Vector4 posEvents = screenAdaptionRelative * Ogre::Vector4(20.0f, 130.0f, 600.0f, 300.0f);
            mWidgetEvents = gui->createWidget<MyGUI::ListBox>("ListBox", posEvents.x, posEvents.y, posEvents.z, posEvents.w, MyGUI::Align::Default, "Middle");
            //mWidgetEvents->setColour(MyGUI::Colour(0.0f, 0.0f, 0.0f));
        }

        //Ogre::Vector4 posIP = screenAdaptionRelative * Ogre::Vector4(320.0f, 100.0f, 0.0f, 0.0f);
        //MyGUI::EditPtr widgetIP = gui->createWidget<MyGUI::EditBox>("EditBox", posIP.x, posIP.y, 300, 26, MyGUI::Align::Default, "Middle");
        //widgetIP->setCaption("78.47.85.155");

        //Ogre::Vector4 posRooms = screenAdaptionRelative * Ogre::Vector4(320.0f, 120.0f, 0.0f, 0.0f);
        //MyGUI::ListPtr widgetRooms = gui->createWidget<MyGUI::ListBox>("ListBox", posRooms.x, posRooms.y, 300, 200, MyGUI::Align::Default, "Middle");
    }

    //MyGUI::PointerManager::getInstance().getByName("arrow")->setResourceName();

}

#if defined(__ANDROID__)
void UIMainMenuMulti::reloadTextures(const GameState& gameState)
{
    loadMisc(gameState.getPFLoaderData(), gameState.getPFLoaderGameshell());
}
#endif

void UIMainMenuMulti::processButtonClick(MyGUI::Widget* sender)
{
    if(sender == mWidgetStart)
    {
        mWidgetStart->setEnabled(false);

        if(mModeContext.mGameState.isMultiplayerMaster())
        {
            std::vector<std::string> gameCars;
            for(size_t q = 0; q < mModeContext.mGameState.getAICount(); ++q)
            {
                gameCars.push_back(mModeContext.mGameState.getAICar(q).getCharacterName());
            }

            static_cast<MultiplayerControllerMaster*>(mMenuMultiMode->getMultiplayerController().get())->setAISkins(gameCars);
        }

        static_cast<MultiplayerControllerMaster *>(mMenuMultiMode->getMultiplayerController().get())->startSession(mModeContext.mGameState.getTrackName());
    }

    if(sender == mWidgetJoin)
    {
        bool changeToReady = true;
        if(mWidgetJoin->getCaption() == "Ready")
        {
            mWidgetJoin->setCaption("Not ready");
        }
        else
        {
            changeToReady = false;
            mWidgetJoin->setCaption("Ready");
        }

        std::string trackName = "";

        if(mModeContext.mGameState.isMultiplayerMaster())
        {
            trackName = mWidgetTrack->getItem(mWidgetTrack->getItemIndexSelected());
        }

        bool success = mMenuMultiMode->getMultiplayerController()->sendLobbyMessage(
            changeToReady, 
            mModeContext.mGameState.getPlayerCar().getCharacterName(), 
            "", 
            trackName,
            mModeContext.mGameState.getAICount());
    }

}

void UIMainMenuMulti::processChangeComboBox(MyGUI::Widget* sender, size_t index)
{
    if(sender == mWidgetTrack)
    {
        mModeContext.getGameState().setRaceParameters(mWidgetTrack->getItem(index), Insane);
    }

    if(sender == mWidgetCharacter)
    {
        mModeContext.getGameState().getPlayerCar().setCharacterName(mWidgetCharacter->getItem(index));
    }

    if(sender == mWidgetAICount)
    {
        mModeContext.getGameState().setAICount(index);
        mMenuMultiMode->recalculateCharacterNames();
        static_cast<MultiplayerControllerMaster *>(mMenuMultiMode->getMultiplayerController().get())->reconfigureSession(index);
    }

    updateRoomState();
}

void UIMainMenuMulti::onStartPossible()
{
    mWidgetStart->setEnabled(true);
}

void UIMainMenuMulti::onStartNotPossible()
{
    mWidgetStart->setEnabled(false);
}

void UIMainMenuMulti::addEvent(const std::string& eventItem)
{
    mWidgetEvents->insertItem(0, eventItem);
}

void UIMainMenuMulti::updateRoomState(const std::string& playerMessage)const
{
    bool changeToReady = true;
    if(mWidgetJoin->getCaption() == "Ready")
        changeToReady = false;

    if(mModeContext.mGameState.isMultiplayerMaster())
    {
        bool success = mMenuMultiMode->getMultiplayerController()->sendLobbyMessage(
            changeToReady, 
            mModeContext.mGameState.getPlayerCar().getCharacterName(), 
            playerMessage, 
            mModeContext.mGameState.getTrackName(),
            mModeContext.mGameState.getAICount());
    }
    else
    {
        bool success = mMenuMultiMode->getMultiplayerController()->sendLobbyMessage(
            changeToReady, 
            mModeContext.mGameState.getPlayerCar().getCharacterName(), 
            playerMessage, 
            "",
            0);
    }
}