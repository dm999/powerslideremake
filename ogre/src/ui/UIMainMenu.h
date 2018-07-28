#ifndef UIMAINMENU_H
#define UIMAINMENU_H

#include "UIMainMenuLabels.h"

#include "../GameState.h"

#include "elements/UIEditBox.h"

class MenuMode;
class LoaderListener;

class UIMainMenu : public UIMainMenuLabels, public UITableOnAction
{
public:

    UIMainMenu(const ModeContext& modeContext, const GameMode gameMode, MenuMode * menuMode, SinglePlayerMenuStates state);
    ~UIMainMenu();

    //UIButtonOnAction
    void onButtonReleased(UIButton * button) override;
    void onButtonOver(UIButton * button) override;

    void load(CustomTrayManager* trayMgr, const GameState& gameState, LoaderListener* loaderListener);

    void frameStarted(const Ogre::FrameEvent &evt) override;

#if defined(__ANDROID__)
    void reloadTextures(const GameState& gameState);
#endif

    void keyUp(MyGUI::KeyCode _key, wchar_t _char);

    void mousePressed(const Ogre::Vector2& pos) override;
    void mouseReleased(const Ogre::Vector2& pos) override;
    void mouseMoved(const Ogre::Vector2& pos) override;

    SinglePlayerMenuStates getSubmenuState() const { return mCurrentState;}
    bool isExitSubmenu()const;
    void setSubmenu(const std::string& title);
    void setTopmostSubmenu();
    void setDefaultBackground(bool isSwitchState);
    void setPodiumSubmenu();

    void destroy(CustomTrayManager* trayMgr) override;

    void onTablePressed(UITable * table, size_t row) override {}
    void onTableReleased(UITable * table, size_t row) override;

protected:

    void switchState(const SinglePlayerMenuStates& state)override;
    void startRace()override;
    void connectToServer()override;

private:

    void hideAll();

    void panelHit(Ogre::PanelOverlayElement* panel) override;

    void loadMisc(const PFLoader& pfLoaderData, const PFLoader& pfLoaderGameshell, LoaderListener* loaderListener);

    void createRoom();
    void joinRoom();

    void onNameChange() override;

    void redrawTable();

    Ogre::Timer mStartingGridTimer;
    bool mIsInStartingGrid;

    MenuMode * mMenuMode;

    SinglePlayerMenuStates mCurrentState;

    UIEditBox mEditBoxUserName;
    UIEditBox mEditBoxMultiIP;
    UIEditBox mEditBoxMultiUserName;
    UIEditBox mEditBoxMultiRoomName;
    UITable mMultiRoomsList;
    UIButton mRoomsMoveTop;
    UIButton mRoomsMoveBottom;
    UITable mMultiRoomPlayersList;
    size_t mSelectedRoomIndex;
    size_t mTableOffset;
    std::vector<std::string> mRoomNames;
    std::vector<std::string> mRoomsDescriptions;
    std::vector<std::string> mRoomsHosts;
    std::vector<std::pair<size_t, size_t> > mPlayersInServerRooms;
    std::vector<std::vector<std::string>> mPlayersNamesInRooms;

    std::string mExitSubmenuTitle;
};

#endif