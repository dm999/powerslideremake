
#include "CustomFontManager.h"
#include "CustomFont.h"

Ogre::Resource* CustomFontManager::createImpl(const Ogre::String& name, Ogre::ResourceHandle handle, 
               const Ogre::String& group, bool isManual, Ogre::ManualResourceLoader* loader, 
            const Ogre::NameValuePairList* params)
{
    return OGRE_NEW CustomFont(this, name, handle, group, isManual, loader);
}