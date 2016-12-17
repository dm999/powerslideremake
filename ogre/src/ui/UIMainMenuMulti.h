#ifndef UIMAINMENU_H
#define UIMAINMENU_H

#include "UIBase.h"

#include "../gamemodes/ModeContext.h"

#include "../GameState.h"

namespace MyGUI
{
    class Gui;
    class Widget;
    class Button;
}

class MenuMultiMode;

class UIMainMenuMulti : public UIBase
{
public:

    UIMainMenuMulti(const ModeContext& modeContext, MenuMultiMode * menuMultiMode);
    ~UIMainMenuMulti();

    void load(MyGUI::Gui* gui, const GameState& gameState);

    void processButtonClick(MyGUI::Widget* sender);

#if defined(__ANDROID__)
    void reloadTextures(const GameState& gameState);
#endif

    //master ready to press start race
    void onStartPossible();

private:

    ModeContext mModeContext;

    void loadMisc(const PFLoader& pfLoaderData, const PFLoader& pfLoaderGameshell);

    MenuMultiMode * mMenuMultiMode;

    MyGUI::Button* mWidgetJoin;
    MyGUI::Button* mWidgetStart;
};

#endif