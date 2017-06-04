#ifndef GAMEMODESWITCHER_H
#define GAMEMODESWITCHER_H

#include "../includes/OgreInclude.h"
#include "../includes/CommonIncludes.h"

#include "../listeners/LoaderListener.h"

#include "../GameState.h"

#include "../gamemodes/ModeContext.h"
#include "../ui/UIBackground.h"

class MenuMode;
class MenuMultiMode;
class BaseRaceMode;

class InputHandler;
class CustomTrayManager;
class CustomOverlaySystem;
struct lua_State;
class SoundsProcesser;
class Graphics2D;

class GameModeSwitcher : public LoaderListener
{
public:

    GameModeSwitcher(const ModeContext& modeContext);
    ~GameModeSwitcher();

    void frameStarted(const Ogre::FrameEvent &evt);
    void frameRenderingQueued(const Ogre::FrameEvent &evt);
    void frameEnded();

    void switchMode(GameMode nextMode);

    void restartRace();

    void tabPressed();

#if defined(__ANDROID__)
    //for UI only
    void reloadTextures();
#endif

    void processCollision(btManifoldPoint& cp, const btCollisionObjectWrapper* colObj0Wrap, const btCollisionObjectWrapper* colObj1Wrap, int triIndex);

    GameMode getMode()const{return mGameMode;}

    //LoaderListener
    void loadState(float percent) override;

private:

    ModeContext mContext;

    GameMode mGameMode;//current
    GameMode mGameModeNext;//next
    bool mIsSwitchMode;

    bool mIsInitialLoadPassed;

    UIBackground mUIBackground;
    UIBackgroundLoader mUILoader;
    UIBackgroundLoader mUIUnloader;


    CommonIncludes::shared_ptr<MenuMode> mMenuMode;
#ifndef NO_MULTIPLAYER
    CommonIncludes::shared_ptr<MenuMultiMode> mMenuMultiMode;
#endif
    CommonIncludes::shared_ptr<BaseRaceMode> mPlayerMode;

    void clear();

};

#endif
