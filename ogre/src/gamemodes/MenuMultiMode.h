#ifndef MENUMULTIMODE_H
#define MENUMULTIMODE_H

#include "BaseMenuMode.h"

#include "../includes/CommonIncludes.h"

#include "../multiplayer/MultiplayerController.h"

class UIMainMenuMulti;

class MenuMultiMode : 
    public BaseMenuMode,
    public MultiplayerControllerEvents
{
public:

    MenuMultiMode(const ModeContext& modeContext);
    MenuMultiMode(const ModeContext& modeContext, const CommonIncludes::shared_ptr<MultiplayerController>& controller);
    virtual ~MenuMultiMode(){}

    void frameStarted(const Ogre::FrameEvent &evt)override;

    CommonIncludes::shared_ptr<MultiplayerController> getMultiplayerController(){return mMultiplayerController;}
    void clearMultiplayerController();

    //multiplayer
    void onRoomEnter(const std::string& roomName, const std::string& player, const std::vector<std::string>& players)override;
    void onPlayerEjected(const std::string& player)override;
    void onPlayerJoined(const std::string& player)override;
    void onPlayerLeft(const std::string& player)override;
    void onNewHost(const std::string& player)override;
    void onRoomClosed(const std::string& player)override;
    void onPlayerReady(const std::string& player)override;
    void onPlayerNotReady(const std::string& player)override;
    void onPlayerAddedToSession(const std::string& player)override{}
    void onPlayerQuitSession(const std::string& player, bool isHost)override{}
    void onSessionReadyToStart()override;
    void onSessionNotReadyToStart()override;
    void onSessionStart(const MultiplayerSessionStartInfo& multiplayerSessionStartInfo)override;
    void onSessionUpdate(const playerToData& otherPlayersSessionData, const std::vector<MultiplayerSessionData>& aiPlayersSessionData, bool isHost)override{}
    void onError(const std::string& message)override;

    MultiplayerSessionStartInfo getMultiplayerSessionStartInfo()const{return mMultiplayerSessionStartInfo;}

#if defined(__ANDROID__)
    //for UI only
    void reloadTextures()override;
#endif

protected:

    void doInitData()override;
    void doClearData()override;

private:

    MultiplayerSessionStartInfo mMultiplayerSessionStartInfo;

    CommonIncludes::shared_ptr<MultiplayerController> mMultiplayerController;
    CommonIncludes::shared_ptr<UIMainMenuMulti> mUIMainMenuMulti;

    bool mIsEnterFromBaseMenu;

};

#endif