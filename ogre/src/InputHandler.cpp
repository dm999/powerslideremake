
#include <sstream>

#include "InputHandler.h"
#include "BaseApp.h"

#include "CameraMan.h"

InputHandler::InputHandler(Ogre::RenderWindow* mWindow, BaseApp * app) :
    baseApp(app),
    mCameraMan(0)
{
    OgreBites::ApplicationContext::setup();

    addInputListener(this);

#if !defined(__ANDROID__)
    memset(mKeyCodes, 0, sizeof(bool) * mKeycodesSize);

    //centered mouse
    //http://www.ogre3d.org/tikiwiki/tiki-index.php?page=Initial+Mouse+Position+With+OIS+and+CEGUI
    //OIS::MouseState &mutableMouseState = const_cast<OIS::MouseState &>(mInputContext.mMouse->getMouseState());
    //mutableMouseState.X.abs = mWindow->getWidth() / 2;
    //mutableMouseState.Y.abs = mWindow->getHeight() / 2;
#endif
}

void InputHandler::resetCameraMenPointer(CameraMan* cameraMan)
{
    mCameraMan = cameraMan;
}

bool InputHandler::noSpecialKey()
{
    return false;/*
    return !mInputContext.mKeyboard->isModifierDown(OIS::Keyboard::Shift)   &&
    !mInputContext.mKeyboard->isModifierDown(OIS::Keyboard::Ctrl)           &&
    !mInputContext.mKeyboard->isModifierDown(OIS::Keyboard::Alt);*/
}

bool InputHandler::keyPressed( const OgreBites::KeyboardEvent &arg )
{

    OgreBites::Keycode key = arg.keysym.sym;

    if(key < mKeycodesSize)
        mKeyCodes[key] = true;

    if (key == SDLK_ESCAPE)
    {
        if(noSpecialKey())
            baseApp->setShutdown(true);
    }
    else if (key == SDLK_RETURN/* || key == SDLK_NUMPADENTER*/)
    {
        if(noSpecialKey())
            baseApp->setShutdown(false);
    }
    else if (key == SDLK_F1)
    {
        if(noSpecialKey())
            baseApp->createBurnByPlayer();
    }
    else if (key == SDLK_F2)
    {
        if(noSpecialKey())
            baseApp->createBombByPlayer();
    }
    else if (key == SDLK_F4)
    {
        if(noSpecialKey())
            baseApp->dropCamera();
    }
    else if (key == SDLK_F5)
    {
        baseApp->restartRace();
    }
    else if (key == SDLK_SPACE)
    {
        if(mCameraMan)
        {
            baseApp->switchRenderType();
        }
    }
    else if (key == SDLK_1)
    {
        if(mCameraMan)
        {
            mCameraMan->setCameraPositionType(CameraPosition_Bumper);
        }
    }
    else if (key == SDLK_2)
    {
        if(mCameraMan)
        {
            mCameraMan->setCameraPositionType(CameraPosition_ChassisA);
        }
    }
    else if (key == SDLK_3)
    {
        if(mCameraMan)
        {
            mCameraMan->setCameraPositionType(CameraPosition_ChassisB);
        }
    }
    else if (key == SDLK_4)
    {
        if(mCameraMan)
        {
            mCameraMan->setCameraPositionType(CameraPosition_ChassisC);
        }
    }
    else if (key == SDLK_p)
    {
        baseApp->enablePause();
    }
    else if (key == SDLK_TAB)
    {
        if(noSpecialKey())
            baseApp->tabPressed();
    }

    baseApp->keyDown(arg);

    //if(mCameraMan)
        //mCameraMan->injectKeyDown(arg);

    return true;
}

bool InputHandler::keyReleased( const OgreBites::KeyboardEvent &arg )
{
    OgreBites::Keycode key = arg.keysym.sym;

    if(key < mKeycodesSize)
        mKeyCodes[key] = false;

    //if(mCameraMan)
        //mCameraMan->injectKeyUp(arg);

    baseApp->keyUp(arg);

    return true;
}

bool InputHandler::mouseMoved( const OgreBites::MouseMotionEvent &arg )
{
    baseApp->mouseMoved(arg);
    //if(mCameraMan)
        //mCameraMan->injectMouseMove(arg);

    return true;
}

bool InputHandler::mousePressed( const OgreBites::MouseButtonEvent &arg)
{
    baseApp->mousePressed(arg);
    //if(mCameraMan)
        //mCameraMan->injectMouseDown(arg, id);

    return true;
}

bool InputHandler::mouseReleased( const OgreBites::MouseButtonEvent &arg)
{
    baseApp->mouseReleased(arg);
    //if(mCameraMan)
        //mCameraMan->injectMouseUp(arg, id);

    return true;
}

bool InputHandler::touchMoved(const OgreBites::TouchFingerEvent &arg)
{
    baseApp->touchMoved(arg);

    return true;
}

bool InputHandler::touchPressed(const OgreBites::TouchFingerEvent &arg)
{
    baseApp->touchPressed(arg);

    return true;
}

bool InputHandler::touchReleased(const OgreBites::TouchFingerEvent &arg)
{
    baseApp->touchReleased(arg);

    return true;
}