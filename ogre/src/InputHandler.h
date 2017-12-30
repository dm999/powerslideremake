
#ifndef INPUTHANDLER_H
#define INPUTHANDLER_H

#include "includes/OgreInclude.h"
#include "includes/OISInclude.h"
#include "SdkTrays.h"

class BaseApp;

class CameraMan;

class InputHandler : public OIS::KeyListener
#if !defined(__ANDROID__)
    , public OIS::MouseListener
#endif
{
    friend class BaseApp;
private:

    BaseApp * baseApp;

#if !defined(__ANDROID__)
    OIS::InputManager* mInputManager;
#endif
    OgreBites::InputContext mInputContext;


    CameraMan* mCameraMan;

    static const int mKeycodesSize = 256;
    bool mKeyCodes[mKeycodesSize];

    bool noSpecialKey();

protected:

    virtual bool keyPressed( const OIS::KeyEvent &arg );
    virtual bool keyReleased( const OIS::KeyEvent &arg );
    
#if !defined(__ANDROID__)
    virtual bool mouseMoved( const OIS::MouseEvent &arg );
    virtual bool mousePressed( const OIS::MouseEvent &arg, OIS::MouseButtonID id );
    virtual bool mouseReleased( const OIS::MouseEvent &arg, OIS::MouseButtonID id );
#endif

public:

    InputHandler(Ogre::RenderWindow* mWindow, BaseApp * app);
    virtual ~InputHandler(){}

    /**
     * consume input for controllers during loading process
     */
    void capture(){mInputContext.capture();}

    void detach();

    OgreBites::InputContext& getInputContext(){return mInputContext;}


    void resetCameraMenPointer(CameraMan* cameraMan);
};

#endif
