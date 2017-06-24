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

#include "../listeners/LoaderListener.h"

#include "../ui/UIRace.h"

#if defined(__ANDROID__)
    #include <android/log.h>

    #define LOGI(...) ((void)__android_log_write(ANDROID_LOG_INFO, "OGRE", __VA_ARGS__))
    #define LOGE(...) ((void)__android_log_write(ANDROID_LOG_ERROR, "OGRE", __VA_ARGS__)) 
#endif

namespace
{
    //http://bulletphysics.org/mediawiki-1.5.8/index.php/Code_Snippets
    void internalTickCallback(btDynamicsWorld *world, btScalar timeStep)
    {
        BaseRaceMode * pBaseRaceMode = reinterpret_cast<BaseRaceMode *>(world->getWorldUserInfo());
        if(pBaseRaceMode)
            pBaseRaceMode->processInternalTick(timeStep);
    }
}

BaseRaceMode::BaseRaceMode(const ModeContext& modeContext) :
    BaseMode(modeContext),
    mShadowLightDistanceFromCar(40.0f),
    mIsGlobalReset(true),
    mRearCamera(0),
    mUIRace(new UIRace()),
    mLoaderListener(NULL)
#if SHOW_DETAILS_PANEL
    ,mDetailsPanel(0)
#endif
{
}

void BaseRaceMode::initData(LoaderListener* loaderListener)
{
    Ogre::LogManager::getSingleton().logMessage(Ogre::LML_NORMAL, "[BaseRaceMode::initData]: Enter");

    mLuaManager.CallFunction_0_0("main", mModeContext.mPipeline);
    initScene(loaderListener);

    mLoaderListener = loaderListener;

    if(loaderListener)
        loaderListener->loadState(0.2f, "scene inited");

    loadResources(loaderListener);

    if(loaderListener)
        loaderListener->loadState(0.4f, "resources loaded");

    initTerrain(loaderListener);

    if(loaderListener)
        loaderListener->loadState(0.8f, "terrain loaded");

    initModel();

    if(loaderListener)
        loaderListener->loadState(0.9f, "models loaded");

    initMisc();

    if(loaderListener)
        loaderListener->loadState(0.95f, "misc loaded");

    initLightLists();

    if(loaderListener)
        loaderListener->loadState(1.0f, "all loaded");

#if SHOW_DETAILS_PANEL
    // create a params panel for displaying sample details
    Ogre::StringVector items;
    //items.push_back("cam.pX");
    //items.push_back("cam.pY");
    //items.push_back("cam.pZ");
    items.push_back("car.pX");
    items.push_back("car.pY");
    items.push_back("car.pZ");
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

    Ogre::LogManager::getSingleton().logMessage(Ogre::LML_NORMAL, "[BaseRaceMode::initData]: Exit");
}


void BaseRaceMode::initCamera()
{

    mCamera = mSceneMgr->createCamera("PlayerCam");
    mCamera->setAutoTracking(false);
    mCamera->setNearClipDistance(0.5f);
    Ogre::Viewport * mViewPort = mModeContext.mWindow->addViewport(mCamera);

    mViewPort->setBackgroundColour(mModeContext.mGameState.getBackgroundColor());
    mCamera->setAspectRatio(1.2f * Ogre::Real(mViewPort->getActualWidth()) / Ogre::Real(mViewPort->getActualHeight()) / (640.0f / 480.0f));
    //mCamera->setFOVy(Ogre::Degree(95.0f));
    mCamera->setFOVy(Ogre::Degree(80.0f));

    mCameraMan.reset(new CameraMan(mCamera, mWorld.get(), mSceneMgr));
    mModeContext.mInputHandler->resetCameraMenPointer(mCameraMan.get());

    mModeContext.mGameState.getPlayerCar().setCameraMan(mCameraMan.get());

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

    mModeContext.mTrayMgr->hideCursor();

    //init misc
    if(mModeContext.mGameState.getMirrorEnabled())
    {
        mRearCamera = mSceneMgr->createCamera("RearViewCamera");
        mRearCamera->setNearClipDistance(0.5f);
        Ogre::Viewport *v = mUIRace->rearViewMirrorPanelTextureAddViewport(mRearCamera);
        v->setOverlaysEnabled(false);
        v->setClearEveryFrame(true);
        v->setBackgroundColour(mModeContext.mGameState.getBackgroundColor());
        mRearCamera->setAspectRatio(1.0f);
        mRearCamera->setFOVy(Ogre::Degree(mLuaManager.ReadScalarFloat("Scene.Mirror.FOV", mModeContext.mPipeline)));
    }

    mUIRace->initTachoNeedle(mSceneMgrCarUI, mModeContext.mGameState);

    mCameraMan->setRearCamera(mRearCamera);
    //init misc end

    mUIRace->createRearViewMirrorPanel(mModeContext.mTrayMgr, mModeContext.mGameState.getMirrorEnabled());
    mUIRace->load(mModeContext.mTrayMgr, mModeContext.mGameState);

    mUIRace->setShowMiscTextRight(true);

    customInitUI();
}

void BaseRaceMode::clearData()
{
#if SHOW_DETAILS_PANEL
    mModeContext.mTrayMgr->destroyWidget(mDetailsPanel);
#endif

    customClearUI();

    clearScene();

    unloadResources();

    mModeContext.mInputHandler->resetCameraMenPointer(NULL);
    mCameraMan.reset();

    mUIRace->destroy(mModeContext.mTrayMgr);
}

void BaseRaceMode::restart()
{
    clearScene();
    mLuaManager.CallFunction_0_0("main", mModeContext.mPipeline);
    initScene(NULL);
    initTerrain(NULL);
    initModel();
    initMisc();

    initLightLists();
}

void BaseRaceMode::initScene(LoaderListener* loaderListener)
{
    Ogre::LogManager::getSingleton().logMessage(Ogre::LML_NORMAL, "[BaseRaceMode::initScene]: Enter");

    mShadowLightDistanceFromCar = mLuaManager.ReadScalarFloat("Model.ShadowLightDistance", mModeContext.mPipeline);

    mSceneMgr = mModeContext.mRoot->createSceneManager(Ogre::ST_GENERIC);

#ifndef NO_OPENAL
    mModeContext.mSoundsProcesser.initSounds(mModeContext.mGameState.getPFLoaderData());
#endif

    //migration from 1.8.1 to 1.9.0
    //http://www.ogre3d.org/forums/viewtopic.php?f=2&t=78278
    mSceneMgr->addRenderQueueListener(mModeContext.mOverlaySystem);

    mSceneMgr->setAmbientLight(Ogre::ColourValue(1.0, 1.0, 1.0));

    mMainNode = mSceneMgr->getRootSceneNode()->createChildSceneNode();

    initWorld(Ogre::Vector3(0.0f, -mLuaManager.ReadScalarFloat("Scene.Gravity", mModeContext.mPipeline), 0.0f));

    if(loaderListener)
        loaderListener->loadState(0.1f, "world inited");

    //load data
    {
        bool isDebugLLT = mLuaManager.ReadScalarBool("Terrain.Scene.IsDebugLLT", mModeContext.mPipeline);

        LLTLoader().load(mModeContext.mGameState, mSceneMgr, isDebugLLT);
    }

    if(loaderListener)
        loaderListener->loadState(0.12f, "LLT loaded");

    {
        bool isDebugExclusion = mLuaManager.ReadScalarBool("Terrain.Scene.IsDebugExclusion", mModeContext.mPipeline);
        ExclusionLoader().load(mModeContext.mGameState, mSceneMgr, isDebugExclusion);
    }

    if(loaderListener)
        loaderListener->loadState(0.13f, "Exclusions loaded");

    PHYLoader().load(mModeContext.mGameState);

    if(loaderListener)
        loaderListener->loadState(0.14f, "PHY loaded");

    {
        bool isDebugAI = mLuaManager.ReadScalarBool("Terrain.Scene.IsDebugAI", mModeContext.mPipeline);

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
            if(mModeContext.mGameState.getTrackName() == "mountain forest track") lateralStabilizationCoeff = 10.0f;
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
            if(mModeContext.mGameState.getTrackName() == "mountain forest track") lateralStabilizationCoeff = 5.0f;
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
            if(mModeContext.mGameState.getTrackName() == "mountain forest track") lateralStabilizationCoeff = 5.0f;
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
            if(mModeContext.mGameState.getTrackName() == "mountain forest track") lateralStabilizationCoeff = 20.0f;
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

        if(loaderListener)
            loaderListener->loadState(0.18f, "AI loaded");
    }

    //init multiplayer
    {
        customInitScene();
    }

    ParticlesLoader().load(mModeContext.mGameState);

    Ogre::LogManager::getSingleton().logMessage(Ogre::LML_NORMAL, "[BaseRaceMode::initScene]: Exit");
}


void BaseRaceMode::clearScene()
{
    mUIRace->clearMiscPanelText();
    mUIRace->setVisibleFinishSign(false);

    mModeContext.mGameState.resetGamePaused();
    mModeContext.mGameState.setRaceStarted(false);
    mModeContext.mGameState.setRaceFinished(false);

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

    deInitWorld();

#ifndef NO_OPENAL
    mModeContext.mSoundsProcesser.stopSounds();
#endif

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
    Ogre::LogManager::getSingleton().logMessage(Ogre::LML_NORMAL, "[BaseRaceMode::initLightLists]: Enter");

    //d.polubotko: make sure light lists created during loading
    mStaticMeshProcesser.queryLights();

    Ogre::LogManager::getSingleton().logMessage(Ogre::LML_NORMAL, "[BaseRaceMode::initLightLists]: Exit");
}

void BaseRaceMode::initTerrain(LoaderListener* loaderListener)
{
    Ogre::LogManager::getSingleton().logMessage(Ogre::LML_NORMAL, "[BaseRaceMode::initTerrain]: Enter");

    mStaticMeshProcesser.initParts( mModeContext.mPipeline, mSceneMgr, mMainNode, mIsGlobalReset, 
                                    mModeContext.mGameState, mWorld.get(),
                                    loaderListener);
    mStaticMeshProcesser.loadTerrainMaps(mModeContext.mGameState);

    //load terrain params
    TerrainLoader terrainLoader;
    terrainLoader.load(mModeContext.mGameState);

    mStaticMeshProcesser.setFrictionRemapArray(terrainLoader.getRemapFriction());
    mStaticMeshProcesser.setLatutuideFrictionArray(terrainLoader.getLatitudeFriction());
    mStaticMeshProcesser.setLongtitudeFrictionArray(terrainLoader.getLongtitudeFriction());

#ifndef NO_OPENAL
    mModeContext.mSoundsProcesser.setRemappers(terrainLoader.getRemapSounds(), terrainLoader.getRemapSoundsCollision());
#endif

    //particle (before loading cars)
    {
        //Ogre::TexturePtr particle = TextureLoader().load(mModeContext.mGameState.getPFLoaderData(), "data/deii", "particle.tga", "OriginalParticle");
        Ogre::TexturePtr particle = TEXLoader().load(mModeContext.mGameState.getPFLoaderData(), "data/deii", "d3d2display1_m_1.tex", "OriginalParticle");
        Ogre::MaterialPtr particleMat = Ogre::MaterialManager::getSingleton().getByName("Test/Particle");
        particleMat->getTechnique(0)->getPass(0)->getTextureUnitState(0)->setTexture(particle);

        Ogre::MaterialPtr particleMatFog = Ogre::MaterialManager::getSingleton().getByName("Test/ParticleFog");
        particleMatFog->getTechnique(0)->getPass(0)->getTextureUnitState(0)->setTexture(particle);
    }

    Ogre::LogManager::getSingleton().logMessage(Ogre::LML_NORMAL, "[BaseRaceMode::initTerrain]: Exit");
}

void BaseRaceMode::initModel()
{

    Ogre::LogManager::getSingleton().logMessage(Ogre::LML_NORMAL, "[BaseRaceMode::initModel]: Enter");

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

    mModeContext.mGameState.getPlayerCar().initModel(mModeContext.mPipeline, mModeContext.mGameState, mSceneMgr, mMainNode, &mModelsPool, mWorld.get(), mModeContext.mGameState.getPlayerCar().getCharacterName(), mModeContext.mGameState.getTrackPositions()[mModeContext.mGameState.getAICount()], !isCamToAI);
    mModeContext.mGameState.getPlayerCar().initSounds(mModeContext.mPipeline, mModeContext.mGameState);

    std::vector<std::string> aiCharacters = mModeContext.getGameState().getAICharacters();

    for(size_t q = 0; q < mModeContext.mGameState.getAICount(); ++q)
    {
        PSAICar& aiCar = mModeContext.mGameState.getAICar(q);

        bool isCam = (q == (mModeContext.mGameState.getAICount() - 1));
        if(!isCamToAI)
            isCam = false;
        aiCar.initModel(mModeContext.mPipeline, mModeContext.mGameState, mSceneMgr, mMainNode, &mModelsPool, mWorld.get(), aiCharacters[q], mModeContext.mGameState.getTrackPositions()[q], isCam);
        aiCar.initSounds(mModeContext.mPipeline, mModeContext.mGameState);

        mLapController.addCar(&aiCar);

        aiCar.getLapUtils().setEvents(NULL);
    }

    Ogre::LogManager::getSingleton().logMessage(Ogre::LML_NORMAL, "[BaseRaceMode::initModel]: Exit");

}

void BaseRaceMode::initMisc()
{

    Ogre::LogManager::getSingleton().logMessage(Ogre::LML_NORMAL, "[BaseRaceMode::initMisc]: Enter");

    //rear mirror camera
    mRearCamera = NULL;

    if(mModeContext.mGameState.getMirrorEnabled())
    {

        //rear mirror texture
        //http://www.ogre3d.org/tikiwiki/tiki-index.php?page=Basic+Tutorial+7
        if(mIsGlobalReset)
        {
            int mirrorSizeW = mLuaManager.ReadScalarInt("Scene.Mirror.RTTSize.w", mModeContext.mPipeline);
            int mirrorSizeH = mLuaManager.ReadScalarInt("Scene.Mirror.RTTSize.h", mModeContext.mPipeline);

            //add this as listener
            mUIRace->createRearViewMirrorPanelTexture(this, mModeContext.mRoot, mirrorSizeW, mirrorSizeH);
            mUIRace->setRearViewMirrorPanelMaterial("Test/RearViewMirror");
        }
        else
        {
            mUIRace->rearViewMirrorPanelTextureRemoveAllViewports();
        }


        mUIRace->setRearViewMirrorPanelShow(true);
    }
    else
    {
        mUIRace->setRearViewMirrorPanelShow(false);
    }

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

    Ogre::LogManager::getSingleton().logMessage(Ogre::LML_NORMAL, "[BaseRaceMode::initMisc]: Exit");
}



void BaseRaceMode::initWorld(const Ogre::Vector3 &gravityVector, const Ogre::AxisAlignedBox &bounds)
{
    Ogre::LogManager::getSingleton().logMessage(Ogre::LML_NORMAL, "[BaseRaceMode::initWorld]: Enter");

    mWorld.reset(new OgreBulletDynamics::DynamicsWorld(mSceneMgr, bounds, gravityVector, true, true, 10000));
    mDebugDrawer.reset(new OgreBulletCollisions::DebugDrawer());
    mWorld->setDebugDrawer(mDebugDrawer.get());
    mWorld->getBulletDynamicsWorld()->setInternalTickCallback(internalTickCallback, this);

    Ogre::SceneNode *node = mSceneMgr->getRootSceneNode()->createChildSceneNode("debugDrawer", Ogre::Vector3::ZERO);
    node->attachObject(static_cast<Ogre::SimpleRenderable *>(mDebugDrawer.get()));

    if(mLuaManager.ReadScalarBool("Terrain.Scene.IsDebugPhysics", mModeContext.mPipeline))
    {
        mDebugDrawer->setDrawWireframe(true);
        mWorld->setShowDebugShapes(true);

        mDebugDrawer->setDrawContactPoints(true);
        mWorld->setShowDebugContactPoints(true);
    }

    Ogre::LogManager::getSingleton().logMessage(Ogre::LML_NORMAL, "[BaseRaceMode::initWorld]: Exit");
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

    Ogre::LogManager::getSingleton().logMessage(Ogre::LML_TRIVIAL, "[BaseRaceMode::frameStarted]: Enter");

    Ogre::Vector3 playerPos = mModeContext.mGameState.getPlayerCar().getModelNode()->getPosition();

    Ogre::Vector3 playerDir = mModeContext.mGameState.getPlayerCar().getForwardAxis();

#ifndef NO_OPENAL
    mModeContext.mSoundsProcesser.setListenerPos(playerPos);
    if(!mModeContext.mGameState.isGamePaused())
        mModeContext.mSoundsProcesser.setListenerGain(mModeContext.mGameState.getListenerGain());
    else
        mModeContext.mSoundsProcesser.setListenerGain(0.0f);
#endif

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

    //http://bulletphysics.org/mediawiki-1.5.8/index.php/Stepping_The_World
    if(mModeContext.mGameState.getRaceStarted() && !mModeContext.mGameState.isGamePaused())
        mWorld->stepSimulation(evt.timeSinceLastFrame, 7);

    mModeContext.mGameState.getPlayerCar().processFrameAfterPhysics(evt, mModeContext.mGameState.getRaceStarted());

    for(size_t q = 0; q < mModeContext.mGameState.getAICount(); ++q)
        mModeContext.mGameState.getAICar(q).processFrameAfterPhysics(evt, mModeContext.mGameState.getRaceStarted());

    customFrameStartedDoProcessFrameAfterPhysics(evt);

    Ogre::LogManager::getSingleton().logMessage(Ogre::LML_TRIVIAL, "[BaseRaceMode::frameStarted]: Exit");
}

void BaseRaceMode::frameRenderingQueued(const Ogre::FrameEvent& evt)
{

    Ogre::LogManager::getSingleton().logMessage(Ogre::LML_TRIVIAL, "[BaseRaceMode::frameRenderingQueued]: Enter");

    mUIRace->setMiscTextRight(Conversions::DMToString(static_cast<size_t>(mModeContext.mWindow->getAverageFPS())));

    customFrameRenderingQueuedDoBegining();

    if( !mModeContext.mGameState.getRaceStarted() && 
        mModeContext.mGameState.getBeforeStartTimerTime() > mModeContext.mGameState.getSTRRacecrud().getFloatValue("on-grid parameters", "ready left start") * 1000.0f && 
        !mModeContext.mGameState.isGamePaused())
    {
        mUIRace->setRearViewMirrorPanelShow(false);
        mUIRace->hideAllStart();
        mUIRace->showBeforeStart1();
#ifndef NO_OPENAL
        mModeContext.mSoundsProcesser.playBeforeStart1();
#endif
    }

    if(!mModeContext.mGameState.getRaceStarted() && 
        mModeContext.mGameState.getBeforeStartTimerTime() > mModeContext.mGameState.getSTRRacecrud().getFloatValue("on-grid parameters", "set left start") * 1000.0f && 
        !mModeContext.mGameState.isGamePaused())
    {
        mUIRace->hideAllStart();
        mUIRace->showBeforeStart2();
#ifndef NO_OPENAL
        mModeContext.mSoundsProcesser.playBeforeStart2();
#endif
    }

    if(!mModeContext.mGameState.getRaceStarted() && 
        mModeContext.mGameState.getBeforeStartTimerTime() > mModeContext.mGameState.getSTRRacecrud().getFloatValue("on-grid parameters", "go left start") * 1000.0f && 
        !mModeContext.mGameState.isGamePaused())
    {
        mUIRace->hideAllStart();
        mUIRace->showBeforeStart3();
#ifndef NO_OPENAL
        mModeContext.mSoundsProcesser.playBeforeStart3();
#endif

        for(size_t q = 0; q < mModeContext.mGameState.getAICount(); ++q)
        {
            mModeContext.mGameState.getAICar(q).raceStarted();
        }

        customFrameRenderingQueuedDoRaceStarted();

        //reset lap timer before race started (for every human player include self, and ai)
        mModeContext.mGameState.getPlayerCar().getLapUtils().resetLapTimer();
        for(int q = 0; q < mModeContext.mGameState.getMaxMultiplayerHumans(); ++q)
        {
            mModeContext.mGameState.getMultiplayerCarHuman(q).getLapUtils().resetLapTimer();
        }
        for(size_t q = 0; q < mModeContext.mGameState.getAICount(); ++q)
        {
            mModeContext.mGameState.getAICar(q).getLapUtils().resetLapTimer();
        }

        mModeContext.mGameState.setRaceStarted(true);
    }

    if(mModeContext.mGameState.getBeforeStartTimerTime() > (mModeContext.mGameState.getSTRRacecrud().getFloatValue("on-grid parameters", "go left start") * 1000.0f + mModeContext.mGameState.getSTRRacecrud().getFloatValue("on-grid parameters", "go left length") * 1000.0f) && !mModeContext.mGameState.isGamePaused())
    {
        mUIRace->hideAllStart();

        if(mModeContext.mGameState.getMirrorEnabled() && !mModeContext.mGameState.getRaceFinished())
        {
            mUIRace->setRearViewMirrorPanelShow(true);
        }
    }

    mModeContext.mInputHandler->capture(evt, mModeContext.mGameState.getPlayerCar().getModelNode(), mModeContext.mGameState.getGlobalLight(), mModeContext.mGameState.getShadowLight(), mShadowLightDistanceFromCar, true);

    mModeContext.mTrayMgr->frameRenderingQueued(evt);

    mUIRace->setShowPausedPanel(mModeContext.mGameState.isGamePaused());

    if (!mModeContext.mTrayMgr->isDialogVisible())
    {
        if(!mModeContext.mGameState.isGamePaused())
            mUIRace->setEngineRPM(mModeContext.mGameState.getPlayerCar().getCarEngine().getEngineRPM());
        mUIRace->setCarSpeed(mModeContext.mGameState.getPlayerCar().getAlignedVelocity());
        mUIRace->setCurrentLap(static_cast<unsigned short>(mModeContext.mGameState.getPlayerCar().getLapUtils().getCurrentLap()), static_cast<unsigned short>(mModeContext.mGameState.getLapsCount()));
        mUIRace->setCarGear(static_cast<unsigned char>(mModeContext.mGameState.getPlayerCar().getCarEngine().getCurrentGear()));
        mUIRace->setCarPos(static_cast<unsigned char>(mLapController.getTotalPosition(0)), static_cast<unsigned char>(mLapController.getTotalCars()));

        mUIRace->hideAIDashboardCars();
        size_t currentPlayerLap = mModeContext.mGameState.getPlayerCar().getLapUtils().getCurrentLap();
        Ogre::Real currentPlayerLapPos = mModeContext.mGameState.getPlayerCar().getLapUtils().getLapPosition();
        mUIRace->setPlayerDashBoardSkin(mModeContext.mGameState);
        for(size_t q = 0; q < mModeContext.mGameState.getAICount(); ++q)
        {
            mUIRace->setAIDashBoardSkin(mModeContext.mGameState, q, mModeContext.mGameState.getAICar(q).getCharacterName());
            mUIRace->setDashCarPos(q, currentPlayerLap, currentPlayerLapPos, mModeContext.mGameState.getAICar(q).getLapUtils().getCurrentLap(), mModeContext.mGameState.getAICar(q).getLapUtils().getLapPosition());
        }

        customFrameRenderingQueuedDo2DUI();

        if(mModeContext.mGameState.getRaceStarted())
        {
            if(mModeContext.mGameState.getPlayerCar().getLapUtils().getAfterFinishLinePassTime() < 3.0f && mModeContext.mGameState.getPlayerCar().getLapUtils().getCurrentLap() > 1)
            {
                mUIRace->setRaceTime(Tools::SecondsToString(mModeContext.mGameState.getPlayerCar().getLapUtils().getLastLapTime()));
            }
            else
            {
                mUIRace->setRaceTime(Tools::SecondsToString(mModeContext.mGameState.getPlayerCar().getLapUtils().getLapTime()));
            }
        }
        else
        {
            mUIRace->setRaceTime("00:00:00");
        }


#if SHOW_DETAILS_PANEL
        //mDetailsPanel->setParamValue(0, Ogre::StringConverter::toString(mCamera->getDerivedPosition().x));
        //mDetailsPanel->setParamValue(1, Ogre::StringConverter::toString(mCamera->getDerivedPosition().y));
        //mDetailsPanel->setParamValue(2, Ogre::StringConverter::toString(mCamera->getDerivedPosition().z));
        mDetailsPanel->setParamValue(0, Ogre::StringConverter::toString(mModeContext.mGameState.getPlayerCar().getModelNode()->getPosition().x));
        mDetailsPanel->setParamValue(1, Ogre::StringConverter::toString(mModeContext.mGameState.getPlayerCar().getModelNode()->getPosition().y));
        mDetailsPanel->setParamValue(2, Ogre::StringConverter::toString(mModeContext.mGameState.getPlayerCar().getModelNode()->getPosition().z));
        
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

    Ogre::LogManager::getSingleton().logMessage(Ogre::LML_TRIVIAL, "[BaseRaceMode::frameRenderingQueued]: Exit");
}


#if defined(__ANDROID__)
void BaseRaceMode::reloadTextures()
{
    mUIRace->reloadTextures(mModeContext.mGameState);
}
#endif

void BaseRaceMode::processCollision(btManifoldPoint& cp, const btCollisionObjectWrapper* colObj0Wrap, const btCollisionObjectWrapper* colObj1Wrap, int triIndex)
{
    mModeContext.mGameState.getPlayerCar().processWheelsCollision(cp, colObj0Wrap, colObj1Wrap, mStaticMeshProcesser, triIndex);
    mModeContext.mGameState.getPlayerCar().processChassisCollision(cp, colObj0Wrap, colObj1Wrap, mStaticMeshProcesser, triIndex);

    for(size_t q = 0; q < mModeContext.mGameState.getAICount(); ++q)
    {
        mModeContext.mGameState.getAICar(q).processWheelsCollision(cp, colObj0Wrap, colObj1Wrap, mStaticMeshProcesser, triIndex);
    }

    customProcessCollision(cp, colObj0Wrap, colObj1Wrap, triIndex);

#ifndef NO_OPENAL
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
#endif
}

void BaseRaceMode::processInternalTick(float timeStep)
{
    mModeContext.mGameState.getPlayerCar().processInternalTick(timeStep, mModeContext.mGameState.getRaceStarted());

    for(size_t q = 0; q < mModeContext.mGameState.getAICount(); ++q)
    {
        mModeContext.mGameState.getAICar(q).processInternalTick(timeStep, mModeContext.mGameState.getRaceStarted());
    }
}

void BaseRaceMode::loadResources(LoaderListener* loaderListener)
{
    Ogre::LogManager::getSingleton().logMessage(Ogre::LML_NORMAL, "[BaseRaceMode::loadResources]: Enter");

#if defined(__ANDROID__)
    LOGI("BaseApp[loadResources]: Begin"); 
#endif

    Ogre::ResourceGroupManager::getSingleton().createResourceGroup(TEMP_RESOURCE_GROUP_NAME);

    //http://ogre3d.org/forums/viewtopic.php?f=2&t=54469
    //mModeContext.mTrayMgr->showLoadingBar(2, 2);
    //Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups();

    Ogre::ResourceGroupManager::getSingleton().addResourceGroupListener(this);

    Ogre::ResourceGroupManager::getSingleton().initialiseResourceGroup("Popular");
    Ogre::ResourceGroupManager::getSingleton().loadResourceGroup("Popular");

    Ogre::ResourceGroupManager::getSingleton().removeResourceGroupListener(this);


    //mModeContext.mTrayMgr->hideLoadingBar();

#if defined(__ANDROID__)
    LOGI("BaseApp[loadResources]: End"); 
#endif

    Ogre::LogManager::getSingleton().logMessage(Ogre::LML_NORMAL, "[BaseRaceMode::loadResources]: Exit");
}

void BaseRaceMode::resourceGroupScriptingStarted(const Ogre::String& groupName, size_t scriptCount)
{
    mResourceCount = scriptCount;
    mResourceCurrent = 0;
}

void BaseRaceMode::scriptParseEnded(const Ogre::String& scriptName, bool skipped)
{
    //ranges from BaseRaceMode::initData
    const float loaderMin = 0.2f;
    const float loaderMax = 0.3f;
    const float loaderDistance = loaderMax - loaderMin;

    ++mResourceCurrent;

    if(mLoaderListener)
        mLoaderListener->loadState(loaderMin + loaderDistance * static_cast<float>(mResourceCurrent) / static_cast<float>(mResourceCount), scriptName);
}

void BaseRaceMode::resourceGroupLoadStarted(const Ogre::String& groupName, size_t resourceCount)
{
    mResourceCount = resourceCount;
    mResourceCurrent = 0;
}

void BaseRaceMode::resourceLoadEnded()
{
    //ranges from BaseRaceMode::initData
    const float loaderMin = 0.3f;
    const float loaderMax = 0.4f;
    const float loaderDistance = loaderMax - loaderMin;

    ++mResourceCurrent;

    if(mLoaderListener)
        mLoaderListener->loadState(loaderMin + loaderDistance * static_cast<float>(mResourceCurrent) / static_cast<float>(mResourceCount), "resources loading");
}

void BaseRaceMode::unloadResources()
{

    mUIRace->setRearViewMirrorPanelMaterial("BaseWhite");
    mUIRace->setRearViewMirrorPanelShow(false);

    //deinit sounds of system

    //clear sounds of cars as well
    mModeContext.mGameState.getPlayerCar().deinitSounds();
    for(size_t q = 0; q < mModeContext.mGameState.getAICount(); ++q)
        mModeContext.mGameState.getAICar(q).deinitSounds();

    customUnloadResources();

#ifndef NO_OPENAL
    mModeContext.mSoundsProcesser.stopSounds();
#endif

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
        mUIRace->addMiscPanelText("Lap finished " + Ogre::String(lapS) + ": [" + Ogre::String(time) + "]", mModeContext.mGameState.getSTRPowerslide().getTrackTimeTrialColor(mModeContext.mGameState.getTrackName()));
    }

    //race finished
    if(lap == mModeContext.mGameState.getLapsCount())
    {
        mModeContext.mGameState.resetAfterFinishTimer();

        std::string totalTime = Tools::SecondsToString(mModeContext.mGameState.getPlayerCar().getLapUtils().getTotalTime());
        mUIRace->addMiscPanelText("Race finished: [" + Ogre::String(totalTime) + "]", mModeContext.mGameState.getSTRPowerslide().getTrackTimeTrialColor(mModeContext.mGameState.getTrackName()));

        mModeContext.mGameState.getPlayerCar().setDisableMouse(false);
        mCamera->setPosition(mModeContext.mGameState.getSTRPowerslide().getFinishCameraPos(mModeContext.mGameState.getTrackName()));
        mUIRace->setRearViewMirrorPanelShow(false);
        mUIRace->setVisibleFinishSign(true, mLapController.getTotalPosition(0));
        mModeContext.mGameState.setRaceFinished(true);
    }
}

void BaseRaceMode::preRenderTargetUpdate(const Ogre::RenderTargetEvent& evt)
{
    //http://www.ogre3d.org/forums/viewtopic.php?t=45499
    if(mCameraMan->getCameraPositionType() != CameraPosition_Bumper)
        mModeContext.mGameState.getPlayerCar().setVisibility(false);

    mUIRace->setVisibleTachoNeedle(false);
}

void BaseRaceMode::postRenderTargetUpdate(const Ogre::RenderTargetEvent& evt)
{
    if(mCameraMan->getCameraPositionType() != CameraPosition_Bumper)
        mModeContext.mGameState.getPlayerCar().setVisibility(true);

    mUIRace->setVisibleTachoNeedle(true);
}
