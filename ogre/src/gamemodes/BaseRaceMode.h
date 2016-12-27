#ifndef BASERACEMODE_H
#define BASERACEMODE_H

#include "BaseMode.h"

#include "../lua/DMLuaManager.h"

#include "../includes/OgreBulletInclude.h"
#include "../includes/CommonIncludes.h"

#include "../mesh/StaticMeshProcesser.h"
#include "../mesh/ModelsPool.h"

#include "../gamelogic/LapController.h"

class CameraMan;
class UIRace;

namespace OgreBites
{
    class ParamsPanel;
}

#define SHOW_DETAILS_PANEL 0

class BaseRaceMode : 
    public BaseMode,
    public LapUtils::Events,
    public Ogre::RenderTargetListener // for rear camera
{
public:

    BaseRaceMode(const ModeContext& modeContext);
    virtual ~BaseRaceMode(){}

    virtual void initData()override;
    virtual void clearData()override;

    void restart();

    void frameStarted(const Ogre::FrameEvent &evt)override;
    void frameRenderingQueued(const Ogre::FrameEvent& evt)override;

#if defined(__ANDROID__)
    //for UI only
    void reloadTextures()override;
#endif

    void processCollision(btManifoldPoint& cp, const btCollisionObjectWrapper* colObj0Wrap, const btCollisionObjectWrapper* colObj1Wrap, int triIndex);

    //LapUtils
    void onLapFinished()override;

    //rear camera listener
    void preRenderTargetUpdate(const Ogre::RenderTargetEvent& evt)override;
    void postRenderTargetUpdate(const Ogre::RenderTargetEvent& evt)override;

protected:

    StaticMeshProcesser mStaticMeshProcesser;
    ModelsPool mModelsPool;

    LapController mLapController;

    CommonIncludes::shared_ptr<CameraMan> mCameraMan;       // basic camera controller

    CommonIncludes::shared_ptr<OgreBulletDynamics::DynamicsWorld> mWorld;

    CommonIncludes::shared_ptr<UIRace> mUIRace;

#if SHOW_DETAILS_PANEL
    OgreBites::ParamsPanel* mDetailsPanel;     // sample details panel
#endif

    virtual void clearScene();
    virtual void initMisc();

    //used in multiplayer mode
    virtual void customInitScene(){}
    virtual void customClearScene(){}
    virtual void customProcessCollision(btManifoldPoint& cp, const btCollisionObjectWrapper* colObj0Wrap, const btCollisionObjectWrapper* colObj1Wrap, int triIndex){}
    virtual void customUnloadResources(){}
    virtual void customFrameStartedDoProcessFrameBeforePhysics(const Ogre::FrameEvent &evt){}
    virtual void customFrameStartedDoProcessFrameAfterPhysics(const Ogre::FrameEvent &evt){}
    virtual void customFrameRenderingQueuedDoBegining(){}
    virtual void customFrameRenderingQueuedDoRaceStarted(){}
    virtual void customFrameRenderingQueuedDo2DUI(){}

private:

    bool mIsGlobalReset;

    DMLuaManager mLuaManager;

    Ogre::Camera* mRearCamera;

    Ogre::SceneManager* mSceneMgrCarUI;

    CommonIncludes::shared_ptr<OgreBulletCollisions::DebugDrawer> mDebugDrawer;

    void initScene();
    void initTerrain();
    void initModel();

    void initLightLists();


    void initWorld(const Ogre::Vector3 &gravityVector = Ogre::Vector3(0.0f, -59.81f, 0.0f),
                   const Ogre::AxisAlignedBox &bounds = Ogre::AxisAlignedBox(   Ogre::Vector3 (-10000.0f, -10000.0f, -10000.0f),
                                                                                Ogre::Vector3 (10000.0f,  10000.0f,  10000.0f)));

    void deInitWorld();

    void loadResources();
    void unloadResources();
};


#endif