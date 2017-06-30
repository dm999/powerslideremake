#ifndef UIMAINMENU_H
#define UIMAINMENU_H

#include "UIBase.h"

#include "../gamemodes/ModeContext.h"

#include "../GameState.h"

class MenuMode;
class LoaderListener;

class UIMainMenu : public UIBase
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

    void panelHit(Ogre::PanelOverlayElement* panel);

    void startRace();

    ModeContext mModeContext;

    void loadMisc(const PFLoader& pfLoaderData, const PFLoader& pfLoaderGameshell, LoaderListener* loaderListener);

    MenuMode * mMenuMode;

#if 0
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
#endif

    Ogre::PanelOverlayElement* mMainBackground;
    Ogre::PanelOverlayElement* mBackgroundA;
    Ogre::PanelOverlayElement* mBackgroundB;

    static const int mControlsCount = 6;
    Ogre::PanelOverlayElement* mControls[mControlsCount];
};

#endif