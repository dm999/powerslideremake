#ifndef BASEMODE_H
#define BASEMODE_H

#include "ModeContext.h"

class BaseMode
{
public:

    BaseMode(const ModeContext& modeContext);
    virtual ~BaseMode(){}

    virtual void initData() = 0;
    virtual void clearData() = 0;

    virtual void frameStarted(const Ogre::FrameEvent &evt) = 0;
    virtual void frameRenderingQueued(const Ogre::FrameEvent& evt) = 0;

protected:

    ModeContext mModeContext;

    Ogre::SceneManager* mSceneMgr;
    Ogre::SceneNode* mMainNode;
    Ogre::Camera* mCamera;

private:

};

#endif