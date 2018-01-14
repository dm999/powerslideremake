#ifndef UIMAINMENU_H
#define UIMAINMENU_H

#ifndef NO_MULTIPLAYER

#include <string>

#include "UIBaseMenu.h"

#include "../gamemodes/ModeContext.h"


#include "../GameState.h"

class MenuMultiMode;

class UIMainMenuMulti : public UIBaseMenu, public OgreBites::SdkTrayListener
{
public:

    UIMainMenuMulti(const ModeContext& modeContext, MenuMultiMode * menuMultiMode);
    ~UIMainMenuMulti(){}

    void load(CustomTrayManager* trayMgr, const GameState& gameState);

    void frameStarted(const Ogre::FrameEvent &evt) override {}

    //void processButtonClick(MyGUI::Widget* sender);
    //void processChangeComboBox(MyGUI::Widget* sender, size_t index);
    //void processKeyPress(MyGUI::Widget* sender, MyGUI::KeyCode key, unsigned int _char);

    void mouseReleased(const Ogre::Vector2& pos);

    void setMiscText(const std::string& text, const Ogre::ColourValue& color = Ogre::ColourValue::White);

#if defined(__ANDROID__)
    void reloadTextures(const GameState& gameState);
#endif

    //master ready to press start race
    void onStartPossible();
    void onStartNotPossible();

    void addEvent(const std::string& eventItem, bool isMessage = false);

    void buttonHit(OgreBites::Button* button) override;

private:

    void loadMisc(const PFLoader& pfLoaderData, const PFLoader& pfLoaderGameshell);

    void updateRoomState(const std::string& playerMessage = "")const;

    void panelHit(Ogre::PanelOverlayElement* panel) override;

    MenuMultiMode * mMenuMultiMode;

    Ogre::PanelOverlayElement* mMainBackground;

    OgreBites::Button* mWidgetJoin;
    OgreBites::Button* mWidgetStart;

    /*
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

    MyGUI::TextBox* mWidgetPingLabel;

    MyGUI::ListBox* mWidgetEvents;
    */
};
#endif
#endif