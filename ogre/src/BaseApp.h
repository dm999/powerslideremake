#ifndef BASEAPP_H
#define BASEAPP_H

#include <memory>

#include "includes/OgreInclude.h"
#include "includes/OISInclude.h"
#include "SdkTrays.h"

#include "lua/DMLuaManager.h"

#include "includes/CommonIncludes.h"

#include "tools/Tools.h"

#include "GameState.h"
#include "SoundsProcesser.h"

#include "gamemodes/ModeContext.h"

#if defined(__ANDROID__)
    #include <jni.h>

    class AAssetManager;
#endif

class CustomSceneManagerFactory;
class CustomTrayManager;
class CustomOverlaySystem;
class InputHandler;
class GameModeSwitcher;

//https://github.com/synasius/ogre-basic-tutorials
class BaseApp : 
    public Ogre::FrameListener, 
    public Ogre::WindowEventListener
{
public:
    BaseApp();
    virtual ~BaseApp();

    void go();

    void restartRace();
    void enablePause();
    void tabPressed();
    void switchRenderType();
    void createBurnByPlayer();
    void createBombByPlayer();
    void dropCamera();

    // scripting functions
    void parseFile(const std::string& fileName);

    //controls
    bool setShutdown(bool isOnEsc);
    void keyDown(const OIS::KeyEvent &arg );
    void keyUp(const OIS::KeyEvent &arg );

#if !defined(__ANDROID__)
    void mouseMoved(const OIS::MouseEvent &arg);
    void mousePressed(const OIS::MouseEvent &arg, OIS::MouseButtonID id);
    void mouseReleased(const OIS::MouseEvent &arg, OIS::MouseButtonID id);
#else
    void touchMoved(const OIS::MultiTouchEvent& arg);
    void touchPressed(const OIS::MultiTouchEvent& arg);
    void touchReleased(const OIS::MultiTouchEvent& arg);

    void androidShowKeyboard();
    void androidHideKeyboard();
#endif

    GameState& getGameState(){return mGameState;}

#if defined(__ANDROID__)
    Ogre::DataStreamPtr openAPKFile(const Ogre::String& fileName);

    void touchDown(int id, float x, float y);
    void touchUp(int id, float x, float y);
    void touchMove(int id, float x, float y);

    bool androidOnBack();

    void androidCreate(JNIEnv * env, jobject obj, jobject assetManager, const std::string& dataDir);
    void androidDestroy();
    void androidInitWindow(JNIEnv * env, jobject obj,  jobject surface);
    void androidTerminateWindow();
    bool androidRenderOneFrame(JNIEnv * env);

    void androidPause(JNIEnv * env);

    AAssetManager* mAssetMgr;
    JavaVM* gVM;
    jclass activityClass;
    jobject activityObj;

    int mTouchLeftID;
    int mTouchRightID;
    int mTouchDownID;
#endif

protected:

    bool setup();
    bool configure();
    void createFrameListener();
    void setupResources();

    //frame listener
    virtual bool frameStarted(const Ogre::FrameEvent &evt)override;
    virtual bool frameEnded(const Ogre::FrameEvent &evt)override;
    virtual bool frameRenderingQueued(const Ogre::FrameEvent& evt)override;

    //window event listener
    virtual void windowResized(Ogre::RenderWindow* rw)override;
    virtual void windowClosed(Ogre::RenderWindow* rw)override;
    virtual void windowFocusChange(Ogre::RenderWindow* rw)override;


    CommonIncludes::shared_ptr<InputHandler> mInputHandler;

    CommonIncludes::shared_ptr<Ogre::Root> mRoot;
    CommonIncludes::shared_ptr<CustomOverlaySystem> mOverlaySystem;
    
    Ogre::RenderWindow* mWindow;
    Ogre::String mResourcesCfg;

    // OgreBites
    CommonIncludes::shared_ptr<CustomTrayManager> mTrayMgr;
    bool mShutDown;

private:

    DMLuaManager mLuaManager;
    int mLuaError;

    CommonIncludes::shared_ptr<CustomSceneManagerFactory> mSMFactory;

#ifndef NO_OPENAL
    SoundsProcesser mSoundsProcesser;
#endif
    GameState mGameState;

    CommonIncludes::shared_ptr<GameModeSwitcher> mGameModeSwitcher;


    void initLua();
    void doLuaMainFile();
    void registerLuaFunctions();
    void deInitLua();

    ModeContext createModeContext();
};

#endif
