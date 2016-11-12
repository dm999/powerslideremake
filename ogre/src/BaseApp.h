#ifndef BASEAPP_H
#define BASEAPP_H

#include <memory>

#include "OgreInclude.h"
#include "OISInclude.h"
#include "OgreBulletInclude.h"
#include "SdkTrays.h"

#include "lua/DMLuaManager.h"

#include "CommonIncludes.h"

#include "CustomSceneManager.h"

#include "CustomTrayManager.h"

#include "StaticMeshProcesser.h"

#include "Tools.h"

#include "LapController.h"

#include "ModelsPool.h"

#include "GameState.h"
#include "OriginalSettings.h"
#include "loaders/PFLoader.h"

#include "Graphics2D.h"

#include "SoundsProcesser.h"

#include "multiplayer/MultiplayerController.h"

#if defined(__ANDROID__)
    #include <jni.h>

    class AAssetManager;
#endif

class CustomOverlaySystem;
class InputHandler;
class CameraMan;

//https://github.com/synasius/ogre-basic-tutorials
class BaseApp : public Ogre::FrameListener, public Ogre::WindowEventListener, OgreBites::SdkTrayListener, public MultiplayerController::Events, public LapUtils::Events
{
public:
    BaseApp(void);
    virtual ~BaseApp(void);

    virtual void go(void);

    void scriptsReload();       // full reload of scene
    void quickScriptsReload();  // reread main params only

    // scripting functions
    void initScene();
    void clearScene();
    void preloadResource(const Ogre::String& fileName, const Ogre::String& fileType);
    void initTerrain();
    void initModel();
    void initMisc();
    void parseFile(const std::string& fileName);

    void setShutdown(bool shutdown){mShutDown = shutdown;}

    //car control
    void keyDown(OIS::KeyCode key);
    void keyUp(OIS::KeyCode key);

    void processCollision(btManifoldPoint& cp, const btCollisionObjectWrapper* colObj0Wrap, const btCollisionObjectWrapper* colObj1Wrap, int triIndex);

    GameState& getGameState(){return mGameState;}

    //multiplayer
    void onPlayerEjected(const std::string& player)override;
    void onPlayerJoined(const std::string& player)override;
    void onPlayerLeft(const std::string& player)override;
    void onNewHost(const std::string& player)override;
    void onRoomClosed(const std::string& player)override;
    void onPlayerReady(const std::string& player)override;
    void onPlayerAddedToSession(const std::string& player)override;
    void onPlayerQuitSession(const std::string& player, bool isHost)override;
    void onSessionStart(uint32_t aiAmount, const std::vector<std::string>& players, size_t playerIndex, bool isHost, const std::vector<GameCars>& aiSkins, const std::map<std::string, GameCars>& playersSkins)override;
    void onSessionUpdate(const MultiplayerController::playerToData& otherPlayersSessionData, const std::vector<MultiplayerSessionData>& aiPlayersSessionData, bool isHost)override;
    void onError(const std::string& message)override;

    //LapUtils
    void onLapFinished()override;

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

    virtual bool setup();
    virtual bool configure(void);
    virtual void createFrameListener(void);
    virtual void setupResources(void);
    virtual void loadResources(void);
    virtual void unloadResources(void);

    virtual bool frameStarted(const Ogre::FrameEvent &evt);
    virtual bool frameEnded(const Ogre::FrameEvent &evt);

    virtual bool frameRenderingQueued(const Ogre::FrameEvent& evt);

    virtual void windowResized(Ogre::RenderWindow* rw);
    virtual void windowClosed(Ogre::RenderWindow* rw);


    CommonIncludes::shared_ptr<InputHandler> mInputHandler;

    CommonIncludes::shared_ptr<Ogre::Root> mRoot;
    CommonIncludes::shared_ptr<CustomOverlaySystem> mOverlaySystem;
    Ogre::Camera* mCamera;
    Ogre::Camera* mRearCamera;
    Ogre::SceneManager* mSceneMgr;
    Ogre::RenderWindow* mWindow;
    Ogre::String mResourcesCfg;

    // OgreBites
    CustomTrayManager* mTrayMgr;
    CommonIncludes::shared_ptr<CameraMan> mCameraMan;       // basic camera controller
    OgreBites::ParamsPanel* mDetailsPanel;     // sample details panel
    bool mCursorWasVisible;                    // was cursor visible before dialog appeared
    bool mShutDown;

    //sounds
    SoundsProcesser mSoundsProcesser;

private:

    Ogre::SceneNode* mMainNode;
    bool mIsGlobalReset;
    bool mIsDisableMouse;

    DMLuaManager mLuaManager;
    int mLuaError;

    CommonIncludes::shared_ptr<CustomSceneManagerFactory> mSMFactory;


    StaticMeshProcesser mStaticMeshProcesser;

    ModelsPool mModelsPool;
    
    LapController mLapController;

    GameState mGameState;

    Graphics2D mGraphics2D;

    CommonIncludes::shared_ptr<OgreBulletDynamics::DynamicsWorld> mWorld;
    CommonIncludes::shared_ptr<OgreBulletCollisions::DebugDrawer> mDebugDrawer;

    CommonIncludes::shared_ptr<MultiplayerController> mMultiplayerController;


    void initWorld(const Ogre::Vector3 &gravityVector = Ogre::Vector3(0.0f, -59.81f, 0.0f),
                   const Ogre::AxisAlignedBox &bounds = Ogre::AxisAlignedBox(   Ogre::Vector3 (-10000.0f, -10000.0f, -10000.0f),
                                                                                Ogre::Vector3 (10000.0f,  10000.0f,  10000.0f)));

    void deInitWorld();

    void initLua();
    void doLuaMainFile();
    void registerLuaFunctions();
    void deInitLua();

    void initLightLists();
};

#endif
