#include "../pcheader.h"

#include "BaseMenuMode.h"

#include "../customs/CustomTrayManager.h"
#include "../customs/CustomOverlaySystem.h"

#include "../InputHandler.h"

#include "../ui/UIMainMenu.h"

BaseMenuMode::BaseMenuMode(const ModeContext& modeContext) :
    BaseMode(modeContext),
    mUIMainMenu(new UIMainMenu())
{}

void BaseMenuMode::initData()
{
    Ogre::ResourceGroupManager::getSingleton().createResourceGroup(TEMP_RESOURCE_GROUP_NAME);

    mSceneMgr = mModeContext.mRoot->createSceneManager(Ogre::ST_GENERIC);
    mSceneMgr->addRenderQueueListener(mModeContext.mOverlaySystem);
    mMainNode = mSceneMgr->getRootSceneNode()->createChildSceneNode();
    mCamera = mSceneMgr->createCamera("MenuCam");
    Ogre::Viewport * mViewPort = mModeContext.mWindow->addViewport(mCamera);
    mCamera->setProjectionType(Ogre::PT_ORTHOGRAPHIC);
    mCamera->setOrthoWindow(static_cast<float>(mViewPort->getActualWidth()), static_cast<float>(mViewPort->getActualHeight()));
    mCamera->setNearClipDistance(0.5f);
    mCamera->setFarClipDistance(10000.0f);
    mCamera->setFOVy(Ogre::Degree(45.0f));
    mCamera->setPosition(0.0f, 0.0f, 100.0f);
    mCamera->lookAt(Ogre::Vector3::ZERO);

    mModeContext.mTrayMgr->showLoadingBar(2, 2);

    Ogre::ResourceGroupManager::getSingleton().initialiseResourceGroup("Popular");
    Ogre::ResourceGroupManager::getSingleton().loadResourceGroup("Popular");


    mModeContext.mTrayMgr->hideLoadingBar();

    mUIMainMenu->load(mModeContext.mTrayMgr, mModeContext.mGameState);

    //mModeContext.mTrayMgr->showCursor();
    mModeContext.mTrayMgr->showCursor("Test/Cursor");
    mModeContext.mTrayMgr->getCursorLayer()->setScale(0.75f, 0.75f);
    mModeContext.mTrayMgr->getCursorContainer()->setPosition(mViewPort->getActualWidth() / 2.0f, mViewPort->getActualHeight() / 2.0f);
}

void BaseMenuMode::clearData()
{
    Ogre::ParticleSystemManager::getSingleton().removeAllTemplates();
    Ogre::ResourceGroupManager::getSingleton().clearResourceGroup("Popular");

    mSceneMgr->clearScene();
    mModeContext.mRoot->destroySceneManager(mSceneMgr);
    mModeContext.mWindow->removeAllViewports();

    mUIMainMenu->destroy();

    Ogre::ResourceGroupManager::getSingleton().destroyResourceGroup(TEMP_RESOURCE_GROUP_NAME);
}

void BaseMenuMode::frameStarted(const Ogre::FrameEvent &evt)
{
}

void BaseMenuMode::frameRenderingQueued(const Ogre::FrameEvent &evt)
{
    mModeContext.mInputHandler->capture(evt, mModeContext.mGameState.getPlayerCar().getModelNode(), mModeContext.mGameState.getGlobalLight(), mModeContext.mGameState.getShadowLight(), true);
    mModeContext.mTrayMgr->frameRenderingQueued(evt);
}

#if defined(__ANDROID__)
void BaseMenuMode::reloadTextures()
{
    mUIMainMenu->reloadTextures(mModeContext.mGameState);
}
#endif