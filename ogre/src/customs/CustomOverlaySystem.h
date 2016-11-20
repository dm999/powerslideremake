
#ifndef CUSTOMOVERLAYSYSTEM
#define CUSTOMOVERLAYSYSTEM

#include "../includes/OgreInclude.h"
#include "OgreOverlaySystem.h"

class CustomFontManager;

class CustomOverlaySystem : public Ogre::OverlayAlloc , public Ogre::RenderQueueListener
{
public :

    CustomOverlaySystem();

    virtual ~CustomOverlaySystem();

    /// @see RenderQueueListener
    virtual void renderQueueStarted(Ogre::uint8 queueGroupId, const Ogre::String& invocation, bool& skipThisInvocation);
    private:
        Ogre::OverlayManager* mOverlayManagerCustom;
        CustomFontManager* mFontManagerCustom;

#if OGRE_PROFILING
    Ogre::OverlayProfileSessionListener* mProfileListenerCustom;
#endif
};

#endif