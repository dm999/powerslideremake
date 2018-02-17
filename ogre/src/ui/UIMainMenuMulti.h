#ifndef UIMAINMENU_H
#define UIMAINMENU_H

#ifndef NO_MULTIPLAYER

#include <string>

#include "UIBaseMenu.h"

#include "../gamemodes/ModeContext.h"


#include "../GameState.h"

#include "elements/UIEditBox.h"

class MenuMultiMode;

class UIMainMenuMulti : public UIBaseMenu, public OgreBites::SdkTrayListener
{
public:

    UIMainMenuMulti(const ModeContext& modeContext, MenuMultiMode * menuMultiMode);
    ~UIMainMenuMulti(){}

    void load(CustomTrayManager* trayMgr, const GameState& gameState, bool isEnterFromBaseMenu);

    void frameStarted(const Ogre::FrameEvent &evt) override;

    //void processButtonClick(MyGUI::Widget* sender);
    //void processChangeComboBox(MyGUI::Widget* sender, size_t index);
    //void processKeyPress(MyGUI::Widget* sender, MyGUI::KeyCode key, unsigned int _char);

    void keyUp(MyGUI::KeyCode _key, wchar_t _char);

    void mousePressed(const Ogre::Vector2& pos);
    void mouseReleased(const Ogre::Vector2& pos);
    void mouseMoved(const Ogre::Vector2& pos);

    void setMiscText(const std::string& text, const Ogre::ColourValue& color = Ogre::ColourValue::White);

#if defined(__ANDROID__)
    void reloadTextures(const GameState& gameState);
#endif

    //master ready to press start race
    void onStartPossible();
    void onStartNotPossible();

    void addEvent(const std::string& eventItem, bool isMessage = false);

    void roomEnter(const std::string& roomName, const std::string& player, const std::vector<std::string>& players);
    void roomJoined(const std::string& player);
    void roomLeft(const std::string& player);
    void playerMessage(const std::string& player, const std::string& message);
    void playerReadyChange(const std::string& player, bool isReady);
    void hostAICountChange(size_t count);

    void buttonHit(OgreBites::Button* button) override;

    void destroy(CustomTrayManager* trayMgr) override;

private:

    void createMaterials(CustomTrayManager* trayMgr);

    void loadMisc(const PFLoader& pfLoaderData, const PFLoader& pfLoaderGameshell);

    void updateRoomState(const std::string& playerMessage = "")const;

    void panelHit(Ogre::PanelOverlayElement* panel) override;
    void aiCountSwitcher(size_t index);

    void addCurrentPlayer(const std::string& player);
    void addOtherPlayer(size_t index, const std::string& player, bool isInSession);

    MenuMultiMode * mMenuMultiMode;

    Ogre::PanelOverlayElement* mMainBackground;

    Ogre::PanelOverlayElement* mMainChatButtons[GameState::mRaceGridCarsMax];

    Ogre::TextAreaOverlayElement * mChatroomPlayers[GameState::mRaceGridCarsMax];
    std::map<std::string, size_t> mPlayerToChatList;
    Ogre::TextAreaOverlayElement * mChatroomPlayersMessages[GameState::mRaceGridCarsMax - 1];//minus self
    UIEditBox mEditBoxMessage;

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