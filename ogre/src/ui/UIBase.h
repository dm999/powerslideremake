#ifndef UIBASE_H
#define UIBASE_H

#include <vector>

#include "../includes/OgreInclude.h"

#include "SdkTrays.h"

class CustomTrayManager;

class UIBase
{
public:
    UIBase(){}
    virtual ~UIBase(){}


    Ogre::PanelOverlayElement* createPanel(const Ogre::String& name, Ogre::Real width, Ogre::Real height, Ogre::Real left, Ogre::Real top, const Ogre::String& material);
    Ogre::PanelOverlayElement* createPanel(const Ogre::String& name, const Ogre::Vector4& pos, const Ogre::String& material);
    Ogre::TextAreaOverlayElement* createTextArea(const Ogre::String& name, Ogre::Real width, Ogre::Real height, Ogre::Real left, Ogre::Real top);

    OgreBites::Button* createButton(CustomTrayManager* trayMgr, OgreBites::TrayLocation trayLoc, const Ogre::String& name, const Ogre::String& caption, Ogre::Real width = 0);

    void destroy(CustomTrayManager* trayMgr);

protected:

    std::vector<Ogre::OverlayElement*> mCreatedElements;//to destroy automatically
    std::vector<OgreBites::Widget*> mCreatedWidgets;//to destroy automatically
};

#endif