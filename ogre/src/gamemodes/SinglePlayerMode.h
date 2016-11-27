#ifndef SINGLEPLAYERMODE_H
#define SINGLEPLAYERMODE_H

#include "BaseRaceMode.h"

class SinglePlayerMode : public BaseRaceMode
{
public:

    SinglePlayerMode(const ModeContext& modeContext);
    virtual ~SinglePlayerMode(){}

    virtual void frameStarted(const Ogre::FrameEvent &evt)override;
    virtual void frameRenderingQueued(const Ogre::FrameEvent& evt)override;
};


#endif