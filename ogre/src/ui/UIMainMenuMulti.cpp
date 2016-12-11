#include "../pcheader.h"

#include "UIMainMenuMulti.h"

#include "MyGUI.h"

#include "../gamelogic/GameModeSwitcher.h"

#include "../tools/OgreTools.h"

#include "../customs/CustomTrayManager.h"

#include "../loaders/TextureLoader.h"

UIMainMenuMulti::UIMainMenuMulti(const ModeContext& modeContext)
: mModeContext(modeContext)
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
        Ogre::Vector4 posJoin = screenAdaptionRelative * Ogre::Vector4(320.0f, 80.0f, 0.0f, 0.0f);
        MyGUI::ButtonPtr widgetJoin = gui->createWidget<MyGUI::Button>("Button", posJoin.x, posJoin.y, 60, 26, MyGUI::Align::Default, "Middle");
        widgetJoin->setCaption("Multi");
        widgetJoin->eventMouseButtonClick += MyGUI::newDelegate(this, &UIMainMenuMulti::processButtonClick);

        Ogre::Vector4 posIP = screenAdaptionRelative * Ogre::Vector4(320.0f, 100.0f, 0.0f, 0.0f);
        MyGUI::EditPtr widgetIP = gui->createWidget<MyGUI::EditBox>("EditBox", posIP.x, posIP.y, 300, 26, MyGUI::Align::Default, "Middle");
        widgetIP->setCaption("78.47.85.155");

        Ogre::Vector4 posRooms = screenAdaptionRelative * Ogre::Vector4(320.0f, 120.0f, 0.0f, 0.0f);
        MyGUI::ListPtr widgetRooms = gui->createWidget<MyGUI::ListBox>("ListBox", posRooms.x, posRooms.y, 300, 200, MyGUI::Align::Default, "Middle");
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
    MyGUI::Button * senderButton = static_cast<MyGUI::Button *>(sender);

    if(senderButton->getCaption() == "Multi")
        mModeContext.getGameModeSwitcher()->switchMode(ModeRaceMulti);
}