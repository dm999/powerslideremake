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

    void mousePressed(const Ogre::Vector2& pos);
    void mouseReleased(const Ogre::Vector2& pos);
    void mouseMoved(const Ogre::Vector2& pos);

    bool isExitSubmenu()const;
    void setExitSubmenu();
    void setTopmostSubmenu();

    virtual void destroy(CustomTrayManager* trayMgr);

protected:

    void switchState(const SinglePlayerMenuStates& state)override;
    void startRace()override;

private:

    void hideAll();

    void panelHit(Ogre::PanelOverlayElement* panel) override;

    void loadMisc(const PFLoader& pfLoaderData, const PFLoader& pfLoaderGameshell, LoaderListener* loaderListener);

    finishBoard_v prepareFinishBoard()const;

    Ogre::Timer mStartingGridTimer;
    bool mIsInStartingGrid;

    MenuMode * mMenuMode;

    SinglePlayerMenuStates mCurrentState;

    UIEditBox mEditBox;
    UIEditBox mEditBox2;
};

#endif