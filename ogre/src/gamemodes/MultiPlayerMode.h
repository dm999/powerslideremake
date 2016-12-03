#ifndef MULTIPLAYERMODE_H
#define MULTIPLAYERMODE_H

#include "BaseRaceMode.h"

#include "../multiplayer/MultiplayerController.h"

class MultiPlayerMode : public BaseRaceMode,
    public MultiplayerController::Events
{
public:

    MultiPlayerMode(const ModeContext& modeContext);
    virtual ~MultiPlayerMode(){}

    virtual void clearData()override;

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

    virtual void customInitScene()override;
    virtual void customClearScene()override;
    virtual void customProcessCollision(btManifoldPoint& cp, const btCollisionObjectWrapper* colObj0Wrap, const btCollisionObjectWrapper* colObj1Wrap, int triIndex)override;
    virtual void customUnloadResources()override;
    virtual void customFrameStartedDoProcessFrameBeforePhysics(const Ogre::FrameEvent &evt)override;
    virtual void customFrameStartedDoProcessFrameAfterPhysics(const Ogre::FrameEvent &evt)override;
    virtual void customFrameRenderingQueuedDoBegining()override;
    virtual void customFrameRenderingQueuedDoRaceStarted()override;
    virtual void customFrameRenderingQueuedDo2DUI()override;

private:

    CommonIncludes::shared_ptr<MultiplayerController> mMultiplayerController;
};

#endif