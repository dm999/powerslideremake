#ifndef MULTIPLAYERMODE_H
#define MULTIPLAYERMODE_H

#include "BaseRaceMode.h"

#include "../multiplayer/MultiplayerController.h"

class MultiplayerHumansLapFinishController;

class MultiPlayerMode : public BaseRaceMode,
    public MultiplayerControllerEvents
{
public:

    MultiPlayerMode(const ModeContext& modeContext, const CommonIncludes::shared_ptr<MultiplayerController>& controller);
    virtual ~MultiPlayerMode(){}

    CommonIncludes::shared_ptr<MultiplayerController> getMultiplayerController(){return mMultiplayerController;}

    //LapUtils
    void onLapFinished()override;

    //multiplayer events
    void onRoomEnter(const std::string& roomName, const std::string& player, const std::vector<std::string>& players)override{}
    void onPlayerEjected(const std::string& player)override;
    void onPlayerJoined(const std::string& player)override;
    void onPlayerLeft(const std::string& player)override;
    void onNewHost(const std::string& player)override;
    void onReconfigure(const std::string& player)override{}
    void onReconfigureFailed(const std::string& player)override{}
    void onRoomClosed(const std::string& player)override;
    void onLobbyMessage(const std::string& player, const MultiplayerLobbyData& data)override{}
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

    friend class MultiplayerHumansLapFinishController;

    //MultiplayerHumansLapFinishController
    void onLapFinishedByHuman(const std::string& humanName, size_t lap, Ogre::Real lastLapTime);
    void onRaceFinishedByHuman(const std::string& humanName);

    void checkRaceFinished();

    bool mIsSessionStarted;

    CommonIncludes::shared_ptr<MultiplayerController> mMultiplayerController;

    std::vector<CommonIncludes::shared_ptr<MultiplayerHumansLapFinishController> > mHumanLapsController;

    bool mIsSelfFinished;
    std::map<std::string, bool> mIsRaceFinishedByHuman;//key - human name, self not included
};

#endif