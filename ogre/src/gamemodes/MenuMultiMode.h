#ifndef MENUMULTIMODE_H
#define MENUMULTIMODE_H

#include "BaseMode.h"

#include "../includes/CommonIncludes.h"

#include "../multiplayer/MultiplayerController.h"

class UIMainMenuMulti;

namespace MyGUI
{
    class Widget;
}

class MenuMultiMode : 
    public BaseMode,
    public MultiplayerController::Events
{
public:

    MenuMultiMode(const ModeContext& modeContext);
    MenuMultiMode(const ModeContext& modeContext, const CommonIncludes::shared_ptr<MultiplayerController>& controller);
    virtual ~MenuMultiMode(){}

    virtual void initData()override;
    virtual void clearData()override;

    virtual void frameStarted(const Ogre::FrameEvent &evt)override;
    virtual void frameRenderingQueued(const Ogre::FrameEvent& evt)override;

    CommonIncludes::shared_ptr<MultiplayerController> getMultiplayerController(){return mMultiplayerController;}
    void clearMultiplayerController();

    //multiplayer
    void onPlayerEjected(const std::string& player)override{}
    void onPlayerJoined(const std::string& player)override{}
    void onPlayerLeft(const std::string& player)override{}
    void onNewHost(const std::string& player)override{}
    void onRoomClosed(const std::string& player)override{}
    void onPlayerReady(const std::string& player)override{}
    void onPlayerAddedToSession(const std::string& player)override{}
    void onPlayerQuitSession(const std::string& player, bool isHost)override{}
    void onSessionStart(uint32_t aiAmount, const std::vector<std::string>& players, size_t playerIndex, bool isHost, const std::vector<std::string>& aiSkins, const std::map<std::string, std::string>& playersSkins)override{}
    void onSessionUpdate(const MultiplayerController::playerToData& otherPlayersSessionData, const std::vector<MultiplayerSessionData>& aiPlayersSessionData, bool isHost)override{}
    void onError(const std::string& message)override{}

#if defined(__ANDROID__)
    //for UI only
    void reloadTextures()override;
#endif

private:

    CommonIncludes::shared_ptr<MultiplayerController> mMultiplayerController;
    CommonIncludes::shared_ptr<UIMainMenuMulti> mUIMainMenuMulti;

};

#endif