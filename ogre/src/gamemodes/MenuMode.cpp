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

#if defined(__ANDROID__)
void MenuMode::reloadTextures()
{
    mUIMainMenu->reloadTextures(mModeContext.mGameState);
}
#endif