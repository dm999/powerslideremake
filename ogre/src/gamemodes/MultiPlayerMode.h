#ifndef MULTIPLAYERMODE_H
#define MULTIPLAYERMODE_H

#include "BaseRaceMode.h"

#include "../multiplayer/MultiplayerController.h"

class MultiPlayerMode : public BaseRaceMode,
    public MultiplayerControllerEvents
{
public:

    MultiPlayerMode(const ModeContext& modeContext, const CommonIncludes::shared_ptr<MultiplayerController>& controller);
    virtual ~MultiPlayerMode(){}

    CommonIncludes::shared_ptr<MultiplayerController> getMultiplayerController(){return mMultiplayerController;}

    //multiplayer events
    void onPlayerEjected(const std::string& player)override;
    void onPlayerJoined(const std::string& player)override;
    void onPlayerLeft(const std::string& player)override;
    void onNewHost(const std::string& player)override;
    void onRoomClosed(const std::string& player)override;
    void onPlayerReady(const std::string& player)override{}
    void onPlayerNotReady(const std::string& player)override{}
    void onPlayerAddedToSession(const std::string& player)override;
    void onPlayerQuitSession(const std::string& player, bool isHost)override;
    void onSessionReadyToStart()override{};
    void onSessionNotReadyToStart()override{};
    void onSessionStart(const MultiplayerSessionStartInfo& multiplayerSessionStartInfo)override{}
    void onSessionUpdate(const playerToData& otherPlayersSessionData, const std::vector<MultiplayerSessionData>& aiPlayersSessionData, bool isHost)override;
    void onError(const std::string& message)override;

    void prepareDataForSession(const MultiplayerSessionStartInfo& sessionStartInfo);

protected:

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

    bool mIsSessionStarted;

    CommonIncludes::shared_ptr<MultiplayerController> mMultiplayerController;
};

#endif