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

    MenuMode(const ModeContext& modeContext, const GameMode gameMode, SinglePlayerMenuStates state);
    virtual ~MenuMode(){}

    void frameStarted(const Ogre::FrameEvent &evt)override;

#if defined(__ANDROID__)
    //for UI only
    void reloadTextures()override;
#endif

    void keyUp(MyGUI::KeyCode _key, wchar_t _char )override;

    void mousePressed(const Ogre::Vector2& pos)override;
    void mouseReleased(const Ogre::Vector2& pos, OIS::MouseButtonID id)override;
    void mouseMoved(const Ogre::Vector2& pos)override;

    SinglePlayerMenuStates getSubmenuState() const;
    bool isExitSubmenu()const override;
    void setSubmenu(const std::string& title)override;
    void setTopmostSubmenu()override;
    void setDefaultBackground(bool isSwitchState)override;
    void setPodiumSubmenu()override;

protected:

    void doInitData(LoaderListener* loaderListener)override;
    void doClearData()override;

private:

    CommonIncludes::shared_ptr<UIMainMenu> mUIMainMenu;

};

#endif