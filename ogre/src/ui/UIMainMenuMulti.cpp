#include "../pcheader.h"

#include "UIMainMenuMulti.h"

#include "MyGUI.h"

#include "../tools/OgreTools.h"

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
            Ogre::Vector4 posReady = screenAdaptionRelative * Ogre::Vector4(320.0f, 80.0f, 0.0f, 0.0f);
            mWidgetJoin = gui->createWidget<MyGUI::Button>("Button", posReady.x, posReady.y, 60, 26, MyGUI::Align::Default, "Middle");
            mWidgetJoin->setCaption("Ready");
            mWidgetJoin->eventMouseButtonClick += MyGUI::newDelegate(this, &UIMainMenuMulti::processButtonClick);

            Ogre::Vector4 posJoin = screenAdaptionRelative * Ogre::Vector4(360.0f, 80.0f, 0.0f, 0.0f);
            mWidgetStart = gui->createWidget<MyGUI::Button>("Button", posJoin.x, posJoin.y, 60, 26, MyGUI::Align::Default, "Middle");
            mWidgetStart->setCaption("Start");
            mWidgetStart->eventMouseButtonClick += MyGUI::newDelegate(this, &UIMainMenuMulti::processButtonClick);
            mWidgetStart->setEnabled(false);

            Ogre::Vector4 posRecalc = screenAdaptionRelative * Ogre::Vector4(400.0f, 80.0f, 0.0f, 0.0f);
            mWidgetRecalc = gui->createWidget<MyGUI::Button>("Button", posRecalc.x, posRecalc.y, 60, 26, MyGUI::Align::Default, "Middle");
            mWidgetRecalc->setCaption("Recalc");
            mWidgetRecalc->eventMouseButtonClick += MyGUI::newDelegate(this, &UIMainMenuMulti::processButtonClick);
            mWidgetRecalc->setEnabled(false);
        }
        else
        {
            Ogre::Vector4 posJoin = screenAdaptionRelative * Ogre::Vector4(320.0f, 80.0f, 0.0f, 0.0f);
            mWidgetJoin = gui->createWidget<MyGUI::Button>("Button", posJoin.x, posJoin.y, 60, 26, MyGUI::Align::Default, "Middle");
            mWidgetJoin->setCaption("Ready");
            mWidgetJoin->eventMouseButtonClick += MyGUI::newDelegate(this, &UIMainMenuMulti::processButtonClick);
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

        static_cast<MultiplayerControllerMaster *>(mMenuMultiMode->getMultiplayerController().get())->startSession();
    }

    if(sender == mWidgetRecalc)
    {
        size_t aiAmount = 2;
        mModeContext.getGameState().setAICount(aiAmount);
        mMenuMultiMode->recalculateCharacterNames();
        static_cast<MultiplayerControllerMaster *>(mMenuMultiMode->getMultiplayerController().get())->reconfigureSession(aiAmount);
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

        bool success = mMenuMultiMode->getMultiplayerController()->saySessionReady(mModeContext.mGameState.getPlayerCar().getCharacterName(), changeToReady);
    }
}

void UIMainMenuMulti::onStartPossible()
{
    mWidgetStart->setEnabled(true);
    mWidgetRecalc->setEnabled(true);
}

void UIMainMenuMulti::onStartNotPossible()
{
    mWidgetStart->setEnabled(false);
    mWidgetRecalc->setEnabled(false);
}

void UIMainMenuMulti::addEvent(const std::string& eventItem)
{
    mWidgetEvents->insertItem(0, eventItem);
}