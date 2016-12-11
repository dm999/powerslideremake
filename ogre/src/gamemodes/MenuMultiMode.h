#ifndef MENUMULTIMODE_H
#define MENUMULTIMODE_H

#include "BaseMode.h"

#include "../includes/CommonIncludes.h"

class UIMainMenuMulti;

namespace MyGUI
{
    class Widget;
}

class MenuMultiMode : 
    public BaseMode
{
public:

    MenuMultiMode(const ModeContext& modeContext);
    virtual ~MenuMultiMode(){}

    virtual void initData()override;
    virtual void clearData()override;

    virtual void frameStarted(const Ogre::FrameEvent &evt)override;
    virtual void frameRenderingQueued(const Ogre::FrameEvent& evt)override;

#if defined(__ANDROID__)
    //for UI only
    void reloadTextures()override;
#endif

private:

    CommonIncludes::shared_ptr<UIMainMenuMulti> mUIMainMenuMulti;

};

#endif