#ifndef UIRACEMULTI_H
#define UIRACEMULTI_H

#ifndef NO_MULTIPLAYER
#include <string>

#include "UIBase.h"

#include "../gamemodes/ModeContext.h"


#include "../GameState.h"

#include "elements/UIEditBox.h"

class MultiPlayerMode;


class UIRaceMulti
{
public:

    UIRaceMulti(const ModeContext& modeContext, MultiPlayerMode * multiPlayerMode);
    ~UIRaceMulti(){}

    void load(CustomTrayManager* trayMgr, const GameState& gameState);

    void switchVisibleMessageWidget();

    void keyUp(MyGUI::KeyCode _key, wchar_t _char);

    void frameStarted(const Ogre::FrameEvent &evt);

    void destroy(CustomTrayManager* trayMgr);

#if defined(__ANDROID__)
    void reloadTextures(const GameState& gameState);
#endif

private:

    ModeContext mModeContext;

    MultiPlayerMode * mMultiPlayerMode;

    UIEditBox mEditBoxMessage;

    void loadMisc(const PFLoader& pfLoaderData, const PFLoader& pfLoaderGameshell);

    //MyGUI::EditBox* mWidgetMessage;
};

#endif
#endif