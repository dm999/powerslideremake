#ifndef BASERACEMODE_H
#define BASERACEMODE_H

#include "BaseMode.h"

#include "../lua/DMLuaManager.h"

#include "../includes/CommonIncludes.h"

#include "../mesh/StaticMeshProcesser.h"
#include "../mesh/ModelsPool.h"

#include "../gamelogic/LapController.h"

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
    public Ogre::ResourceGroupListener  // for loader
{
public:

    BaseRaceMode(const ModeContext& modeContext);
    virtual ~BaseRaceMode();

    void initData(LoaderListener* loaderListener)override;
    void initCamera()override;
    void clearData()override;

    void restart();

    void createBurnByPlayer();
    void createBombByPlayer();

    void frameStarted(const Ogre::FrameEvent &evt)override;
    void frameRenderingQueued(const Ogre::FrameEvent& evt)override;

#if defined(__ANDROID__)
    //for UI only
    void reloadTextures()override;
#endif

    void processCollision(int triIndex);

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

    LapController getLapController() const {return mLapController;}

protected:

    StaticMeshProcesser mStaticMeshProcesser;
    ModelsPool mModelsPool;

    LapController mLapController;

    CommonIncludes::shared_ptr<CameraMan> mCameraMan;       // basic camera controller

    CommonIncludes::shared_ptr<Physics> mWorld;
    CommonIncludes::shared_ptr<Cheats> mCheats;

    CommonIncludes::shared_ptr<UIRace> mUIRace;

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
    virtual void customFrameStartedDoProcessFrameBeforePhysics(const Ogre::FrameEvent &evt){}
    virtual void customFrameStartedDoProcessFrameAfterPhysics(const Ogre::FrameEvent &evt){}
    virtual void customFrameRenderingQueuedDoBegining(){}
    virtual void customFrameRenderingQueuedDoRaceStarted(){}
    virtual void customFrameRenderingQueuedDo2DUI(){}

private:

    float mShadowLightDistanceFromCar;

    bool mIsGlobalReset;

    DMLuaManager mLuaManager;

    Ogre::Camera* mRearCamera;

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
};


#endif