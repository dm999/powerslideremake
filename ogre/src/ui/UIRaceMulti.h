#ifndef UIRACEMULTI_H
#define UIRACEMULTI_H

#include <string>

#include "UIBase.h"
#include "MyGUI_KeyCode.h"

#include "../gamemodes/ModeContext.h"


#include "../GameState.h"

namespace MyGUI
{
    class Gui;
    class Widget;
    class EditBox;
}

class MultiPlayerMode;


class UIRaceMulti
{
public:

    UIRaceMulti(const ModeContext& modeContext, MultiPlayerMode * multiPlayerMode);
    ~UIRaceMulti(){}

    void load(MyGUI::Gui* gui, const GameState& gameState);

    void switchVisibleMessageWidget();

    void processKeyPress(MyGUI::Widget* sender, MyGUI::KeyCode key, unsigned int _char);

private:

    ModeContext mModeContext;

    MultiPlayerMode * mMultiPlayerMode;

    MyGUI::EditBox* mWidgetMessage;
};


#endif