
#include "BaseMode.h"

BaseMode::BaseMode(const ModeContext& modeContext) :
    mModeContext(modeContext),
    mSceneMgr(0),
    mCamera(0)
{}


Ogre::ColourValue BaseMode::pingToColor(size_t ping)const
{
    Ogre::ColourValue pingColor = Ogre::ColourValue::Green;
    if(ping == 0 || ping >= 500) pingColor = Ogre::ColourValue::Red;
    if(ping > 350 && ping < 500) pingColor = Ogre::ColourValue(1.0f, 0.5f, 0.0f);

    return pingColor;
}