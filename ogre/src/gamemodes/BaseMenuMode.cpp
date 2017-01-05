#include "../pcheader.h"

#include "BaseMenuMode.h"

#include "../gamelogic/RacingGridGeneration.h"

#include "MyGUI.h"
#include "MyGUI_OgrePlatform.h"

#include "../customs/CustomTrayManager.h"
#include "../customs/CustomOverlaySystem.h"

#include "../InputHandler.h"

BaseMenuMode::BaseMenuMode(const ModeContext& modeContext, bool isInitialCreationMultiplayer) :
    BaseMode(modeContext)
{
    //flag for multiplayer only, indicate switch from single menu to multi
    if(isInitialCreationMultiplayer)
    {
        mModeContext.getGameState().setMultiplayerServerPort(8800);

        mModeContext.getGameState().setListenerGain(0.0f);

        mModeContext.getGameState().setMirrorEnabled(true);

        std::vector<std::string> playersCharacters;
        playersCharacters.push_back(mModeContext.getGameState().getPlayerCar().getCharacterName());
        recalculateCharacterNames(playersCharacters);
    }
}

void BaseMenuMode::recalculateCharacterNames(const std::vector<std::string>& playersCharacters)
{
    //based on AICount
    std::vector<std::string> aiCharacters = RacingGridGeneration().generate(mModeContext.getGameState(), playersCharacters);
    mModeContext.getGameState().setAICharacters(aiCharacters);
    for(size_t q = 0; q < mModeContext.getGameState().getAICount(); ++q)
    {
        mModeContext.getGameState().getAICar(q).setCharacterName(aiCharacters[q]);
    }
}

void BaseMenuMode::initData()
{
    Ogre::ResourceGroupManager::getSingleton().createResourceGroup(TEMP_RESOURCE_GROUP_NAME);

    Ogre::ResourceGroupManager::getSingleton().initialiseResourceGroup("UI");
    Ogre::ResourceGroupManager::getSingleton().loadResourceGroup("UI");

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

    //to load textures & materials
    mModeContext.mWindow->update(false);

    mModeContext.mPlatform->initialise(mModeContext.mWindow, mSceneMgr);
    mModeContext.mGUI->initialise();

    //update mouse position
    const OIS::MouseState &ms = mModeContext.mInputHandler->getInputContext().mMouse->getMouseState();
    MyGUI::InputManager::getInstance().injectMouseMove(ms.X.abs, ms.Y.abs, ms.Z.abs);
    MyGUI::PointerManager::getInstance().setVisible(false);

    doInitData();

    //mModeContext.mTrayMgr->showCursor();
    mModeContext.mTrayMgr->showCursor("Test/Cursor");
    //mModeContext.mTrayMgr->getCursorLayer()->setScale(0.75f, 0.75f);
    //mModeContext.mTrayMgr->getCursorContainer()->setPosition(mViewPort->getActualWidth() / 2.0f, mViewPort->getActualHeight() / 2.0f);
}

void BaseMenuMode::clearData()
{
    doClearData();

    mModeContext.mGUI->shutdown();
    mModeContext.mPlatform->shutdown();

    mSceneMgr->clearScene();
    mModeContext.mRoot->destroySceneManager(mSceneMgr);
    mModeContext.mWindow->removeAllViewports();

    mModeContext.mTrayMgr->hideCursor();

    Ogre::ResourceGroupManager::getSingleton().clearResourceGroup("UI");

    Ogre::ResourceGroupManager::getSingleton().destroyResourceGroup(TEMP_RESOURCE_GROUP_NAME);
}

void BaseMenuMode::frameRenderingQueued(const Ogre::FrameEvent &evt)
{
    customFrameRenderingQueued();

    mModeContext.mInputHandler->capture(evt, mModeContext.mGameState.getPlayerCar().getModelNode(), mModeContext.mGameState.getGlobalLight(), mModeContext.mGameState.getShadowLight(), true);
    mModeContext.mTrayMgr->frameRenderingQueued(evt);
}