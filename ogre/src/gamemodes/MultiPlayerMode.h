#ifndef MULTIPLAYERMODE_H
#define MULTIPLAYERMODE_H

#include "BaseRaceMode.h"

#include "../multiplayer/MultiplayerController.h"

class MiltiPlayerMode : public BaseRaceMode,
    public MultiplayerController::Events
{
public:

    MiltiPlayerMode(const ModeContext& modeContext);
    virtual ~MiltiPlayerMode(){}

    virtual void clearData()override;

    virtual void frameStarted(const Ogre::FrameEvent &evt)override;
    virtual void frameRenderingQueued(const Ogre::FrameEvent& evt)override;

    //multiplayer
    void onPlayerEjected(const std::string& player)override;
    void onPlayerJoined(const std::string& player)override;
    void onPlayerLeft(const std::string& player)override;
    void onNewHost(const std::string& player)override;
    void onRoomClosed(const std::string& player)override;
    void onPlayerReady(const std::string& player)override;
    void onPlayerAddedToSession(const std::string& player)override;
    void onPlayerQuitSession(const std::string& player, bool isHost)override;
    void onSessionStart(uint32_t aiAmount, const std::vector<std::string>& players, size_t playerIndex, bool isHost, const std::vector<std::string>& aiSkins, const std::map<std::string, std::string>& playersSkins)override;
    void onSessionUpdate(const MultiplayerController::playerToData& otherPlayersSessionData, const std::vector<MultiplayerSessionData>& aiPlayersSessionData, bool isHost)override;
    void onError(const std::string& message)override;

protected:

    virtual void clearScene()override;
    virtual void initMisc()override;

private:

    CommonIncludes::shared_ptr<MultiplayerController> mMultiplayerController;
};

#endif