#ifndef CUSTOMFONTMANAGER
#define CUSTOMFONTMANAGER

#include "OgreInclude.h"
#include "OgreOverlaySystem.h"

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