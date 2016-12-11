#ifndef MENUMODE_H
#define MENUMODE_H

#include "BaseMode.h"

#include "../includes/CommonIncludes.h"

class UIMainMenu;

namespace MyGUI
{
    class Widget;
}

class MenuMode : 
    public BaseMode
{
public:

    MenuMode(const ModeContext& modeContext);
    virtual ~MenuMode(){}

    virtual void initData()override;
    virtual void clearData()override;

    virtual void frameStarted(const Ogre::FrameEvent &evt)override;
    virtual void frameRenderingQueued(const Ogre::FrameEvent& evt)override;

#if defined(__ANDROID__)
    //for UI only
    void reloadTextures()override;
#endif

private:

    CommonIncludes::shared_ptr<UIMainMenu> mUIMainMenu;

};

#endif