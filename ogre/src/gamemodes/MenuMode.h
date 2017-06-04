#ifndef MENUMODE_H
#define MENUMODE_H

#include "BaseMenuMode.h"

#include "../includes/CommonIncludes.h"

class UIMainMenu;

namespace MyGUI
{
    class Widget;
}

class MenuMode : 
    public BaseMenuMode
{
public:

    MenuMode(const ModeContext& modeContext);
    virtual ~MenuMode(){}

    void frameStarted(const Ogre::FrameEvent &evt)override{}

#if defined(__ANDROID__)
    //for UI only
    void reloadTextures()override;
#endif

protected:

    void doInitData(LoaderListener* loaderListener)override;
    void doClearData()override;

private:

    CommonIncludes::shared_ptr<UIMainMenu> mUIMainMenu;

};

#endif