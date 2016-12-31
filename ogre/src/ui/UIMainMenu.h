#ifndef UIMAINMENU_H
#define UIMAINMENU_H

#include "UIBase.h"

#include "MyGUI_KeyCode.h"

#include "../gamemodes/ModeContext.h"

#include "../GameState.h"

namespace MyGUI
{
    class Gui;
    class Widget;
    class ListBox;
    class EditBox;
    class ComboBox;
}

class MenuMode;

class UIMainMenu : public UIBase
{
public:

    UIMainMenu(const ModeContext& modeContext, MenuMode * menuMode);
    ~UIMainMenu();

    void load(MyGUI::Gui* gui, const GameState& gameState);

    void processButtonClick(MyGUI::Widget* sender);
    void processKeyPress(MyGUI::Widget* sender, MyGUI::KeyCode key, unsigned int _char);
    void processItemSelected(MyGUI::Widget* sender, size_t index);
    void processChangeComboBox(MyGUI::Widget* sender, size_t index);

#if defined(__ANDROID__)
    void reloadTextures(const GameState& gameState);
#endif

private:

    ModeContext mModeContext;

    void loadMisc(const PFLoader& pfLoaderData, const PFLoader& pfLoaderGameshell);

    MenuMode * mMenuMode;

    MyGUI::ComboBox* mWidgetTrack;
    MyGUI::ComboBox* mWidgetCar;
    MyGUI::ComboBox* mWidgetCharacter;
    MyGUI::ComboBox* mWidgetAICount;
    MyGUI::ComboBox* mWidgetAIStrength;

    MyGUI::EditBox* mWidgetIP;
    MyGUI::EditBox* mWidgetRoom;
    MyGUI::EditBox* mWidgetUserName;
    MyGUI::ListBox* mWidgetRooms;
    MyGUI::ListBox* mWidgetRoomPlayers;

    //Ogre::PanelOverlayElement* mMainBackground;
    //Ogre::PanelOverlayElement* mBackgroundA;
    //Ogre::PanelOverlayElement* mBackgroundB;
};

#endif