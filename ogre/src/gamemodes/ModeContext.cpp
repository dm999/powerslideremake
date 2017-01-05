#include "../pcheader.h"

#include "ModeContext.h"

ModeContext::ModeContext(
        Ogre::Root* root, Ogre::RenderWindow* window,
        InputHandler* inputHandler,
        CustomTrayManager* trayMgr, CustomOverlaySystem* overlaySystem,
        lua_State * pipeline,
        GameState& gameState,
#ifndef NO_OPENAL
        SoundsProcesser& soundsProcesser,
#endif
        MyGUI::Gui* gui, MyGUI::OgrePlatform* platform
    )
    : mRoot(root), mWindow(window),
    mInputHandler(inputHandler),
    mTrayMgr(trayMgr), mOverlaySystem(overlaySystem),
    mPipeline(pipeline),
    mGameState(gameState),
#ifndef NO_OPENAL
    mSoundsProcesser(soundsProcesser),
#endif
    mGUI(gui), mPlatform(platform)
{}

ModeContext::ModeContext(const ModeContext& other)
        : mRoot(other.mRoot), mWindow(other.mWindow),
        mInputHandler(other.mInputHandler),
        mTrayMgr(other.mTrayMgr), mOverlaySystem(other.mOverlaySystem),
        mPipeline(other.mPipeline),
        mGameModeSwitcher(other.mGameModeSwitcher),
        mGameState(other.mGameState),
#ifndef NO_OPENAL
        mSoundsProcesser(other.mSoundsProcesser),
#endif
        mGUI(other.mGUI), mPlatform(other.mPlatform)
{}