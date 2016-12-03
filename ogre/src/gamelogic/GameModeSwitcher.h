#ifndef GAMEMODESWITCHER_H
#define GAMEMODESWITCHER_H

#include "../includes/OgreInclude.h"
#include "../includes/CommonIncludes.h"

#include "../GameState.h"

#include "../gamemodes/ModeContext.h"

class BaseMenuMode;
class BaseRaceMode;

class InputHandler;
class CustomTrayManager;
class CustomOverlaySystem;
struct lua_State;
class SoundsProcesser;
class Graphics2D;

class GameModeSwitcher
{
public:

    GameModeSwitcher(const ModeContext& modeContext);
    ~GameModeSwitcher();

    void frameStarted(const Ogre::FrameEvent &evt);
    void frameRenderingQueued(const Ogre::FrameEvent &evt);
    void frameEnded();

    void switchMode(){mIsSwitchMode = true;}

    void restartRace();
    void reloadRace();

#if defined(__ANDROID__)
    //for UI only
    void reloadTextures();
#endif

    void processCollision(btManifoldPoint& cp, const btCollisionObjectWrapper* colObj0Wrap, const btCollisionObjectWrapper* colObj1Wrap, int triIndex);

private:

    ModeContext mContext;

    GameMode mGameMode;
    bool mIsSwitchMode;


    CommonIncludes::shared_ptr<BaseMenuMode> mMenuMode;
    CommonIncludes::shared_ptr<BaseRaceMode> mPlayerMode;

    void clear();

};

#endif
