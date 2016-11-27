#include "../pcheader.h"

#include "BaseMode.h"

BaseMode::BaseMode(const ModeContext& modeContext) :
    mModeContext(modeContext),
    mSceneMgr(0),
    mCamera(0)
{}