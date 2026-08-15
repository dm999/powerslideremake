#ifndef BASERACEMODE_H
#define BASERACEMODE_H

#include "BaseMode.h"

#include "../lua/DMLuaManager.h"

#include "../includes/CommonIncludes.h"

#include "../mesh/StaticMeshProcesser.h"
#include "../mesh/ModelsPool.h"

#include "../gamelogic/LapController.h"
#include "../gamelogic/TrialGhost.h"

class CameraMan;
class UIRace;

class Physics;
class Cheats;

namespace OgreBites
{
    class ParamsPanel;
}

#define SHOW_DETAILS_PANEL 0

class LoaderListener;

class BaseRaceMode : 
    public BaseMode,
    public LapUtils::Events,
    public Ogre::RenderTargetListener,  // for rear camera, arrow
    public PhysicsListener,
    public Ogre::ResourceGroupListener  // for loader
{
public:

    BaseRaceMode(const ModeContext& modeContext);
    virtual ~BaseRaceMode();

    void initData(LoaderListener* loaderListener)override;
    void initCamera()override;
    void clearData()override;

    void restart();

    virtual void createBurnByPlayer();
    virtual void createBombByPlayer();
    virtual void nitroByPlayer(){}
    virtual void cheatByPlayer(bool isEnabled, InputKeyMapping cheat);

    void mousePressed(const Ogre::Vector2& pos);
    void mouseReleased(const Ogre::Vector2& pos);
    void mouseMoved(const Ogre::Vector2& pos);

    void frameStarted(const Ogre::FrameEvent &evt)override;
    void frameRenderingQueued(const Ogre::FrameEvent& evt)override;

#if defined(__ANDROID__)
    //for UI only
    void reloadTextures()override;
#endif

    void processSounds();

    //LapUtils
    void onLapFinished()override;

    //rear camera, arrow listener
    void preRenderTargetUpdate(const Ogre::RenderTargetEvent& evt)override;
    void postRenderTargetUpdate(const Ogre::RenderTargetEvent& evt)override;
    //arrow listener
    void preViewportUpdate(const Ogre::RenderTargetViewportEvent& evt)override;

    //ResourceGroupListener
    void resourceGroupScriptingStarted(const Ogre::String& groupName, size_t scriptCount) override;
    void scriptParseStarted(const Ogre::String& scriptName, bool& skipThisScript) override {}
    void scriptParseEnded(const Ogre::String& scriptName, bool skipped) override;
    void resourceGroupScriptingEnded(const Ogre::String& groupName) override {}
    void resourceGroupLoadStarted(const Ogre::String& groupName, size_t resourceCount) override;
    void resourceGroupLoadEnded(const Ogre::String& groupName) override {}
    void resourceLoadStarted(const Ogre::ResourcePtr& resource) override {}
    void resourceLoadEnded(void) override;
    void worldGeometryStageStarted(const Ogre::String& description) override {}
    void worldGeometryStageEnded(void) override {}

    //PhysicsListener
    void timeStepBefore(Physics * physics)override;
    void timeStepAfter(Physics * physics)override;

    LapController getLapController() const {return mLapController;}

    //deathmatch post-race statistics (empty in every mode except DeathmatchMode,
    //which populates it as AI are eliminated). Copied into the switcher's
    //ModeContext on teardown so the post-race menu can render it — same pattern
    //as getLapController() above.
    deathmatchResultVec getDeathmatchResults() const {return mDeathmatchResults;}

    //massacre score/statistics (computed at session end in DeathmatchMode).
    //Copied into ModeContext on teardown alongside the deathmatch results.
    size_t getDeathmatchEliminatedCount() const {return mDeathmatchEliminatedCount;}
    size_t getDeathmatchInjuredCount() const {return mDeathmatchInjuredCount;}
    size_t getDeathmatchScore() const {return mDeathmatchScore;}

protected:

    StaticMeshProcesser mStaticMeshProcesser;
    ModelsPool mModelsPool;

    LapController mLapController;

    deathmatchResultVec mDeathmatchResults;

    //score/statistics for massacre mode: computed at session end.
    size_t mDeathmatchEliminatedCount;
    size_t mDeathmatchInjuredCount;
    size_t mDeathmatchScore;

    //massacre countdown base: the player's race clock (totalTime + lapTime) value
    //captured at the instant the player's car is released from the grid. The race
    //clock starts at GO, but the player rides in the final batch and can sit
    //parked for several seconds; subtracting this base keeps the countdown at the
    //full time limit until the player actually starts. -1.0f == not captured yet.
    Ogre::Real mMassacrePlayerStartClock;

    //accurate race clock for the player, reading the raw lap timer directly
    //instead of the physics-cached mLapTime (which can be stale by one frame
    //at GO, causing a wrong countdown offset). See BaseRaceMode.cpp.
    Ogre::Real getPlayerRaceClock() const;

    CommonIncludes::shared_ptr<CameraMan> mCameraMan;       // basic camera controller

    CommonIncludes::shared_ptr<Physics> mWorld;
    CommonIncludes::shared_ptr<Cheats> mCheats;

    CommonIncludes::shared_ptr<UIRace> mUIRace;

    DMLuaManager mLuaManager;

#if SHOW_DETAILS_PANEL
    OgreBites::ParamsPanel* mDetailsPanel;     // sample details panel
#endif

    virtual void clearScene();
    virtual void initMisc();

    //used in multiplayer mode
    virtual void customInitScene(){}
    virtual void customClearScene(){}
    virtual void customInitUI(){}
    virtual void customClearUI(){}
    virtual void customProcessCollision(int triIndex){}
    virtual void customUnloadResources(){}
    virtual void customFrameStartedDoProcessFrameBeforePhysics(){}
    virtual void customFrameStartedDoProcessFrameAfterPhysics(){}
    virtual void customFrameRenderingQueuedDoBegining(){}
    virtual void customFrameRenderingQueuedDoRaceStarted(){}
    virtual void customFrameRenderingQueuedDo2DUI(){}

private:

    void beforeStartSequence();

    float mShadowLightDistanceFromCar;

    bool mIsGlobalReset;

    Ogre::Camera* mRearCamera;

    //d.polubotko: player car reflection cubemap (desktop only)
    //NOTE: no listener is ever attached to the face targets - a cubemap exposes 6
    //separate RenderTargets, so a listener would fire 6 times per update and restore
    //car visibility between faces. Faces are driven manually instead, see updateReflectionCube()
    static const size_t mReflectionCubeFaces = 6;
    static const int mReflectionCubeSize = 128;
    Ogre::Camera* mReflectionCubeCamera;
    Ogre::RenderTarget* mReflectionCubeRT[mReflectionCubeFaces];
    size_t mReflectionCubeFaceIndex;

    void createReflectionCube();
    void initReflectionCubeCamera();
    void updateReflectionCube();
    static Ogre::Vector3 getReflectionCubeFaceDirection(size_t face);
    static Ogre::Vector3 getReflectionCubeFaceUp(size_t face);

    Ogre::SceneManager* mSceneMgrCarUI;

    void initScene(LoaderListener* loaderListener);
    void initTerrain(LoaderListener* loaderListener);
    void initModel(LoaderListener* loaderListener);

    void initLightLists();


    void initWorld();

    void deInitWorld();

    void loadResources();
    void unloadResources();

    LoaderListener* mLoaderListener;

    Ogre::Viewport * mViewPortScene;
    Ogre::Viewport * mViewPortCarUI;

    LinearController<float> mFOVNitro;

    PSBaseGraphicsVehicle mGhost;//time trial
    PSBaseGraphicsVehicle mGhostUser;//time trial
    TrialGhost mTrialGhost;
    bool mGhostVisible;
    bool mGhostUserVisible;
};


#endif