#ifndef MODECONTEXT_H
#define MODECONTEXT_H

#include "../GameState.h"
#include "../SoundsProcesser.h"

namespace Ogre
{
    class Root;
    class RenderWindow;
}

namespace MyGUI
{
    class Gui;
    class OgrePlatform;
}

struct lua_State;

class InputHandler;
class CustomTrayManager;
class CustomOverlaySystem;
class GameModeSwitcher;

class ModeContext
{
public:

    ModeContext(
        Ogre::Root* root, Ogre::RenderWindow* window,
        InputHandler* inputHandler,
        CustomTrayManager* trayMgr, CustomOverlaySystem* overlaySystem,
        lua_State * pipeline,
        GameState& gameState,
#ifndef NO_OPENAL
        SoundsProcesser& soundsProcesser,
#endif
        MyGUI::Gui* gui, MyGUI::OgrePlatform* platform
    );

    ModeContext(const ModeContext& other);

    ~ModeContext(){}

    void setGameModeSwitcher(GameModeSwitcher* gameModeSwitcher){mGameModeSwitcher = gameModeSwitcher;}
    GameModeSwitcher * getGameModeSwitcher() const{return mGameModeSwitcher;}

    GameState& getGameState(){return mGameState;}
    const GameState& getGameState() const{return mGameState;}

    CustomTrayManager* getTrayManager(){return mTrayMgr;}

    friend class GameModeSwitcher;
    friend class BaseMenuMode;
    friend class MenuMode;
    friend class MenuMultiMode;
    friend class UIMainMenu;
    friend class UIMainMenuMulti;
    friend class BaseRaceMode;
    friend class SinglePlayerMode;
    friend class MultiPlayerMode;
    friend class UIBackground;

private:

    ModeContext& operator=(const ModeContext&);

    Ogre::Root* mRoot;
    Ogre::RenderWindow* mWindow;

    InputHandler* mInputHandler;

    CustomTrayManager* mTrayMgr;
    CustomOverlaySystem* mOverlaySystem;

    lua_State * mPipeline;

    GameModeSwitcher* mGameModeSwitcher;

    GameState& mGameState;

#ifndef NO_OPENAL
    SoundsProcesser& mSoundsProcesser;
#endif

    MyGUI::Gui* mGUI;
    MyGUI::OgrePlatform* mPlatform;
};

#endif