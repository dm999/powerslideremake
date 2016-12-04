#ifndef GAMEMODESWITCHER_H
#define GAMEMODESWITCHER_H

#include "SdkTrays.h"

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

class GameModeSwitcher : public OgreBites::SdkTrayListener
{
public:

    GameModeSwitcher(const ModeContext& modeContext);
    ~GameModeSwitcher();

    //SdkTrayListener
    virtual void buttonHit(OgreBites::Button* button)override;

    void frameStarted(const Ogre::FrameEvent &evt);
    void frameRenderingQueued(const Ogre::FrameEvent &evt);
    void frameEnded();

    void switchMode(GameMode nextMode);

    void restartRace();
    void reloadRace();

#if defined(__ANDROID__)
    //for UI only
    void reloadTextures();
#endif

    void processCollision(btManifoldPoint& cp, const btCollisionObjectWrapper* colObj0Wrap, const btCollisionObjectWrapper* colObj1Wrap, int triIndex);

    GameMode getMode()const{return mGameMode;}

private:

    ModeContext mContext;

    GameMode mGameMode;//current
    GameMode mGameModeNext;//next
    bool mIsSwitchMode;


    CommonIncludes::shared_ptr<BaseMenuMode> mMenuMode;
    CommonIncludes::shared_ptr<BaseRaceMode> mPlayerMode;

    void clear();

};

#endif
