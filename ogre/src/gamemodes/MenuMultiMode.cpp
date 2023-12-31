
#include "MenuMultiMode.h"

#include "../ui/UIMainMenuMulti.h"

#include "../tools/Conversions.h"

#include "../gamelogic/GameModeSwitcher.h"

#include "../multiplayer/MultiplayerControllerMaster.h"
#include "../multiplayer/MultiplayerControllerSlave.h"

/**
 * Initial creation of multiplayer session (switch from single menu to multi menu)
 */
MenuMultiMode::MenuMultiMode(const ModeContext& modeContext) :
    BaseMenuMode(modeContext, true),
    mIsEnterFromBaseMenu(true),
    mIsLobbyEntered(true)
{
    mUIMainMenuMulti = std::make_shared<UIMainMenuMulti>(modeContext, this);

    //start multiplayer controller
    if(mModeContext.mGameState.isMultiplayerMaster())
    {
        mMultiplayerController = std::make_shared<MultiplayerControllerMaster>(this, mModeContext.mGameState.getMultiplayerBroadcastInterval());
    }
    else
    {
        mMultiplayerController = std::make_shared<MultiplayerControllerSlave>(this, mModeContext.mGameState.getMultiplayerBroadcastInterval());
    }
}

/**
 * Multiplayer session already created (switch from multi race to multi menu)
 */
MenuMultiMode::MenuMultiMode(const ModeContext& modeContext, const CommonIncludes::shared_ptr<MultiplayerController>& controller, const MultiplayerSessionStartInfo& sessionInfo) :
    BaseMenuMode(modeContext, false),
    mIsEnterFromBaseMenu(false),
    mIsLobbyEntered(true)
{
    mUIMainMenuMulti = std::make_shared<UIMainMenuMulti>(modeContext, this);

    //get multiplayer controller from previous (racing) mode
    assert(controller.get());
    mMultiplayerController = controller;
    mMultiplayerSessionStartInfo = sessionInfo;
}

void MenuMultiMode::doInitData(LoaderListener* loaderListener)
{
    mUIMainMenuMulti->load(mModeContext.mTrayMgr, mModeContext.mGameState, mIsEnterFromBaseMenu, mMultiplayerSessionStartInfo.mPlayersSkins);

    //do room operations after UI created

    if(mMultiplayerController.get())
        mMultiplayerController->setEvents(this);

    if(mIsEnterFromBaseMenu)
    {
        bool success;
        if(mModeContext.mGameState.isMultiplayerMaster())
        {
            success = mMultiplayerController->startLobbyMaster(mModeContext.mGameState.getMultiplayerServerIP(), static_cast<uint16_t>(mModeContext.mGameState.getMultiplayerServerPort()), mModeContext.mGameState.getMultiplayerUserName(), mModeContext.mGameState.getMultiplayerRoomName(), 12 - mModeContext.mGameState.getAICount(), mModeContext.mGameState.getAICount(), mModeContext.mGameState.getVersion());
        }
        else
        {
            success = mMultiplayerController->startLobbySlave(mModeContext.mGameState.getMultiplayerServerIP(), static_cast<uint16_t>(mModeContext.mGameState.getMultiplayerServerPort()), mModeContext.mGameState.getMultiplayerUserName(), mModeContext.mGameState.getMultiplayerRoomName());
        }

        mIsLobbyEntered = success;
    }
}

void MenuMultiMode::doClearData()
{
    mUIMainMenuMulti->destroy(mModeContext.mTrayMgr);
}

void MenuMultiMode::customFrameRenderingQueued()
{
    if(mMultiplayerController.get())
    {
        size_t ping = mMultiplayerController->getLobbyPing();
        mUIMainMenuMulti->setMiscText(Conversions::DMToString(ping), pingToColor(ping));
    }
}

void MenuMultiMode::frameStarted(const Ogre::FrameEvent &evt)
{
    if(mMultiplayerController.get())
    {
        mMultiplayerController->receiveData();
    }

    mUIMainMenuMulti->frameStarted(evt);
}

void MenuMultiMode::keyUp(MyGUI::KeyCode _key, wchar_t _char )
{
    mUIMainMenuMulti->keyUp(_key, _char);
}

void MenuMultiMode::mousePressed(const Ogre::Vector2& pos)
{
    mUIMainMenuMulti->mousePressed(pos);
}

void MenuMultiMode::mouseReleased(const Ogre::Vector2& pos, OIS::MouseButtonID id)
{
    mUIMainMenuMulti->mouseReleased(pos, id);
}

void MenuMultiMode::mouseMoved(const Ogre::Vector2& pos)
{
    mUIMainMenuMulti->mouseMoved(pos);
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

    if(!mMultiplayerSessionStartInfo.mIsHost)
    {
        mModeContext.getGameState().setRaceParameters(mMultiplayerSessionStartInfo.mTrackName, static_cast<AIStrength>(mMultiplayerSessionStartInfo.mAIStrength), mMultiplayerSessionStartInfo.mLapsCount);
    }

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

    mUIMainMenuMulti->roomEnter(roomName, player, players);

    mUIMainMenuMulti->addEvent("room entered: " + roomName + " by player: " + player);

    for(size_t q = 0; q < players.size(); ++q)
    {
        mUIMainMenuMulti->addEvent("player: " + players[q]);
    }
}

void MenuMultiMode::onPlayerEjected(const std::string& player)
{
    mUIMainMenuMulti->roomLeft(player);
    mUIMainMenuMulti->addEvent("player [" + player + "] ejected");
}

void MenuMultiMode::onPlayerJoined(const std::string& player)
{
    mUIMainMenuMulti->roomJoined(player);
    mUIMainMenuMulti->addEvent("player [" + player + "] joined");
}

void MenuMultiMode::onPlayerLeft(const std::string& player)
{
    mUIMainMenuMulti->roomLeft(player);
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
    if(data.mDataType == lobbyDataRegular)
    {
        mUIMainMenuMulti->playerReadyChange(player, data.mIsReady);
        mUIMainMenuMulti->playerSkinChanged(player, data.mCharacterName);
    }

    mUIMainMenuMulti->addEvent("player [" + player + "] select character: " + data.mCharacterName);

    //host data
    if(!data.mTrackName.empty())
    {
        mUIMainMenuMulti->addEvent("player [" + player + "] set track: " + data.mTrackName);
        mUIMainMenuMulti->addEvent("player [" + player + "] set AI count: " + Conversions::DMToString(data.mAICount));
        mUIMainMenuMulti->addEvent("player [" + player + "] set AI strentgh: " + Conversions::DMToString(data.mAIStrength));
        mUIMainMenuMulti->addEvent("player [" + player + "] set laps count: " + Conversions::DMToString(data.mLapsCount));

        mModeContext.getGameState().setRaceParameters(data.mTrackName, static_cast<AIStrength>(data.mAIStrength), data.mLapsCount);

        mUIMainMenuMulti->hostTrackUpdate(data.mAICount);
    }

    mUIMainMenuMulti->playerMessage(player, data.mPlayerMessage);
}

void MenuMultiMode::onError(const std::string& message)
{
    mUIMainMenuMulti->addEvent("Error: " + message);
}
