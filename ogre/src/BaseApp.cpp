
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

#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
#include "includes/MyGUI_KeyCode.h"
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

#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
    //https://github.com/MyGUI/mygui/blob/master/Common/Input/OIS/InputManager.cpp
    wchar_t translateWin32Text(MyGUI::KeyCode kc)
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

    wchar_t OISToWCharT(const OIS::KeyEvent& _arg)
    {
        wchar_t text = (wchar_t)_arg.text;
        MyGUI::KeyCode key = MyGUI::KeyCode::Enum(_arg.key);
        int scan_code = key.getValue();

        if (scan_code > 70 && scan_code < 84)
        {
            static wchar_t nums[13] = { 55, 56, 57, 45, 52, 53, 54, 43, 49, 50, 51, 48, 46 };
            text = nums[scan_code-71];
        }
        else if (key == MyGUI::KeyCode::Divide)
        {
            text = '/';
        }
        else
        {
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
            text = translateWin32Text(key);
#endif
        }

        return text;
    }
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
        selectedRenderSystem->setConfigOption("Full Screen", mGameState.isFullscreen() ? "Yes" : "No");
    
    if(configOptions.find("FSAA") != configOptions.end())
        selectedRenderSystem->setConfigOption("FSAA", mLuaManager.ReadScalarString("Window.FSAA", mPipeline));
    
    if(configOptions.find("Colour Depth") != configOptions.end())
        selectedRenderSystem->setConfigOption("Colour Depth", "32");
    
    if(configOptions.find("RTT Preferred Mode") != configOptions.end())
        selectedRenderSystem->setConfigOption("RTT Preferred Mode", "FBO");

    if(configOptions.find("Video Mode") != configOptions.end())
        selectedRenderSystem->setConfigOption("Video Mode", mGameState.getResolution());

    if(configOptions.find("VSync") != configOptions.end())
        selectedRenderSystem->setConfigOption("VSync", mGameState.isVsync() ? "Yes" : "No");

    if(configOptions.find("VSync Interval") != configOptions.end())
        selectedRenderSystem->setConfigOption("VSync Interval", mLuaManager.ReadScalarString("Window.VSyncInterval", mPipeline));



    // Show the configuration dialog and initialise the system
    // You can skip this and use root.restoreConfig() to load configuration
    // settings if you were sure there are valid ones saved in ogre.cfg
    //if(mRoot->showConfigDialog())
    {
        // If returned true, user clicked OK so initialise
        // Here we choose to let the system create a default rendering window by passing 'true'
        mWindow = mRoot->initialise(true, "Powerslide Remake");

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

            if(secName != "PF")
            {
                Ogre::ResourceGroupManager::getSingleton().addResourceLocation(archName, typeName, secName);
            }
            else
            {
                //d.polubotko: use [PF] to write player settings file, so make it not read-only
                Ogre::ResourceGroupManager::getSingleton().addResourceLocation(archName, typeName, secName, false, false);
            }
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

    mGameState.initOriginalData();
    if(!mGameState.isOriginalDataInited())
    {
        OGRE_EXCEPT(Ogre::Exception::ERR_FILE_NOT_FOUND, "Packed file not found!", "BaseApp::setup");
        return false;
    }


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

void BaseApp::restartRace()
{
    if(mGameModeSwitcher->getMode() == ModeRaceSingle)
        mGameModeSwitcher->restartRace();
}

void BaseApp::tabPressed()
{
    if(mGameModeSwitcher->isLoadPassed())
        mGameModeSwitcher->tabPressed();
}

void BaseApp::switchRenderType()
{
    if(
        mGameModeSwitcher->getMode() == ModeRaceSingle      ||
        mGameModeSwitcher->getMode() == ModeRaceTimetrial   ||
        mGameModeSwitcher->getMode() == ModeRaceChampionship
        )
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

void BaseApp::createBurnByPlayer()
{
    if(
        mGameModeSwitcher->getMode() == ModeRaceSingle  ||
        mGameModeSwitcher->getMode() == ModeRaceMulti
    )
    {
        if(mGameState.getRaceStarted())
            mGameModeSwitcher->createBurnByPlayer();
    }
}

void BaseApp::createBombByPlayer()
{
    if(
        mGameModeSwitcher->getMode() == ModeRaceSingle  ||
        mGameModeSwitcher->getMode() == ModeRaceMulti
    )
    {
        if(mGameState.getRaceStarted())
            mGameModeSwitcher->createBombByPlayer();
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

bool BaseApp::setShutdown(bool isOnEsc)
{
    if(mGameModeSwitcher->isLoadPassed())
    {
        //in race
        if(
            mGameModeSwitcher->getMode() == ModeRaceSingle          ||
            mGameModeSwitcher->getMode() == ModeRaceTimetrial       ||
            mGameModeSwitcher->getMode() == ModeRaceChampionship    ||
            mGameModeSwitcher->getMode() == ModeMenuMulti           ||
            mGameModeSwitcher->getMode() == ModeRaceMulti
        )
        {

            if(isOnEsc)
            {
                if(
                    mGameModeSwitcher->getMode() == ModeRaceSingle      ||
                    mGameModeSwitcher->getMode() == ModeRaceTimetrial   ||
                    mGameModeSwitcher->getMode() == ModeRaceChampionship
                    )
                {
                    if(getGameState().isGamePaused())
                    {
                        if(mGameModeSwitcher->getMode() == ModeRaceSingle)
                            mGameModeSwitcher->switchMode(ModeMenu);

                        if(mGameModeSwitcher->getMode() == ModeRaceTimetrial)
                            mGameModeSwitcher->switchMode(ModeMenuTimetrial);

                        if(mGameModeSwitcher->getMode() == ModeRaceChampionship)
                            mGameModeSwitcher->switchMode(ModeMenuChampionship);
                    }
                    else
                    {
                        getGameState().setGamePaused();
                    }
                }

                if(mGameModeSwitcher->getMode() == ModeMenuMulti)
                    mGameModeSwitcher->switchMode(ModeMenu);

                if(mGameModeSwitcher->getMode() == ModeRaceMulti)
                    mGameModeSwitcher->switchMode(ModeMenuMulti);
            }
        }

        if(mGameModeSwitcher->getMode() == ModeMenu)
        {
            if(mGameModeSwitcher->getSubmenuState() != State_Options_Trophies)
            {
                if(mGameModeSwitcher->isExitSubmenu())//if final dialog displayed
                {
                    if(!isOnEsc)
                        mShutDown = true;
                    else
                        mGameModeSwitcher->setTopmostSubmenu();
                }
                else
                {
                    if(isOnEsc)
                    {
                        mGameModeSwitcher->setSubmenu("Exit the game?");
                    }
                }
            }
        }

        if(mGameModeSwitcher->getMode() == ModeMenuChampionship)
        {
            if(mGameModeSwitcher->getSubmenuState() != State_FinishChampionship)
            {
                if(mGameModeSwitcher->isExitSubmenu())//if final dialog displayed
                {
                    if(!isOnEsc)
                    {
                        mGameModeSwitcher->switchMode(ModeMenu);
                        mGameModeSwitcher->setTopmostSubmenu();
                    }
                    else
                    {
                        mGameModeSwitcher->setPodiumSubmenu();
                    }
                }
                else
                {
                    if(isOnEsc)
                    {
                        mGameModeSwitcher->setSubmenu("Retire from Championship?");
                    }
                }
            }
        }
    }

    return mShutDown;
}

void BaseApp::keyDown(const OIS::KeyEvent &arg )
{
    if(mGameModeSwitcher->isLoadPassed() && 
            (
            mGameModeSwitcher->getMode() == ModeRaceSingle          ||
            mGameModeSwitcher->getMode() == ModeRaceTimetrial       ||
            mGameModeSwitcher->getMode() == ModeRaceChampionship    ||
            mGameModeSwitcher->getMode() == ModeRaceMulti
            )
        )
    {
        if(!mGameState.isGamePaused())
            mGameState.getPlayerCar().keyDown(arg.key);
    }
    
}

void BaseApp::keyUp(const OIS::KeyEvent &arg )
{
    if(mGameModeSwitcher->isLoadPassed() &&
            (
            mGameModeSwitcher->getMode() == ModeRaceSingle          ||
            mGameModeSwitcher->getMode() == ModeRaceTimetrial       ||
            mGameModeSwitcher->getMode() == ModeRaceChampionship    ||
            mGameModeSwitcher->getMode() == ModeRaceMulti
            )
        )
    {
        mGameState.getPlayerCar().keyUp(arg.key);
    }

    if(
        mGameModeSwitcher->getMode() == ModeRaceSingle      ||
        mGameModeSwitcher->getMode() == ModeRaceTimetrial   ||
        mGameModeSwitcher->getMode() == ModeRaceChampionship
        )
    {
        if(getGameState().isGamePaused() && arg.key == OIS::KC_RETURN)
        {
            getGameState().resetGamePaused();
        }
    }

    if(mGameModeSwitcher->isLoadPassed())
    {
#if OGRE_PLATFORM == OGRE_PLATFORM_LINUX
        mGameModeSwitcher->keyUp(MyGUI::KeyCode::Enum(arg.key), mInputHandler->getInputContext().mKeyboard->getAsString(arg.key)[0]);
#else
        mGameModeSwitcher->keyUp(MyGUI::KeyCode::Enum(arg.key), OISToWCharT(arg));
#endif
    }
}

#if !defined(__ANDROID__)
void BaseApp::mouseMoved(const OIS::MouseEvent &arg)
{
    if(mGameModeSwitcher->isLoadPassed())
    {
        //if(mGameModeSwitcher->getMode() == ModeMenu || mGameModeSwitcher->getMode() == ModeMenuMulti)
        {
            mTrayMgr->injectMouseMove(arg);
            mGameModeSwitcher->mouseMoved(Ogre::Vector2(arg.state.X.abs, arg.state.Y.abs));
        }

        if(mGameState.getInputType() == itMouse)
        {
            if(mGameModeSwitcher->isLoadPassed() && 
                    (
                    mGameModeSwitcher->getMode() == ModeRaceSingle          ||
                    mGameModeSwitcher->getMode() == ModeRaceTimetrial       ||
                    mGameModeSwitcher->getMode() == ModeRaceChampionship    ||
                    mGameModeSwitcher->getMode() == ModeRaceMulti
                    )
                )
            {
                if(!mGameState.isGamePaused())
                {
                    Ogre::Real winWidth = mWindow->getWidth();
                    mGameState.getPlayerCar().mouseMoved(Ogre::Vector2(arg.state.X.abs, arg.state.Y.abs), winWidth);
                }
            }
        }
    }
}
void BaseApp::mousePressed(const OIS::MouseEvent &arg, OIS::MouseButtonID id)
{
    if(mGameModeSwitcher->isLoadPassed())
    {
        //if(mGameModeSwitcher->getMode() == ModeMenu || mGameModeSwitcher->getMode() == ModeMenuMulti)
        {
            mTrayMgr->injectMouseDown(arg, id);
            mGameModeSwitcher->mousePressed(Ogre::Vector2(arg.state.X.abs, arg.state.Y.abs));
        }

        if(mGameState.getInputType() == itMouse)
        {
            if(mGameModeSwitcher->isLoadPassed() && 
                    (
                        mGameModeSwitcher->getMode() == ModeRaceSingle          ||
                        mGameModeSwitcher->getMode() == ModeRaceTimetrial       ||
                        mGameModeSwitcher->getMode() == ModeRaceChampionship    ||
                        mGameModeSwitcher->getMode() == ModeRaceMulti
                    )
                )
            {
                if(!mGameState.isGamePaused())
                    mGameState.getPlayerCar().mousePressed(id);
            }
        }
    }
}
void BaseApp::mouseReleased(const OIS::MouseEvent &arg, OIS::MouseButtonID id)
{
    if(mGameModeSwitcher->isLoadPassed())
    {
        //if(mGameModeSwitcher->getMode() == ModeMenu || mGameModeSwitcher->getMode() == ModeMenuMulti)
        {
            mTrayMgr->injectMouseUp(arg, id);
            mGameModeSwitcher->mouseReleased(Ogre::Vector2(arg.state.X.abs, arg.state.Y.abs));
        }

        if(mGameState.getInputType() == itMouse)
        {
            if(mGameModeSwitcher->isLoadPassed() && 
                    (
                    mGameModeSwitcher->getMode() == ModeRaceSingle          ||
                    mGameModeSwitcher->getMode() == ModeRaceTimetrial       ||
                    mGameModeSwitcher->getMode() == ModeRaceChampionship    ||
                    mGameModeSwitcher->getMode() == ModeRaceMulti
                    )
                )
            {
                if(!mGameState.isGamePaused())
                    mGameState.getPlayerCar().mouseReleased(id);
            }
        }

        if(
            mGameModeSwitcher->getMode() == ModeRaceSingle      ||
            mGameModeSwitcher->getMode() == ModeRaceTimetrial   ||
            mGameModeSwitcher->getMode() == ModeRaceChampionship
            )
        {
            if(getGameState().isGamePaused())
            {
                getGameState().resetGamePaused();
            }
        }
    }
}
#else
void BaseApp::touchMoved(const OIS::MultiTouchEvent& arg)
{
    LOGI("BaseApp[touchMoved]: Begin"); 

    if(mGameModeSwitcher->isLoadPassed())
    {
        //if(mGameModeSwitcher->getMode() == ModeMenu || mGameModeSwitcher->getMode() == ModeMenuMulti)
        {
            mTrayMgr->injectMouseMove(arg);
            mGameModeSwitcher->mouseMoved(Ogre::Vector2(arg.state.X.abs, arg.state.Y.abs));
        }
    }

    LOGI("BaseApp[touchMoved]: End"); 
}
void BaseApp::touchPressed(const OIS::MultiTouchEvent& arg)
{
    LOGI("BaseApp[touchPressed]: Begin"); 

    if(mGameModeSwitcher->isLoadPassed())
    {
        //if(mGameModeSwitcher->getMode() == ModeMenu || mGameModeSwitcher->getMode() == ModeMenuMulti)
        {
            mTrayMgr->injectMouseDown(arg);
            mGameModeSwitcher->mousePressed(Ogre::Vector2(arg.state.X.abs, arg.state.Y.abs));
        }
    }

    LOGI("BaseApp[touchPressed]: End"); 
}
void BaseApp::touchReleased(const OIS::MultiTouchEvent& arg)
{
    LOGI("BaseApp[touchReleased]: Begin"); 

    if(mGameModeSwitcher->isLoadPassed())
    {
        //if(mGameModeSwitcher->getMode() == ModeMenu || mGameModeSwitcher->getMode() == ModeMenuMulti)
        {
            LOGI("BaseApp[touchReleased]: before injectMouseUp");
            LOGI("BaseApp[touchPressed]: %d %d", arg.state.X.abs, arg.state.Y.abs); 
            mTrayMgr->injectMouseUp(arg);
            mGameModeSwitcher->mouseReleased(Ogre::Vector2(arg.state.X.abs, arg.state.Y.abs));
        }

        if(
            mGameModeSwitcher->getMode() == ModeRaceSingle      ||
            mGameModeSwitcher->getMode() == ModeRaceTimetrial   ||
            mGameModeSwitcher->getMode() == ModeRaceChampionship
            )
        {
            if(getGameState().isGamePaused())
            {
                getGameState().resetGamePaused();
            }
        }
    }

    LOGI("BaseApp[touchReleased]: End"); 
}

void BaseApp::androidShowKeyboard()
{
    LOGI("BaseApp[androidShowKeyboard]: Begin"); 

    JNIEnv *env;
    gVM->AttachCurrentThread(&env, NULL);
    jmethodID method = env->GetMethodID(activityClass, "showKeyboard", "()V");
    env->CallVoidMethod(activityObj, method);

    LOGI("BaseApp[androidShowKeyboard]: End"); 
}

void BaseApp::androidHideKeyboard()
{
    LOGI("BaseApp[androidHideKeyboard]: Begin"); 

    JNIEnv *env;
    gVM->AttachCurrentThread(&env, NULL);
    jmethodID method = env->GetMethodID(activityClass, "hideKeyboard", "()V");
    env->CallVoidMethod(activityObj, method);

    LOGI("BaseApp[androidHideKeyboard]: End"); 
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
        mGameState,
        this
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
    static int prevX = x;
    static int prevY = y;
    state.X.abs = (int)x;
    state.X.rel = state.X.abs - prevX;
    state.Y.abs = (int)y;
    state.Y.rel = state.Y.abs - prevY;
    state.Z.abs = 0;
    state.Z.rel = 0;
    prevX = x;
    prevY = y;
    state.width = viewportWidth;
    state.height = viewportHeight;
    state.touchType = OIS::MT_Pressed;
    touchPressed(OIS::MultiTouchEvent(NULL, state));

    if(cursorPos.x < (200.0f * viewportWidth / 1280.0f))
    {
        mTouchLeftID = id;
        keyDown(OIS::KeyEvent(NULL, OIS::KC_LEFT, 0));
    }

    if(cursorPos.x > viewportWidth - (200.0f * viewportWidth / 1280.0f))
    {
        mTouchRightID = id;
        keyDown(OIS::KeyEvent(NULL, OIS::KC_RIGHT, 0));
    }

    if(cursorPos.y > viewportHeight - (200.0f * viewportHeight / 1024.0f))
    {
        mTouchDownID = id;
        keyDown(OIS::KeyEvent(NULL, OIS::KC_DOWN, 0));
    }
}

void BaseApp::touchUp(int id, float x, float y)
{
    Ogre::OverlayManager& om = Ogre::OverlayManager::getSingleton(); 
    Ogre::Real viewportWidth = om.getViewportWidth(); 
    Ogre::Real viewportHeight = om.getViewportHeight(); 

    OIS::MultiTouchState state;
    static int prevX = x;
    static int prevY = y;
    state.X.abs = (int)x;
    state.X.rel = state.X.abs - prevX;
    state.Y.abs = (int)y;
    state.Y.rel = state.Y.abs - prevY;
    state.Z.abs = 0;
    state.Z.rel = 0;
    prevX = x;
    prevY = y;
    state.width = viewportWidth;
    state.height = viewportHeight;
    state.touchType = OIS::MT_Released;
    touchReleased(OIS::MultiTouchEvent(NULL, state));

    if(id == mTouchLeftID)
    {
        keyUp(OIS::KeyEvent(NULL, OIS::KC_LEFT, 0));
    }

    if(id == mTouchRightID)
    {
        keyUp(OIS::KeyEvent(NULL, OIS::KC_RIGHT, 0));
    }

    if(id == mTouchDownID)
    {
        keyUp(OIS::KeyEvent(NULL, OIS::KC_DOWN, 0));
    }
}

void BaseApp::touchMove(int id, float x, float y)
{
    Ogre::OverlayManager& om = Ogre::OverlayManager::getSingleton(); 
    Ogre::Real viewportWidth = om.getViewportWidth(); 
    Ogre::Real viewportHeight = om.getViewportHeight(); 

    OIS::MultiTouchState state;
    static int prevX = x;
    static int prevY = y;
    state.X.abs = (int)x;
    state.X.rel = state.X.abs - prevX;
    state.Y.abs = (int)y;
    state.Y.rel = state.Y.abs - prevY;
    state.Z.abs = 0;
    state.Z.rel = 0;
    prevX = x;
    prevY = y;
    state.width = viewportWidth;
    state.height = viewportHeight;
    state.touchType = OIS::MT_Moved;

    touchMoved(OIS::MultiTouchEvent(NULL, state));
}

bool BaseApp::androidOnBack()
{
    return setShutdown(true);
}

void BaseApp::androidCreate(JNIEnv * env, jobject obj, jobject assetManager, const std::string& dataDir)
{
    LOGI("BaseApp[androidCreate]: Begin"); 

    mGameState.setDataDir(dataDir);

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

        AConfiguration* config = AConfiguration_new();
        AConfiguration_fromAssetManager(config, mAssetMgr); 

        if (!mWindow) 
        {
            LOGI("BaseApp[androidInitWindow]: Before window creating"); 

            Ogre::NameValuePairList opt;
            opt["externalWindowHandle"] = Ogre::StringConverter::toString((int)nativeWnd);
            opt["androidConfig"] = Ogre::StringConverter::toString((int)config); 
            mWindow = Ogre::Root::getSingleton().createRenderWindow("Powerslide Remake", 0, 0, false, &opt);

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

            static_cast<Ogre::AndroidEGLWindow*>(mWindow)->_createInternalResources(nativeWnd, config);

            //http://www.ogre3d.org/forums/viewtopic.php?f=21&t=78172&hilit=android+texture
            {
                Ogre::TextureManager& m = Ogre::TextureManager::getSingleton();
                std::vector<std::string> tempTextures;

                Ogre::TextureManager::ResourceMapIterator rmi = m.getResourceIterator();
                for(Ogre::TextureManager::ResourceHandleMap::const_iterator i = rmi.begin(); i != rmi.end(); ++i)
                {
                    //LOGI("BaseApp[androidInitWindow]: %s %s %d %d", i->second->getName().c_str(), i->second->getGroup().c_str(), i->second->isReloadable(), i->second->getLoadingState()); 
                    if( i->second->getGroup() == TEMP_RESOURCE_GROUP_NAME || 
                        !i->second->isReloadable())//UIBackground
                    {
                        tempTextures.push_back(i->second->getName().c_str());
                    }
                }

                for(size_t q = 0; q < tempTextures.size(); ++q)
                {
                    if(tempTextures[q] != "RearViewMirrorTex")
                        m.remove(tempTextures[q]);
                }
            }

            mGameModeSwitcher->reloadTextures();

            {
                Ogre::MaterialManager& m = Ogre::MaterialManager::getSingleton();

                Ogre::MaterialManager::ResourceMapIterator rmi = m.getResourceIterator();
                for(Ogre::MaterialManager::ResourceHandleMap::const_iterator i = rmi.begin(); i != rmi.end(); ++i)
                {
                    if(i->second->getGroup() == TEMP_RESOURCE_GROUP_NAME)
                    {
                        unsigned short textUnitStates = static_cast<Ogre::Material*>(i->second.get())->getTechnique(0)->getPass(0)->getNumTextureUnitStates();
                        for(unsigned short q = 0; q < textUnitStates; ++q)
                        {
                            std::string textureName = static_cast<Ogre::Material*>(i->second.get())->getTechnique(0)->getPass(0)->getTextureUnitState(q)->getTextureName();
                            try{
                                static_cast<Ogre::Material*>(i->second.get())->getTechnique(0)->getPass(0)->getTextureUnitState(q)->setTexture(Ogre::TextureManager::getSingleton().getByName(textureName, TEMP_RESOURCE_GROUP_NAME));
                            }
                            catch(...){}
                        }

                        i->second->reload();
                    }

                    //UIBackground
                    if(i->second->getGroup() == Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME)
                    {
                        bool toReload = false;
                        unsigned short textUnitStates = static_cast<Ogre::Material*>(i->second.get())->getTechnique(0)->getPass(0)->getNumTextureUnitStates();
                        for(unsigned short q = 0; q < textUnitStates; ++q)
                        {
                            std::string textureName = static_cast<Ogre::Material*>(i->second.get())->getTechnique(0)->getPass(0)->getTextureUnitState(q)->getTextureName();
                            if(textureName.find("Loaders/Texture") != std::string::npos)
                            {
                                toReload = true;
                                try{
                                    static_cast<Ogre::Material*>(i->second.get())->getTechnique(0)->getPass(0)->getTextureUnitState(q)->setTexture(Ogre::TextureManager::getSingleton().getByName(textureName, Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME));
                                }
                                catch(...){}
                            }
                        }
                        if(toReload)
                            i->second->reload();
                    }
                }
            }

        }

        AConfiguration_delete(config); 
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

bool BaseApp::androidRenderOneFrame(JNIEnv * env)
{
    if(mWindow != NULL && mWindow->isActive())
    {
        try
        {
            if(gVM->AttachCurrentThread(&env, NULL) < 0)
                return true;

            //swap buffers problem
            //http://www.ogre3d.org/forums/viewtopic.php?f=21&t=79898
            //https://bitbucket.org/sinbad/ogre/commits/3883151e77c98e3cec9349ee4e8c091e91e96d3d

            mWindow->windowMovedOrResized();
            mRoot->renderOneFrame();

            //gVM->DetachCurrentThread();
        }catch(Ogre::RenderingAPIException ex) {}
    }

    return mShutDown;
}

void BaseApp::androidPause(JNIEnv * env)
{
    LOGI("BaseApp[androidPause]: Begin"); 

    if(
        mGameModeSwitcher->getMode() == ModeRaceSingle      ||
        mGameModeSwitcher->getMode() == ModeRaceTimetrial   ||
        mGameModeSwitcher->getMode() == ModeRaceChampionship
        )
    {
        mGameState.setGamePaused();
    }

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