#ifndef BASEMENUMODE_H
#define BASEMENUMODE_H

#include "BaseMode.h"

#include "../includes/MyGUI_KeyCode.h"

class LoaderListener;

class BaseMenuMode : public BaseMode
{
public:

    BaseMenuMode(const ModeContext& modeContext, bool isInitialCreation);
    virtual ~BaseMenuMode(){}

    void initData(LoaderListener* loaderListener)override;
    void initCamera()override;
    void clearData()override;

    virtual void keyUp(MyGUI::KeyCode _key, wchar_t _char ) = 0;

    virtual void mousePressed(const Ogre::Vector2& pos) = 0;
    virtual void mouseReleased(const Ogre::Vector2& pos, OIS::MouseButtonID id) = 0;
    virtual void mouseMoved(const Ogre::Vector2& pos) = 0;

    virtual bool isExitSubmenu()const = 0;
    virtual void setSubmenu(const std::string& title) = 0;
    virtual void setTopmostSubmenu() = 0;
    virtual void setDefaultBackground(bool isSwitchState) = 0;
    virtual void setPodiumSubmenu() = 0;

    void frameRenderingQueued(const Ogre::FrameEvent& evt)override;

    void recalculateCharacterNames(const std::vector<std::string>& playersCharacters);

protected:

    virtual void doInitData(LoaderListener* loaderListener) = 0;
    virtual void doClearData() = 0;

    virtual void customFrameRenderingQueued(){}
};

#endif