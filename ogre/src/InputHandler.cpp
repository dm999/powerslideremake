#include "pcheader.h"

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

bool InputHandler::keyPressed( const OIS::KeyEvent &arg )
{

    if(arg.key < mKeycodesSize)
        mKeyCodes[arg.key] = true;

    if (    arg.key == OIS::KC_ESCAPE && 
            !mInputContext.mKeyboard->isModifierDown(OIS::Keyboard::Shift)    &&
            !mInputContext.mKeyboard->isModifierDown(OIS::Keyboard::Ctrl)     &&
            !mInputContext.mKeyboard->isModifierDown(OIS::Keyboard::Alt)
        )
    {
        baseApp->setShutdown(true);

    }else if (arg.key == OIS::KC_F4)
    {
        
        if(mCameraMan->getCameraPositionType() != CameraPosition_Bumper && !baseApp->getGameState().getRaceFinished())
        {
            PSPlayerCar& playerCar = baseApp->getGameState().getPlayerCar();
            playerCar.setDisableMouse(!playerCar.getDisableMouse());
        }

    }else if (arg.key == OIS::KC_F5)
    {
        baseApp->quickScriptsReload();

    }else if (arg.key == OIS::KC_F6)
    {
        baseApp->scriptsReload();

    }else if (arg.key == OIS::KC_SPACE)
    {
        if(mCameraMan)
        {
            Ogre::PolygonMode pm;

            switch (mCameraMan->getCamera()->getPolygonMode())
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

            mCameraMan->getCamera()->setPolygonMode(pm);
        }
    }else if (arg.key == OIS::KC_1)
    {
        if(mCameraMan)
        {
            mCameraMan->setCameraPositionType(CameraPosition_Bumper);
        }
    }else if (arg.key == OIS::KC_2)
    {
        if(mCameraMan)
        {
            mCameraMan->setCameraPositionType(CameraPosition_ChassisA);
        }
    }else if (arg.key == OIS::KC_3)
    {
        if(mCameraMan)
        {
            mCameraMan->setCameraPositionType(CameraPosition_ChassisB);
        }
    }else if (arg.key == OIS::KC_4)
    {
        if(mCameraMan)
        {
            mCameraMan->setCameraPositionType(CameraPosition_ChassisC);
        }
    }
    else if (arg.key == OIS::KC_P)
    {
        if(baseApp->getGameState().isGamePaused())
        {
            baseApp->getGameState().resetGamePaused();
        }
        else
        {
            baseApp->getGameState().setGamePaused();
        }
    }

    baseApp->keyDown(arg.key);

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

    baseApp->keyUp(arg.key);

    return true;
}

#if !defined(__ANDROID__)
bool InputHandler::mouseMoved( const OIS::MouseEvent &arg )
{
    //if(mCameraMan)
        //mCameraMan->injectMouseMove(arg);

    return true;
}

bool InputHandler::mousePressed( const OIS::MouseEvent &arg, OIS::MouseButtonID id )
{
    //if(mCameraMan)
        //mCameraMan->injectMouseDown(arg, id);

    return true;
}

bool InputHandler::mouseReleased( const OIS::MouseEvent &arg, OIS::MouseButtonID id )
{
    //if(mCameraMan)
        //mCameraMan->injectMouseUp(arg, id);

    return true;
}
#endif

void InputHandler::capture(const Ogre::FrameEvent& evt, 
                            Ogre::SceneNode * modelNode, Ogre::Light * globalLight, Ogre::Light * shadowLight,
                            bool isDisableMouse)
{

    if (mKeyCodes[OIS::KC_MINUS] || mKeyCodes[OIS::KC_SUBTRACT])
    {
    }
    else if (mKeyCodes[OIS::KC_EQUALS] || mKeyCodes[OIS::KC_ADD])
    {
    }

    if(modelNode && mCameraMan)
    {

        if(globalLight && shadowLight)
        {
            Ogre::Vector3 lightPos = globalLight->getDerivedPosition();
            Ogre::Vector3 modelPos = modelNode->getPosition();
            Ogre::Vector3 lightDir = modelPos - lightPos;
            lightDir.normalise();

            float distanceFromCar = 40.0f;
            Ogre::Vector3 shadowLightPos = modelPos + (-lightDir) * distanceFromCar;
            shadowLight->setPosition(shadowLightPos);
            shadowLight->setDirection(lightDir);
        }

    }

    mInputContext.capture();

}