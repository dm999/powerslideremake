#include "../pcheader.h"

#include "MenuMultiMode.h"

#include "../ui/UIMainMenuMulti.h"

#include "../gamelogic/GameModeSwitcher.h"

#include "../multiplayer/MultiplayerControllerMaster.h"
#include "../multiplayer/MultiplayerControllerSlave.h"

MenuMultiMode::MenuMultiMode(const ModeContext& modeContext) :
    BaseMenuMode(modeContext)
{
    mUIMainMenuMulti.reset(new UIMainMenuMulti(modeContext, this));

    if(mModeContext.mGameState.isMultiplayerMaster())
    {
        mMultiplayerController.reset(new MultiplayerControllerMaster(this, mModeContext.mGameState.getMultiplayerBroadcastInterval()));
        bool success = mMultiplayerController->startLobbyMaster(mModeContext.mGameState.getMultiplayerServerIP(), mModeContext.mGameState.getMultiplayerServerPort(), mModeContext.mGameState.getMultiplayerUserName(), mModeContext.mGameState.getMultiplayerRoomName(), mModeContext.mGameState.getMultiplayerPlayersLimits(), mModeContext.mGameState.getAICount());
    }
    else
    {
        mMultiplayerController.reset(new MultiplayerControllerSlave(this, mModeContext.mGameState.getMultiplayerBroadcastInterval()));
        bool success = mMultiplayerController->startLobbySlave(mModeContext.mGameState.getMultiplayerServerIP(), mModeContext.mGameState.getMultiplayerServerPort(), mModeContext.mGameState.getMultiplayerUserName(), mModeContext.mGameState.getMultiplayerRoomName());
    }
}

MenuMultiMode::MenuMultiMode(const ModeContext& modeContext, const CommonIncludes::shared_ptr<MultiplayerController>& controller) :
    BaseMenuMode(modeContext)
{
    mUIMainMenuMulti.reset(new UIMainMenuMulti(modeContext, this));

    assert(controller.get());
    mMultiplayerController = controller;
    if(mMultiplayerController.get())
        mMultiplayerController->setEvents(this);
}

void MenuMultiMode::clearMultiplayerController()
{
    //to execute onQuit
    if(mMultiplayerController.get())
    {
        mMultiplayerController->clearSession();
        mMultiplayerController->clearLobby();
    }
    mMultiplayerController.reset();
}

void MenuMultiMode::doInitData()
{
    mUIMainMenuMulti->load(mModeContext.mGUI, mModeContext.mGameState);
}

void MenuMultiMode::doClearData()
{
    mUIMainMenuMulti->destroy(mModeContext.mTrayMgr);
}

void MenuMultiMode::frameStarted(const Ogre::FrameEvent &evt)
{
    if(mMultiplayerController.get())
    {
        mMultiplayerController->receiveData();
    }
}

void MenuMultiMode::onSessionReadyToStart()
{
    if(mModeContext.mGameState.isMultiplayerMaster())
        mUIMainMenuMulti->onStartPossible();
}

void MenuMultiMode::onSessionNotReadyToStart()
{
    if(mModeContext.mGameState.isMultiplayerMaster())
        mUIMainMenuMulti->onStartNotPossible();
}

void MenuMultiMode::onSessionStart(const MultiplayerSessionStartInfo& multiplayerSessionStartInfo)
{
    mMultiplayerSessionStartInfo = multiplayerSessionStartInfo;

    mModeContext.getGameModeSwitcher()->switchMode(ModeRaceMulti);
}

#if defined(__ANDROID__)
void MenuMultiMode::reloadTextures()
{
    mUIMainMenuMulti->reloadTextures(mModeContext.mGameState);
}
#endif

void MenuMultiMode::onPlayerEjected(const std::string& player)
{
    mUIMainMenuMulti->addEvent("Player ejected: " + player);
}

void MenuMultiMode::onPlayerJoined(const std::string& player)
{
    mUIMainMenuMulti->addEvent("Player joined: " + player);
}

void MenuMultiMode::onPlayerLeft(const std::string& player)
{
    mUIMainMenuMulti->addEvent("Player left: " + player);
}

void MenuMultiMode::onNewHost(const std::string& player)
{
    mUIMainMenuMulti->addEvent("New host: " + player);
}

void MenuMultiMode::onRoomClosed(const std::string& player)
{
    mUIMainMenuMulti->addEvent("room closed: " + player);
}

void MenuMultiMode::onPlayerReady(const std::string& player)
{
    mUIMainMenuMulti->addEvent("player ready: " + player);
}

void MenuMultiMode::onPlayerNotReady(const std::string& player)
{
    mUIMainMenuMulti->addEvent("player NOT ready: " + player);
}

void MenuMultiMode::onError(const std::string& message)
{
    mUIMainMenuMulti->addEvent("Error: " + message);
}