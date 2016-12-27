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

namespace MyGUI
{
    class Gui;
    class OgrePlatform;
}

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

    void quickScriptsReload();  // reread main params only

    // scripting functions
    void parseFile(const std::string& fileName);

    void processCollision(btManifoldPoint& cp, const btCollisionObjectWrapper* colObj0Wrap, const btCollisionObjectWrapper* colObj1Wrap, int triIndex);

    //controls
    void setShutdown(bool shutdown);
    void keyDown(const OIS::KeyEvent &arg );
    void keyUp(const OIS::KeyEvent &arg );

#if !defined(__ANDROID__)
    void mouseMoved(const OIS::MouseEvent &arg);
    void mousePressed(const OIS::MouseEvent &arg, OIS::MouseButtonID id);
    void mouseReleased(const OIS::MouseEvent &arg, OIS::MouseButtonID id);
#endif

    GameState& getGameState(){return mGameState;}

#if defined(__ANDROID__)
    Ogre::DataStreamPtr openAPKFile(const Ogre::String& fileName);

    void touchDown(int id, float x, float y);
    void touchUp(int id, float x, float y);

    void androidCreate(JNIEnv * env, jobject obj, jobject assetManager);
    void androidDestroy();
    void androidInitWindow(JNIEnv * env, jobject obj,  jobject surface);
    void androidTerminateWindow();
    void androidRenderOneFrame(JNIEnv * env);

    void androidPause(JNIEnv * env);

    AAssetManager* mAssetMgr;
    JavaVM* gVM;

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

    CommonIncludes::shared_ptr<MyGUI::Gui> mGUI;
    CommonIncludes::shared_ptr<MyGUI::OgrePlatform> mPlatform;

private:

    DMLuaManager mLuaManager;
    int mLuaError;

    CommonIncludes::shared_ptr<CustomSceneManagerFactory> mSMFactory;

    SoundsProcesser mSoundsProcesser;
    GameState mGameState;

    CommonIncludes::shared_ptr<GameModeSwitcher> mGameModeSwitcher;


    void initLua();
    void doLuaMainFile();
    void registerLuaFunctions();
    void deInitLua();

    ModeContext createModeContext();
};

#endif
