#include "../pcheader.h"

#include "MenuMode.h"

#include "../ui/UIMainMenu.h"

MenuMode::MenuMode(const ModeContext& modeContext) :
    BaseMenuMode(modeContext),
    mUIMainMenu(new UIMainMenu(modeContext))
{}

void MenuMode::doInitData()
{
    mUIMainMenu->load(mModeContext.mGUI, mModeContext.mGameState);
}

void MenuMode::doClearData()
{
    mUIMainMenu->destroy(mModeContext.mTrayMgr);
}

#if defined(__ANDROID__)
void MenuMode::reloadTextures()
{
    mUIMainMenu->reloadTextures(mModeContext.mGameState);
}
#endif