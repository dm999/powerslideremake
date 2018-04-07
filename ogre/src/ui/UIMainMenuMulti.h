#ifndef UIMAINMENU_H
#define UIMAINMENU_H

#ifndef NO_MULTIPLAYER

#include <string>

#include "UIBaseMenu.h"

#include "../gamemodes/ModeContext.h"


#include "../GameState.h"

#include "elements/UIEditBox.h"
#include "elements/UIButton.h"

class MenuMultiMode;

class UIMainMenuMulti : public UIBaseMenu, public UIButtonOnAction
{
public:

    UIMainMenuMulti(const ModeContext& modeContext, MenuMultiMode * menuMultiMode);
    ~UIMainMenuMulti(){}

    void load(CustomTrayManager* trayMgr, const GameState& gameState, bool isEnterFromBaseMenu, const std::map<std::string, std::string>& playersSkins);

    void onButtonPressed(UIButton * button) override {}
    void onButtonReleased(UIButton * button) override;

    void frameStarted(const Ogre::FrameEvent &evt) override;

    //void processButtonClick(MyGUI::Widget* sender);
    //void processChangeComboBox(MyGUI::Widget* sender, size_t index);
    //void processKeyPress(MyGUI::Widget* sender, MyGUI::KeyCode key, unsigned int _char);

    void keyUp(MyGUI::KeyCode _key, wchar_t _char);

    void mousePressed(const Ogre::Vector2& pos) override;
    void mouseReleased(const Ogre::Vector2& pos) override;
    void mouseMoved(const Ogre::Vector2& pos) override;

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
    void playerSkinChanged(const std::string& player, const std::string& characterName);
    void hostTrackUpdate(size_t aiCount);

    void destroy(CustomTrayManager* trayMgr) override;

private:

    void createMaterials(CustomTrayManager* trayMgr);

    void loadMisc(const PFLoader& pfLoaderData, const PFLoader& pfLoaderGameshell);

    void updateRoomState(const std::string& playerMessage = "")const;

    void panelHit(Ogre::PanelOverlayElement* panel) override;
    void readySwitcher();
    void aiCountSwitcher(size_t index);

    void addCurrentPlayer(const std::string& player);
    void addOtherPlayer(size_t index, const std::string& player, bool isInSession);

    void switchTrack(size_t trackIndex);
    void switchCar(size_t carIndex);
    void switchCharacter(size_t characterIndex);
    bool isAITrack() const;
    std::string getLapsCountString() const;
    std::string getAIStrengthString() const;
    std::string getCarString() const;
    std::string getCharacterString() const;
    size_t getCurrentTrackIndex(size_t& total) const;
    size_t getCurrentTrackIndex() const;
    size_t getCurrentCarIndex(size_t& total) const;
    size_t getCurrentCarIndex() const;
    size_t getCurrentCharacterIndex(size_t& total) const;
    size_t getCurrentCharacterIndex() const;
    size_t mAICountBeforeNonAITrack;

    MenuMultiMode * mMenuMultiMode;

    Ogre::PanelOverlayElement* mMainBackground;

    Ogre::PanelOverlayElement* mMainChatButtons[GameState::mRaceGridCarsMax];

    Ogre::PanelOverlayElement* mMainCarIcons[GameState::mRaceGridCarsMax];

    Ogre::TextAreaOverlayElement * mChatroomPlayers[GameState::mRaceGridCarsMax];
    std::map<std::string, size_t> mPlayerToChatList;
    Ogre::TextAreaOverlayElement * mChatroomPlayersMessages[GameState::mRaceGridCarsMax - 1];//minus self
    UIEditBox mEditBoxMessage;

    Ogre::TextAreaOverlayElement * mTrackName;
    Ogre::TextAreaOverlayElement * mTrackNameSelected;

    Ogre::TextAreaOverlayElement * mLaps;
    Ogre::TextAreaOverlayElement * mLapsCount;

    Ogre::TextAreaOverlayElement * mAIStrength;
    Ogre::TextAreaOverlayElement * mAIStrengthVal;

    Ogre::TextAreaOverlayElement * mMode;
    Ogre::TextAreaOverlayElement * mModeVal;

    Ogre::TextAreaOverlayElement * mWeapons;

    Ogre::TextAreaOverlayElement * mCar;
    Ogre::TextAreaOverlayElement * mCarVal;

    Ogre::TextAreaOverlayElement * mCharacter;
    Ogre::TextAreaOverlayElement * mCharacterVal;

    Ogre::TextAreaOverlayElement * mChatroom;
    
    UIButtonTick mWeaponsVal;

    UIButton mTracksValLeft;
    UIButton mTracksValRight;

    UIButton mLapsValLeft;
    UIButton mLapsValRight;

    UIButton mCarValLeft;
    UIButton mCarValRight;

    UIButton mCharacterValLeft;
    UIButton mCharacterValRight;

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