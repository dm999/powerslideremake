#ifndef CUSTOMFONTMANAGER
#define CUSTOMFONTMANAGER

#include "../includes/OgreInclude.h"
#include "OgreOverlaySystem.h"

#include "OgreFontManager.h"

class CustomFontManager : public Ogre::FontManager
{
public:
    CustomFontManager(){}
    virtual ~CustomFontManager(){}

protected:
    Ogre::Resource* createImpl(const Ogre::String& name, Ogre::ResourceHandle handle, 
               const Ogre::String& group, bool isManual, Ogre::ManualResourceLoader* loader, 
            const Ogre::NameValuePairList* params);
};

#endif