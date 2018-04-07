#ifndef UIMAINMENU_H
#define UIMAINMENU_H

#include "UIMainMenuLabels.h"

#include "../GameState.h"

#include "elements/UIEditBox.h"

class MenuMode;
class LoaderListener;

class UIMainMenu : public UIMainMenuLabels
{
public:

    UIMainMenu(const ModeContext& modeContext, MenuMode * menuMode, SinglePlayerMenuStates state);
    ~UIMainMenu();

    void load(CustomTrayManager* trayMgr, const GameState& gameState, LoaderListener* loaderListener);

    void frameStarted(const Ogre::FrameEvent &evt) override;

#if defined(__ANDROID__)
    void reloadTextures(const GameState& gameState);
#endif

    void keyUp(MyGUI::KeyCode _key, wchar_t _char);

    void mousePressed(const Ogre::Vector2& pos) override;
    void mouseReleased(const Ogre::Vector2& pos) override;
    void mouseMoved(const Ogre::Vector2& pos) override;

    bool isExitSubmenu()const;
    void setExitSubmenu();
    void setTopmostSubmenu();

    void destroy(CustomTrayManager* trayMgr) override;

protected:

    void switchState(const SinglePlayerMenuStates& state)override;
    void startRace()override;

private:

    void hideAll();

    void panelHit(Ogre::PanelOverlayElement* panel) override;

    void loadMisc(const PFLoader& pfLoaderData, const PFLoader& pfLoaderGameshell, LoaderListener* loaderListener);

    void connectToServer();
    void createRoom();
    void joinRoom();

    finishBoard_v prepareFinishBoard()const;

    Ogre::Timer mStartingGridTimer;
    bool mIsInStartingGrid;

    MenuMode * mMenuMode;

    SinglePlayerMenuStates mCurrentState;

    UIEditBox mEditBox;
    UIEditBox mEditBoxIP;
    UIEditBox mEditBoxUserName;
    UIEditBox mEditBoxRoomName;

    OgreBites::SelectMenu* mRoomsTable;
};

#endif