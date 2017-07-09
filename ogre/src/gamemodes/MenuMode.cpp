
#include "MenuMode.h"

#include "../ui/UIMainMenu.h"

MenuMode::MenuMode(const ModeContext& modeContext, SinglePlayerMenuStates state) :
    BaseMenuMode(modeContext, true)
{
    mUIMainMenu.reset(new UIMainMenu(modeContext, this, state));
}

void MenuMode::doInitData(LoaderListener* loaderListener)
{
    mUIMainMenu->load(mModeContext.mTrayMgr, mModeContext.mGameState, loaderListener);
}

void MenuMode::doClearData()
{
    mUIMainMenu->destroy(mModeContext.mTrayMgr);
}

void MenuMode::frameStarted(const Ogre::FrameEvent &evt)
{
    mUIMainMenu->frameStarted(evt);
}

void MenuMode::keyUp(MyGUI::KeyCode _key, wchar_t _char )
{
    mUIMainMenu->keyUp(_key, _char);
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

bool MenuMode::isExitSubmenu() const
{
    return mUIMainMenu->isExitSubmenu();
}

void MenuMode::setExitSubmenu()
{
    mUIMainMenu->setExitSubmenu();
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