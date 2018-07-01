#ifndef MULTIPLAYERMODE_H
#define MULTIPLAYERMODE_H

#ifndef NO_MULTIPLAYER

#include "BaseRaceMode.h"

#include "../multiplayer/MultiplayerController.h"

#include "../includes/MyGUI_KeyCode.h"

class MultiplayerAILapFinishController;
class UIRaceMulti;

class MultiPlayerMode : public BaseRaceMode,
    public MultiplayerControllerEvents
{
public:

    MultiPlayerMode(const ModeContext& modeContext, const CommonIncludes::shared_ptr<MultiplayerController>& controller);
    virtual ~MultiPlayerMode(){}

    void initCamera()override;

    CommonIncludes::shared_ptr<MultiplayerController> getMultiplayerController(){return mMultiplayerController;}

    void frameStarted(const Ogre::FrameEvent &evt)override;

    void keyUp(MyGUI::KeyCode _key, wchar_t _char);

    void createBurnByPlayer() override;
    void createBombByPlayer() override;
    void nitroByPlayer() override;

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
    void onLobbyMessage(const std::string& player, const MultiplayerLobbyData& data)override;
    void onPlayerAddedToSession(const std::string& player)override;
    void onPlayerQuitSession(const std::string& player, bool isHost)override;
    void onSessionReadyToStart()override{};
    void onSessionNotReadyToStart()override{};
    void onSessionStart(const MultiplayerSessionStartInfo& multiplayerSessionStartInfo)override{}
    void onSessionUpdate(const playerToData& otherPlayersSessionData, const std::vector<MultiplayerSessionData>& aiPlayersSessionData, bool isHost)override;
    void onError(const std::string& message)override;

    /**
     * create vehicles and positioning them according data
     */
    void prepareDataForSession(const MultiplayerSessionStartInfo& sessionStartInfo);
    const MultiplayerSessionStartInfo& getMultiplayerSessionStartInfo()const{return mSessionStartInfo;}

    void tabPressed();

    void clearData()override;

#if defined(__ANDROID__)
    //for UI only
    void reloadTextures()override;
#endif

protected:

    virtual void customInitScene()override;
    virtual void customClearScene()override;
    virtual void customInitUI()override;
    virtual void customClearUI()override;
    virtual void customProcessCollision(int triIndex)override;
    virtual void customUnloadResources()override;
    virtual void customFrameStartedDoProcessFrameBeforePhysics()override;
    virtual void customFrameStartedDoProcessFrameAfterPhysics()override;
    virtual void customFrameRenderingQueuedDoBegining()override;
    virtual void customFrameRenderingQueuedDoRaceStarted()override;
    virtual void customFrameRenderingQueuedDo2DUI()override;

private:

    friend class MultiplayerAILapFinishController;

    //MultiplayerAILapFinishController
    void onRaceFinishedByAI(PSAICar& aiCar);

    void checkRaceFinished();

    bool mIsSessionStarted;

    CommonIncludes::shared_ptr<MultiplayerController> mMultiplayerController;

    CommonIncludes::shared_ptr<UIRaceMulti> mUIRaceMulti;

    std::vector<CommonIncludes::shared_ptr<MultiplayerAILapFinishController> > mAILapsController;

    MultiplayerSessionStartInfo mSessionStartInfo; // to pass back to menu skins info

    bool mIsSelfFinished;
    Ogre::Real mSelfTotalRaceTime;
    std::map<std::string, bool> mIsRaceFinishedByHuman;//key - human name, self not included
    std::map<std::string, Ogre::Real> mRaceTimeByHuman;//key - human name, self not included
    std::vector<Ogre::Real> mRaceTimeByAI;

    bool mIsBurn;                       // self
    bool mIsBomb;                       // self
    bool mIsNitro;                      // self
    std::set<std::string> mIsOtherBurn; // other
    std::set<std::string> mIsOtherBomb; // other
    std::set<std::string> mIsOtherNitro;// other
};
#endif
#endif