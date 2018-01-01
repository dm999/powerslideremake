#ifndef GAMEMODESWITCHER_H
#define GAMEMODESWITCHER_H

#include "../includes/OgreInclude.h"
#include "../includes/CommonIncludes.h"
#include "../includes/MyGUI_KeyCode.h"

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

    void createBombByPlayer();

    void tabPressed();

    void keyUp(MyGUI::KeyCode _key, wchar_t _char );

    void mousePressed(const Ogre::Vector2& pos);
    void mouseReleased(const Ogre::Vector2& pos);
    void mouseMoved(const Ogre::Vector2& pos);

#if defined(__ANDROID__)
    //for UI only
    void reloadTextures();
#endif

    void processCollision(int triIndex);

    GameMode getMode()const{return mGameMode;}

    bool isExitSubmenu()const;
    void setExitSubmenu();
    void setTopmostSubmenu();

    //LoaderListener
    void loadState(float percent, const std::string& info) override;

    /**
     * indicates resources loaded
     */
    bool isLoadPassed()const {return mIsLoadPassed;}
private:

    ModeContext mContext;

    GameMode mGameMode;//current
    GameMode mGameModeNext;//next
    bool mIsSwitchMode;

    bool mIsInitialLoadPassed;
    bool mIsLoadPassed;

    UIBackground mUIBackground;
    CommonIncludes::shared_ptr<UIBackgroundLoaderProgressTracks> mUILoader;
    CommonIncludes::shared_ptr<UIBackgroundLoaderProgress> mUIUnloader;


    CommonIncludes::shared_ptr<MenuMode> mMenuMode;
#ifndef NO_MULTIPLAYER
    CommonIncludes::shared_ptr<MenuMultiMode> mMenuMultiMode;
#endif
    CommonIncludes::shared_ptr<BaseRaceMode> mPlayerMode;

    void clear();

};

#endif
