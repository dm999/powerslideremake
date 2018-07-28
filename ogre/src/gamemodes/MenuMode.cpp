
#include "MenuMode.h"

#include "../ui/UIMainMenu.h"

MenuMode::MenuMode(const ModeContext& modeContext, const GameMode gameMode, SinglePlayerMenuStates state) :
    BaseMenuMode(modeContext, true)
{
    mUIMainMenu.reset(new UIMainMenu(modeContext, gameMode, this, state));
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

SinglePlayerMenuStates MenuMode::getSubmenuState() const
{
    return mUIMainMenu->getSubmenuState();
}

bool MenuMode::isExitSubmenu() const
{
    return mUIMainMenu->isExitSubmenu();
}

void MenuMode::setSubmenu(const std::string& title)
{
    mUIMainMenu->setSubmenu(title);
}

void MenuMode::setTopmostSubmenu()
{
    mUIMainMenu->setTopmostSubmenu();
}

void MenuMode::setDefaultBackground(bool isSwitchState)
{
    mUIMainMenu->setDefaultBackground(isSwitchState);
}

void MenuMode::setPodiumSubmenu()
{
    mUIMainMenu->setPodiumSubmenu();
}

#if defined(__ANDROID__)
void MenuMode::reloadTextures()
{
    mUIMainMenu->reloadTextures(mModeContext.mGameState);
}
#endif