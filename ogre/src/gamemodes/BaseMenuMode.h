#ifndef BASEMENUMODE_H
#define BASEMENUMODE_H

#include "BaseMode.h"

class BaseMenuMode : public BaseMode
{
public:

    BaseMenuMode(const ModeContext& modeContext);
    virtual ~BaseMenuMode(){}

    void initData()override;
    void clearData()override;

    void frameRenderingQueued(const Ogre::FrameEvent& evt)override;

    void recalculateCharacterNames();

protected:

    virtual void doInitData() = 0;
    virtual void doClearData() = 0;
};

#endif