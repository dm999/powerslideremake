#include "../pcheader.h"

#include "ModeContext.h"

ModeContext::ModeContext(
        Ogre::Root* root, Ogre::RenderWindow* window,
        InputHandler* inputHandler,
        CustomTrayManager* trayMgr, CustomOverlaySystem* overlaySystem,
        lua_State * pipeline,
        GameState& gameState, SoundsProcesser& soundsProcesser, Graphics2D& graphics2D
    )
    : mRoot(root), mWindow(window),
    mInputHandler(inputHandler),
    mTrayMgr(trayMgr), mOverlaySystem(overlaySystem),
    mPipeline(pipeline),
    mGameState(gameState), mSoundsProcesser(soundsProcesser), mGraphics2D(graphics2D)
{}

ModeContext::ModeContext(const ModeContext& other)
        : mRoot(other.mRoot), mWindow(other.mWindow),
        mInputHandler(other.mInputHandler),
        mTrayMgr(other.mTrayMgr), mOverlaySystem(other.mOverlaySystem),
        mPipeline(other.mPipeline),
        mGameState(other.mGameState), mSoundsProcesser(other.mSoundsProcesser), mGraphics2D(other.mGraphics2D)
{}