#include "../pcheader.h"

#include "BaseMenuMode.h"

#include "../customs/CustomTrayManager.h"
#include "../customs/CustomOverlaySystem.h"

#include "../InputHandler.h"

BaseMenuMode::BaseMenuMode(const ModeContext& modeContext) :
    BaseMode(modeContext)
{}

void BaseMenuMode::initData()
{
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

    //mModeContext.mTrayMgr->showCursor("Test/Cursor");
}

void BaseMenuMode::clearData()
{
    Ogre::ParticleSystemManager::getSingleton().removeAllTemplates();
    Ogre::ResourceGroupManager::getSingleton().clearResourceGroup("Popular");

    mSceneMgr->clearScene();
    mModeContext.mRoot->destroySceneManager(mSceneMgr);
    mModeContext.mWindow->removeAllViewports();
}

void BaseMenuMode::frameStarted(const Ogre::FrameEvent &evt)
{
}

void BaseMenuMode::frameRenderingQueued(const Ogre::FrameEvent &evt)
{
    mModeContext.mInputHandler->capture(evt, mModeContext.mGameState.getPlayerCar().getModelNode(), mModeContext.mGameState.getGlobalLight(), mModeContext.mGameState.getShadowLight(), true);
    mModeContext.mTrayMgr->frameRenderingQueued(evt);
}