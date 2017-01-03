#include "../pcheader.h"

#include "UIRaceMulti.h"

#include "MyGUI.h"

#include "../gamemodes/MultiPlayerMode.h"

UIRaceMulti::UIRaceMulti(const ModeContext& modeContext, MultiPlayerMode * multiPlayerMode)
    : mModeContext(modeContext),
    mMultiPlayerMode(multiPlayerMode)
{}

void UIRaceMulti::load(MyGUI::Gui* gui, const GameState& gameState)
{
    Ogre::OverlayManager& om = Ogre::OverlayManager::getSingleton(); 
    Ogre::Real viewportWidth = om.getViewportWidth(); 
    Ogre::Real viewportHeight = om.getViewportHeight(); 

    Ogre::Matrix4 screenAdaptionRelative(
        viewportWidth / 640.0f, 0.0f,                       0.0f,                   0.0f,
        0.0f,                   viewportHeight / 480.0f,    0.0f,                   0.0f,
        0.0f,                   0.0f,                       viewportWidth / 640.0f, 0.0f,
        0.0f,                   0.0f,                       0.0f,                   viewportHeight / 480.0f);

    Ogre::Vector4 posMessage = screenAdaptionRelative * Ogre::Vector4(430.0f, 0.0f, 200.0f, 12.0f);
    mWidgetMessage = gui->createWidget<MyGUI::EditBox>("EditBox", posMessage.x, posMessage.y, posMessage.z, posMessage.w, MyGUI::Align::Default, "Middle");
    mWidgetMessage->eventKeyButtonPressed += MyGUI::newDelegate(this, &UIRaceMulti::processKeyPress);
    mWidgetMessage->setTabPrinting(false);
    mWidgetMessage->setVisible(false);

    //focus widget
    MyGUI::InputManager::getInstance().setKeyFocusWidget(mWidgetMessage);
    MyGUI::InputManager::getInstance().getKeyFocusWidget();
}

void UIRaceMulti::switchVisibleMessageWidget()
{
    mWidgetMessage->setVisible(!mWidgetMessage->isVisible());

    if(mWidgetMessage->isVisible())
    {
        MyGUI::InputManager::getInstance().setKeyFocusWidget(mWidgetMessage);
        MyGUI::InputManager::getInstance().getKeyFocusWidget();
    }
}

void UIRaceMulti::processKeyPress(MyGUI::Widget* sender, MyGUI::KeyCode key, unsigned int _char)
{
    if(sender == mWidgetMessage)
    {
        if(key == MyGUI::KeyCode::Return)
        {
            std::string message = mWidgetMessage->getCaption();
            if(!message.empty())
            {
                mWidgetMessage->setCaption("");

                MultiplayerLobbyData multiplayerLobbyData(
                    false, 
                    "", 
                    message, 
                    "", 
                    0, 0, 
                    0);

                bool success = mMultiPlayerMode->getMultiplayerController()->sendLobbyMessage(multiplayerLobbyData, true, 10);
            }
        }
    }
}