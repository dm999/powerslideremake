#include "../pcheader.h"

#include "MenuMode.h"

#include "../ui/UIMainMenu.h"

MenuMode::MenuMode(const ModeContext& modeContext) :
    BaseMenuMode(modeContext, true)
{
    mUIMainMenu.reset(new UIMainMenu(modeContext, this));
}

void MenuMode::doInitData(LoaderListener* loaderListener)
{
    mUIMainMenu->load(mModeContext.mTrayMgr, mModeContext.mGameState, loaderListener);
}

void MenuMode::doClearData()
{
    mUIMainMenu->destroy(mModeContext.mTrayMgr);
}

void MenuMode::mousePressed(const Ogre::Vector2& pos)
{
    mUIMainMenu->mousePressed(pos);
}

void MenuMode::mouseReleased(const Ogre::Vector2& pos)
{
    mUIMainMenu->mouseReleased(pos);
}

void MenuMode::mouseMoved(const Ogre::Vector2& pos)
{
    mUIMainMenu->mouseMoved(pos);
}

bool MenuMode::isTopmostSubmenu() const
{
    return mUIMainMenu->isTopmostSubmenu();
}

void MenuMode::setTopmostSubmenu()
{
    mUIMainMenu->setTopmostSubmenu();
}

#if defined(__ANDROID__)
void MenuMode::reloadTextures()
{
    mUIMainMenu->reloadTextures(mModeContext.mGameState);
}
#endif