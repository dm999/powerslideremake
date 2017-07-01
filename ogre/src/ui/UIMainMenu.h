#ifndef UIMAINMENU_H
#define UIMAINMENU_H

#include "UIBaseMenu.h"

#include "../gamemodes/ModeContext.h"

#include "../GameState.h"

class MenuMode;
class LoaderListener;

class UIMainMenu : public UIBaseMenu
{
public:

    UIMainMenu(const ModeContext& modeContext, MenuMode * menuMode);
    ~UIMainMenu();

    void load(CustomTrayManager* trayMgr, const GameState& gameState, LoaderListener* loaderListener);

#if 0
    void processButtonClick(MyGUI::Widget* sender);
    void processKeyPress(MyGUI::Widget* sender, MyGUI::KeyCode key, unsigned int _char);
    void processItemSelected(MyGUI::Widget* sender, size_t index);
    void processChangeComboBox(MyGUI::Widget* sender, size_t index);
#endif

#if defined(__ANDROID__)
    void reloadTextures(const GameState& gameState);
#endif

    //OgreBites::SdkTrayListener
    virtual void buttonHit(OgreBites::Button* button);
    virtual void itemSelected(OgreBites::SelectMenu* menu);

    void mousePressed(const Ogre::Vector2& pos);
    void mouseReleased(const Ogre::Vector2& pos);
    void mouseMoved(const Ogre::Vector2& pos);

private:

    void createSpecificMaterials();

    void panelHit(Ogre::PanelOverlayElement* panel) override;

    void startRace();

    ModeContext mModeContext;

    void loadMisc(const PFLoader& pfLoaderData, const PFLoader& pfLoaderGameshell, LoaderListener* loaderListener);

    MenuMode * mMenuMode;

    Ogre::PanelOverlayElement* mMainBackground;
    Ogre::PanelOverlayElement* mBackgroundA;
    Ogre::PanelOverlayElement* mBackgroundB;
};

#endif