#include "../pcheader.h"

#include "MenuMultiMode.h"

#include "MyGUI.h"
#include "MyGUI_OgrePlatform.h"

#include "../customs/CustomTrayManager.h"
#include "../customs/CustomOverlaySystem.h"

#include "../InputHandler.h"

#include "../ui/UIMainMenuMulti.h"

MenuMultiMode::MenuMultiMode(const ModeContext& modeContext) :
    BaseMode(modeContext),
    mUIMainMenuMulti(new UIMainMenuMulti(modeContext))
{}

void MenuMultiMode::initData()
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

    mModeContext.mPlatform->initialise(mModeContext.mWindow, mSceneMgr);
    mModeContext.mGUI->initialise();

    mUIMainMenuMulti->load(mModeContext.mGUI, mModeContext.mGameState);

    //mModeContext.mTrayMgr->showCursor();
    //mModeContext.mTrayMgr->showCursor("Test/Cursor");
    //mModeContext.mTrayMgr->getCursorLayer()->setScale(0.75f, 0.75f);
    //mModeContext.mTrayMgr->getCursorContainer()->setPosition(mViewPort->getActualWidth() / 2.0f, mViewPort->getActualHeight() / 2.0f);
}

void MenuMultiMode::clearData()
{
    Ogre::ParticleSystemManager::getSingleton().removeAllTemplates();
    Ogre::ResourceGroupManager::getSingleton().clearResourceGroup("Popular");

    mModeContext.mGUI->shutdown();
    mModeContext.mPlatform->shutdown();

    mSceneMgr->clearScene();
    mModeContext.mRoot->destroySceneManager(mSceneMgr);
    mModeContext.mWindow->removeAllViewports();

    mUIMainMenuMulti->destroy(mModeContext.mTrayMgr);

    Ogre::ResourceGroupManager::getSingleton().destroyResourceGroup(TEMP_RESOURCE_GROUP_NAME);
}

void MenuMultiMode::frameStarted(const Ogre::FrameEvent &evt)
{
}

void MenuMultiMode::frameRenderingQueued(const Ogre::FrameEvent &evt)
{
    mModeContext.mInputHandler->capture(evt, mModeContext.mGameState.getPlayerCar().getModelNode(), mModeContext.mGameState.getGlobalLight(), mModeContext.mGameState.getShadowLight(), true);
    mModeContext.mTrayMgr->frameRenderingQueued(evt);
}

#if defined(__ANDROID__)
void MenuMultiMode::reloadTextures()
{
    mUIMainMenuMulti->reloadTextures(mModeContext.mGameState);
}
#endif