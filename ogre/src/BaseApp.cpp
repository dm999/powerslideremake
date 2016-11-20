#include "pcheader.h"

#include "BaseApp.h"

#include "InputHandler.h"
#include "tools/OgreTools.h"
#include "tools/Conversions.h"

#include "lua/Lua_Include.h"

#include "customs/CustomOverlaySystem.h"

#include "CameraMan.h"

#include "tools/Randomizer.h"

#include "loaders/ExclusionLoader.h"
#include "loaders/LLTLoader.h"
#include "loaders/PHYLoader.h"
#include "loaders/AILoader.h"
#include "loaders/TerrainLoader.h"
#include "loaders/ParticlesLoader.h"
#include "loaders/TEXLoader.h"
#include "loaders/TextureLoader.h"

#include "gamelogic/RacingGridGeneration.h"

#include "BulletCollision/CollisionDispatch/btInternalEdgeUtility.h"

#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
#include "res/resource.h"
#endif

#define SHOW_DETAILS_PANEL 0

#if defined(__ANDROID__)
    #include <EGL/egl.h>
    #include <android/api-level.h>
    #include <android/native_window_jni.h>
    #include <android/asset_manager.h>
    #include <android/asset_manager_jni.h>
    #include <android/log.h>

    #define LOGI(...) ((void)__android_log_write(ANDROID_LOG_INFO, "OGRE", __VA_ARGS__))
    #define LOGE(...) ((void)__android_log_write(ANDROID_LOG_ERROR, "OGRE", __VA_ARGS__)) 

    #include "Android/OgreAndroidEGLWindow.h"
    #include "Android/OgreAPKFileSystemArchive.h"
    #include "Android/OgreAPKZipArchive.h"
#endif


namespace
{
    lua_State * mPipeline = NULL;
    BaseApp * baseApp = NULL;
}

extern ContactAddedCallback gContactAddedCallback;



inline static void ClampInPlace(btScalar& value,btScalar minValue,btScalar maxValue)	{
	if (value < minValue) value = minValue;
	else if (value > maxValue) value = maxValue;	
}
inline static btScalar CalculateCombinedFriction(btScalar friction0,btScalar friction1)	{
	btScalar friction = friction0 * friction1;	// Default behavior AFAIK
						//(friction0 + friction1)*0.5f;
	const btScalar MAX_FRICTION  = 10.f;	// I don't know what happens if friction>1, to be honest...
	ClampInPlace(friction,0,MAX_FRICTION);	// Does negative friction have any sense ?
	return friction;
}
inline static btScalar CalculateCombinedRestitution(btScalar restitution0,btScalar restitution1)	{
	btScalar restitution = restitution0 * restitution1;	// Default behavior AFAIK
						//(restitution0 + restitution1)*0.5f;
	const btScalar MAX_RESTITUTION  = 2.f;		// I don't know what happens if restitution>1, to be honest...
	ClampInPlace(restitution,0,MAX_RESTITUTION);// Does negative restitution have any sense ?
	return restitution;
}

//http://bulletphysics.org/Bullet/phpBB3/viewtopic.php?t=6662
//https://github.com/kripken/ammo.js/blob/master/bullet/Demos/InternalEdgeDemo/InternalEdgeDemo.cpp
//http://bulletphysics.org/Bullet/phpBB3/viewtopic.php?f=9&t=8082
//http://bulletphysics.org/Bullet/phpBB3/viewtopic.php?f=9&t=8146
static bool CustomMaterialCombinerCallback(btManifoldPoint& cp,	const btCollisionObjectWrapper* colObj0Wrap,int partId0,int index0,const btCollisionObjectWrapper* colObj1Wrap,int partId1,int index1)
{
   btAdjustInternalEdgeContacts(cp,colObj1Wrap,colObj0Wrap, partId1,index1);

    if (    colObj0Wrap->getCollisionObject()->getCollisionFlags() & btCollisionObject::CF_CUSTOM_MATERIAL_CALLBACK ||
            colObj1Wrap->getCollisionObject()->getCollisionFlags() & btCollisionObject::CF_CUSTOM_MATERIAL_CALLBACK
    )
    {
        cp.m_combinedFriction = CalculateCombinedFriction(colObj0Wrap->getCollisionObject()->getFriction(),colObj1Wrap->getCollisionObject()->getFriction());
        cp.m_combinedRestitution = CalculateCombinedRestitution(colObj0Wrap->getCollisionObject()->getRestitution(),colObj1Wrap->getCollisionObject()->getRestitution());
        cp.m_combinedRollingFriction = CalculateCombinedFriction(colObj0Wrap->getCollisionObject()->getRollingFriction(),colObj1Wrap->getCollisionObject()->getRollingFriction());
    }

    if(baseApp != NULL)
    {
        baseApp->processCollision(cp, colObj0Wrap, colObj1Wrap, index1);
    }
   return true;
}

BaseApp::BaseApp(void) :
    mCamera(0),
    mRearCamera(0),
    mSceneMgr(0),
    mWindow(0),
    mResourcesCfg(Ogre::StringUtil::BLANK),
    mTrayMgr(0),
    mDetailsPanel(0),
    mCursorWasVisible(false),
    mShutDown(false),
    mIsGlobalReset(true),
    mIsDisableMouse(true),
    mLuaError(0),
    mSMFactory(new CustomSceneManagerFactory())
{
#if defined(__ANDROID__)
    LOGI("BaseApp[BaseApp]: Begin"); 
#endif
    gContactAddedCallback = CustomMaterialCombinerCallback;
    baseApp = this;
    initLua();
#if defined(__ANDROID__)
    LOGI("BaseApp[BaseApp]: End"); 
#endif
}

BaseApp::~BaseApp(void)
{
    //to execute onQuit
    if(mMultiplayerController.get())
        mMultiplayerController->clearSessionAndLobby();
    mMultiplayerController.reset();

    deInitWorld();

    unloadResources();

    baseApp = NULL;
    deInitLua();

    mGraphics2D.destroy();

    if (mTrayMgr) delete mTrayMgr;
    mInputHandler->resetCameraMenPointer(NULL);
    mCameraMan.reset();

    //Remove ourself as a Window listener
    Ogre::WindowEventUtilities::removeWindowEventListener(mWindow, this);
    windowClosed(mWindow);

    mRoot->removeSceneManagerFactory(mSMFactory.get());
    mOverlaySystem.reset();
    mRoot.reset();

    mSoundsProcesser.deinitSounds();
    mSoundsProcesser.deInitSoundSystem();
}

bool BaseApp::configure(void)
{
#if defined(__ANDROID__)
    Ogre::Root::getSingleton().installPlugin(new Ogre::GLES2Plugin());
#else
    Ogre::Root::getSingleton().installPlugin(new Ogre::GLPlugin());
#endif
    Ogre::Root::getSingleton().installPlugin(new Ogre::OctreePlugin());
    Ogre::Root::getSingleton().installPlugin(new Ogre::ParticleFXPlugin());

    const Ogre::RenderSystemList& rsList = mRoot->getAvailableRenderers();
    Ogre::RenderSystem *selectedRenderSystem = rsList.at(0);

    mRoot->setRenderSystem(selectedRenderSystem);
    selectedRenderSystem->setConfigOption("Full Screen", mLuaManager.ReadScalarString("Window.IsFullScreen", mPipeline));
    selectedRenderSystem->setConfigOption("FSAA", mLuaManager.ReadScalarString("Window.FSAA", mPipeline));
    selectedRenderSystem->setConfigOption("Colour Depth", "32");
    selectedRenderSystem->setConfigOption("RTT Preferred Mode", "FBO");
    selectedRenderSystem->setConfigOption("Video Mode", mLuaManager.ReadScalarString("Window.Resolution", mPipeline));
    selectedRenderSystem->setConfigOption("VSync", mLuaManager.ReadScalarString("Window.VSync", mPipeline));
    selectedRenderSystem->setConfigOption("VSync Interval", mLuaManager.ReadScalarString("Window.VSyncInterval", mPipeline));



    // Show the configuration dialog and initialise the system
    // You can skip this and use root.restoreConfig() to load configuration
    // settings if you were sure there are valid ones saved in ogre.cfg
    //if(mRoot->showConfigDialog())
    {
        // If returned true, user clicked OK so initialise
        // Here we choose to let the system create a default rendering window by passing 'true'
        mWindow = mRoot->initialise(true, "Powerslide HD");

#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
      HWND hwnd;
      mWindow->getCustomAttribute("WINDOW", (void*)&hwnd);
      LONG iconID   = (LONG)LoadIcon( GetModuleHandle(0), MAKEINTRESOURCE(IDI_MAIN_ICON) );
      SetClassLong( hwnd, GCL_HICON, iconID );
#endif

        mInputHandler.reset(new InputHandler(mWindow, this));

        mRoot->addSceneManagerFactory(mSMFactory.get());

        return true;
    }
	/*
    else
    {
        return false;
    }*/
}

void BaseApp::createFrameListener(void)
{

    //Set initial mouse clipping size
    windowResized(mWindow);

    //Register as a Window listener
#if !defined(__ANDROID__)
    Ogre::WindowEventUtilities::addWindowEventListener(mWindow, this);
#endif


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

    mDetailsPanel = mTrayMgr->createParamsPanel(OgreBites::TL_TOPLEFT, "DetailsPanel", 200, items);
    //mDetailsPanel->hide();
#endif

    Ogre::OverlayManager& om = Ogre::OverlayManager::getSingleton(); 
    Ogre::Real viewportWidth = om.getViewportWidth(); 
    Ogre::Real viewportHeight = om.getViewportHeight(); 


    mGraphics2D.createRearViewMirrorPanel(mTrayMgr, mGameState.getMirrorEnabled());
    mGraphics2D.load(mTrayMgr, mGameState);



    mRoot->addFrameListener(this);
}

void BaseApp::setupResources(void)
{
    // Load resource paths from config file
    Ogre::ConfigFile cf;
#if defined(__ANDROID__)
    cf.load(openAPKFile("resources.cfg"));
#else
    std::string resourcePath = "./";
    cf.load(resourcePath + "resources.cfg");
#endif

    // Go through all sections & settings in the file
    Ogre::ConfigFile::SectionIterator seci = cf.getSectionIterator();

    Ogre::String secName, typeName, archName;
    while (seci.hasMoreElements())
    {
        secName = seci.peekNextKey();
        Ogre::ConfigFile::SettingsMultiMap *settings = seci.getNext();
        Ogre::ConfigFile::SettingsMultiMap::iterator i;
        for (i = settings->begin(); i != settings->end(); ++i)
        {
            typeName = i->first;
            archName = i->second;

            Ogre::ResourceGroupManager::getSingleton().addResourceLocation(archName, typeName, secName);
        }
    }

    doLuaMainFile();
}

void BaseApp::loadResources(void)
{
#if defined(__ANDROID__)
    LOGI("BaseApp[loadResources]: Begin"); 
#endif

    Ogre::ResourceGroupManager::getSingleton().createResourceGroup(TEMP_RESOURCE_GROUP_NAME);

    //http://ogre3d.org/forums/viewtopic.php?f=2&t=54469
    mTrayMgr->showLoadingBar(2, 2);
    //Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups();
    Ogre::ResourceGroupManager::getSingleton().initialiseResourceGroup("Popular");
    Ogre::ResourceGroupManager::getSingleton().loadResourceGroup("Popular");

    mTrayMgr->hideLoadingBar();

#if defined(__ANDROID__)
    LOGI("BaseApp[loadResources]: End"); 
#endif
}

void BaseApp::unloadResources(void)
{

    mGraphics2D.setRearViewMirrorPanelMaterial("BaseWhite");
    mGraphics2D.setRearViewMirrorPanelShow(false);

    //deinit sounds of system

    //clear sounds of cars as well
    mGameState.getPlayerCar().deinitSounds();
    for(size_t q = 0; q < mGameState.getAICount(); ++q)
        mGameState.getAICar(q).deinitSounds();

    if(mGameState.isMultiplayerEnabled())
    {
        //AI
        for(size_t q = 0; q < mGameState.getMultiplayerCountAI(); ++q)
            mGameState.getMultiplayerCarAI(q).deinitSounds();

        //human
        std::vector<std::string> playerNames = mGameState.getMultiplayerCarHumanNames();
        for(size_t q = 0; q < playerNames.size(); ++q)
        {
            mGameState.getMultiplayerCarHuman(playerNames[q]).deinitSounds();
        }
    }

    mSoundsProcesser.stopSounds();

    Ogre::ParticleSystemManager::getSingleton().removeAllTemplates();

    Ogre::ResourceGroupManager::getSingleton().clearResourceGroup("Popular");
    Ogre::ResourceGroupManager::getSingleton().destroyResourceGroup(TEMP_RESOURCE_GROUP_NAME);

    //Ogre::MaterialManager::getSingleton().removeUnreferencedResources();
    //Ogre::MaterialManager::getSingleton().initialise();
}

void BaseApp::go(void)
{
    if (!setup())
        return;

    mRoot->startRendering();

    // clean up
    //destroyScene();
}

bool BaseApp::setup(void)
{
    mSoundsProcesser.initSoundSystem();

    mRoot.reset(new Ogre::Root());
    mOverlaySystem.reset(new CustomOverlaySystem());

    //read config, parse main lua
    setupResources();

    if (mLuaError )
    {
        return false;
    }


    //setup renderer, create window, init input
    bool carryOn = configure();
    if (!carryOn) return false;

    registerLuaFunctions();

    //chooseSceneManager();
    //createCamera();
    //createViewports();

    // Set default mipmap level (NB some APIs ignore this)
    //Ogre::TextureManager::getSingleton().setDefaultNumMipmaps(5);

    Ogre::ResourceGroupManager::getSingleton().initialiseResourceGroup("General");

    mTrayMgr = new CustomTrayManager("InterfaceName", mWindow, mInputHandler->getInputContext(), this);
    mTrayMgr->showFrameStats(OgreBites::TL_TOPRIGHT);
    //mTrayMgr->showLogo(OgreBites::TL_BOTTOMRIGHT);
    mTrayMgr->hideCursor();
    //mTrayMgr->toggleAdvancedFrameStats();

    //create scene
    mLuaManager.CallFunction_0_0("main", mPipeline);

    // Create any resource listeners (for loading screens)
    //createResourceListener();
    // Load resources
    loadResources();

    mLuaManager.CallFunction_0_0("create", mPipeline);

    initLightLists();

    // Create the scene
    //createScene();
    

    

    createFrameListener();

    return true;
}

bool BaseApp::frameStarted(const Ogre::FrameEvent &evt)
{
    if(mWindow->isClosed())
        return false;

    if(mShutDown)
        return false;

    Ogre::Vector3 playerPos = mGameState.getPlayerCar().getModelNode()->getPosition();

    Ogre::Vector3 playerDir = mGameState.getPlayerCar().getForwardAxis();

    mSoundsProcesser.setListenerPos(playerPos);
    if(!mGameState.isGamePaused())
        mSoundsProcesser.setListenerGain(mGameState.getListenerGain());
    else
        mSoundsProcesser.setListenerGain(0.0f);

    if(!mGameState.getRaceFinished())
        mGameState.getPlayerCar().getLapUtils().checkCheckPoints(playerPos);

    mGameState.getArrowNode()->setOrientation(mGameState.getPlayerCar().getLapUtils().getArrowOrientation(playerPos, playerDir));

    if(!mGameState.isGamePaused())
    {
        mGameState.getPlayerCar().processFrameBeforePhysics(evt, mStaticMeshProcesser, mGameState.getRaceStarted());

        for(size_t q = 0; q < mGameState.getAICount(); ++q)
        {
            if(!mGameState.getRaceFinished())
                mGameState.getAICar(q).getLapUtils().checkCheckPoints(mGameState.getAICar(q).getModelNode()->getPosition());

            if(mGameState.getRaceStarted())
            {
                mGameState.getAICar(q).performAICorrection(mGameState.isGamePaused());
            }
            mGameState.getAICar(q).processFrameBeforePhysics(evt, mStaticMeshProcesser, mGameState.getRaceStarted());
        }

        if(mGameState.isMultiplayerEnabled())
        {
            //AI
            for(size_t q = 0; q < mGameState.getMultiplayerCountAI(); ++q)
            {
                mGameState.getMultiplayerCarAI(q).processFrameBeforePhysics(evt, mStaticMeshProcesser, mGameState.getRaceStarted());
            }

            //human
            std::vector<std::string> playerNames = mGameState.getMultiplayerCarHumanNames();
            for(size_t q = 0; q < playerNames.size(); ++q)
            {
                mGameState.getMultiplayerCarHuman(playerNames[q]).processFrameBeforePhysics(evt, mStaticMeshProcesser, mGameState.getRaceStarted());
            }
        }
    }

    mLapController.calculateLapPositions();

    if(mGameState.getRaceStarted() && !mGameState.isGamePaused())
        mWorld->stepSimulation(evt.timeSinceLastFrame, 7);

    mGameState.getPlayerCar().processFrameAfterPhysics(evt, mGameState.getRaceStarted());

    for(size_t q = 0; q < mGameState.getAICount(); ++q)
        mGameState.getAICar(q).processFrameAfterPhysics(evt, mGameState.getRaceStarted());

    if(mGameState.isMultiplayerEnabled())
    {
        //AI
        for(size_t q = 0; q < mGameState.getMultiplayerCountAI(); ++q)
            mGameState.getMultiplayerCarAI(q).processFrameAfterPhysics(evt, mGameState.getRaceStarted());

        //human
        std::vector<std::string> playerNames = mGameState.getMultiplayerCarHumanNames();
        for(size_t q = 0; q < playerNames.size(); ++q)
        {
            mGameState.getMultiplayerCarHuman(playerNames[q]).processFrameAfterPhysics(evt, mGameState.getRaceStarted());
        }
    }

    if(mGameState.isMultiplayerEnabled())
    {
        Ogre::Vector3 playerVel = mGameState.getPlayerCar().getLinearVelocity();
        Ogre::Vector3 playerAngVel = mGameState.getPlayerCar().getAngularVelocity();
        Ogre::Quaternion playerRot = mGameState.getPlayerCar().getModelNode()->getOrientation();

        Ogre::Vector3 wheel0;
        Ogre::Vector3 wheel1;
        Ogre::Vector3 wheel2;
        Ogre::Vector3 wheel3;
        mGameState.getPlayerCar().getWheelsPositions(wheel0, wheel1, wheel2, wheel3);
        
        MultiplayerSessionData data;
        data.pos = playerPos;
        data.rot = playerRot;
        data.vel = playerVel;
        data.velang = playerAngVel;
        data.wheelpos[0] = wheel0;
        data.wheelpos[1] = wheel1;
        data.wheelpos[2] = wheel2;
        data.wheelpos[3] = wheel3;
        data.isAcc = mGameState.getPlayerCar().getAcceleration();
        data.isBrake = mGameState.getPlayerCar().getBrake();
        data.isLeft = mGameState.getPlayerCar().getSteerLeft();
        data.isRight = mGameState.getPlayerCar().getSteerRight();
        data.currentLap = mGameState.getPlayerCar().getLapUtils().getCurrentLap();
        data.lapPosition = mGameState.getPlayerCar().getLapUtils().getLapPosition();

        std::vector<MultiplayerSessionData> dataAI;
        for(size_t q = 0; q < mGameState.getAICount(); ++q)
        {
            Ogre::Vector3 aiVel = mGameState.getAICar(q).getLinearVelocity();
            Ogre::Vector3 aiAngVel = mGameState.getAICar(q).getAngularVelocity();
            Ogre::Quaternion aiRot = mGameState.getAICar(q).getModelNode()->getOrientation();
            Ogre::Vector3 aiPos = mGameState.getAICar(q).getModelNode()->getPosition();

            Ogre::Vector3 aiWheel0;
            Ogre::Vector3 aiWheel1;
            Ogre::Vector3 aiWheel2;
            Ogre::Vector3 aiWheel3;
            mGameState.getAICar(q).getWheelsPositions(aiWheel0, aiWheel1, aiWheel2, aiWheel3);

            MultiplayerSessionData aiData;
            aiData.pos = aiPos;
            aiData.rot = aiRot;
            aiData.vel = aiVel;
            aiData.velang = aiAngVel;

            aiData.wheelpos[0] = aiWheel0;
            aiData.wheelpos[1] = aiWheel1;
            aiData.wheelpos[2] = aiWheel2;
            aiData.wheelpos[3] = aiWheel3;

            aiData.isAcc = mGameState.getAICar(q).getAcceleration();
            aiData.isBrake = mGameState.getAICar(q).getBrake();
            aiData.isLeft = mGameState.getAICar(q).getSteerLeft();
            aiData.isRight = mGameState.getAICar(q).getSteerRight();

            aiData.currentLap = mGameState.getAICar(q).getLapUtils().getCurrentLap();
            aiData.lapPosition = mGameState.getAICar(q).getLapUtils().getLapPosition();

            dataAI.push_back(aiData);
        }

        if(mMultiplayerController.get())
        {
            mMultiplayerController->receiveSessionData();
            mMultiplayerController->updateSessionData(data, dataAI, evt.timeSinceLastFrame);
        }

        //mGameState.getPlayerCar().getModelNode()->setOrientation(rot);
    }

    return true;
}

bool BaseApp::frameEnded(const Ogre::FrameEvent &evt)
{
    if(mWindow->isClosed())
        return false;

    if(mShutDown)
        return false;

    //mVehicle->debugDraw(mDebugDrawer.get());

    return true;
}

bool BaseApp::frameRenderingQueued(const Ogre::FrameEvent& evt)
{
    if(mWindow->isClosed())
        return false;

    if(mShutDown)
        return false;

    if(mGameState.isMultiplayerEnabled())
    {
        if(mMultiplayerController.get())
        {
            if(mMultiplayerController->getReadySent()) // make sure room is connected
            {
                if(!mMultiplayerController->getStartHappen())
                {
                    mGameState.resetBeforeStartTimer();
                }
            }
        }
    }

    if( !mGameState.getRaceStarted() && 
        mGameState.getBeforeStartTimerTime() > mGameState.getSTRRacecrud().getFloatValue("on-grid parameters", "ready left start") * 1000.0f && 
        !mGameState.isGamePaused())
    {
        mGraphics2D.setRearViewMirrorPanelShow(false);
        mGraphics2D.hideAllStart();
        mGraphics2D.showBeforeStart1();
        mSoundsProcesser.playBeforeStart1();
    }

    if(!mGameState.getRaceStarted() && 
        mGameState.getBeforeStartTimerTime() > mGameState.getSTRRacecrud().getFloatValue("on-grid parameters", "set left start") * 1000.0f && 
        !mGameState.isGamePaused())
    {
        mGraphics2D.hideAllStart();
        mGraphics2D.showBeforeStart2();
        mSoundsProcesser.playBeforeStart2();
    }

    if(!mGameState.getRaceStarted() && 
        mGameState.getBeforeStartTimerTime() > mGameState.getSTRRacecrud().getFloatValue("on-grid parameters", "go left start") * 1000.0f && 
        !mGameState.isGamePaused())
    {
        mGraphics2D.hideAllStart();
        mGraphics2D.showBeforeStart3();
        mSoundsProcesser.playBeforeStart3();

        for(size_t q = 0; q < mGameState.getAICount(); ++q)
        {
            mGameState.getAICar(q).raceStarted();
        }

        if(mGameState.isMultiplayerEnabled())
        {
            //AI
            for(size_t q = 0; q < mGameState.getMultiplayerCountAI(); ++q)
            {
                mGameState.getMultiplayerCarAI(q).raceStarted();
            }

            //human
            std::vector<std::string> playerNames = mGameState.getMultiplayerCarHumanNames();
            for(size_t q = 0; q < playerNames.size(); ++q)
            {
                mGameState.getMultiplayerCarHuman(playerNames[q]).raceStarted();
            }
        }

        mGameState.getPlayerCar().getLapUtils().resetLapTimer();

        mGameState.setRaceStarted(true);
    }

    if(mGameState.getBeforeStartTimerTime() > (mGameState.getSTRRacecrud().getFloatValue("on-grid parameters", "go left start") * 1000.0f + mGameState.getSTRRacecrud().getFloatValue("on-grid parameters", "go left length") * 1000.0f) && !mGameState.isGamePaused())
    {
        mGraphics2D.hideAllStart();

        if(mGameState.getMirrorEnabled() && !mGameState.getRaceFinished())
        {
            mGraphics2D.setRearViewMirrorPanelShow(true);
        }
    }

    mInputHandler->capture(evt, mGameState.getPlayerCar().getModelNode(), mGameState.getGlobalLight(), mGameState.getShadowLight(), mIsDisableMouse);

    mTrayMgr->frameRenderingQueued(evt);

    mGraphics2D.setShowPausedPanel(mGameState.isGamePaused());

    if (!mTrayMgr->isDialogVisible())
    {
        if(!mGameState.isGamePaused())
            mGraphics2D.setEngineRPM(mGameState.getPlayerCar().getCarEngine().getEngineRPM());
        mGraphics2D.setCarSpeed(mGameState.getPlayerCar().getAlignedVelocity());
        mGraphics2D.setCurrentLap(static_cast<unsigned short>(mGameState.getPlayerCar().getLapUtils().getCurrentLap()), static_cast<unsigned short>(mGameState.getLapsCount()));
        mGraphics2D.setCarGear(static_cast<unsigned char>(mGameState.getPlayerCar().getCarEngine().getCurrentGear()));
        mGraphics2D.setCarPos(static_cast<unsigned char>(mLapController.getTotalPosition(0)), static_cast<unsigned char>(mLapController.getTotalCars()));

        mGraphics2D.hideAIDashboardCars();
        size_t currentPlayerLap = mGameState.getPlayerCar().getLapUtils().getCurrentLap();
        Ogre::Real currentPlayerLapPos = mGameState.getPlayerCar().getLapUtils().getLapPosition();
        mGraphics2D.setPlayerDashBoardSkin(mGameState);
        for(size_t q = 0; q < mGameState.getAICount(); ++q)
        {
            mGraphics2D.setAIDashBoardSkin(mGameState, q, mGameState.getAICar(q).getCharacterName());
            mGraphics2D.setDashCarPos(q, currentPlayerLap, currentPlayerLapPos, mGameState.getAICar(q).getLapUtils().getCurrentLap(), mGameState.getAICar(q).getLapUtils().getLapPosition());
        }

        if(mGameState.isMultiplayerEnabled())
        {
            //AI
            for(size_t q = 0; q < mGameState.getMultiplayerCountAI(); ++q)
            {
                mGraphics2D.setAIDashBoardSkin(mGameState, mGameState.getAICount() + q, mGameState.getMultiplayerCarAI(q).getCharacterName());
                mGraphics2D.setDashCarPos(mGameState.getAICount() + q, currentPlayerLap, currentPlayerLapPos, mGameState.getMultiplayerCarAI(q).getCurrentLap(), mGameState.getMultiplayerCarAI(q).getLapPosition());
            }

            //human
            std::vector<std::string> playerNames = mGameState.getMultiplayerCarHumanNames();
            for(size_t q = 0; q < playerNames.size(); ++q)
            {
                mGraphics2D.setAIDashBoardSkin(mGameState, mGameState.getAICount() + mGameState.getMultiplayerCountAI() + q, mGameState.getMultiplayerCarHuman(playerNames[q]).getCharacterName());
                mGraphics2D.setDashCarPos(mGameState.getAICount() + mGameState.getMultiplayerCountAI() + q, currentPlayerLap, currentPlayerLapPos, mGameState.getMultiplayerCarHuman(playerNames[q]).getCurrentLap(), mGameState.getMultiplayerCarHuman(playerNames[q]).getLapPosition());
            }
        }

        if(mGameState.getRaceStarted())
        {
            if(mGameState.getPlayerCar().getLapUtils().getAfterFinishLinePassTime() < 3.0f && mGameState.getPlayerCar().getLapUtils().getCurrentLap() > 1)
            {
                mGraphics2D.setRaceTime(Tools::SecondsToString(mGameState.getPlayerCar().getLapUtils().getLastLapTime()));
            }
            else
            {
                mGraphics2D.setRaceTime(Tools::SecondsToString(mGameState.getPlayerCar().getLapUtils().getLapTime()));
            }
        }
        else
        {
            mGraphics2D.setRaceTime("00:00:00");
        }

        if(mGameState.isMultiplayerEnabled())
        {
            mGraphics2D.setShowMiscText(true);
            if(mMultiplayerController.get())
            {
                mGraphics2D.setMiscText(Conversions::DMToString(mMultiplayerController->getSessionPing()));
            }
        }
#if SHOW_DETAILS_PANEL
        if (mDetailsPanel->isVisible())   // if details panel is visible, then update its contents
        {
            mDetailsPanel->setParamValue(0, Ogre::StringConverter::toString(mCamera->getDerivedPosition().x));
            mDetailsPanel->setParamValue(1, Ogre::StringConverter::toString(mCamera->getDerivedPosition().y));
            mDetailsPanel->setParamValue(2, Ogre::StringConverter::toString(mCamera->getDerivedPosition().z));
            
            mDetailsPanel->setParamValue(4, Ogre::StringConverter::toString(mGameState.getPlayerCar().getLinearVelocity().length()));
            mDetailsPanel->setParamValue(5, Ogre::StringConverter::toString(mGameState.getPlayerCar().getAlignedVelocity()));
            mDetailsPanel->setParamValue(6, Ogre::StringConverter::toString(mGameState.getPlayerCar().getLateralVelocity()));
#if 0
            mDetailsPanel->setParamValue(4, Ogre::StringConverter::toString(mGameState.getAICar(0].getLinearVelocity().length()));
            mDetailsPanel->setParamValue(5, Ogre::StringConverter::toString(mGameState.getAICar(0].getAlignedVelocity()));
            mDetailsPanel->setParamValue(6, Ogre::StringConverter::toString(mGameState.getAICar(0].getLateralVelocity()));
#endif

            mDetailsPanel->setParamValue(8, mGameState.getPlayerCar().getFrontLWheelColliderString());
            mDetailsPanel->setParamValue(9, mGameState.getPlayerCar().getFrontRWheelColliderString());
            mDetailsPanel->setParamValue(10, mGameState.getPlayerCar().getBackLWheelColliderString());
            mDetailsPanel->setParamValue(11, mGameState.getPlayerCar().getBackRWheelColliderString());
#if 0
            mDetailsPanel->setParamValue(8, mGameState.getAICar(0].getFrontLWheelColliderString());
            mDetailsPanel->setParamValue(9, mGameState.getAICar(0].getFrontRWheelColliderString());
            mDetailsPanel->setParamValue(10, mGameState.getAICar(0].getBackLWheelColliderString());
            mDetailsPanel->setParamValue(11, mGameState.getAICar(0].getBackRWheelColliderString());
#endif

            mDetailsPanel->setParamValue(13, Ogre::StringConverter::toString(mStaticMeshProcesser.getLongtitudeFriction(mGameState.getPlayerCar().getFrontLWheelColliderIndex())));
            mDetailsPanel->setParamValue(14, Ogre::StringConverter::toString(mStaticMeshProcesser.getLongtitudeFriction(mGameState.getPlayerCar().getFrontRWheelColliderIndex())));
            mDetailsPanel->setParamValue(15, Ogre::StringConverter::toString(mStaticMeshProcesser.getLongtitudeFriction(mGameState.getPlayerCar().getBackLWheelColliderIndex())));
            mDetailsPanel->setParamValue(16, Ogre::StringConverter::toString(mStaticMeshProcesser.getLongtitudeFriction(mGameState.getPlayerCar().getBackRWheelColliderIndex())));
#if 0
            mDetailsPanel->setParamValue(13, Ogre::StringConverter::toString(mStaticMeshProcesser.getLongtitudeFriction(mGameState.getAICar(0].getFrontLWheelColliderIndex())));
            mDetailsPanel->setParamValue(14, Ogre::StringConverter::toString(mStaticMeshProcesser.getLongtitudeFriction(mGameState.getAICar(0].getFrontRWheelColliderIndex())));
            mDetailsPanel->setParamValue(15, Ogre::StringConverter::toString(mStaticMeshProcesser.getLongtitudeFriction(mGameState.getAICar(0].getBackLWheelColliderIndex())));
            mDetailsPanel->setParamValue(16, Ogre::StringConverter::toString(mStaticMeshProcesser.getLongtitudeFriction(mGameState.getAICar(0].getBackRWheelColliderIndex())));
#endif

            mDetailsPanel->setParamValue(18, Ogre::StringConverter::toString(mStaticMeshProcesser.getLatitudeFriction(mGameState.getPlayerCar().getFrontLWheelColliderIndex())));
            mDetailsPanel->setParamValue(19, Ogre::StringConverter::toString(mStaticMeshProcesser.getLatitudeFriction(mGameState.getPlayerCar().getFrontRWheelColliderIndex())));
            mDetailsPanel->setParamValue(20, Ogre::StringConverter::toString(mStaticMeshProcesser.getLatitudeFriction(mGameState.getPlayerCar().getBackLWheelColliderIndex())));
            mDetailsPanel->setParamValue(21, Ogre::StringConverter::toString(mStaticMeshProcesser.getLatitudeFriction(mGameState.getPlayerCar().getBackRWheelColliderIndex())));
#if 0
            mDetailsPanel->setParamValue(18, Ogre::StringConverter::toString(mStaticMeshProcesser.getLatitudeFriction(mGameState.getAICar(0].getFrontLWheelColliderIndex())));
            mDetailsPanel->setParamValue(19, Ogre::StringConverter::toString(mStaticMeshProcesser.getLatitudeFriction(mGameState.getAICar(0].getFrontRWheelColliderIndex())));
            mDetailsPanel->setParamValue(20, Ogre::StringConverter::toString(mStaticMeshProcesser.getLatitudeFriction(mGameState.getAICar(0].getBackLWheelColliderIndex())));
            mDetailsPanel->setParamValue(21, Ogre::StringConverter::toString(mStaticMeshProcesser.getLatitudeFriction(mGameState.getAICar(0].getBackRWheelColliderIndex())));
#endif

            mDetailsPanel->setParamValue(23, Ogre::StringConverter::toString(mGameState.getPlayerCar().getLapUtils().getLapPosition()));
            mDetailsPanel->setParamValue(24, Tools::SecondsToString(mGameState.getPlayerCar().getLapUtils().getLastLapTime()));
            mDetailsPanel->setParamValue(25, Tools::SecondsToString(mGameState.getPlayerCar().getLapUtils().getLapTime()));
            mDetailsPanel->setParamValue(26, Ogre::StringConverter::toString(mGameState.getPlayerCar().getLapUtils().getCurrentLap()));
            mDetailsPanel->setParamValue(27, Ogre::StringConverter::toString(mLapController.getTotalPosition(0)));
            mDetailsPanel->setParamValue(28, Ogre::StringConverter::toString(mGameState.getPlayerCar().getCarEngine().getCurrentGear()));
            mDetailsPanel->setParamValue(29, Ogre::StringConverter::toString(mGameState.getPlayerCar().getCarEngine().getEngineRPM()));
        }
#endif
    }

    return true;
}

void BaseApp::windowResized(Ogre::RenderWindow* rw)
{
    unsigned int width, height, depth;
    int left, top;
    rw->getMetrics(width, height, depth, left, top);

#if defined (__ANDROID__)
#else
    if(mInputHandler->getInputContext().mMouse)
    {
        const OIS::MouseState &ms = mInputHandler->getInputContext().mMouse->getMouseState();
        ms.width = width;
        ms.height = height;
    }
#endif
}

void BaseApp::windowClosed(Ogre::RenderWindow* rw)
{
    //Only close for window that created OIS (the main window in these demos)
    if( rw == mWindow )
    {
        mInputHandler->detach();
    }
}

void BaseApp::scriptsReload()
{
    mIsGlobalReset = true;
    mLuaManager.CallFunction_0_0("clear", mPipeline);
    unloadResources();
    doLuaMainFile();
    mLuaManager.CallFunction_0_0("main", mPipeline);
    loadResources();
    mLuaManager.CallFunction_0_0("create", mPipeline);

    initLightLists();
}

void BaseApp::quickScriptsReload()
{
    mLuaManager.CallFunction_0_0("clear", mPipeline);
    mLuaManager.CallFunction_0_0("main", mPipeline);
    mLuaManager.CallFunction_0_0("create", mPipeline);

    initLightLists();
}

void BaseApp::initScene()
{
    mSceneMgr = mRoot->createSceneManager(Ogre::ST_GENERIC);

    mGameState.setPlayerCharacterName(mLuaManager.ReadScalarString("Main.Character", mPipeline));

    mGameState.initOriginalData(mLuaManager.ReadScalarString("Main.Track", mPipeline),
                                mLuaManager.ReadScalarString("Main.OriginalPathData", mPipeline),
                                mLuaManager.ReadScalarString("Main.OriginalPathCommon", mPipeline),
                                static_cast<AIStrength>(mLuaManager.ReadScalarInt("Scene.AIStrength", mPipeline)));

    mSoundsProcesser.initSounds(mGameState.getPFLoaderData());

    //migration from 1.8.1 to 1.9.0
    //http://www.ogre3d.org/forums/viewtopic.php?f=2&t=78278
    mSceneMgr->addRenderQueueListener(mOverlaySystem.get());

    mSceneMgr->setAmbientLight(Ogre::ColourValue(1.0, 1.0, 1.0));

    mMainNode = mSceneMgr->getRootSceneNode()->createChildSceneNode();

    mGameState.setListenerGain(mLuaManager.ReadScalarFloat("Scene.SoundMasterGain", mPipeline));

    mIsDisableMouse = mLuaManager.ReadScalarBool("Scene.IsDisableMouse", mPipeline);

    mCamera = mSceneMgr->createCamera("PlayerCam");
    mCamera->setNearClipDistance(0.5f);
    Ogre::Viewport * mViewPort = mWindow->addViewport(mCamera);

    mViewPort->setBackgroundColour(mGameState.getBackgroundColor());
    mCamera->setAspectRatio(Ogre::Real(mViewPort->getActualWidth()) / Ogre::Real(mViewPort->getActualHeight()) / (640.0f / 480.0f));
    mCamera->setFOVy(Ogre::Degree(95.0f));

    initWorld(Ogre::Vector3(0.0f, -mLuaManager.ReadScalarFloat("Scene.Gravity", mPipeline), 0.0f));

    mCameraMan.reset(new CameraMan(mCamera, mWorld.get(), mSceneMgr));
    mInputHandler->resetCameraMenPointer(mCameraMan.get());

    //load data
    {
        bool isDebugLLT = mLuaManager.ReadScalarBool("Terrain.Scene.IsDebugLLT", mPipeline);

        LLTLoader().load(mGameState, mSceneMgr, isDebugLLT);
    }

    {
        bool isDebugExclusion = mLuaManager.ReadScalarBool("Terrain.Scene.IsDebugExclusion", mPipeline);
        ExclusionLoader().load(mGameState, mSceneMgr, isDebugExclusion);
    }

    PHYLoader().load(mGameState);

    {
        bool isDebugAI = mLuaManager.ReadScalarBool("Terrain.Scene.IsDebugAI", mPipeline);

        mGameState.setAICount(mLuaManager.ReadScalarInt("Scene.AIOpponents", mPipeline));

        Ogre::Real speedCoeff;
        Ogre::Real lateralStabilizationCoeff;

        switch(mGameState.getAIStrength())
        {
        case Medium :
            speedCoeff = 0.7f;
            lateralStabilizationCoeff = 0.7f;
            if(mGameState.getTrackName() == "desert track") lateralStabilizationCoeff = 0.7f;
            if(mGameState.getTrackName() == "speedway track") lateralStabilizationCoeff = 0.2f;
            if(mGameState.getTrackName() == "speedway night track") lateralStabilizationCoeff = 0.2f;
            if(mGameState.getTrackName() == "dam") lateralStabilizationCoeff = 0.7f;
            if(mGameState.getTrackName() == "mountain forest track") lateralStabilizationCoeff = 0.7f;
            if(mGameState.getTrackName() == "mineshaft") lateralStabilizationCoeff = 0.7f;
            if(mGameState.getTrackName() == "alpine track") lateralStabilizationCoeff = 0.7f;
            if(mGameState.getTrackName() == "city track") lateralStabilizationCoeff = 1.4f;
            if(mGameState.getTrackName() == "nutopia track") lateralStabilizationCoeff = 1.4f;
            break;
        case Hard :
            speedCoeff = 0.9f;
            lateralStabilizationCoeff = 0.8f;
            if(mGameState.getTrackName() == "desert track") lateralStabilizationCoeff = 0.8f;
            if(mGameState.getTrackName() == "speedway track") lateralStabilizationCoeff = 0.2f;
            if(mGameState.getTrackName() == "speedway night track") lateralStabilizationCoeff = 0.2f;
            if(mGameState.getTrackName() == "dam") lateralStabilizationCoeff = 0.8f;
            if(mGameState.getTrackName() == "mountain forest track") lateralStabilizationCoeff = 0.8f;
            if(mGameState.getTrackName() == "mineshaft") lateralStabilizationCoeff = 0.8f;
            if(mGameState.getTrackName() == "alpine track") lateralStabilizationCoeff = 0.8f;
            if(mGameState.getTrackName() == "city track") lateralStabilizationCoeff = 1.6f;
            if(mGameState.getTrackName() == "nutopia track") lateralStabilizationCoeff = 1.4f;
            break;
        case Insane :
            speedCoeff = 1.0f;
            lateralStabilizationCoeff = 1.0f;
            if(mGameState.getTrackName() == "desert track") lateralStabilizationCoeff = 1.0f;
            if(mGameState.getTrackName() == "speedway track") lateralStabilizationCoeff = 0.2f;
            if(mGameState.getTrackName() == "speedway night track") lateralStabilizationCoeff = 0.2f;
            if(mGameState.getTrackName() == "dam") lateralStabilizationCoeff = 1.0f;
            if(mGameState.getTrackName() == "mountain forest track") lateralStabilizationCoeff = 1.0f;
            if(mGameState.getTrackName() == "mineshaft") lateralStabilizationCoeff = 1.0f;
            if(mGameState.getTrackName() == "alpine track") lateralStabilizationCoeff = 1.0f;
            if(mGameState.getTrackName() == "city track") lateralStabilizationCoeff = 1.6f;
            if(mGameState.getTrackName() == "nutopia track") lateralStabilizationCoeff = 1.4f;
            break;
        default:
            speedCoeff = 0.6f;
            lateralStabilizationCoeff = 0.2f;
            if(mGameState.getTrackName() == "desert track") lateralStabilizationCoeff = 0.2f;
            if(mGameState.getTrackName() == "speedway track") lateralStabilizationCoeff = 0.2f;
            if(mGameState.getTrackName() == "speedway night track") lateralStabilizationCoeff = 0.2f;
            if(mGameState.getTrackName() == "dam") lateralStabilizationCoeff = 0.2f;
            if(mGameState.getTrackName() == "mountain forest track") lateralStabilizationCoeff = 0.2f;
            if(mGameState.getTrackName() == "mineshaft") lateralStabilizationCoeff = 0.2f;
            if(mGameState.getTrackName() == "alpine track") lateralStabilizationCoeff = 0.2f;
            if(mGameState.getTrackName() == "city track") lateralStabilizationCoeff = 1.2f;
            if(mGameState.getTrackName() == "nutopia track") lateralStabilizationCoeff = 1.4f;
        }

        Ogre::Real speedCoeffMinLimit = speedCoeff - 0.1f;

        for(size_t w = 0; w < mGameState.getAICount(); ++w)
        {
            mGameState.getAICar(w).clearAIData();

            mGameState.getAICar(w).setLateralStabilizationCoeff(lateralStabilizationCoeff);
            mGameState.getAICar(w).setSpeedCoeff(Randomizer::GetInstance().GetRandomFloat(speedCoeffMinLimit, speedCoeff));
        }

        AILoader().load(mGameState, mSceneMgr, isDebugAI);
    }

    //init multiplayer
    {
        mGameState.setMultiplayerEnabled(mLuaManager.ReadScalarBool("Scene.Multiplayer.Enabled", mPipeline));
        mGameState.setMultiplayerMaster(mLuaManager.ReadScalarBool("Scene.Multiplayer.IsMaster", mPipeline));
        mGameState.setMultiplayerRoomName(mLuaManager.ReadScalarString("Scene.Multiplayer.RoomName", mPipeline));
        mGameState.setMultiplayerUserName(mLuaManager.ReadScalarString("Scene.Multiplayer.UserName", mPipeline));
        mGameState.setMultiplayerServerIP(mLuaManager.ReadScalarString("Scene.Multiplayer.ServerIP", mPipeline));
        mGameState.setMultiplayerServerPort(mLuaManager.ReadScalarInt("Scene.Multiplayer.ServerPort", mPipeline));
        mGameState.setMultiplayerPlayersLimits(mLuaManager.ReadScalarInt("Scene.Multiplayer.PlayersLimits", mPipeline));
        mGameState.setMultiplayerBroadcastInterval(mLuaManager.ReadScalarInt("Scene.Multiplayer.BroadcastInterval", mPipeline));

        if(mGameState.isMultiplayerEnabled() && !mGameState.isMultiplayerMaster())
        {
            mGameState.setAICount(0);
        }
    }

    ParticlesLoader().load(mGameState);

    //http://www.ogre3d.org/forums/viewtopic.php?p=331138
    //http://www.ogre3d.org/forums/viewtopic.php?f=2&t=79581
    mSceneMgrCarUI = mRoot->createSceneManager(Ogre::ST_GENERIC);
    Ogre::Camera * cameraCarUI = mSceneMgrCarUI->createCamera("PlayerCam");
    cameraCarUI->setProjectionType(Ogre::PT_ORTHOGRAPHIC);
    cameraCarUI->setOrthoWindow(static_cast<float>(mViewPort->getActualWidth()), static_cast<float>(mViewPort->getActualHeight()));
    cameraCarUI->setNearClipDistance(0.5f);
    cameraCarUI->setFarClipDistance(10000.0f);
    Ogre::Viewport * viewPortCarUI = mWindow->addViewport(cameraCarUI, 1);
    viewPortCarUI->setClearEveryFrame(true, Ogre::FBT_DEPTH);
    viewPortCarUI->setOverlaysEnabled(false);
    viewPortCarUI->setSkiesEnabled(false);
    cameraCarUI->setAspectRatio(static_cast<float>(mViewPort->getActualWidth()) / static_cast<float>(mViewPort->getActualHeight()));
    cameraCarUI->setFOVy(Ogre::Degree(45.0f));
    cameraCarUI->setPosition(0.0f, 0.0f, 100.0f);
    cameraCarUI->lookAt(Ogre::Vector3::ZERO);
}

void BaseApp::initWorld(const Ogre::Vector3 &gravityVector, const Ogre::AxisAlignedBox &bounds)
{
    mWorld.reset(new OgreBulletDynamics::DynamicsWorld(mSceneMgr, bounds, gravityVector, true, true, 10000));
    mDebugDrawer.reset(new OgreBulletCollisions::DebugDrawer());
    mWorld->setDebugDrawer(mDebugDrawer.get());

    Ogre::SceneNode *node = mSceneMgr->getRootSceneNode()->createChildSceneNode("debugDrawer", Ogre::Vector3::ZERO);
    node->attachObject(static_cast<Ogre::SimpleRenderable *>(mDebugDrawer.get()));

    if(mLuaManager.ReadScalarBool("Terrain.Scene.IsDebugPhysics", mPipeline))
    {
        mDebugDrawer->setDrawWireframe(true);
        mWorld->setShowDebugShapes(true);

        mDebugDrawer->setDrawContactPoints(true);
        mWorld->setShowDebugContactPoints(true);
    }
}

void BaseApp::deInitWorld()
{
    mStaticMeshProcesser.deinit();
    mDebugDrawer.reset();
    mWorld->setDebugDrawer(NULL);
    mWorld.reset();
}

void BaseApp::initLightLists()
{
    //d.polubotko: make sure light lists created during loading
    mStaticMeshProcesser.queryLights();
}

void BaseApp::clearScene()
{

    mGraphics2D.clearMiscPanelText();
    mGraphics2D.setVisibleFinishSign(false);

    if(mMultiplayerController.get())
        mMultiplayerController->clearSessionAndLobby();
    mMultiplayerController.reset();

    mGameState.resetGamePaused();
    mGameState.setRaceStarted(false);
    mGameState.setRaceFinished(false);

    deInitWorld();

    //deinit sounds of system

    //clear sounds of cars as well
    mGameState.getPlayerCar().clear();
    mGameState.getPlayerCar().deinitSounds();
    for(size_t q = 0; q < mGameState.getAICount(); ++q)
    {
        mGameState.getAICar(q).clear();
        mGameState.getAICar(q).deinitSounds();
    }
    if(mGameState.isMultiplayerEnabled())
    {
        //AI
        for(size_t q = 0; q < mGameState.getMultiplayerCountAI(); ++q)
        {
            mGameState.getMultiplayerCarAI(q).clear();
            mGameState.getMultiplayerCarAI(q).deinitSounds();
        }

        //human
        std::vector<std::string> playerNames = mGameState.getMultiplayerCarHumanNames();
        for(size_t q = 0; q < playerNames.size(); ++q)
        {
            mGameState.getMultiplayerCarHuman(playerNames[q]).clear();
            mGameState.getMultiplayerCarHuman(playerNames[q]).deinitSounds();
        }
    }

    mSoundsProcesser.stopSounds();

    mGameState.setGlobalLight(NULL);
    mGameState.setShadowLight(NULL);

    mInputHandler->resetCameraMenPointer(NULL);
    mCameraMan.reset();

    mTrayMgr->getTraysLayer()->remove3D(mGameState.getArrowNode());

    mSceneMgr->clearScene();
    mRoot->destroySceneManager(mSceneMgr);
    mSceneMgrCarUI->clearScene();
    mRoot->destroySceneManager(mSceneMgrCarUI);
    mWindow->removeAllViewports();
}

void BaseApp::preloadResource(const Ogre::String& fileName, const Ogre::String& fileType)
{
    Ogre::ResourceGroupManager::getSingleton().declareResource(fileName, fileType, "Popular");
}

void BaseApp::initTerrain()
{
    mStaticMeshProcesser.initParts( mPipeline, mSceneMgr, mMainNode, mIsGlobalReset, 
                                    mGameState, mWorld.get());
    mStaticMeshProcesser.loadTerrainMaps(mGameState);

    //load terrain params
    TerrainLoader terrainLoader;
    terrainLoader.load(mGameState);

    mStaticMeshProcesser.setFrictionRemapArray(terrainLoader.getRemapFriction());
    mStaticMeshProcesser.setLatutuideFrictionArray(terrainLoader.getLatitudeFriction());
    mStaticMeshProcesser.setLongtitudeFrictionArray(terrainLoader.getLongtitudeFriction());
    mSoundsProcesser.setRemappers(terrainLoader.getRemapSounds(), terrainLoader.getRemapSoundsCollision());

    //particle (before loading cars)
    {
        //Ogre::TexturePtr particle = TextureLoader().load(mGameState.getPFLoaderData(), "data/deii", "particle.tga", "OriginalParticle");
        Ogre::TexturePtr particle = TEXLoader().load(mGameState.getPFLoaderData(), "data/deii", "d3d2display1_m_1.tex", "OriginalParticle");
        Ogre::MaterialPtr particleMat = Ogre::MaterialManager::getSingleton().getByName("Test/Particle");
        particleMat->getTechnique(0)->getPass(0)->getTextureUnitState(0)->setTexture(particle);
    }
}

void BaseApp::initModel()
{

    //arrow (before loading models)
    {
        TEXLoader().load(mGameState.getPFLoaderData(), "data/misc", "arrow_m_3.tex", "OriginalArrow");
    }

    mGameState.setPLayerCarPrevVel(Ogre::Vector3::ZERO);

    bool isCamToAI = mLuaManager.ReadScalarBool("Scene.IsCamToAI", mPipeline);

    mLapController.clear();
    mLapController.addCar(&mGameState.getPlayerCar());//should be first
    mGameState.getPlayerCar().getLapUtils().setEvents(this);

    mModelsPool.initModels(mSceneMgr, mGameState);

    mGameState.getPlayerCar().initModel(mPipeline, mGameState, mSceneMgr, mMainNode, mCameraMan.get(), &mModelsPool, mWorld.get(), mGameState.getPlayerCharacterName(), mGameState.getTrackPositions()[mGameState.getAICount()], !isCamToAI);
    mGameState.getPlayerCar().initSounds(mPipeline, mGameState);

    std::vector<std::string> aiCharacters = RacingGridGeneration().generate(mGameState);

    for(size_t q = 0; q < mGameState.getAICount(); ++q)
    {
        bool isCam = (q == (mGameState.getAICount() - 1));
        if(!isCamToAI)
            isCam = false;
        mGameState.getAICar(q).initModel(mPipeline, mGameState, mSceneMgr, mMainNode, mCameraMan.get(), &mModelsPool, mWorld.get(), aiCharacters[q], mGameState.getTrackPositions()[q], isCam);
        mGameState.getAICar(q).initSounds(mPipeline, mGameState);

        mLapController.addCar(&mGameState.getAICar(q));
    }

}

void BaseApp::initMisc()
{
    //rear mirror camera
    mRearCamera = NULL;

    mGameState.setMirrorEnabled(mLuaManager.ReadScalarBool("Scene.Mirror.Enable", mPipeline));
    if(mGameState.getMirrorEnabled())
    {

        //rear mirror texture
        //http://www.ogre3d.org/tikiwiki/tiki-index.php?page=Basic+Tutorial+7
        if(mIsGlobalReset)
        {
            int mirrorSizeW = mLuaManager.ReadScalarInt("Scene.Mirror.RTTSize.w", mPipeline);
            int mirrorSizeH = mLuaManager.ReadScalarInt("Scene.Mirror.RTTSize.h", mPipeline);

            mGraphics2D.createRearViewMirrorPanelTexture(this, mRoot.get(), mirrorSizeW, mirrorSizeH);
            mGraphics2D.setRearViewMirrorPanelMaterial("Test/RearViewMirror");
        }
        else
        {
            mGraphics2D.rearViewMirrorPanelTextureRemoveAllViewports();
        }

        mGraphics2D.initTachoNeedle(mSceneMgrCarUI, mGameState);


        mRearCamera = mSceneMgr->createCamera("RearViewCamera");
        mRearCamera->setNearClipDistance(0.5f);
        Ogre::Viewport *v = mGraphics2D.rearViewMirrorPanelTextureAddViewport(mRearCamera);
        v->setOverlaysEnabled(false);
        v->setClearEveryFrame(true);
        v->setBackgroundColour(mGameState.getBackgroundColor());
        mRearCamera->setAspectRatio(1.0f);
        mRearCamera->setFOVy(Ogre::Degree(mLuaManager.ReadScalarFloat("Scene.Mirror.FOV", mPipeline)));

        mGraphics2D.setRearViewMirrorPanelShow(true);
    }
    else
    {
        mGraphics2D.setRearViewMirrorPanelShow(false);
    }
    mCameraMan->setRearCamera(mRearCamera);

    //as long as last procedure before draw started
    mGameState.resetBeforeStartTimer();

    //arrow
    Ogre::Entity * arrowEntity = mModelsPool.getArrow();
    mGameState.setArrowNode(mSceneMgr->createSceneNode(arrowEntity->getName()));
    mGameState.getArrowNode()->attachObject(arrowEntity);
    mTrayMgr->getTraysLayer()->add3D(mGameState.getArrowNode());
    mGameState.getArrowNode()->setPosition(-0.75f, 0.75f, -1.0f);
    mGameState.getArrowNode()->setScale(0.20f, 0.20f, 0.20f);
    mGameState.getArrowNode()->setVisible(true);

    mIsGlobalReset = false;

    
    if(mGameState.isMultiplayerEnabled())
    {
        mMultiplayerController.reset(new MultiplayerController(this, mGameState.getMultiplayerBroadcastInterval()));

        if(mGameState.isMultiplayerMaster())
        {
            std::vector<std::string> gameCars;
            for(size_t q = 0; q < mGameState.getAICount(); ++q)
            {
                gameCars.push_back(mGameState.getAICar(q).getCharacterName());
            }
            bool success = mMultiplayerController->startSessionMaster(mGameState.getMultiplayerServerIP(), mGameState.getMultiplayerServerPort(), mGameState.getMultiplayerUserName(), mGameState.getMultiplayerRoomName(), mGameState.getMultiplayerPlayersLimits(), mGameState.getAICount(), gameCars, mGameState.getPlayerCar().getCharacterName());

            if(!success)
            {
                mGameState.setMultiplayerEnabled(false);
                mMultiplayerController.reset();
            }
        }
        else
        {
            bool success = mMultiplayerController->startSessionSlave(mGameState.getMultiplayerServerIP(), mGameState.getMultiplayerServerPort(), mGameState.getMultiplayerUserName(), mGameState.getMultiplayerRoomName(), mGameState.getPlayerCar().getCharacterName());

            if(!success)
            {
                mGameState.setMultiplayerEnabled(false);
                mMultiplayerController.reset();
            }
        }
    }
}

void BaseApp::keyDown(OIS::KeyCode key)
{
    if(!mGameState.isGamePaused())
        mGameState.getPlayerCar().keyDown(key);
}

void BaseApp::keyUp(OIS::KeyCode key)
{
    mGameState.getPlayerCar().keyUp(key);
}

void BaseApp::processCollision(btManifoldPoint& cp, const btCollisionObjectWrapper* colObj0Wrap, const btCollisionObjectWrapper* colObj1Wrap, int triIndex)
{
    mGameState.getPlayerCar().processWheelsCollision(cp, colObj0Wrap, colObj1Wrap, mStaticMeshProcesser, triIndex);
    mGameState.getPlayerCar().processChassisCollision(cp, colObj0Wrap, colObj1Wrap, mStaticMeshProcesser, triIndex);

    for(size_t q = 0; q < mGameState.getAICount(); ++q)
    {
        mGameState.getAICar(q).processWheelsCollision(cp, colObj0Wrap, colObj1Wrap, mStaticMeshProcesser, triIndex);
    }

    if(mGameState.isMultiplayerEnabled())
    {
        //AI
        for(size_t q = 0; q < mGameState.getMultiplayerCountAI(); ++q)
        {
            mGameState.getMultiplayerCarAI(q).processWheelsCollision(cp, colObj0Wrap, colObj1Wrap, mStaticMeshProcesser, triIndex);
        }

        //human
        std::vector<std::string> playerNames = mGameState.getMultiplayerCarHumanNames();
        for(size_t q = 0; q < playerNames.size(); ++q)
        {
            mGameState.getMultiplayerCarHuman(playerNames[q]).processWheelsCollision(cp, colObj0Wrap, colObj1Wrap, mStaticMeshProcesser, triIndex);
        }
    }

    //surface sound
    bool isSoundPlay = false;
    if(mGameState.getPlayerCar().checkFrontCollision() || mGameState.getPlayerCar().checkRearCollision())
    {
        //if(mGameState.getPlayerCar().getAcceleration() || mGameState.getPlayerCar().getBrake())
        {
            Ogre::Real lateralVel = Ogre::Math::Abs(mGameState.getPlayerCar().getLateralVelocity());
            isSoundPlay = true;
            unsigned char backLWheel = mGameState.getPlayerCar().getBackLWheelColliderIndex();
            mSoundsProcesser.playSurface(backLWheel, lateralVel);
        }
    }
    if(!isSoundPlay)
    {
        mSoundsProcesser.stopSoundSurfaces();
    }

    //surface crash sound
    Ogre::Vector3 playerVel = mGameState.getPlayerCar().getLinearVelocity();
    Ogre::Real playerVelDiff = playerVel.distance(mGameState.getPLayerCarPrevVel());
    mGameState.setPLayerCarPrevVel(playerVel);
    if(playerVelDiff > 10.0f)
    {
        if(mGameState.getPlayerCar().checkFrontCollision() || mGameState.getPlayerCar().checkRearCollision() || mGameState.getPlayerCar().checkChassisCollision())
        {

            unsigned char collisionSurface = 0;

            if(mGameState.getPlayerCar().checkFrontCollision())
            {
                collisionSurface = mGameState.getPlayerCar().getFrontLWheelColliderIndex();
            }

            if(mGameState.getPlayerCar().checkRearCollision())
            {
                collisionSurface = mGameState.getPlayerCar().getBackLWheelColliderIndex();
            }

            if(mGameState.getPlayerCar().checkChassisCollision())
            {
                collisionSurface = mGameState.getPlayerCar().getChassisColliderIndex();
            }

            mSoundsProcesser.playSurfaceCrash(collisionSurface);
        }
    }

    //ai with player crash sounds
    for(size_t q = 0; q < mGameState.getAICount(); ++q)
    {
        if(mGameState.getPlayerCar().isCollideChassis(mGameState.getAICar(q), colObj0Wrap, colObj1Wrap))
        {
            Ogre::Vector3 aiVel = mGameState.getAICar(q).getLinearVelocity();
            Ogre::Real velDiff = playerVel.distance(aiVel);
            if(velDiff > 20.0f && velDiff <= 50.0f)
            {
                mSoundsProcesser.playCrash1();
                break;
            }
            else if(velDiff > 50.0f)
            {
                mSoundsProcesser.playCrash2();
                break;
            }
            
        }
    }
}

void BaseApp::preRenderTargetUpdate(const Ogre::RenderTargetEvent& evt)
{
    //http://www.ogre3d.org/forums/viewtopic.php?t=45499
    if(mCameraMan->getCameraPositionType() != CameraPosition_Bumper)
        mGameState.getPlayerCar().setVisibility(false);

    mGraphics2D.setVisibleTachoNeedle(false);
}

void BaseApp::postRenderTargetUpdate(const Ogre::RenderTargetEvent& evt)
{
    if(mCameraMan->getCameraPositionType() != CameraPosition_Bumper)
        mGameState.getPlayerCar().setVisibility(true);

    mGraphics2D.setVisibleTachoNeedle(true);
}

void BaseApp::onPlayerEjected(const std::string& player)
{
    mGraphics2D.addMiscPanelText("Player ejected: [" + player + "]", Ogre::ColourValue::Red);
}

void BaseApp::onPlayerJoined(const std::string& player)
{
    mGraphics2D.addMiscPanelText("Player joined lobby: [" + player + "]", Ogre::ColourValue::Blue);
}

void BaseApp::onPlayerLeft(const std::string& player)
{
    mGraphics2D.addMiscPanelText("Player left lobby: [" + player + "]", Ogre::ColourValue::Red);
}

void BaseApp::onNewHost(const std::string& player)
{
    mGraphics2D.addMiscPanelText("New host: [" + player + "]", Ogre::ColourValue::Red);
}

void BaseApp::onRoomClosed(const std::string& player)
{
    mGraphics2D.addMiscPanelText("Room closed: [" + player + "]", Ogre::ColourValue::Red);
}

void BaseApp::onPlayerReady(const std::string& player)
{
    mGraphics2D.addMiscPanelText("Player ready for session: [" + player + "]", Ogre::ColourValue::Green);
}

void BaseApp::onPlayerAddedToSession(const std::string& player)
{
    mGraphics2D.addMiscPanelText("Player added to session: [" + player + "]");
}

void BaseApp::onPlayerQuitSession(const std::string& player, bool isHost)
{
    //isHost - true if remote player was host (not current host!!!)

    if(!isHost)
        mGraphics2D.addMiscPanelText("Player quit session: [" + player + "]", Ogre::ColourValue::Red);
    else
        mGraphics2D.addMiscPanelText("Host player quit session: [" + player + "]", Ogre::ColourValue::Red);
    
    //remove human
    std::vector<std::string>multiplayerNames = mGameState.getMultiplayerCarHumanNames();
    if(std::find(multiplayerNames.begin(), multiplayerNames.end(), player) != multiplayerNames.end())
    {
        PSMultiplayerCar& carHuman = mGameState.getMultiplayerCarHuman(player);
        mLapController.removeCar(&carHuman);
        carHuman.removeFromScene(mSceneMgr);

        //remove PSMultiplayerCar object
        mGameState.removeMultiplayerCarHuman(player);
    }

    //remove ai
    if(isHost)
    {
        for(size_t q = 0; q < mGameState.getMultiplayerCountAI(); ++q)
        {
            PSMultiplayerCar& carAI = mGameState.getMultiplayerCarAI(q);
            mLapController.removeCar(&carAI);
            carAI.removeFromScene(mSceneMgr);
        }
        mGameState.setMultiplayerCountAI(0);
    }
}

void BaseApp::onSessionStart(uint32_t aiAmount, const std::vector<std::string>& players, size_t playerIndex, bool isHost, const std::vector<std::string>& aiSkins, const std::map<std::string, std::string>& playersSkins)
{
    //playerIndex should start from 0
    mGameState.getPlayerCar().setModelPositionOnGrid(mGameState.getTrackPositions()[aiAmount + playerIndex]);

    mGameState.clearMultiplayerCarsHuman();
    mGameState.setMultiplayerCountAI(0);

    if(isHost)
    {
        //only human cars
        for(size_t q = 0; q < players.size(); ++q)
        {
            if(q != playerIndex)// minus self
            {
                std::string humanCharacter = "frantic";
                std::map<std::string, std::string>::const_iterator foundPlayerSkin = playersSkins.find(players[q]);
                if(foundPlayerSkin != playersSkins.end())
                {
                    humanCharacter = (*foundPlayerSkin).second;
                }

                mGameState.addMultiplayerCarHuman(players[q]);

                PSMultiplayerCar& humanCar = mGameState.getMultiplayerCarHuman(players[q]);

                humanCar.setCharacterName(humanCharacter);

                humanCar.initModel(mPipeline, mGameState, mSceneMgr, mMainNode, mCameraMan.get(), &mModelsPool, mWorld.get(), humanCharacter, mGameState.getTrackPositions()[q], false);
                humanCar.initSounds(mPipeline, mGameState);

                humanCar.setModelPositionOnGrid(mGameState.getTrackPositions()[aiAmount + q]);

                mLapController.addCar(&humanCar);
            }
        }
    }
    else
    {
        //human
        for(size_t q = 0; q < players.size(); ++q)
        {
            if(q != playerIndex)// minus self
            {
                std::string humanCharacter = "frantic";
                std::map<std::string, std::string>::const_iterator foundPlayerSkin = playersSkins.find(players[q]);
                if(foundPlayerSkin != playersSkins.end())
                {
                    humanCharacter = (*foundPlayerSkin).second;
                }

                mGameState.addMultiplayerCarHuman(players[q]);

                PSMultiplayerCar& humanCar = mGameState.getMultiplayerCarHuman(players[q]);

                humanCar.setCharacterName(humanCharacter);

                humanCar.initModel(mPipeline, mGameState, mSceneMgr, mMainNode, mCameraMan.get(), &mModelsPool, mWorld.get(), humanCharacter, mGameState.getTrackPositions()[q], false);
                humanCar.initSounds(mPipeline, mGameState);

                humanCar.setModelPositionOnGrid(mGameState.getTrackPositions()[aiAmount + q]);

                mLapController.addCar(&humanCar);
            }
        }

        //AI
        mGameState.setMultiplayerCountAI(aiAmount );

        for(size_t q = 0; q < aiAmount; ++q)
        {
            std::string aiCharacter = "frantic";
            if(aiSkins.size() > q)
            {
                aiCharacter = aiSkins[q];
            }

            mGameState.getMultiplayerCarAI(q).setCharacterName(aiCharacter);

            mGameState.getMultiplayerCarAI(q).initModel(mPipeline, mGameState, mSceneMgr, mMainNode, mCameraMan.get(), &mModelsPool, mWorld.get(), aiCharacter, mGameState.getTrackPositions()[q], false);

            mGameState.getMultiplayerCarAI(q).initSounds(mPipeline, mGameState);
            mGameState.getMultiplayerCarAI(q).setModelPositionOnGrid(mGameState.getTrackPositions()[q]);

            mLapController.addCar(&mGameState.getMultiplayerCarAI(q));
        }
    }
}

void BaseApp::onSessionUpdate(const MultiplayerController::playerToData& otherPlayersSessionData, const std::vector<MultiplayerSessionData>& aiPlayersSessionData, bool isHost)
{
    const float posDiffMax = 30.0f;

    if(!isHost)
    {
        //AI
        if(mGameState.getMultiplayerCountAI() == aiPlayersSessionData.size())
        {
            for(size_t q = 0; q < aiPlayersSessionData.size(); ++q)
            {
                Ogre::Vector3 aiPos = mGameState.getMultiplayerCarAI(q).getModelNode()->getPosition();
                if(aiPos.distance(aiPlayersSessionData[q].pos) > posDiffMax)
                {
                    mGameState.getMultiplayerCarAI(q).setModelPosition(aiPlayersSessionData[q].pos, aiPlayersSessionData[q].rot, aiPlayersSessionData[q].wheelpos[0], aiPlayersSessionData[q].wheelpos[1], aiPlayersSessionData[q].wheelpos[2], aiPlayersSessionData[q].wheelpos[3]);
                }
                else
                {
                    mGameState.getMultiplayerCarAI(q).setModelRotationOnly(aiPlayersSessionData[q].rot);
                }

                mGameState.getMultiplayerCarAI(q).setModelVelocity(aiPlayersSessionData[q].vel, aiPlayersSessionData[q].velang);
                mGameState.getMultiplayerCarAI(q).setAcceleration(aiPlayersSessionData[q].isAcc);
                mGameState.getMultiplayerCarAI(q).setBrake(aiPlayersSessionData[q].isBrake);
                mGameState.getMultiplayerCarAI(q).setSteerLeft(aiPlayersSessionData[q].isLeft);
                mGameState.getMultiplayerCarAI(q).setSteerRight(aiPlayersSessionData[q].isRight);
                mGameState.getMultiplayerCarAI(q).setCurrentLap(aiPlayersSessionData[q].currentLap);
                mGameState.getMultiplayerCarAI(q).setLapPosition(aiPlayersSessionData[q].lapPosition);
            }
        }


        //human
        for(MultiplayerController::playerToData::const_iterator i = otherPlayersSessionData.begin(), j = otherPlayersSessionData.end(); 
            i != j; ++i)
        {
            PSMultiplayerCar& humanCar = mGameState.getMultiplayerCarHuman((*i).first);
            Ogre::Vector3 humanPos = humanCar.getModelNode()->getPosition();
            if(humanPos.distance((*i).second.pos) > posDiffMax)
            {
                humanCar.setModelPosition((*i).second.pos, (*i).second.rot, (*i).second.wheelpos[0], (*i).second.wheelpos[1], (*i).second.wheelpos[2], (*i).second.wheelpos[3]);
            }
            else
            {
                humanCar.setModelRotationOnly((*i).second.rot);
            }

            humanCar.setModelVelocity((*i).second.vel, (*i).second.velang);
            humanCar.setAcceleration((*i).second.isAcc);
            humanCar.setBrake((*i).second.isBrake);
            humanCar.setSteerLeft((*i).second.isLeft);
            humanCar.setSteerRight((*i).second.isRight);
            humanCar.setCurrentLap((*i).second.currentLap);
            humanCar.setLapPosition((*i).second.lapPosition);
        }
    }
    else
    {
        //human only
        for(MultiplayerController::playerToData::const_iterator i = otherPlayersSessionData.begin(), j = otherPlayersSessionData.end(); 
            i != j; ++i)
        {
            PSMultiplayerCar& humanCar = mGameState.getMultiplayerCarHuman((*i).first);
            Ogre::Vector3 humanPos = humanCar.getModelNode()->getPosition();
            if(humanPos.distance((*i).second.pos) > posDiffMax)
            {
                humanCar.setModelPosition((*i).second.pos, (*i).second.rot, (*i).second.wheelpos[0], (*i).second.wheelpos[1], (*i).second.wheelpos[2], (*i).second.wheelpos[3]);
            }
            else
            {
                humanCar.setModelRotationOnly((*i).second.rot);
            }

            humanCar.setModelVelocity((*i).second.vel, (*i).second.velang);
            humanCar.setAcceleration((*i).second.isAcc);
            humanCar.setBrake((*i).second.isBrake);
            humanCar.setSteerLeft((*i).second.isLeft);
            humanCar.setSteerRight((*i).second.isRight);
            humanCar.setCurrentLap((*i).second.currentLap);
            humanCar.setLapPosition((*i).second.lapPosition);
        }
    }
}

void BaseApp::onError(const std::string& message)
{
    mGraphics2D.addMiscPanelText("Error: [" + message + "]");
}

void BaseApp::onLapFinished()
{
    size_t lap = mGameState.getPlayerCar().getLapUtils().getCurrentLap() - 1;

    if(lap <= mGameState.getLapsCount())
    {
        std::string time = Tools::SecondsToString(mGameState.getPlayerCar().getLapUtils().getLastLapTime());
        std::string lapS = Conversions::DMToString(lap);
        mGraphics2D.addMiscPanelText("Lap finished " + Ogre::String(lapS) + ": [" + Ogre::String(time) + "]", mGameState.getSTRPowerslide().getTrackTimeTrialColor(mGameState.getTrackName()));
    }

    //race finished
    if(lap == mGameState.getLapsCount())
    {
        std::string totalTime = Tools::SecondsToString(mGameState.getPlayerCar().getLapUtils().getTotalTime());
        mGraphics2D.addMiscPanelText("Race finished: [" + Ogre::String(totalTime) + "]", mGameState.getSTRPowerslide().getTrackTimeTrialColor(mGameState.getTrackName()));

        mGameState.getPlayerCar().setDisableMouse(false);
        mCamera->setPosition(mGameState.getSTRPowerslide().getFinishCameraPos(mGameState.getTrackName()));
        mGraphics2D.setRearViewMirrorPanelShow(false);
        mGraphics2D.setVisibleFinishSign(true, mLapController.getTotalPosition(0));
        mGameState.setRaceFinished(true);
    }
}

void BaseApp::parseFile(const std::string& fileName)
{
    Ogre::DataStreamPtr pStream = Ogre::ResourceGroupManager::getSingleton().openResource( fileName.c_str(), "Lua" );
    Ogre::String mainScript = pStream->getAsString();
    mLuaError = luaL_dostring(mPipeline, mainScript.c_str());
    if(mLuaError)
    {
     std::printf("Lua Main Script File Error [%s]", fileName.c_str());
     std::string debugInfo = lua_tostring(mPipeline, -1);
     std::printf("Debug: %s", debugInfo.c_str());
     Ogre::LogManager::getSingleton().logMessage(Ogre::LML_CRITICAL, "[CRITICAL]: Lua Main Script File Error: " + debugInfo);
    }
}

static int LuaPanic(lua_State * St)
{
    std::string debugInfo = lua_tostring(mPipeline,-1);

    Ogre::LogManager::getSingleton().logMessage(Ogre::LML_CRITICAL, "[CRITICAL]: Lua Panic: " + debugInfo);

    return 0;
}

void BaseApp::initLua()
{
    mPipeline = lua_open();
    luaL_openlibs(mPipeline);
    lua_atpanic(mPipeline, LuaPanic);
}

void BaseApp::doLuaMainFile()
{
    {
        Ogre::DataStreamPtr pStream = Ogre::ResourceGroupManager::getSingleton().openResource( "Main.lua", "Lua" );
        Ogre::String mainScript = pStream->getAsString();
        mLuaError = luaL_dostring(mPipeline, mainScript.c_str());
         if(mLuaError)
         {
             std::printf("Lua Main Script File Error [Main.lua]");
             std::string debugInfo = lua_tostring(mPipeline, -1);
             std::printf("Debug: %s", debugInfo.c_str());
             Ogre::LogManager::getSingleton().logMessage(Ogre::LML_CRITICAL, "[CRITICAL]: Lua Main Script File Error: " + debugInfo);
             lua_close(mPipeline);
             mPipeline = NULL;
         }
    }
}

static int initSceneInternal(lua_State * St){
    if(baseApp) baseApp->initScene();
    return 0;
}

static int clearSceneInternal(lua_State * St){
    if(baseApp) baseApp->clearScene();
    return 0;
}

static int preloadResourceInternal(lua_State * St){

    std::string fileName = lua_tostring(St,1);
    std::string type = lua_tostring(St,2);

    if(baseApp) baseApp->preloadResource(fileName, type);
    return 0;
}

static int initTerrainInternal(lua_State * St){
    if(baseApp) baseApp->initTerrain();
    return 0;
}

static int initModelInternal(lua_State * St){
    if(baseApp) baseApp->initModel();
    return 0;
}

static int initMiscInternal(lua_State * St){
    if(baseApp) baseApp->initMisc();
    return 0;
}

static int parseFileInternal(lua_State * St){
    std::string fileName = lua_tostring(St,1);
    if(baseApp) baseApp->parseFile(fileName);
    return 0;
}

void BaseApp::registerLuaFunctions()
{
    lua_register(mPipeline,"initScene", initSceneInternal);
    lua_register(mPipeline,"clearScene", clearSceneInternal);

    lua_register(mPipeline,"preloadResource", preloadResourceInternal);

    lua_register(mPipeline,"initTerrain", initTerrainInternal);
    lua_register(mPipeline,"initModel", initModelInternal);
    lua_register(mPipeline,"initMisc", initMiscInternal);

    lua_register(mPipeline,"parseFile", parseFileInternal);
}

void BaseApp::deInitLua()
{
    if(mPipeline)
    {
        lua_close(mPipeline);
        mPipeline = NULL;
    }
}

#if defined(__ANDROID__)

Ogre::DataStreamPtr  BaseApp::openAPKFile(const Ogre::String& fileName)
{
    Ogre::DataStreamPtr stream;
    AAsset* asset = AAssetManager_open(mAssetMgr, fileName.c_str(), AASSET_MODE_BUFFER);
    if(asset)
    {
        off_t length = AAsset_getLength(asset);
        void* membuf = OGRE_MALLOC(length, Ogre::MEMCATEGORY_GENERAL);
        memcpy(membuf, AAsset_getBuffer(asset), length);
        AAsset_close(asset);
        
        stream = Ogre::DataStreamPtr(new Ogre::MemoryDataStream(membuf, length, true, true));
    }
    return stream;
}

void BaseApp::touchDown(int id, float x, float y)
{
    if(mGameState.isGamePaused())
    {
        mGameState.resetGamePaused();
    }

    Ogre::OverlayManager& om = Ogre::OverlayManager::getSingleton(); 
    Ogre::Real viewportWidth = om.getViewportWidth(); 
    Ogre::Real viewportHeight = om.getViewportHeight(); 

    Ogre::Vector2 cursorPos(x, y);
    if(cursorPos.x < 200.0f)
    {
        mTouchLeftID = id;
        baseApp->keyDown(OIS::KC_LEFT);
    }

    if(cursorPos.x > viewportWidth - 200)
    {
        mTouchRightID = id;
        baseApp->keyDown(OIS::KC_RIGHT);
    }

    if(cursorPos.y > viewportHeight - 200)
    {
        mTouchDownID = id;
        baseApp->keyDown(OIS::KC_DOWN);
    }
}

void BaseApp::touchUp(int id, float x, float y)
{
    Ogre::OverlayManager& om = Ogre::OverlayManager::getSingleton(); 
    Ogre::Real viewportWidth = om.getViewportWidth(); 

    Ogre::Vector2 cursorPos(x, y);

    if(/*cursorPos.x < 200.0f*/ id == mTouchLeftID)
    {
        baseApp->keyUp(OIS::KC_LEFT);
    }

    if(/*cursorPos.x > viewportWidth - 200*/ id == mTouchRightID)
    {
        baseApp->keyUp(OIS::KC_RIGHT);
    }

    if(/*cursorPos.x > viewportWidth - 200*/ id == mTouchDownID)
    {
        baseApp->keyUp(OIS::KC_DOWN);
    }
}

void BaseApp::androidCreate(JNIEnv * env, jobject obj, jobject assetManager)
{
    LOGI("BaseApp[androidCreate]: Begin"); 

    mSoundsProcesser.initSoundSystem();

    mRoot.reset(new Ogre::Root());
    mOverlaySystem.reset(new CustomOverlaySystem());

    AAssetManager* assetMgr = AAssetManager_fromJava(env, assetManager);
    mAssetMgr = assetMgr;

    if (assetMgr) 
    {
        mAssetMgr = assetMgr;
        Ogre::ArchiveManager::getSingleton().addArchiveFactory( new Ogre::APKFileSystemArchiveFactory(assetMgr) );
        Ogre::ArchiveManager::getSingleton().addArchiveFactory( new Ogre::APKZipArchiveFactory(assetMgr) );
    }

    //read config, parse main lua
    setupResources();

    if (mLuaError)
    {
        //return false;
    }

    LOGI(std::string("Lua error: " + Conversions::DMToString(mLuaError)).c_str());


    mRoot->installPlugin(new Ogre::GLES2Plugin());
    mRoot->installPlugin(new Ogre::OctreePlugin());
    mRoot->installPlugin(new Ogre::ParticleFXPlugin());

    const Ogre::RenderSystemList& rsList = mRoot->getAvailableRenderers();
    LOGI(std::string("Renderers amount: " + Conversions::DMToString(rsList.size())).c_str());
    Ogre::RenderSystem *selectedRenderSystem = rsList.at(0);

    mRoot->setRenderSystem(selectedRenderSystem);
    mRoot->initialise(false);

    registerLuaFunctions();

    LOGI("BaseApp[androidCreate]: End"); 
}

void BaseApp::androidDestroy()
{
    LOGI("BaseApp[androidDestroy]: Begin"); 
    LOGI("BaseApp[androidDestroy]: End"); 
}

void BaseApp::androidInitWindow(JNIEnv * env, jobject obj,  jobject surface)
{
    LOGI("BaseApp[androidInitWindow]: Begin"); 

    ANativeWindow* nativeWnd = ANativeWindow_fromSurface(env, surface);
    if (nativeWnd && mRoot.get())
    {

        if (!mWindow) 
        {
            LOGI("BaseApp[androidInitWindow]: Before window creating"); 

            Ogre::NameValuePairList opt;
            opt["externalWindowHandle"] = Ogre::StringConverter::toString((int)nativeWnd);
            mWindow = Ogre::Root::getSingleton().createRenderWindow("OgreWindow", 0, 0, false, &opt);

            mInputHandler.reset(new InputHandler(mWindow, this));

            mRoot->addSceneManagerFactory(mSMFactory.get());

            LOGI("BaseApp[androidInitWindow]: Before initialiseResourceGroup"); 

            Ogre::ResourceGroupManager::getSingleton().initialiseResourceGroup("General");

            LOGI("BaseApp[androidInitWindow]: Before CustomTrayManager"); 

            mTrayMgr = new CustomTrayManager("InterfaceName", mWindow, mInputHandler->getInputContext(), this);
            mTrayMgr->showFrameStats(OgreBites::TL_TOPRIGHT);
            //mTrayMgr->showLogo(OgreBites::TL_BOTTOMRIGHT);
            mTrayMgr->hideCursor();
            //mTrayMgr->toggleAdvancedFrameStats();

            LOGI("BaseApp[androidInitWindow]: Before create scene"); 

            //create scene
            mLuaManager.CallFunction_0_0("main", mPipeline);

            // Create any resource listeners (for loading screens)
            //createResourceListener();
            // Load resources
            LOGI("BaseApp[androidInitWindow]: Before loadResources"); 
            loadResources();

            LOGI("BaseApp[androidInitWindow]: Before create"); 
            mLuaManager.CallFunction_0_0("create", mPipeline);

            LOGI("BaseApp[androidInitWindow]: Before initLightLists"); 
            initLightLists();

            LOGI("BaseApp[androidInitWindow]: Before createFrameListener"); 
            createFrameListener();
        }
        else
        {
            LOGI("BaseApp[androidInitWindow]: Before _createInternalResources"); 

            //mLuaManager.CallFunction_0_0("main", mPipeline);
            //mLuaManager.CallFunction_0_0("create", mPipeline);

            //initLightLists();

            static_cast<Ogre::AndroidEGLWindow*>(mWindow)->_createInternalResources(nativeWnd, NULL);
            mGraphics2D.reloadTextures(mGameState);
        }
    }

    LOGI("BaseApp[androidInitWindow]: End"); 
}

void BaseApp::androidTerminateWindow()
{
    LOGI("BaseApp[androidTerminateWindow]: Begin"); 

    if(mRoot.get() && mWindow)
    {

        LOGI("BaseApp[androidTerminateWindow]: before pause"); 
        //mLuaManager.CallFunction_0_0("clear", mPipeline);

        static_cast<Ogre::AndroidEGLWindow*>(mWindow)->_destroyInternalResources();
    }

    LOGI("BaseApp[androidTerminateWindow]: End"); 
}

void BaseApp::androidRenderOneFrame(JNIEnv * env)
{
    if(mWindow != NULL && mWindow->isActive())
    {
        try
        {
            if(gVM->AttachCurrentThread(&env, NULL) < 0)
                return;

            //swap buffers problem
            //http://www.ogre3d.org/forums/viewtopic.php?f=21&t=79898
            //https://bitbucket.org/sinbad/ogre/commits/3883151e77c98e3cec9349ee4e8c091e91e96d3d

            mWindow->windowMovedOrResized();
            mRoot->renderOneFrame();

            //gVM->DetachCurrentThread();
        }catch(Ogre::RenderingAPIException ex) {}
    }
}

void BaseApp::androidPause(JNIEnv * env)
{
    LOGI("BaseApp[androidPause]: Begin"); 

    mGameState.setGamePaused();

    if(mWindow != NULL && mWindow->isActive())
    {
        try
        {
            if(gVM->AttachCurrentThread(&env, NULL) < 0)
                return;

            mWindow->windowMovedOrResized();

            LOGI("BaseApp[androidPause]: render one frame"); 
            mRoot->renderOneFrame();

        }catch(Ogre::RenderingAPIException ex) {}
    }

    LOGI("BaseApp[androidPause]: End"); 
}
#endif