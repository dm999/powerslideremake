#ifndef UIMAINMENU_H
#define UIMAINMENU_H

#include <string>

#include "UIBase.h"
#include "MyGUI_KeyCode.h"

#include "../gamemodes/ModeContext.h"


#include "../GameState.h"

namespace MyGUI
{
    class Gui;
    class Widget;
    class Button;
    class ComboBox;
    class ListBox;
    class EditBox;
}

class MenuMultiMode;

class UIMainMenuMulti : public UIBase
{
public:

    UIMainMenuMulti(const ModeContext& modeContext, MenuMultiMode * menuMultiMode);
    ~UIMainMenuMulti();

    void load(MyGUI::Gui* gui, const GameState& gameState);

    void processButtonClick(MyGUI::Widget* sender);
    void processChangeComboBox(MyGUI::Widget* sender, size_t index);
    void processKeyPress(MyGUI::Widget* sender, MyGUI::KeyCode key, unsigned int _char);

#if defined(__ANDROID__)
    void reloadTextures(const GameState& gameState);
#endif

    //master ready to press start race
    void onStartPossible();
    void onStartNotPossible();

    void addEvent(const std::string& eventItem, bool isMessage = false);

private:

    ModeContext mModeContext;

    void loadMisc(const PFLoader& pfLoaderData, const PFLoader& pfLoaderGameshell);

    void updateRoomState(const std::string& playerMessage = "")const;

    MenuMultiMode * mMenuMultiMode;

    MyGUI::Button* mWidgetJoin;
    MyGUI::Button* mWidgetStart;
    MyGUI::ComboBox* mWidgetTrack;
    MyGUI::ComboBox* mWidgetCar;
    MyGUI::ComboBox* mWidgetCharacter;
    MyGUI::ComboBox* mWidgetAICount;
    MyGUI::ComboBox* mWidgetAIStrength;
    MyGUI::ComboBox* mWidgetLapsCount;
    MyGUI::ComboBox* mWidgetBroadcast;

    MyGUI::EditBox* mWidgetMessage;
    MyGUI::Button* mWidgetSendMessage;

    MyGUI::ListBox* mWidgetEvents;
};

#endif