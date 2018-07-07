
#include "ModeContext.h"

ModeContext::ModeContext(
        Ogre::Root* root, Ogre::RenderWindow* window,
        InputHandler* inputHandler,
        CustomTrayManager* trayMgr, CustomOverlaySystem* overlaySystem,
        lua_State * pipeline,
        GameState& gameState,
        BaseApp * baseApp
#ifndef NO_OPENAL
        ,SoundsProcesser& soundsProcesser,
        MusicProcessor& musicProcessor
#endif
    )
    : mRoot(root), mWindow(window),
    mInputHandler(inputHandler),
    mTrayMgr(trayMgr), mOverlaySystem(overlaySystem),
    mPipeline(pipeline),
    mGameState(gameState),
    mBaseApp(baseApp)
#ifndef NO_OPENAL
    ,mSoundsProcesser(soundsProcesser),
    mMusicProcessor(musicProcessor)
#endif
{}

ModeContext::ModeContext(const ModeContext& other)
        : mRoot(other.mRoot), mWindow(other.mWindow),
        mInputHandler(other.mInputHandler),
        mTrayMgr(other.mTrayMgr), mOverlaySystem(other.mOverlaySystem),
        mPipeline(other.mPipeline),
        mGameModeSwitcher(other.mGameModeSwitcher),
        mGameState(other.mGameState),
        mBaseApp(other.mBaseApp),
        mLapController(other.mLapController),
        mFinishBoard(other.mFinishBoard)
#ifndef NO_OPENAL
        ,mSoundsProcesser(other.mSoundsProcesser),
        mMusicProcessor(other.mMusicProcessor)
#endif
{}