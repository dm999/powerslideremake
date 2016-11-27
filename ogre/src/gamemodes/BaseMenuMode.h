#ifndef BASEMENUMODE_H
#define BASEMENUMODE_H

#include "BaseMode.h"

class BaseMenuMode : 
    public BaseMode
{
public:

    BaseMenuMode(const ModeContext& modeContext);
    virtual ~BaseMenuMode(){}

    virtual void initData()override;
    virtual void clearData()override;

    virtual void frameStarted(const Ogre::FrameEvent &evt)override;
    virtual void frameRenderingQueued(const Ogre::FrameEvent& evt)override;

private:

};

#endif