
#include <sstream>

#include "InputHandler.h"
#include "BaseApp.h"

#include "lua/DMLuaManager.h"

#include "CameraMan.h"

InputHandler::InputHandler(Ogre::RenderWindow* mWindow, BaseApp * app) :
    baseApp(app),
#if !defined(__ANDROID__)
    mInputManager(0),
#endif
    mCameraMan(0)
{
#if !defined(__ANDROID__)
    OIS::ParamList pl;
    size_t windowHnd = 0;
    std::ostringstream windowHndStr;

    memset(mKeyCodes, 0, sizeof(bool) * mKeycodesSize);

    mWindow->getCustomAttribute("WINDOW", &windowHnd);
    windowHndStr << windowHnd;
    pl.insert(std::make_pair(std::string("WINDOW"), windowHndStr.str()));

    mInputManager = OIS::InputManager::createInputSystem( pl );

    mInputContext.mKeyboard = static_cast<OIS::Keyboard*>(mInputManager->createInputObject( OIS::OISKeyboard, true ));
    mInputContext.mKeyboard->setEventCallback(this);

    mInputContext.mMouse = static_cast<OIS::Mouse*>(mInputManager->createInputObject( OIS::OISMouse, true ));
    mInputContext.mMouse->setEventCallback(this);
    
    //centered mouse
    //http://www.ogre3d.org/tikiwiki/tiki-index.php?page=Initial+Mouse+Position+With+OIS+and+CEGUI
    OIS::MouseState &mutableMouseState = const_cast<OIS::MouseState &>(mInputContext.mMouse->getMouseState());
    mutableMouseState.X.abs = mWindow->getWidth() / 2;
    mutableMouseState.Y.abs = mWindow->getHeight() / 2;
#endif
}

void InputHandler::detach()
{
#if !defined(__ANDROID__)
    //Only close for window that created OIS (the main window in these demos)
    //if( rw == mWindow )
    {
        if( mInputManager )
        {
            if(mInputContext.mKeyboard)
            {
                mInputContext.mKeyboard->setEventCallback(NULL);
                mInputManager->destroyInputObject( mInputContext.mKeyboard );
            }

            if(mInputContext.mMouse)
            {
                mInputContext.mMouse->setEventCallback(NULL); 
                mInputManager->destroyInputObject( mInputContext.mMouse );
            }

            OIS::InputManager::destroyInputSystem(mInputManager);
            mInputManager = NULL;
        }
    }

#endif
}

void InputHandler::resetCameraMenPointer(CameraMan* cameraMan)
{
    mCameraMan = cameraMan;
}

bool InputHandler::noSpecialKey()
{
    return !mInputContext.mKeyboard->isModifierDown(OIS::Keyboard::Shift)   &&
    !mInputContext.mKeyboard->isModifierDown(OIS::Keyboard::Ctrl)           &&
    !mInputContext.mKeyboard->isModifierDown(OIS::Keyboard::Alt);
}

bool InputHandler::keyPressed( const OIS::KeyEvent &arg )
{

    if(arg.key < mKeycodesSize)
        mKeyCodes[arg.key] = true;

    if (arg.key == OIS::KC_ESCAPE)
    {
        if(noSpecialKey())
            baseApp->setShutdown(true);
    }
    else if (arg.key == OIS::KC_RETURN || arg.key == OIS::KC_NUMPADENTER)
    {
        if(noSpecialKey())
            baseApp->setShutdown(false);
    }
    else if (baseApp->getGameState().checkKeyCode(arg.key, InputKeyMapping::kmBurn))
    {
        if(noSpecialKey())
            baseApp->createBurnByPlayer();
    }
    else if (baseApp->getGameState().checkKeyCode(arg.key, InputKeyMapping::kmBomb))
    {
        if(noSpecialKey())
            baseApp->createBombByPlayer();
    }
    else if (baseApp->getGameState().checkKeyCode(arg.key, InputKeyMapping::kmNitro))
    {
        baseApp->enableNitro();
    }
    else if (baseApp->getGameState().checkKeyCode(arg.key, InputKeyMapping::kmDropCam))
    {
        if(noSpecialKey())
            baseApp->dropCamera();
    }
    else if(baseApp->getGameState().checkKeyCode(arg.key, InputKeyMapping::kmSticky))
    {
        baseApp->enableSticky();
    }
    else if(baseApp->getGameState().checkKeyCode(arg.key, InputKeyMapping::kmSpider))
    {
        baseApp->enableSpider();
    }
    else if(baseApp->getGameState().checkKeyCode(arg.key, InputKeyMapping::kmICBM))
    {
        baseApp->enableICBM();
    }
    else if(baseApp->getGameState().checkKeyCode(arg.key, InputKeyMapping::kmApollo))
    {
        baseApp->enableApollo();
    }
    else if(baseApp->getGameState().checkKeyCode(arg.key, InputKeyMapping::kmLunar))
    {
        baseApp->enableLunar();
    }
    else if(baseApp->getGameState().checkKeyCode(arg.key, InputKeyMapping::kmJump))
    {
        baseApp->enableJump();
    }
    else if(baseApp->getGameState().checkKeyCode(arg.key, InputKeyMapping::kmBlast))
    {
        baseApp->enableBlast();
    }
    else if(baseApp->getGameState().checkKeyCode(arg.key, InputKeyMapping::kmGlider))
    {
        baseApp->enableGlider();
    }
    else if (arg.key == OIS::KC_F5)
    {
        baseApp->restartRace();
    }
    else if (arg.key == OIS::KC_GRAVE)//tilda
    {
        if(mCameraMan)
        {
            baseApp->switchRenderType();
        }
    }
    else if (baseApp->getGameState().checkKeyCode(arg.key, InputKeyMapping::kmView))
    {
        if(mCameraMan)
        {
            int camPos = static_cast<int>(mCameraMan->getCameraPositionType());
            ++camPos;
            if(camPos > CameraPosition_ChassisC)
            {
                camPos = CameraPosition_Bumper;
            }
            mCameraMan->setCameraPositionType(static_cast<CameraPositions>(camPos));
            baseApp->getGameState().setCameraPositionType(static_cast<CameraPositions>(camPos));
            baseApp->getGameState().savePlayerData();
        }
    }
    else if (arg.key == OIS::KC_TAB)
    {
        if(noSpecialKey())
            baseApp->tabPressed();
    }

    baseApp->keyDown(arg);

    //if(mCameraMan)
        //mCameraMan->injectKeyDown(arg);

    return true;
}

bool InputHandler::keyReleased( const OIS::KeyEvent &arg )
{
    if(arg.key < mKeycodesSize)
        mKeyCodes[arg.key] = false;

    //if(mCameraMan)
        //mCameraMan->injectKeyUp(arg);

    baseApp->keyUp(arg);

    return true;
}

#if !defined(__ANDROID__)
bool InputHandler::mouseMoved( const OIS::MouseEvent &arg )
{
    baseApp->mouseMoved(arg);
    //if(mCameraMan)
        //mCameraMan->injectMouseMove(arg);

    return true;
}

bool InputHandler::mousePressed( const OIS::MouseEvent &arg, OIS::MouseButtonID id )
{
    baseApp->mousePressed(arg, id);
    //if(mCameraMan)
        //mCameraMan->injectMouseDown(arg, id);


    if (baseApp->getGameState().checkKeyCode(id, InputKeyMapping::kmBurn))
    {
        if (noSpecialKey())
            baseApp->createBurnByPlayer();
    }
    else if (baseApp->getGameState().checkKeyCode(id, InputKeyMapping::kmBomb))
    {
        if (noSpecialKey())
            baseApp->createBombByPlayer();
    }
    else if (baseApp->getGameState().checkKeyCode(id, InputKeyMapping::kmNitro))
    {
        baseApp->enableNitro();
    }
    else if (baseApp->getGameState().checkKeyCode(id, InputKeyMapping::kmDropCam))
    {
        if (noSpecialKey())
            baseApp->dropCamera();
    }
    else if (baseApp->getGameState().checkKeyCode(id, InputKeyMapping::kmView))
    {
        if (mCameraMan)
        {
            int camPos = static_cast<int>(mCameraMan->getCameraPositionType());
            ++camPos;
            if (camPos > CameraPosition_ChassisC)
            {
                camPos = CameraPosition_Bumper;
            }
            mCameraMan->setCameraPositionType(static_cast<CameraPositions>(camPos));
            baseApp->getGameState().setCameraPositionType(static_cast<CameraPositions>(camPos));
            baseApp->getGameState().savePlayerData();
        }
    }

    return true;
}

bool InputHandler::mouseReleased( const OIS::MouseEvent &arg, OIS::MouseButtonID id )
{
    baseApp->mouseReleased(arg, id);
    //if(mCameraMan)
        //mCameraMan->injectMouseUp(arg, id);

    return true;
}
#endif