#include "../pcheader.h"

#include "BaseRaceMode.h"

#include "../CameraMan.h"
#include "../InputHandler.h"

#include "../tools/Randomizer.h"

#include "../customs/CustomTrayManager.h"
#include "../customs/CustomOverlaySystem.h"

#include "../loaders/ExclusionLoader.h"
#include "../loaders/LLTLoader.h"
#include "../loaders/PHYLoader.h"
#include "../loaders/AILoader.h"
#include "../loaders/TerrainLoader.h"
#include "../loaders/ParticlesLoader.h"
#include "../loaders/TEXLoader.h"
#include "../loaders/TextureLoader.h"

#include "../gamelogic/RacingGridGeneration.h"

#if defined(__ANDROID__)
    #include <android/log.h>

    #define LOGI(...) ((void)__android_log_write(ANDROID_LOG_INFO, "OGRE", __VA_ARGS__))
    #define LOGE(...) ((void)__android_log_write(ANDROID_LOG_ERROR, "OGRE", __VA_ARGS__)) 
#endif

BaseRaceMode::BaseRaceMode(const ModeContext& modeContext) :
    BaseMode(modeContext),
    mIsGlobalReset(true),
    mRearCamera(0)
#if SHOW_DETAILS_PANEL
    ,mDetailsPanel(0)
#endif
{}

void BaseRaceMode::initData()
{
    mLuaManager.CallFunction_0_0("main", mModeContext.mPipeline);
    initScene();

    loadResources();

    initTerrain();
    initModel();
    initMisc();

    initLightLists();

#if SHOW_DETAILS_PANEL
    // create a params panel for displaying sample details
    Ogre::StringVector items;
    items.push_back("cam.pX");
    items.push_back("cam.pY");
    items.push_back("cam.pZ");
    items.push_back("");
    items.push_back("carSpeedLinear");
    items.push_back("carSpeedProj");
    items.push_back("carSpeedLateral");
    items.push_back("");
    items.push_back("FrontL");
    items.push_back("FrontR");
    items.push_back("BackL");
    items.push_back("BackR");
    items.push_back("");
    items.push_back("FrontL");
    items.push_back("FrontR");
    items.push_back("BackL");
    items.push_back("BackR");
    items.push_back("");
    items.push_back("FrontL");
    items.push_back("FrontR");
    items.push_back("BackL");
    items.push_back("BackR");
    items.push_back("");
    items.push_back("LapPosition");
    items.push_back("LapTime");
    items.push_back("Time");
    items.push_back("Lap");
    items.push_back("Position");
    items.push_back("Gear");
    items.push_back("RPM");
    //items.push_back("Filtering");
    //items.push_back("Poly Mode");

    mDetailsPanel = mModeContext.mTrayMgr->createParamsPanel(OgreBites::TL_TOPLEFT, "DetailsPanel", 200, items);
    //mDetailsPanel->hide();
#endif
}

void BaseRaceMode::clearData()
{
    clearScene();

    unloadResources();

    mModeContext.mInputHandler->resetCameraMenPointer(NULL);
    mCameraMan.reset();
}

void BaseRaceMode::restart()
{
    clearScene();
    mLuaManager.CallFunction_0_0("main", mModeContext.mPipeline);
    initScene();
    initTerrain();
    initModel();
    initMisc();

    initLightLists();
}

void BaseRaceMode::reload()
{
    mIsGlobalReset = true;
    clearScene();
    unloadResources();
    mLuaManager.CallFunction_0_0("main", mModeContext.mPipeline);
    initScene();
    loadResources();

    initTerrain();
    initModel();
    initMisc();


    initLightLists();

}


void BaseRaceMode::initScene()
{
    mSceneMgr = mModeContext.mRoot->createSceneManager(Ogre::ST_GENERIC);

    mModeContext.mGameState.setPlayerCharacterName(mLuaManager.ReadScalarString("Main.Character", mModeContext.mPipeline));

    mModeContext.mGameState.setRaceParameters(mLuaManager.ReadScalarString("Main.Track", mModeContext.mPipeline),
                                static_cast<AIStrength>(mLuaManager.ReadScalarInt("Scene.AIStrength", mModeContext.mPipeline)));

    mModeContext.mSoundsProcesser.initSounds(mModeContext.mGameState.getPFLoaderData());

    //migration from 1.8.1 to 1.9.0
    //http://www.ogre3d.org/forums/viewtopic.php?f=2&t=78278
    mSceneMgr->addRenderQueueListener(mModeContext.mOverlaySystem);

    mSceneMgr->setAmbientLight(Ogre::ColourValue(1.0, 1.0, 1.0));

    mMainNode = mSceneMgr->getRootSceneNode()->createChildSceneNode();

    mModeContext.mGameState.setListenerGain(mLuaManager.ReadScalarFloat("Scene.SoundMasterGain", mModeContext.mPipeline));

    mCamera = mSceneMgr->createCamera("PlayerCam");
    mCamera->setNearClipDistance(0.5f);
    Ogre::Viewport * mViewPort = mModeContext.mWindow->addViewport(mCamera);

    mViewPort->setBackgroundColour(mModeContext.mGameState.getBackgroundColor());
    mCamera->setAspectRatio(Ogre::Real(mViewPort->getActualWidth()) / Ogre::Real(mViewPort->getActualHeight()) / (640.0f / 480.0f));
    mCamera->setFOVy(Ogre::Degree(95.0f));

    initWorld(Ogre::Vector3(0.0f, -mLuaManager.ReadScalarFloat("Scene.Gravity", mModeContext.mPipeline), 0.0f));

    mCameraMan.reset(new CameraMan(mCamera, mWorld.get(), mSceneMgr));
    mModeContext.mInputHandler->resetCameraMenPointer(mCameraMan.get());

    //load data
    {
        bool isDebugLLT = mLuaManager.ReadScalarBool("Terrain.Scene.IsDebugLLT", mModeContext.mPipeline);

        LLTLoader().load(mModeContext.mGameState, mSceneMgr, isDebugLLT);
    }

    {
        bool isDebugExclusion = mLuaManager.ReadScalarBool("Terrain.Scene.IsDebugExclusion", mModeContext.mPipeline);
        ExclusionLoader().load(mModeContext.mGameState, mSceneMgr, isDebugExclusion);
    }

    PHYLoader().load(mModeContext.mGameState);

    {
        bool isDebugAI = mLuaManager.ReadScalarBool("Terrain.Scene.IsDebugAI", mModeContext.mPipeline);

        mModeContext.mGameState.setAICount(mLuaManager.ReadScalarInt("Scene.AIOpponents", mModeContext.mPipeline));

        Ogre::Real speedCoeff;
        Ogre::Real lateralStabilizationCoeff;

        switch(mModeContext.mGameState.getAIStrength())
        {
        case Medium :
            speedCoeff = 0.7f;
            lateralStabilizationCoeff = 0.7f;
            if(mModeContext.mGameState.getTrackName() == "desert track") lateralStabilizationCoeff = 0.7f;
            if(mModeContext.mGameState.getTrackName() == "speedway track") lateralStabilizationCoeff = 0.2f;
            if(mModeContext.mGameState.getTrackName() == "speedway night track") lateralStabilizationCoeff = 0.2f;
            if(mModeContext.mGameState.getTrackName() == "dam") lateralStabilizationCoeff = 0.7f;
            if(mModeContext.mGameState.getTrackName() == "mountain forest track") lateralStabilizationCoeff = 0.7f;
            if(mModeContext.mGameState.getTrackName() == "mineshaft") lateralStabilizationCoeff = 0.7f;
            if(mModeContext.mGameState.getTrackName() == "alpine track") lateralStabilizationCoeff = 0.7f;
            if(mModeContext.mGameState.getTrackName() == "city track") lateralStabilizationCoeff = 1.4f;
            if(mModeContext.mGameState.getTrackName() == "nutopia track") lateralStabilizationCoeff = 1.4f;
            break;
        case Hard :
            speedCoeff = 0.9f;
            lateralStabilizationCoeff = 0.8f;
            if(mModeContext.mGameState.getTrackName() == "desert track") lateralStabilizationCoeff = 0.8f;
            if(mModeContext.mGameState.getTrackName() == "speedway track") lateralStabilizationCoeff = 0.2f;
            if(mModeContext.mGameState.getTrackName() == "speedway night track") lateralStabilizationCoeff = 0.2f;
            if(mModeContext.mGameState.getTrackName() == "dam") lateralStabilizationCoeff = 0.8f;
            if(mModeContext.mGameState.getTrackName() == "mountain forest track") lateralStabilizationCoeff = 0.8f;
            if(mModeContext.mGameState.getTrackName() == "mineshaft") lateralStabilizationCoeff = 0.8f;
            if(mModeContext.mGameState.getTrackName() == "alpine track") lateralStabilizationCoeff = 0.8f;
            if(mModeContext.mGameState.getTrackName() == "city track") lateralStabilizationCoeff = 1.6f;
            if(mModeContext.mGameState.getTrackName() == "nutopia track") lateralStabilizationCoeff = 1.4f;
            break;
        case Insane :
            speedCoeff = 1.0f;
            lateralStabilizationCoeff = 1.0f;
            if(mModeContext.mGameState.getTrackName() == "desert track") lateralStabilizationCoeff = 1.0f;
            if(mModeContext.mGameState.getTrackName() == "speedway track") lateralStabilizationCoeff = 0.2f;
            if(mModeContext.mGameState.getTrackName() == "speedway night track") lateralStabilizationCoeff = 0.2f;
            if(mModeContext.mGameState.getTrackName() == "dam") lateralStabilizationCoeff = 1.0f;
            if(mModeContext.mGameState.getTrackName() == "mountain forest track") lateralStabilizationCoeff = 1.0f;
            if(mModeContext.mGameState.getTrackName() == "mineshaft") lateralStabilizationCoeff = 1.0f;
            if(mModeContext.mGameState.getTrackName() == "alpine track") lateralStabilizationCoeff = 1.0f;
            if(mModeContext.mGameState.getTrackName() == "city track") lateralStabilizationCoeff = 1.6f;
            if(mModeContext.mGameState.getTrackName() == "nutopia track") lateralStabilizationCoeff = 1.4f;
            break;
        default:
            speedCoeff = 0.6f;
            lateralStabilizationCoeff = 0.2f;
            if(mModeContext.mGameState.getTrackName() == "desert track") lateralStabilizationCoeff = 0.2f;
            if(mModeContext.mGameState.getTrackName() == "speedway track") lateralStabilizationCoeff = 0.2f;
            if(mModeContext.mGameState.getTrackName() == "speedway night track") lateralStabilizationCoeff = 0.2f;
            if(mModeContext.mGameState.getTrackName() == "dam") lateralStabilizationCoeff = 0.2f;
            if(mModeContext.mGameState.getTrackName() == "mountain forest track") lateralStabilizationCoeff = 0.2f;
            if(mModeContext.mGameState.getTrackName() == "mineshaft") lateralStabilizationCoeff = 0.2f;
            if(mModeContext.mGameState.getTrackName() == "alpine track") lateralStabilizationCoeff = 0.2f;
            if(mModeContext.mGameState.getTrackName() == "city track") lateralStabilizationCoeff = 1.2f;
            if(mModeContext.mGameState.getTrackName() == "nutopia track") lateralStabilizationCoeff = 1.4f;
        }

        Ogre::Real speedCoeffMinLimit = speedCoeff - 0.1f;

        for(size_t w = 0; w < mModeContext.mGameState.getAICount(); ++w)
        {
            mModeContext.mGameState.getAICar(w).clearAIData();

            mModeContext.mGameState.getAICar(w).setLateralStabilizationCoeff(lateralStabilizationCoeff);
            mModeContext.mGameState.getAICar(w).setSpeedCoeff(Randomizer::GetInstance().GetRandomFloat(speedCoeffMinLimit, speedCoeff));
        }

        AILoader().load(mModeContext.mGameState, mSceneMgr, isDebugAI);
    }

    //init multiplayer
    {
        mModeContext.mGameState.setMultiplayerMaster(mLuaManager.ReadScalarBool("Scene.Multiplayer.IsMaster", mModeContext.mPipeline));
        mModeContext.mGameState.setMultiplayerRoomName(mLuaManager.ReadScalarString("Scene.Multiplayer.RoomName", mModeContext.mPipeline));
        mModeContext.mGameState.setMultiplayerUserName(mLuaManager.ReadScalarString("Scene.Multiplayer.UserName", mModeContext.mPipeline));
        mModeContext.mGameState.setMultiplayerServerIP(mLuaManager.ReadScalarString("Scene.Multiplayer.ServerIP", mModeContext.mPipeline));
        mModeContext.mGameState.setMultiplayerServerPort(mLuaManager.ReadScalarInt("Scene.Multiplayer.ServerPort", mModeContext.mPipeline));
        mModeContext.mGameState.setMultiplayerPlayersLimits(mLuaManager.ReadScalarInt("Scene.Multiplayer.PlayersLimits", mModeContext.mPipeline));
        mModeContext.mGameState.setMultiplayerBroadcastInterval(mLuaManager.ReadScalarInt("Scene.Multiplayer.BroadcastInterval", mModeContext.mPipeline));

        customInitScene();
    }

    ParticlesLoader().load(mModeContext.mGameState);

    //http://www.ogre3d.org/forums/viewtopic.php?p=331138
    //http://www.ogre3d.org/forums/viewtopic.php?f=2&t=79581
    mSceneMgrCarUI = mModeContext.mRoot->createSceneManager(Ogre::ST_GENERIC);
    Ogre::Camera * cameraCarUI = mSceneMgrCarUI->createCamera("PlayerCam");
    cameraCarUI->setProjectionType(Ogre::PT_ORTHOGRAPHIC);
    cameraCarUI->setOrthoWindow(static_cast<float>(mViewPort->getActualWidth()), static_cast<float>(mViewPort->getActualHeight()));
    cameraCarUI->setNearClipDistance(0.5f);
    cameraCarUI->setFarClipDistance(10000.0f);
    Ogre::Viewport * viewPortCarUI = mModeContext.mWindow->addViewport(cameraCarUI, 1);
    viewPortCarUI->setClearEveryFrame(true, Ogre::FBT_DEPTH);
    viewPortCarUI->setOverlaysEnabled(false);
    viewPortCarUI->setSkiesEnabled(false);
    cameraCarUI->setAspectRatio(static_cast<float>(mViewPort->getActualWidth()) / static_cast<float>(mViewPort->getActualHeight()));
    cameraCarUI->setFOVy(Ogre::Degree(45.0f));
    cameraCarUI->setPosition(0.0f, 0.0f, 100.0f);
    cameraCarUI->lookAt(Ogre::Vector3::ZERO);
}

void BaseRaceMode::clearScene()
{
    mModeContext.mGraphics2D.clearMiscPanelText();
    mModeContext.mGraphics2D.setVisibleFinishSign(false);

    mModeContext.mGameState.resetGamePaused();
    mModeContext.mGameState.setRaceStarted(false);
    mModeContext.mGameState.setRaceFinished(false);

    deInitWorld();

    //deinit sounds of system

    //clear sounds of cars as well
    mModeContext.mGameState.getPlayerCar().clear();
    mModeContext.mGameState.getPlayerCar().deinitSounds();
    for(size_t q = 0; q < mModeContext.mGameState.getAICount(); ++q)
    {
        mModeContext.mGameState.getAICar(q).clear();
        mModeContext.mGameState.getAICar(q).deinitSounds();
    }

    customClearScene();

    mModeContext.mSoundsProcesser.stopSounds();

    mModeContext.mGameState.setGlobalLight(NULL);
    mModeContext.mGameState.setShadowLight(NULL);

    mModeContext.mInputHandler->resetCameraMenPointer(NULL);
    mCameraMan.reset();

    mModeContext.mTrayMgr->getTraysLayer()->remove3D(mModeContext.mGameState.getArrowNode());

    mSceneMgr->clearScene();
    mModeContext.mRoot->destroySceneManager(mSceneMgr);
    mSceneMgrCarUI->clearScene();
    mModeContext.mRoot->destroySceneManager(mSceneMgrCarUI);
    mModeContext.mWindow->removeAllViewports();
}

void BaseRaceMode::initLightLists()
{
    //d.polubotko: make sure light lists created during loading
    mStaticMeshProcesser.queryLights();
}

void BaseRaceMode::initTerrain()
{
    mStaticMeshProcesser.initParts( mModeContext.mPipeline, mSceneMgr, mMainNode, mIsGlobalReset, 
                                    mModeContext.mGameState, mWorld.get());
    mStaticMeshProcesser.loadTerrainMaps(mModeContext.mGameState);

    //load terrain params
    TerrainLoader terrainLoader;
    terrainLoader.load(mModeContext.mGameState);

    mStaticMeshProcesser.setFrictionRemapArray(terrainLoader.getRemapFriction());
    mStaticMeshProcesser.setLatutuideFrictionArray(terrainLoader.getLatitudeFriction());
    mStaticMeshProcesser.setLongtitudeFrictionArray(terrainLoader.getLongtitudeFriction());
    mModeContext.mSoundsProcesser.setRemappers(terrainLoader.getRemapSounds(), terrainLoader.getRemapSoundsCollision());

    //particle (before loading cars)
    {
        //Ogre::TexturePtr particle = TextureLoader().load(mModeContext.mGameState.getPFLoaderData(), "data/deii", "particle.tga", "OriginalParticle");
        Ogre::TexturePtr particle = TEXLoader().load(mModeContext.mGameState.getPFLoaderData(), "data/deii", "d3d2display1_m_1.tex", "OriginalParticle");
        Ogre::MaterialPtr particleMat = Ogre::MaterialManager::getSingleton().getByName("Test/Particle");
        particleMat->getTechnique(0)->getPass(0)->getTextureUnitState(0)->setTexture(particle);
    }
}

void BaseRaceMode::initModel()
{

    //arrow (before loading models)
    {
        TEXLoader().load(mModeContext.mGameState.getPFLoaderData(), "data/misc", "arrow_m_3.tex", "OriginalArrow");
    }

    mModeContext.mGameState.setPLayerCarPrevVel(Ogre::Vector3::ZERO);

    bool isCamToAI = mLuaManager.ReadScalarBool("Scene.IsCamToAI", mModeContext.mPipeline);

    mLapController.clear();
    mLapController.addCar(&mModeContext.mGameState.getPlayerCar());//should be first
    mModeContext.mGameState.getPlayerCar().getLapUtils().setEvents(this);

    mModelsPool.initModels(mSceneMgr, mModeContext.mGameState);

    mModeContext.mGameState.getPlayerCar().initModel(mModeContext.mPipeline, mModeContext.mGameState, mSceneMgr, mMainNode, mCameraMan.get(), &mModelsPool, mWorld.get(), mModeContext.mGameState.getPlayerCharacterName(), mModeContext.mGameState.getTrackPositions()[mModeContext.mGameState.getAICount()], !isCamToAI);
    mModeContext.mGameState.getPlayerCar().initSounds(mModeContext.mPipeline, mModeContext.mGameState);

    std::vector<std::string> aiCharacters = RacingGridGeneration().generate(mModeContext.mGameState);

    for(size_t q = 0; q < mModeContext.mGameState.getAICount(); ++q)
    {
        bool isCam = (q == (mModeContext.mGameState.getAICount() - 1));
        if(!isCamToAI)
            isCam = false;
        mModeContext.mGameState.getAICar(q).initModel(mModeContext.mPipeline, mModeContext.mGameState, mSceneMgr, mMainNode, mCameraMan.get(), &mModelsPool, mWorld.get(), aiCharacters[q], mModeContext.mGameState.getTrackPositions()[q], isCam);
        mModeContext.mGameState.getAICar(q).initSounds(mModeContext.mPipeline, mModeContext.mGameState);

        mLapController.addCar(&mModeContext.mGameState.getAICar(q));
    }

}

void BaseRaceMode::initMisc()
{
    //rear mirror camera
    mRearCamera = NULL;

    mModeContext.mGameState.setMirrorEnabled(mLuaManager.ReadScalarBool("Scene.Mirror.Enable", mModeContext.mPipeline));
    if(mModeContext.mGameState.getMirrorEnabled())
    {

        //rear mirror texture
        //http://www.ogre3d.org/tikiwiki/tiki-index.php?page=Basic+Tutorial+7
        if(mIsGlobalReset)
        {
            int mirrorSizeW = mLuaManager.ReadScalarInt("Scene.Mirror.RTTSize.w", mModeContext.mPipeline);
            int mirrorSizeH = mLuaManager.ReadScalarInt("Scene.Mirror.RTTSize.h", mModeContext.mPipeline);

            //add this as listener
            mModeContext.mGraphics2D.createRearViewMirrorPanelTexture(this, mModeContext.mRoot, mirrorSizeW, mirrorSizeH);
            mModeContext.mGraphics2D.setRearViewMirrorPanelMaterial("Test/RearViewMirror");
        }
        else
        {
            mModeContext.mGraphics2D.rearViewMirrorPanelTextureRemoveAllViewports();
        }

        mModeContext.mGraphics2D.initTachoNeedle(mSceneMgrCarUI, mModeContext.mGameState);


        mRearCamera = mSceneMgr->createCamera("RearViewCamera");
        mRearCamera->setNearClipDistance(0.5f);
        Ogre::Viewport *v = mModeContext.mGraphics2D.rearViewMirrorPanelTextureAddViewport(mRearCamera);
        v->setOverlaysEnabled(false);
        v->setClearEveryFrame(true);
        v->setBackgroundColour(mModeContext.mGameState.getBackgroundColor());
        mRearCamera->setAspectRatio(1.0f);
        mRearCamera->setFOVy(Ogre::Degree(mLuaManager.ReadScalarFloat("Scene.Mirror.FOV", mModeContext.mPipeline)));

        mModeContext.mGraphics2D.setRearViewMirrorPanelShow(true);
    }
    else
    {
        mModeContext.mGraphics2D.setRearViewMirrorPanelShow(false);
    }
    mCameraMan->setRearCamera(mRearCamera);

    //as long as last procedure before draw started
    mModeContext.mGameState.resetBeforeStartTimer();

    //arrow
    Ogre::Entity * arrowEntity = mModelsPool.getArrow();
    mModeContext.mGameState.setArrowNode(mSceneMgr->createSceneNode(arrowEntity->getName()));
    mModeContext.mGameState.getArrowNode()->attachObject(arrowEntity);
    mModeContext.mTrayMgr->getTraysLayer()->add3D(mModeContext.mGameState.getArrowNode());
    mModeContext.mGameState.getArrowNode()->setPosition(-0.75f, 0.75f, -1.0f);
    mModeContext.mGameState.getArrowNode()->setScale(0.20f, 0.20f, 0.20f);
    mModeContext.mGameState.getArrowNode()->setVisible(true);

    mIsGlobalReset = false;
}



void BaseRaceMode::initWorld(const Ogre::Vector3 &gravityVector, const Ogre::AxisAlignedBox &bounds)
{
    mWorld.reset(new OgreBulletDynamics::DynamicsWorld(mSceneMgr, bounds, gravityVector, true, true, 10000));
    mDebugDrawer.reset(new OgreBulletCollisions::DebugDrawer());
    mWorld->setDebugDrawer(mDebugDrawer.get());

    Ogre::SceneNode *node = mSceneMgr->getRootSceneNode()->createChildSceneNode("debugDrawer", Ogre::Vector3::ZERO);
    node->attachObject(static_cast<Ogre::SimpleRenderable *>(mDebugDrawer.get()));

    if(mLuaManager.ReadScalarBool("Terrain.Scene.IsDebugPhysics", mModeContext.mPipeline))
    {
        mDebugDrawer->setDrawWireframe(true);
        mWorld->setShowDebugShapes(true);

        mDebugDrawer->setDrawContactPoints(true);
        mWorld->setShowDebugContactPoints(true);
    }
}

void BaseRaceMode::deInitWorld()
{
    mStaticMeshProcesser.deinit();
    mDebugDrawer.reset();
    mWorld->setDebugDrawer(NULL);
    mWorld.reset();
}

void BaseRaceMode::frameStarted(const Ogre::FrameEvent &evt)
{
    Ogre::Vector3 playerPos = mModeContext.mGameState.getPlayerCar().getModelNode()->getPosition();

    Ogre::Vector3 playerDir = mModeContext.mGameState.getPlayerCar().getForwardAxis();

    mModeContext.mSoundsProcesser.setListenerPos(playerPos);
    if(!mModeContext.mGameState.isGamePaused())
        mModeContext.mSoundsProcesser.setListenerGain(mModeContext.mGameState.getListenerGain());
    else
        mModeContext.mSoundsProcesser.setListenerGain(0.0f);

    if(!mModeContext.mGameState.getRaceFinished())
        mModeContext.mGameState.getPlayerCar().getLapUtils().checkCheckPoints(playerPos);

    mModeContext.mGameState.getArrowNode()->setOrientation(mModeContext.mGameState.getPlayerCar().getLapUtils().getArrowOrientation(playerPos, playerDir));

    if(!mModeContext.mGameState.isGamePaused())
    {
        mModeContext.mGameState.getPlayerCar().processFrameBeforePhysics(evt, mStaticMeshProcesser, mModeContext.mGameState.getRaceStarted());

        for(size_t q = 0; q < mModeContext.mGameState.getAICount(); ++q)
        {
            if(!mModeContext.mGameState.getRaceFinished())
                mModeContext.mGameState.getAICar(q).getLapUtils().checkCheckPoints(mModeContext.mGameState.getAICar(q).getModelNode()->getPosition());

            if(mModeContext.mGameState.getRaceStarted())
            {
                mModeContext.mGameState.getAICar(q).performAICorrection(mModeContext.mGameState.isGamePaused());
            }
            mModeContext.mGameState.getAICar(q).processFrameBeforePhysics(evt, mStaticMeshProcesser, mModeContext.mGameState.getRaceStarted());
        }

        customFrameStartedDoProcessFrameBeforePhysics(evt);
    }

    mLapController.calculateLapPositions();

    if(mModeContext.mGameState.getRaceStarted() && !mModeContext.mGameState.isGamePaused())
        mWorld->stepSimulation(evt.timeSinceLastFrame, 7);

    mModeContext.mGameState.getPlayerCar().processFrameAfterPhysics(evt, mModeContext.mGameState.getRaceStarted());

    for(size_t q = 0; q < mModeContext.mGameState.getAICount(); ++q)
        mModeContext.mGameState.getAICar(q).processFrameAfterPhysics(evt, mModeContext.mGameState.getRaceStarted());

    customFrameStartedDoProcessFrameAfterPhysics(evt);
}

void BaseRaceMode::frameRenderingQueued(const Ogre::FrameEvent& evt)
{
    customFrameRenderingQueuedDoBegining();

    if( !mModeContext.mGameState.getRaceStarted() && 
        mModeContext.mGameState.getBeforeStartTimerTime() > mModeContext.mGameState.getSTRRacecrud().getFloatValue("on-grid parameters", "ready left start") * 1000.0f && 
        !mModeContext.mGameState.isGamePaused())
    {
        mModeContext.mGraphics2D.setRearViewMirrorPanelShow(false);
        mModeContext.mGraphics2D.hideAllStart();
        mModeContext.mGraphics2D.showBeforeStart1();
        mModeContext.mSoundsProcesser.playBeforeStart1();
    }

    if(!mModeContext.mGameState.getRaceStarted() && 
        mModeContext.mGameState.getBeforeStartTimerTime() > mModeContext.mGameState.getSTRRacecrud().getFloatValue("on-grid parameters", "set left start") * 1000.0f && 
        !mModeContext.mGameState.isGamePaused())
    {
        mModeContext.mGraphics2D.hideAllStart();
        mModeContext.mGraphics2D.showBeforeStart2();
        mModeContext.mSoundsProcesser.playBeforeStart2();
    }

    if(!mModeContext.mGameState.getRaceStarted() && 
        mModeContext.mGameState.getBeforeStartTimerTime() > mModeContext.mGameState.getSTRRacecrud().getFloatValue("on-grid parameters", "go left start") * 1000.0f && 
        !mModeContext.mGameState.isGamePaused())
    {
        mModeContext.mGraphics2D.hideAllStart();
        mModeContext.mGraphics2D.showBeforeStart3();
        mModeContext.mSoundsProcesser.playBeforeStart3();

        for(size_t q = 0; q < mModeContext.mGameState.getAICount(); ++q)
        {
            mModeContext.mGameState.getAICar(q).raceStarted();
        }

        customFrameRenderingQueuedDoRaceStarted();

        mModeContext.mGameState.getPlayerCar().getLapUtils().resetLapTimer();

        mModeContext.mGameState.setRaceStarted(true);
    }

    if(mModeContext.mGameState.getBeforeStartTimerTime() > (mModeContext.mGameState.getSTRRacecrud().getFloatValue("on-grid parameters", "go left start") * 1000.0f + mModeContext.mGameState.getSTRRacecrud().getFloatValue("on-grid parameters", "go left length") * 1000.0f) && !mModeContext.mGameState.isGamePaused())
    {
        mModeContext.mGraphics2D.hideAllStart();

        if(mModeContext.mGameState.getMirrorEnabled() && !mModeContext.mGameState.getRaceFinished())
        {
            mModeContext.mGraphics2D.setRearViewMirrorPanelShow(true);
        }
    }

    mModeContext.mInputHandler->capture(evt, mModeContext.mGameState.getPlayerCar().getModelNode(), mModeContext.mGameState.getGlobalLight(), mModeContext.mGameState.getShadowLight(), true);

    mModeContext.mTrayMgr->frameRenderingQueued(evt);

    mModeContext.mGraphics2D.setShowPausedPanel(mModeContext.mGameState.isGamePaused());

    if (!mModeContext.mTrayMgr->isDialogVisible())
    {
        if(!mModeContext.mGameState.isGamePaused())
            mModeContext.mGraphics2D.setEngineRPM(mModeContext.mGameState.getPlayerCar().getCarEngine().getEngineRPM());
        mModeContext.mGraphics2D.setCarSpeed(mModeContext.mGameState.getPlayerCar().getAlignedVelocity());
        mModeContext.mGraphics2D.setCurrentLap(static_cast<unsigned short>(mModeContext.mGameState.getPlayerCar().getLapUtils().getCurrentLap()), static_cast<unsigned short>(mModeContext.mGameState.getLapsCount()));
        mModeContext.mGraphics2D.setCarGear(static_cast<unsigned char>(mModeContext.mGameState.getPlayerCar().getCarEngine().getCurrentGear()));
        mModeContext.mGraphics2D.setCarPos(static_cast<unsigned char>(mLapController.getTotalPosition(0)), static_cast<unsigned char>(mLapController.getTotalCars()));

        mModeContext.mGraphics2D.hideAIDashboardCars();
        size_t currentPlayerLap = mModeContext.mGameState.getPlayerCar().getLapUtils().getCurrentLap();
        Ogre::Real currentPlayerLapPos = mModeContext.mGameState.getPlayerCar().getLapUtils().getLapPosition();
        mModeContext.mGraphics2D.setPlayerDashBoardSkin(mModeContext.mGameState);
        for(size_t q = 0; q < mModeContext.mGameState.getAICount(); ++q)
        {
            mModeContext.mGraphics2D.setAIDashBoardSkin(mModeContext.mGameState, q, mModeContext.mGameState.getAICar(q).getCharacterName());
            mModeContext.mGraphics2D.setDashCarPos(q, currentPlayerLap, currentPlayerLapPos, mModeContext.mGameState.getAICar(q).getLapUtils().getCurrentLap(), mModeContext.mGameState.getAICar(q).getLapUtils().getLapPosition());
        }

        customFrameRenderingQueuedDo2DUI();

        if(mModeContext.mGameState.getRaceStarted())
        {
            if(mModeContext.mGameState.getPlayerCar().getLapUtils().getAfterFinishLinePassTime() < 3.0f && mModeContext.mGameState.getPlayerCar().getLapUtils().getCurrentLap() > 1)
            {
                mModeContext.mGraphics2D.setRaceTime(Tools::SecondsToString(mModeContext.mGameState.getPlayerCar().getLapUtils().getLastLapTime()));
            }
            else
            {
                mModeContext.mGraphics2D.setRaceTime(Tools::SecondsToString(mModeContext.mGameState.getPlayerCar().getLapUtils().getLapTime()));
            }
        }
        else
        {
            mModeContext.mGraphics2D.setRaceTime("00:00:00");
        }


#if SHOW_DETAILS_PANEL
        mDetailsPanel->setParamValue(0, Ogre::StringConverter::toString(mCamera->getDerivedPosition().x));
        mDetailsPanel->setParamValue(1, Ogre::StringConverter::toString(mCamera->getDerivedPosition().y));
        mDetailsPanel->setParamValue(2, Ogre::StringConverter::toString(mCamera->getDerivedPosition().z));
        
        mDetailsPanel->setParamValue(4, Ogre::StringConverter::toString(mModeContext.mGameState.getPlayerCar().getLinearVelocity().length()));
        mDetailsPanel->setParamValue(5, Ogre::StringConverter::toString(mModeContext.mGameState.getPlayerCar().getAlignedVelocity()));
        mDetailsPanel->setParamValue(6, Ogre::StringConverter::toString(mModeContext.mGameState.getPlayerCar().getLateralVelocity()));
#if 0
        mDetailsPanel->setParamValue(4, Ogre::StringConverter::toString(mModeContext.mGameState.getAICar(0].getLinearVelocity().length()));
        mDetailsPanel->setParamValue(5, Ogre::StringConverter::toString(mModeContext.mGameState.getAICar(0].getAlignedVelocity()));
        mDetailsPanel->setParamValue(6, Ogre::StringConverter::toString(mModeContext.mGameState.getAICar(0].getLateralVelocity()));
#endif

        mDetailsPanel->setParamValue(8, mModeContext.mGameState.getPlayerCar().getFrontLWheelColliderString());
        mDetailsPanel->setParamValue(9, mModeContext.mGameState.getPlayerCar().getFrontRWheelColliderString());
        mDetailsPanel->setParamValue(10, mModeContext.mGameState.getPlayerCar().getBackLWheelColliderString());
        mDetailsPanel->setParamValue(11, mModeContext.mGameState.getPlayerCar().getBackRWheelColliderString());
#if 0
        mDetailsPanel->setParamValue(8, mModeContext.mGameState.getAICar(0].getFrontLWheelColliderString());
        mDetailsPanel->setParamValue(9, mModeContext.mGameState.getAICar(0].getFrontRWheelColliderString());
        mDetailsPanel->setParamValue(10, mModeContext.mGameState.getAICar(0].getBackLWheelColliderString());
        mDetailsPanel->setParamValue(11, mModeContext.mGameState.getAICar(0].getBackRWheelColliderString());
#endif

        mDetailsPanel->setParamValue(13, Ogre::StringConverter::toString(mStaticMeshProcesser.getLongtitudeFriction(mModeContext.mGameState.getPlayerCar().getFrontLWheelColliderIndex())));
        mDetailsPanel->setParamValue(14, Ogre::StringConverter::toString(mStaticMeshProcesser.getLongtitudeFriction(mModeContext.mGameState.getPlayerCar().getFrontRWheelColliderIndex())));
        mDetailsPanel->setParamValue(15, Ogre::StringConverter::toString(mStaticMeshProcesser.getLongtitudeFriction(mModeContext.mGameState.getPlayerCar().getBackLWheelColliderIndex())));
        mDetailsPanel->setParamValue(16, Ogre::StringConverter::toString(mStaticMeshProcesser.getLongtitudeFriction(mModeContext.mGameState.getPlayerCar().getBackRWheelColliderIndex())));
#if 0
        mDetailsPanel->setParamValue(13, Ogre::StringConverter::toString(mStaticMeshProcesser.getLongtitudeFriction(mModeContext.mGameState.getAICar(0].getFrontLWheelColliderIndex())));
        mDetailsPanel->setParamValue(14, Ogre::StringConverter::toString(mStaticMeshProcesser.getLongtitudeFriction(mModeContext.mGameState.getAICar(0].getFrontRWheelColliderIndex())));
        mDetailsPanel->setParamValue(15, Ogre::StringConverter::toString(mStaticMeshProcesser.getLongtitudeFriction(mModeContext.mGameState.getAICar(0].getBackLWheelColliderIndex())));
        mDetailsPanel->setParamValue(16, Ogre::StringConverter::toString(mStaticMeshProcesser.getLongtitudeFriction(mModeContext.mGameState.getAICar(0].getBackRWheelColliderIndex())));
#endif

        mDetailsPanel->setParamValue(18, Ogre::StringConverter::toString(mStaticMeshProcesser.getLatitudeFriction(mModeContext.mGameState.getPlayerCar().getFrontLWheelColliderIndex())));
        mDetailsPanel->setParamValue(19, Ogre::StringConverter::toString(mStaticMeshProcesser.getLatitudeFriction(mModeContext.mGameState.getPlayerCar().getFrontRWheelColliderIndex())));
        mDetailsPanel->setParamValue(20, Ogre::StringConverter::toString(mStaticMeshProcesser.getLatitudeFriction(mModeContext.mGameState.getPlayerCar().getBackLWheelColliderIndex())));
        mDetailsPanel->setParamValue(21, Ogre::StringConverter::toString(mStaticMeshProcesser.getLatitudeFriction(mModeContext.mGameState.getPlayerCar().getBackRWheelColliderIndex())));
#if 0
        mDetailsPanel->setParamValue(18, Ogre::StringConverter::toString(mStaticMeshProcesser.getLatitudeFriction(mModeContext.mGameState.getAICar(0].getFrontLWheelColliderIndex())));
        mDetailsPanel->setParamValue(19, Ogre::StringConverter::toString(mStaticMeshProcesser.getLatitudeFriction(mModeContext.mGameState.getAICar(0].getFrontRWheelColliderIndex())));
        mDetailsPanel->setParamValue(20, Ogre::StringConverter::toString(mStaticMeshProcesser.getLatitudeFriction(mModeContext.mGameState.getAICar(0].getBackLWheelColliderIndex())));
        mDetailsPanel->setParamValue(21, Ogre::StringConverter::toString(mStaticMeshProcesser.getLatitudeFriction(mModeContext.mGameState.getAICar(0].getBackRWheelColliderIndex())));
#endif

        mDetailsPanel->setParamValue(23, Ogre::StringConverter::toString(mModeContext.mGameState.getPlayerCar().getLapUtils().getLapPosition()));
        mDetailsPanel->setParamValue(24, Tools::SecondsToString(mModeContext.mGameState.getPlayerCar().getLapUtils().getLastLapTime()));
        mDetailsPanel->setParamValue(25, Tools::SecondsToString(mModeContext.mGameState.getPlayerCar().getLapUtils().getLapTime()));
        mDetailsPanel->setParamValue(26, Ogre::StringConverter::toString(mModeContext.mGameState.getPlayerCar().getLapUtils().getCurrentLap()));
        mDetailsPanel->setParamValue(27, Ogre::StringConverter::toString(mLapController.getTotalPosition(0)));
        mDetailsPanel->setParamValue(28, Ogre::StringConverter::toString(mModeContext.mGameState.getPlayerCar().getCarEngine().getCurrentGear()));
        mDetailsPanel->setParamValue(29, Ogre::StringConverter::toString(mModeContext.mGameState.getPlayerCar().getCarEngine().getEngineRPM()));
#endif
    }
}

void BaseRaceMode::processCollision(btManifoldPoint& cp, const btCollisionObjectWrapper* colObj0Wrap, const btCollisionObjectWrapper* colObj1Wrap, int triIndex)
{
    mModeContext.mGameState.getPlayerCar().processWheelsCollision(cp, colObj0Wrap, colObj1Wrap, mStaticMeshProcesser, triIndex);
    mModeContext.mGameState.getPlayerCar().processChassisCollision(cp, colObj0Wrap, colObj1Wrap, mStaticMeshProcesser, triIndex);

    for(size_t q = 0; q < mModeContext.mGameState.getAICount(); ++q)
    {
        mModeContext.mGameState.getAICar(q).processWheelsCollision(cp, colObj0Wrap, colObj1Wrap, mStaticMeshProcesser, triIndex);
    }

    customProcessCollision(cp, colObj0Wrap, colObj1Wrap, triIndex);

    //surface sound
    bool isSoundPlay = false;
    if(mModeContext.mGameState.getPlayerCar().checkFrontCollision() || mModeContext.mGameState.getPlayerCar().checkRearCollision())
    {
        //if(mModeContext.mGameState.getPlayerCar().getAcceleration() || mModeContext.mGameState.getPlayerCar().getBrake())
        {
            Ogre::Real lateralVel = Ogre::Math::Abs(mModeContext.mGameState.getPlayerCar().getLateralVelocity());
            isSoundPlay = true;
            unsigned char backLWheel = mModeContext.mGameState.getPlayerCar().getBackLWheelColliderIndex();
            mModeContext.mSoundsProcesser.playSurface(backLWheel, lateralVel);
        }
    }
    if(!isSoundPlay)
    {
        mModeContext.mSoundsProcesser.stopSoundSurfaces();
    }

    //surface crash sound
    Ogre::Vector3 playerVel = mModeContext.mGameState.getPlayerCar().getLinearVelocity();
    Ogre::Real playerVelDiff = playerVel.distance(mModeContext.mGameState.getPLayerCarPrevVel());
    mModeContext.mGameState.setPLayerCarPrevVel(playerVel);
    if(playerVelDiff > 10.0f)
    {
        if(mModeContext.mGameState.getPlayerCar().checkFrontCollision() || mModeContext.mGameState.getPlayerCar().checkRearCollision() || mModeContext.mGameState.getPlayerCar().checkChassisCollision())
        {

            unsigned char collisionSurface = 0;

            if(mModeContext.mGameState.getPlayerCar().checkFrontCollision())
            {
                collisionSurface = mModeContext.mGameState.getPlayerCar().getFrontLWheelColliderIndex();
            }

            if(mModeContext.mGameState.getPlayerCar().checkRearCollision())
            {
                collisionSurface = mModeContext.mGameState.getPlayerCar().getBackLWheelColliderIndex();
            }

            if(mModeContext.mGameState.getPlayerCar().checkChassisCollision())
            {
                collisionSurface = mModeContext.mGameState.getPlayerCar().getChassisColliderIndex();
            }

            mModeContext.mSoundsProcesser.playSurfaceCrash(collisionSurface);
        }
    }

    //ai with player crash sounds
    for(size_t q = 0; q < mModeContext.mGameState.getAICount(); ++q)
    {
        if(mModeContext.mGameState.getPlayerCar().isCollideChassis(mModeContext.mGameState.getAICar(q), colObj0Wrap, colObj1Wrap))
        {
            Ogre::Vector3 aiVel = mModeContext.mGameState.getAICar(q).getLinearVelocity();
            Ogre::Real velDiff = playerVel.distance(aiVel);
            if(velDiff > 20.0f && velDiff <= 50.0f)
            {
                mModeContext.mSoundsProcesser.playCrash1();
                break;
            }
            else if(velDiff > 50.0f)
            {
                mModeContext.mSoundsProcesser.playCrash2();
                break;
            }
            
        }
    }
}

void BaseRaceMode::loadResources()
{
#if defined(__ANDROID__)
    LOGI("BaseApp[loadResources]: Begin"); 
#endif

    Ogre::ResourceGroupManager::getSingleton().createResourceGroup(TEMP_RESOURCE_GROUP_NAME);

    //http://ogre3d.org/forums/viewtopic.php?f=2&t=54469
    mModeContext.mTrayMgr->showLoadingBar(2, 2);
    //Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups();

    Ogre::ResourceGroupManager::getSingleton().initialiseResourceGroup("Popular");
    Ogre::ResourceGroupManager::getSingleton().loadResourceGroup("Popular");


    mModeContext.mTrayMgr->hideLoadingBar();

#if defined(__ANDROID__)
    LOGI("BaseApp[loadResources]: End"); 
#endif
}

void BaseRaceMode::unloadResources()
{

    mModeContext.mGraphics2D.setRearViewMirrorPanelMaterial("BaseWhite");
    mModeContext.mGraphics2D.setRearViewMirrorPanelShow(false);

    //deinit sounds of system

    //clear sounds of cars as well
    mModeContext.mGameState.getPlayerCar().deinitSounds();
    for(size_t q = 0; q < mModeContext.mGameState.getAICount(); ++q)
        mModeContext.mGameState.getAICar(q).deinitSounds();

    customUnloadResources();

    mModeContext.mSoundsProcesser.stopSounds();

    Ogre::ParticleSystemManager::getSingleton().removeAllTemplates();

    Ogre::ResourceGroupManager::getSingleton().clearResourceGroup("Popular");

    Ogre::ResourceGroupManager::getSingleton().destroyResourceGroup(TEMP_RESOURCE_GROUP_NAME);

    //Ogre::MaterialManager::getSingleton().removeUnreferencedResources();
    //Ogre::MaterialManager::getSingleton().initialise();
}

void BaseRaceMode::onLapFinished()
{
    size_t lap = mModeContext.mGameState.getPlayerCar().getLapUtils().getCurrentLap() - 1;

    if(lap <= mModeContext.mGameState.getLapsCount())
    {
        std::string time = Tools::SecondsToString(mModeContext.mGameState.getPlayerCar().getLapUtils().getLastLapTime());
        std::string lapS = Conversions::DMToString(lap);
        mModeContext.mGraphics2D.addMiscPanelText("Lap finished " + Ogre::String(lapS) + ": [" + Ogre::String(time) + "]", mModeContext.mGameState.getSTRPowerslide().getTrackTimeTrialColor(mModeContext.mGameState.getTrackName()));
    }

    //race finished
    if(lap == mModeContext.mGameState.getLapsCount())
    {
        mModeContext.mGameState.resetAfterFinishTimer();

        std::string totalTime = Tools::SecondsToString(mModeContext.mGameState.getPlayerCar().getLapUtils().getTotalTime());
        mModeContext.mGraphics2D.addMiscPanelText("Race finished: [" + Ogre::String(totalTime) + "]", mModeContext.mGameState.getSTRPowerslide().getTrackTimeTrialColor(mModeContext.mGameState.getTrackName()));

        mModeContext.mGameState.getPlayerCar().setDisableMouse(false);
        mCamera->setPosition(mModeContext.mGameState.getSTRPowerslide().getFinishCameraPos(mModeContext.mGameState.getTrackName()));
        mModeContext.mGraphics2D.setRearViewMirrorPanelShow(false);
        mModeContext.mGraphics2D.setVisibleFinishSign(true, mLapController.getTotalPosition(0));
        mModeContext.mGameState.setRaceFinished(true);
    }
}

void BaseRaceMode::preRenderTargetUpdate(const Ogre::RenderTargetEvent& evt)
{
    //http://www.ogre3d.org/forums/viewtopic.php?t=45499
    if(mCameraMan->getCameraPositionType() != CameraPosition_Bumper)
        mModeContext.mGameState.getPlayerCar().setVisibility(false);

    mModeContext.mGraphics2D.setVisibleTachoNeedle(false);
}

void BaseRaceMode::postRenderTargetUpdate(const Ogre::RenderTargetEvent& evt)
{
    if(mCameraMan->getCameraPositionType() != CameraPosition_Bumper)
        mModeContext.mGameState.getPlayerCar().setVisibility(true);

    mModeContext.mGraphics2D.setVisibleTachoNeedle(true);
}
