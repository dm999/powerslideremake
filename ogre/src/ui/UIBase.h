#ifndef UIBASE_H
#define UIBASE_H

#include <vector>

#include "../includes/OgreInclude.h"

#include "SdkTrays.h"

class UIBase
{
public:
    UIBase(){}
    virtual ~UIBase(){}


    Ogre::PanelOverlayElement* createPanel(const Ogre::String& name, Ogre::Real width, Ogre::Real height, Ogre::Real left, Ogre::Real top, const Ogre::String& material);
    Ogre::PanelOverlayElement* createPanel(const Ogre::String& name, const Ogre::Vector4& pos, const Ogre::String& material);
    Ogre::TextAreaOverlayElement* createTextArea(const Ogre::String& name, Ogre::Real width, Ogre::Real height, Ogre::Real left, Ogre::Real top);

    void destroy();

protected:

    std::vector<Ogre::OverlayElement*> mCreatedElements;//to destroy automatically
};

#endif