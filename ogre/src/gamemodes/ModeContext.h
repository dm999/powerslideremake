#ifndef MODECONTEXT_H
#define MODECONTEXT_H

#include "../GameState.h"
#include "../SoundsProcesser.h"
#include "../Graphics2D.h"

namespace Ogre
{
    class Root;
    class RenderWindow;
}

struct lua_State;

class InputHandler;
class CustomTrayManager;
class CustomOverlaySystem;

class ModeContext
{
public:

    ModeContext(
        Ogre::Root* root, Ogre::RenderWindow* window,
        InputHandler* inputHandler,
        CustomTrayManager* trayMgr, CustomOverlaySystem* overlaySystem,
        lua_State * pipeline,
        GameState& gameState, SoundsProcesser& soundsProcesser, Graphics2D& graphics2D
    );

    ModeContext(const ModeContext& other);

    ~ModeContext(){}

    friend class GameModeSwitcher;
    friend class BaseMenuMode;
    friend class BaseRaceMode;
    friend class SinglePlayerMode;
    friend class MultiPlayerMode;

private:

    ModeContext& operator=(const ModeContext&);

    Ogre::Root* mRoot;
    Ogre::RenderWindow* mWindow;

    InputHandler* mInputHandler;

    CustomTrayManager* mTrayMgr;
    CustomOverlaySystem* mOverlaySystem;

    lua_State * mPipeline;

    GameState& mGameState;
    SoundsProcesser& mSoundsProcesser;
    Graphics2D& mGraphics2D;
};

#endif