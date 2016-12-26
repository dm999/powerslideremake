#include "../pcheader.h"

#include "MenuMultiMode.h"

#include "../ui/UIMainMenuMulti.h"

#include "../gamelogic/GameModeSwitcher.h"

#include "../multiplayer/MultiplayerControllerMaster.h"
#include "../multiplayer/MultiplayerControllerSlave.h"

/**
 * Initial creation of multiplayer session (switch from single menu to multi menu)
 */
MenuMultiMode::MenuMultiMode(const ModeContext& modeContext) :
    BaseMenuMode(modeContext, true),
    mIsEnterFromBaseMenu(true)
{
    mUIMainMenuMulti.reset(new UIMainMenuMulti(modeContext, this));

    //start multiplayer controller
    if(mModeContext.mGameState.isMultiplayerMaster())
    {
        mMultiplayerController.reset(new MultiplayerControllerMaster(this, mModeContext.mGameState.getMultiplayerBroadcastInterval()));
    }
    else
    {
        mMultiplayerController.reset(new MultiplayerControllerSlave(this, mModeContext.mGameState.getMultiplayerBroadcastInterval()));
    }
}

/**
 * Multiplayer session already created (switch from multi race to multi menu)
 */
MenuMultiMode::MenuMultiMode(const ModeContext& modeContext, const CommonIncludes::shared_ptr<MultiplayerController>& controller) :
    BaseMenuMode(modeContext, false),
    mIsEnterFromBaseMenu(false)
{
    mUIMainMenuMulti.reset(new UIMainMenuMulti(modeContext, this));

    //get multiplayer controller from previous (racing) mode
    assert(controller.get());
    mMultiplayerController = controller;
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

    //do room operations after UI created

    if(mMultiplayerController.get())
        mMultiplayerController->setEvents(this);

    if(mIsEnterFromBaseMenu)
    {
        if(mModeContext.mGameState.isMultiplayerMaster())
        {
            bool success = mMultiplayerController->startLobbyMaster(mModeContext.mGameState.getMultiplayerServerIP(), mModeContext.mGameState.getMultiplayerServerPort(), mModeContext.mGameState.getMultiplayerUserName(), mModeContext.mGameState.getMultiplayerRoomName(), 12 - mModeContext.mGameState.getAICount(), mModeContext.mGameState.getAICount(), mModeContext.mGameState.getVersion());
        }
        else
        {
            bool success = mMultiplayerController->startLobbySlave(mModeContext.mGameState.getMultiplayerServerIP(), mModeContext.mGameState.getMultiplayerServerPort(), mModeContext.mGameState.getMultiplayerUserName(), mModeContext.mGameState.getMultiplayerRoomName());
        }
    }
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

void MenuMultiMode::onRoomEnter(const std::string& roomName, const std::string& player, const std::vector<std::string>& players)
{
    mUIMainMenuMulti->addEvent("room entered: " + roomName + " by player: " + player);

    for(size_t q = 0; q < players.size(); ++q)
    {
        mUIMainMenuMulti->addEvent("player: " + players[q]);
    }
}

void MenuMultiMode::onPlayerEjected(const std::string& player)
{
    mUIMainMenuMulti->addEvent("player [" + player + "] ejected");
}

void MenuMultiMode::onPlayerJoined(const std::string& player)
{
    mUIMainMenuMulti->addEvent("player [" + player + "] joined");
}

void MenuMultiMode::onPlayerLeft(const std::string& player)
{
    mUIMainMenuMulti->addEvent("player [" + player + "] left");
}

void MenuMultiMode::onNewHost(const std::string& player)
{
    mUIMainMenuMulti->addEvent("New host: " + player);
}

void MenuMultiMode::onReconfigure(const std::string& player)
{
    mUIMainMenuMulti->addEvent("Reconfigure: " + player);
}

void MenuMultiMode::onReconfigureFailed(const std::string& player)
{
    mUIMainMenuMulti->addEvent("Reconfigure failed: " + player);
}

void MenuMultiMode::onRoomClosed(const std::string& player)
{
    mUIMainMenuMulti->addEvent("room closed: " + player);
}

void MenuMultiMode::onLobbyMessage(const std::string& player, const MultiplayerLobbyData& data)
{
    if(data.mIsReady)
        mUIMainMenuMulti->addEvent("player [" + player + "] ready");
    else
        mUIMainMenuMulti->addEvent("player [" + player + "] not ready");

    mUIMainMenuMulti->addEvent("player [" + player + "] select character: " + data.mCharacterName);

    if(!data.mPlayerMessage.empty())
        mUIMainMenuMulti->addEvent("player [" + player + "] said: " + data.mPlayerMessage);

    //host data
    if(!data.mTrackName.empty())
    {
        mUIMainMenuMulti->addEvent("player [" + player + "] set track: " + data.mTrackName);
        mUIMainMenuMulti->addEvent("player [" + player + "] set AI count: " + Conversions::DMToString(data.mAICount));
        mUIMainMenuMulti->addEvent("player [" + player + "] set AI strentgh: " + Conversions::DMToString(data.mAIStrength));
        mUIMainMenuMulti->addEvent("player [" + player + "] set laps count: " + Conversions::DMToString(data.mLapsCount));

        mModeContext.getGameState().setRaceParameters(data.mTrackName, static_cast<AIStrength>(data.mAIStrength), data.mLapsCount);
    }
}

void MenuMultiMode::onError(const std::string& message)
{
    mUIMainMenuMulti->addEvent("Error: " + message);
}