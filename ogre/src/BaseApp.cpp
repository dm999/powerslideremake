#include "pcheader.h"

#include "BaseApp.h"

#include "InputHandler.h"
#include "tools/OgreTools.h"
#include "tools/Conversions.h"

#include "lua/Lua_Include.h"

#include "customs/CustomSceneManager.h"
#include "customs/CustomTrayManager.h"
#include "customs/CustomOverlaySystem.h"

#include "gamemodes/SinglePlayerMode.h"
#include "gamemodes/MultiPlayerMode.h"

#include "BulletCollision/CollisionDispatch/btInternalEdgeUtility.h"

#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
#include "res/resource.h"
#endif

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

BaseApp::BaseApp() :
    mWindow(0),
    mResourcesCfg(Ogre::StringUtil::BLANK),
    mShutDown(false),
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

BaseApp::~BaseApp()
{
    if(mPlayerMode.get())
        mPlayerMode->clearData();
    mPlayerMode.reset();

    baseApp = NULL;
    deInitLua();

    mGraphics2D.destroy();

    mTrayMgr.reset();

    //Remove ourself as a Window listener
    Ogre::WindowEventUtilities::removeWindowEventListener(mWindow, this);
    windowClosed(mWindow);

    mRoot->removeSceneManagerFactory(mSMFactory.get());
    mOverlaySystem.reset();
    mRoot.reset();

    mSoundsProcesser.deinitSounds();
    mSoundsProcesser.deInitSoundSystem();
}

bool BaseApp::configure()
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

void BaseApp::createFrameListener()
{

    //Set initial mouse clipping size
    windowResized(mWindow);

    //Register as a Window listener
#if !defined(__ANDROID__)
    Ogre::WindowEventUtilities::addWindowEventListener(mWindow, this);
#endif


    mGraphics2D.createRearViewMirrorPanel(mTrayMgr.get(), mGameState.getMirrorEnabled());
    mGraphics2D.load(mTrayMgr.get(), mGameState);

    mRoot->addFrameListener(this);
}

void BaseApp::setupResources()
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

void BaseApp::go()
{
    if (!setup())
        return;

    mRoot->startRendering();
}

bool BaseApp::setup()
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

    Ogre::ResourceGroupManager::getSingleton().initialiseResourceGroup("General");

    mTrayMgr.reset(new CustomTrayManager("InterfaceName", mWindow, mInputHandler->getInputContext(), this));
    mTrayMgr->showFrameStats(OgreBites::TL_TOPRIGHT);
    //mTrayMgr->showLogo(OgreBites::TL_BOTTOMRIGHT);
    mTrayMgr->hideCursor();
    //mTrayMgr->toggleAdvancedFrameStats();

    mPlayerMode.reset(new SinglePlayerMode(ModeContext(
        mRoot.get(), mWindow,
        mInputHandler.get(),
        mTrayMgr.get(), mOverlaySystem.get(),
        mPipeline,
        mGameState, mSoundsProcesser, mGraphics2D
        )));

    if(mPlayerMode.get())
        mPlayerMode->initData();

    createFrameListener();

    //mTrayMgr->showCursor("Test/Cursor");
    //Ogre::OverlayContainer* cursor = mTrayMgr->getCursorContainer();
    //cursor->setWidth(0.0001f);

    return true;
}

bool BaseApp::frameStarted(const Ogre::FrameEvent &evt)
{
    if(mWindow->isClosed())
        return false;

    if(mShutDown)
        return false;

    if(mPlayerMode.get())
        mPlayerMode->frameStarted(evt);

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

    if(mPlayerMode.get())
        mPlayerMode->frameRenderingQueued(evt);

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

void BaseApp::windowFocusChange(Ogre::RenderWindow* rw)
{
    (void) rw;
}

void BaseApp::scriptsReload()
{
    doLuaMainFile();
    if(mPlayerMode.get())
        mPlayerMode->reload();
}

void BaseApp::quickScriptsReload()
{
    if(mPlayerMode.get())
        mPlayerMode->restart();
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

#if !defined(__ANDROID__)
void BaseApp::mouseMoved(const OIS::MouseEvent &arg)
{
    mTrayMgr->injectMouseMove(arg);
}
void BaseApp::mousePressed(const OIS::MouseEvent &arg, OIS::MouseButtonID id)
{
    mTrayMgr->injectMouseDown(arg, id);
}
void BaseApp::mouseReleased(const OIS::MouseEvent &arg, OIS::MouseButtonID id)
{
    mTrayMgr->injectMouseUp(arg, id);
}
#endif

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

void BaseApp::processCollision(btManifoldPoint& cp, const btCollisionObjectWrapper* colObj0Wrap, const btCollisionObjectWrapper* colObj1Wrap, int triIndex)
{
    if(mPlayerMode.get())
        mPlayerMode->processCollision(cp, colObj0Wrap, colObj1Wrap, triIndex);
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

static int parseFileInternal(lua_State * St){
    std::string fileName = lua_tostring(St,1);
    if(baseApp) baseApp->parseFile(fileName);
    return 0;
}

void BaseApp::registerLuaFunctions()
{
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

            mTrayMgr.reset(new CustomTrayManager("InterfaceName", mWindow, mInputHandler->getInputContext(), this));
            mTrayMgr->showFrameStats(OgreBites::TL_TOPRIGHT);
            //mTrayMgr->showLogo(OgreBites::TL_BOTTOMRIGHT);
            mTrayMgr->hideCursor();
            //mTrayMgr->toggleAdvancedFrameStats();

            LOGI("BaseApp[androidInitWindow]: Before create scene"); 

            if(mPlayerMode.get())
                mPlayerMode->initData();

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