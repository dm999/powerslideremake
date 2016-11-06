#include "pcheader.h"

#include "CustomOverlaySystem.h"
#include "CustomFontManager.h"

CustomOverlaySystem::CustomOverlaySystem()
{
		mOverlayManagerCustom = OGRE_NEW Ogre::OverlayManager();
		mOverlayManagerCustom->addOverlayElementFactory(OGRE_NEW Ogre::PanelOverlayElementFactory());

		mOverlayManagerCustom->addOverlayElementFactory(OGRE_NEW Ogre::BorderPanelOverlayElementFactory());

		mOverlayManagerCustom->addOverlayElementFactory(OGRE_NEW Ogre::TextAreaOverlayElementFactory());

		mFontManagerCustom = OGRE_NEW CustomFontManager();
#if OGRE_PROFILING
		mProfileListenerCustom = new Ogre::OverlayProfileSessionListener();
		Ogre::Profiler* prof = Ogre::Profiler::getSingletonPtr();
        if (prof)
		{
			prof->addListener(mProfileListenerCustom);
		}
#endif
}
CustomOverlaySystem::~CustomOverlaySystem()
{
#if OGRE_PROFILING
	Ogre::Profiler* prof = Ogre::Profiler::getSingletonPtr();
    if (prof)
	{
		prof->removeListener(mProfileListenerCustom);
	}
	delete mProfileListenerCustom;
#endif
	OGRE_DELETE mOverlayManagerCustom;
	OGRE_DELETE mFontManagerCustom;
}

void CustomOverlaySystem::renderQueueStarted(Ogre::uint8 queueGroupId, const Ogre::String& invocation, 
		bool& skipThisInvocation)
{
	if(queueGroupId == Ogre::RENDER_QUEUE_OVERLAY)
	{
		Ogre::Viewport* vp = Ogre::Root::getSingletonPtr()->getRenderSystem()->_getViewport();
		if(vp != NULL)
		{
			Ogre::SceneManager* sceneMgr = vp->getCamera()->getSceneManager();
			if (vp->getOverlaysEnabled() && sceneMgr->_getCurrentRenderStage() != Ogre::SceneManager::IRS_RENDER_TO_TEXTURE)
			{
				Ogre::OverlayManager::getSingleton()._queueOverlaysForRendering(vp->getCamera(), sceneMgr->getRenderQueue(), vp);
			}
		}
	}
}
