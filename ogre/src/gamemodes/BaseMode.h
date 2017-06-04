#ifndef BASEMODE_H
#define BASEMODE_H

#include "ModeContext.h"

class LoaderListener;

class BaseMode
{
public:

    BaseMode(const ModeContext& modeContext);
    virtual ~BaseMode(){}

    virtual void initData(LoaderListener* loaderListener) = 0;
    virtual void initCamera() = 0;
    virtual void clearData() = 0;

    virtual void frameStarted(const Ogre::FrameEvent &evt) = 0;
    virtual void frameRenderingQueued(const Ogre::FrameEvent& evt) = 0;

    Ogre::ColourValue pingToColor(size_t ping)const;

#if defined(__ANDROID__)
    //for UI only
    virtual void reloadTextures() = 0;
#endif

protected:

    ModeContext mModeContext;

    Ogre::SceneManager* mSceneMgr;
    Ogre::SceneNode* mMainNode;
    Ogre::Camera* mCamera;

private:

};

#endif