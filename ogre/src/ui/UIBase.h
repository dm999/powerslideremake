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


    Ogre::PanelOverlayElement* createEmptyPanel(const Ogre::String& name, Ogre::Real width, Ogre::Real height, Ogre::Real left, Ogre::Real top);
    Ogre::PanelOverlayElement* createEmptyPanel(const Ogre::String& name, const Ogre::Vector4& pos);
    Ogre::PanelOverlayElement* createPanel(const Ogre::String& name, Ogre::Real width, Ogre::Real height, Ogre::Real left, Ogre::Real top, const Ogre::String& material);
    Ogre::PanelOverlayElement* createPanel(const Ogre::String& name, const Ogre::Vector4& pos, const Ogre::String& material);
    Ogre::TextAreaOverlayElement* createTextArea(const Ogre::String& name, Ogre::Real width, Ogre::Real height, Ogre::Real left, Ogre::Real top);
    Ogre::TextAreaOverlayElement* createTextArea(const Ogre::String& name, const Ogre::Vector4& pos);

    OgreBites::Label* createLabel(CustomTrayManager* trayMgr, OgreBites::TrayLocation trayLoc, const Ogre::String& name, const Ogre::String& caption, Ogre::Real width = 0);
    OgreBites::Button* createButton(CustomTrayManager* trayMgr, OgreBites::TrayLocation trayLoc, const Ogre::String& name, const Ogre::String& caption, Ogre::Real width = 0);
    OgreBites::TextBox* createTextBox(CustomTrayManager* trayMgr, OgreBites::TrayLocation trayLoc, const Ogre::String& name, const Ogre::String& caption, Ogre::Real width = 0, Ogre::Real height = 0);
    OgreBites::SelectMenu* createSelectMenu(CustomTrayManager* trayMgr, OgreBites::TrayLocation trayLoc, const Ogre::String& name, const Ogre::String& caption, Ogre::Real width, unsigned int maxItemsShown, const Ogre::StringVector& items);

    virtual void destroy(CustomTrayManager* trayMgr);

    void setVisible(bool isVisible);//to hide for different viewports - arrow issue

private:

    std::vector<bool> mCreatedElementsVisible;
    std::vector<Ogre::OverlayElement*> mCreatedElements;//to destroy automatically

    std::vector<bool> mCreatedWidgetsVisible;
    std::vector<OgreBites::Widget*> mCreatedWidgets;//to destroy automatically
};

#endif