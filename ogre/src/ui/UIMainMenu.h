#ifndef UIMAINMENU_H
#define UIMAINMENU_H

#include "../includes/OgreInclude.h"

#include "SdkTrays.h"

#include "../GameState.h"

class CustomTrayManager;

class UIMainMenu
{
public:

    UIMainMenu();
    ~UIMainMenu();

    void load(CustomTrayManager* trayMgr, const GameState& gameState);

    void destroy();

#if defined(__ANDROID__)
    void reloadTextures(const GameState& gameState);
#endif

private:

    void loadMisc(const PFLoader& pfLoaderData, const PFLoader& pfLoaderGameshell);

    Ogre::PanelOverlayElement* mMainBackground;
    Ogre::PanelOverlayElement* mBackgroundA;
    Ogre::PanelOverlayElement* mBackgroundB;
};

#endif