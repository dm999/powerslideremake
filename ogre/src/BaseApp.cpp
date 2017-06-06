#include "pcheader.h"

#include "BaseApp.h"

#include "InputHandler.h"
#include "tools/OgreTools.h"
#include "tools/Conversions.h"

#include "CameraMan.h"

#include "lua/Lua_Include.h"

#include "customs/CustomSceneManager.h"
#include "customs/CustomTrayManager.h"
#include "customs/CustomOverlaySystem.h"
#include "gamelogic/GameModeSwitcher.h"

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

    #define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO, "OGRE", __VA_ARGS__))
    #define LOGE(...) ((void)__android_log_print(ANDROID_LOG_ERROR, "OGRE", __VA_ARGS__)) 

    #include "Android/OgreAndroidEGLWindow.h"
    #include "Android/OgreAPKFileSystemArchive.h"
    #include "Android/OgreAPKZipArchive.h"
#endif


namespace
{
    lua_State * mPipeline = NULL;
    BaseApp * baseApp = NULL;

#if 0
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
    //https://github.com/MyGUI/mygui/blob/master/Common/Input/OIS/InputManager.cpp
    MyGUI::Char translateWin32Text(MyGUI::KeyCode kc)
    {
        static WCHAR deadKey = 0;

        BYTE keyState[256];
        HKL  layout = GetKeyboardLayout(0);
        if ( GetKeyboardState(keyState) == 0 )
            return 0;

        int code = *((int*)&kc);
        unsigned int vk = MapVirtualKeyEx((UINT)code, 3, layout);
        if ( vk == 0 )
            return 0;

        WCHAR buff[3] = { 0, 0, 0 };
        int ascii = ToUnicodeEx(vk, (UINT)code, keyState, buff, 3, 0, layout);
        if (ascii == 1 && deadKey != '\0' )
        {
            // A dead key is stored and we have just converted a character key
            // Combine the two into a single character
            WCHAR wcBuff[3] = { buff[0], deadKey, '\0' };
            WCHAR out[3];

            deadKey = '\0';
            if (FoldStringW(MAP_PRECOMPOSED, (LPWSTR)wcBuff, 3, (LPWSTR)out, 3))
                return out[0];
        }
        else if (ascii == 1)
        {
            // We have a single character
            deadKey = '\0';
            return buff[0];
        }
        else if (ascii == 2)
        {
            // Convert a non-combining diacritical mark into a combining diacritical mark
            // Combining versions range from 0x300 to 0x36F; only 5 (for French) have been mapped below
            // http://www.fileformat.info/info/unicode/block/combining_diacritical_marks/images.htm
            switch (buff[0])
            {
            case 0x5E: // Circumflex accent:
                deadKey = 0x302;
                break;
            case 0x60: // Grave accent:
                deadKey = 0x300;
                break;
            case 0xA8: // Diaeresis:
                deadKey = 0x308;
                break;
            case 0xB4: // Acute accent:
                deadKey = 0x301;
                break;
            case 0xB8: // Cedilla:
                deadKey = 0x327;
                break;
            default:
                deadKey = buff[0];
                break;
            }
        }

        return 0;
    }
#endif
#endif
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
    mTrayMgr->setListener(NULL);

    mGameModeSwitcher.reset();

    baseApp = NULL;
    deInitLua();

    mTrayMgr.reset();

    //Remove ourself as a Window listener
    Ogre::WindowEventUtilities::removeWindowEventListener(mWindow, this);
    windowClosed(mWindow);

    mRoot->removeSceneManagerFactory(mSMFactory.get());
    mOverlaySystem.reset();
    mRoot.reset();

#ifndef NO_OPENAL
    mSoundsProcesser.deinitSounds();
    mSoundsProcesser.deInitSoundSystem();
#endif
}

bool BaseApp::configure()
{
#if defined(__ANDROID__)
    Ogre::Root::getSingleton().installPlugin(new Ogre::GLES2Plugin());
#else
    Ogre::Root::getSingleton().installPlugin(new Ogre::GLPlugin());
#endif
    Ogre::Root::getSingleton().installPlugin(new Ogre::ParticleFXPlugin());

    const Ogre::RenderSystemList& rsList = mRoot->getAvailableRenderers();
    Ogre::RenderSystem *selectedRenderSystem = rsList.at(0);

    mRoot->setRenderSystem(selectedRenderSystem);
    Ogre::ConfigOptionMap& configOptions = selectedRenderSystem->getConfigOptions();
    
    if(configOptions.find("Full Screen") != configOptions.end())
        selectedRenderSystem->setConfigOption("Full Screen", mLuaManager.ReadScalarString("Window.IsFullScreen", mPipeline));
    
    if(configOptions.find("FSAA") != configOptions.end())
        selectedRenderSystem->setConfigOption("FSAA", mLuaManager.ReadScalarString("Window.FSAA", mPipeline));
    
    if(configOptions.find("Colour Depth") != configOptions.end())
        selectedRenderSystem->setConfigOption("Colour Depth", "32");
    
    if(configOptions.find("RTT Preferred Mode") != configOptions.end())
        selectedRenderSystem->setConfigOption("RTT Preferred Mode", "FBO");

    if(configOptions.find("Video Mode") != configOptions.end())
        selectedRenderSystem->setConfigOption("Video Mode", mLuaManager.ReadScalarString("Window.Resolution", mPipeline));

    if(configOptions.find("VSync") != configOptions.end())
        selectedRenderSystem->setConfigOption("VSync", mLuaManager.ReadScalarString("Window.VSync", mPipeline));

    if(configOptions.find("VSync Interval") != configOptions.end())
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
#ifndef NO_OPENAL
    mSoundsProcesser.initSoundSystem();
#endif

    mRoot.reset(new Ogre::Root());
    mOverlaySystem.reset(new CustomOverlaySystem());

    //read config, parse main lua
    setupResources();

    if (mLuaError )
    {
        return false;
    }

    if(mLuaManager.ReadScalarBool("Main.LevelLogDetailed", mPipeline))
        Ogre::LogManager::getSingleton().setLogDetail(Ogre::LL_BOREME);


    //setup renderer, create window, init input
    bool carryOn = configure();
    if (!carryOn) return false;

    registerLuaFunctions();

    Ogre::ResourceGroupManager::getSingleton().initialiseResourceGroup("General");

    mTrayMgr.reset(new CustomTrayManager("InterfaceName", mWindow, mInputHandler->getInputContext()));
    //mTrayMgr->showFrameStats(OgreBites::TL_TOPRIGHT);
    //mTrayMgr->showLogo(OgreBites::TL_BOTTOMRIGHT);
    mTrayMgr->hideCursor();
    //mTrayMgr->toggleAdvancedFrameStats();

    //mPlatform.reset(new MyGUI::OgrePlatform());
    //mGUI.reset(new MyGUI::Gui());

    mGameState.initOriginalData();
    if(!mGameState.isOriginalDataInited())
    {
        OGRE_EXCEPT(Ogre::Exception::ERR_FILE_NOT_FOUND, "Packed file not found!", "BaseApp::setup");
        return false;
    }

    mGameModeSwitcher.reset(new GameModeSwitcher(createModeContext()));

    createFrameListener();

    return true;
}

bool BaseApp::frameStarted(const Ogre::FrameEvent &evt)
{
    if(mWindow->isClosed())
        return false;

    if(mShutDown)
        return false;

    mGameModeSwitcher->frameStarted(evt);

    return true;
}

bool BaseApp::frameEnded(const Ogre::FrameEvent &evt)
{
    if(mWindow->isClosed())
        return false;

    if(mShutDown)
        return false;

    mGameModeSwitcher->frameEnded();

    return true;
}

bool BaseApp::frameRenderingQueued(const Ogre::FrameEvent& evt)
{
    if(mWindow->isClosed())
        return false;

    if(mShutDown)
        return false;

    mGameModeSwitcher->frameRenderingQueued(evt);

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

void BaseApp::quickScriptsReload()
{
    if(mGameModeSwitcher->getMode() == ModeRaceSingle)
        mGameModeSwitcher->restartRace();
}

void BaseApp::enablePause()
{
    if(mGameModeSwitcher->getMode() == ModeRaceSingle)
    {
        if(getGameState().isGamePaused())
        {
            getGameState().resetGamePaused();
        }
        else
        {
            getGameState().setGamePaused();
        }
    }
}

void BaseApp::tabPressed()
{
    mGameModeSwitcher->tabPressed();
}

void BaseApp::switchRenderType()
{
    if(mGameModeSwitcher->getMode() == ModeRaceSingle)
    {
        Ogre::PolygonMode pm;

        switch (mInputHandler->mCameraMan->getCamera()->getPolygonMode())
        {
        case Ogre::PM_SOLID:
            //newVal = "Wireframe";
            pm = Ogre::PM_WIREFRAME;
            break;
        case Ogre::PM_WIREFRAME:
            //newVal = "Points";
            pm = Ogre::PM_POINTS;
            break;
        default:
            //newVal = "Solid";
            pm = Ogre::PM_SOLID;
        }

        mInputHandler->mCameraMan->getCamera()->setPolygonMode(pm);
    }
}

void BaseApp::dropCamera()
{
    if(
        mGameModeSwitcher->getMode() == ModeRaceSingle  ||
        mGameModeSwitcher->getMode() == ModeRaceMulti
    )
    {
        if(
            mInputHandler->mCameraMan->getCameraPositionType() != CameraPosition_Bumper && 
            !mGameState.getRaceFinished()
        )
        {
            PSPlayerCar& playerCar = mGameState.getPlayerCar();
            playerCar.setDisableMouse(!playerCar.getDisableMouse());
        }
    }
}

void BaseApp::setShutdown(bool shutdown)
{
    if(
        mGameModeSwitcher->getMode() == ModeRaceSingle  ||
        mGameModeSwitcher->getMode() == ModeMenuMulti   ||
        mGameModeSwitcher->getMode() == ModeRaceMulti
    )
    {
        if(
            mGameModeSwitcher->getMode() == ModeRaceSingle  ||
            mGameModeSwitcher->getMode() == ModeMenuMulti
            )
            mGameModeSwitcher->switchMode(ModeMenu);

        if(mGameModeSwitcher->getMode() == ModeRaceMulti)
            mGameModeSwitcher->switchMode(ModeMenuMulti);
    }
    if(mGameModeSwitcher->getMode() == ModeMenu)
    {
        mShutDown = true;
    }
}

void BaseApp::keyDown(const OIS::KeyEvent &arg )
{
    if(!mGameState.isGamePaused())
        mGameState.getPlayerCar().keyDown(arg.key);

#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
    //if(mGameModeSwitcher->getMode() == ModeMenu || mGameModeSwitcher->getMode() == ModeMenuMulti || mGameModeSwitcher->getMode() == ModeRaceMulti)
        //MyGUI::InputManager::getInstance().injectKeyPress(MyGUI::KeyCode::Enum(arg.key), translateWin32Text(MyGUI::KeyCode::Enum(arg.key)));
#else
    //if(mGameModeSwitcher->getMode() == ModeMenu || mGameModeSwitcher->getMode() == ModeMenuMulti || mGameModeSwitcher->getMode() == ModeRaceMulti)
        //MyGUI::InputManager::getInstance().injectKeyPress(MyGUI::KeyCode::Enum(arg.key), arg.text);
#endif
    
}

void BaseApp::keyUp(const OIS::KeyEvent &arg )
{
    mGameState.getPlayerCar().keyUp(arg.key);

    //if(mGameModeSwitcher->getMode() == ModeMenu || mGameModeSwitcher->getMode() == ModeMenuMulti || mGameModeSwitcher->getMode() == ModeRaceMulti)
        //MyGUI::InputManager::getInstance().injectKeyRelease(MyGUI::KeyCode::Enum(arg.key));
}

#if !defined(__ANDROID__)
void BaseApp::mouseMoved(const OIS::MouseEvent &arg)
{
    if(mGameModeSwitcher->getMode() == ModeMenu || mGameModeSwitcher->getMode() == ModeMenuMulti)
    {
        mTrayMgr->injectMouseMove(arg);

        //MyGUI::InputManager::getInstance().injectMouseMove(arg.state.X.abs, arg.state.Y.abs, arg.state.Z.abs);
    }
}
void BaseApp::mousePressed(const OIS::MouseEvent &arg, OIS::MouseButtonID id)
{
    if(mGameModeSwitcher->getMode() == ModeMenu || mGameModeSwitcher->getMode() == ModeMenuMulti)
    {
        mTrayMgr->injectMouseDown(arg, id);

        //MyGUI::InputManager::getInstance().injectMousePress(arg.state.X.abs, arg.state.Y.abs, MyGUI::MouseButton::Enum(id));
    }
}
void BaseApp::mouseReleased(const OIS::MouseEvent &arg, OIS::MouseButtonID id)
{
    if(mGameModeSwitcher->getMode() == ModeMenu || mGameModeSwitcher->getMode() == ModeMenuMulti)
    {
        mTrayMgr->injectMouseUp(arg, id);

        //MyGUI::InputManager::getInstance().injectMouseRelease(arg.state.X.abs, arg.state.Y.abs, MyGUI::MouseButton::Enum(id));
    }
}
#else
void BaseApp::mouseMoved(const OIS::MultiTouchEvent& arg)
{
    if(mGameModeSwitcher->getMode() == ModeMenu || mGameModeSwitcher->getMode() == ModeMenuMulti)
    {
        mTrayMgr->injectMouseMove(arg);
    }
}
void BaseApp::mousePressed(const OIS::MultiTouchEvent& arg)
{
    if(mGameModeSwitcher->getMode() == ModeMenu || mGameModeSwitcher->getMode() == ModeMenuMulti)
    {
        mTrayMgr->injectMouseDown(arg);
    }
}
void BaseApp::mouseReleased(const OIS::MultiTouchEvent& arg)
{
    if(mGameModeSwitcher->getMode() == ModeMenu || mGameModeSwitcher->getMode() == ModeMenuMulti)
    {
        mTrayMgr->injectMouseUp(arg);
    }
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
    mGameModeSwitcher->processCollision(cp, colObj0Wrap, colObj1Wrap, triIndex);
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

ModeContext BaseApp::createModeContext()
{
    return ModeContext(
        mRoot.get(), mWindow,
        mInputHandler.get(),
        mTrayMgr.get(), mOverlaySystem.get(),
        mPipeline,
        mGameState
#ifndef NO_OPENAL
        ,mSoundsProcesser
#endif
    );
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

    OIS::MultiTouchState state;
    state.X.abs = x;
    state.Y.abs = y;
    state.Z.abs = 0;
    mousePressed(OIS::MultiTouchEvent(NULL, state));

    if(cursorPos.x < 200.0f)
    {
        mTouchLeftID = id;
        keyDown(OIS::KeyEvent(NULL, OIS::KC_LEFT, 0));
    }

    if(cursorPos.x > viewportWidth - 200)
    {
        mTouchRightID = id;
        keyDown(OIS::KeyEvent(NULL, OIS::KC_RIGHT, 0));
    }

    if(cursorPos.y > viewportHeight - 200)
    {
        mTouchDownID = id;
        keyDown(OIS::KeyEvent(NULL, OIS::KC_DOWN, 0));
    }
}

void BaseApp::touchUp(int id, float x, float y)
{
    Ogre::OverlayManager& om = Ogre::OverlayManager::getSingleton(); 
    Ogre::Real viewportWidth = om.getViewportWidth(); 

    Ogre::Vector2 cursorPos(x, y);

    OIS::MultiTouchState state;
    state.X.abs = x;
    state.Y.abs = y;
    state.Z.abs = 0;
    mousePressed(OIS::MultiTouchEvent(NULL, state));
    mouseReleased(OIS::MultiTouchEvent(NULL, state));

    if(/*cursorPos.x < 200.0f*/ id == mTouchLeftID)
    {
        keyUp(OIS::KeyEvent(NULL, OIS::KC_LEFT, 0));
    }

    if(/*cursorPos.x > viewportWidth - 200*/ id == mTouchRightID)
    {
        keyUp(OIS::KeyEvent(NULL, OIS::KC_RIGHT, 0));
    }

    if(/*cursorPos.x > viewportWidth - 200*/ id == mTouchDownID)
    {
        keyUp(OIS::KeyEvent(NULL, OIS::KC_DOWN, 0));
    }
}

void BaseApp::touchMove(int id, float x, float y)
{
    Ogre::OverlayManager& om = Ogre::OverlayManager::getSingleton(); 
    Ogre::Real viewportWidth = om.getViewportWidth(); 

    OIS::MultiTouchState state;
    state.X.abs = x;
    state.Y.abs = y;
    state.Z.abs = 0;

    mouseMoved(OIS::MultiTouchEvent(NULL, state));
}

bool BaseApp::androidOnBack()
{
    bool ret = false;

    if(
        mGameModeSwitcher->getMode() == ModeRaceSingle  ||
        mGameModeSwitcher->getMode() == ModeMenuMulti   ||
        mGameModeSwitcher->getMode() == ModeRaceMulti
    )
    {
        if(
            mGameModeSwitcher->getMode() == ModeRaceSingle  ||
            mGameModeSwitcher->getMode() == ModeMenuMulti
            )
            mGameModeSwitcher->switchMode(ModeMenu);

        if(mGameModeSwitcher->getMode() == ModeRaceMulti)
            mGameModeSwitcher->switchMode(ModeMenuMulti);
    }

    if(mGameModeSwitcher->getMode() == ModeMenu)
    {
        ret = true;
    }

    return ret;
}

void BaseApp::androidCreate(JNIEnv * env, jobject obj, jobject assetManager)
{
    LOGI("BaseApp[androidCreate]: Begin"); 

#ifndef NO_OPENAL
    mSoundsProcesser.initSoundSystem();
#endif

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

            mTrayMgr.reset(new CustomTrayManager("InterfaceName", mWindow, mInputHandler->getInputContext()));
            //mTrayMgr->showFrameStats(OgreBites::TL_TOPRIGHT);
            //mTrayMgr->showLogo(OgreBites::TL_BOTTOMRIGHT);
            mTrayMgr->hideCursor();
            //mTrayMgr->toggleAdvancedFrameStats();

            LOGI("BaseApp[androidInitWindow]: Before create scene"); 

            mGameState.initOriginalData();
            if(!mGameState.isOriginalDataInited())
            {
                OGRE_EXCEPT(Ogre::Exception::ERR_FILE_NOT_FOUND, "Packed file not found!", "BaseApp::setup");
                return;
            }

            mGameModeSwitcher.reset(new GameModeSwitcher(createModeContext()));

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
            mGameModeSwitcher->reloadTextures();
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