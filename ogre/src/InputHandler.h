
#ifndef INPUTHANDLER_H
#define INPUTHANDLER_H

#include "includes/OgreInclude.h"
#include "includes/OISInclude.h"
#include "OgreTrays.h"
#include <OgreApplicationContext.h>

class BaseApp;

class CameraMan;

class InputHandler : public OgreBites::ApplicationContext, public OgreBites::InputListener
{
    friend class BaseApp;
private:

    BaseApp * baseApp;

    CameraMan* mCameraMan;

    static const int mKeycodesSize = 256;
    bool mKeyCodes[mKeycodesSize];

    bool noSpecialKey();

protected:

    virtual bool keyPressed( const OgreBites::KeyboardEvent &arg );
    virtual bool keyReleased( const OgreBites::KeyboardEvent &arg );
    virtual bool mouseMoved( const OgreBites::MouseMotionEvent &arg );
    virtual bool mousePressed( const OgreBites::MouseButtonEvent &arg);
    virtual bool mouseReleased( const OgreBites::MouseButtonEvent &arg);
    virtual bool touchMoved(const OgreBites::TouchFingerEvent& evt);
    virtual bool touchPressed(const OgreBites::TouchFingerEvent& evt);
    virtual bool touchReleased(const OgreBites::TouchFingerEvent& evt);

public:

    InputHandler(Ogre::RenderWindow* mWindow, BaseApp * app);
    virtual ~InputHandler(){}


    void resetCameraMenPointer(CameraMan* cameraMan);
};

#endif
