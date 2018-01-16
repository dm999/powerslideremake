#ifndef CUSTOMFONT
#define CUSTOMFONT

#include "../includes/OgreInclude.h"
#include "OgreOverlaySystem.h"

#include "OgreFont.h"

class CustomFont : public Ogre::Font
{
public:
    CustomFont(Ogre::ResourceManager* creator, const Ogre::String& name, Ogre::ResourceHandle handle,
               const Ogre::String& group, bool isManual = false, Ogre::ManualResourceLoader* loader = 0);

    virtual ~CustomFont(){}

protected:

    virtual void loadImpl();

};

#endif