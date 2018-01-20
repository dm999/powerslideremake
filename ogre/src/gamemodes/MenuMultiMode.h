#ifndef MENUMULTIMODE_H
#define MENUMULTIMODE_H

#ifndef NO_MULTIPLAYER
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

    void keyUp(MyGUI::KeyCode _key, wchar_t _char )override;

    void mousePressed(const Ogre::Vector2& pos)override;
    void mouseReleased(const Ogre::Vector2& pos)override;
    void mouseMoved(const Ogre::Vector2& pos)override;

    bool isExitSubmenu()const override{return true;}
    void setExitSubmenu()override {}
    void setTopmostSubmenu()override {}

    CommonIncludes::shared_ptr<MultiplayerController> getMultiplayerController(){return mMultiplayerController;}

    //multiplayer
    void onRoomEnter(const std::string& roomName, const std::string& player, const std::vector<std::string>& players)override;
    void onPlayerEjected(const std::string& player)override;
    void onPlayerJoined(const std::string& player)override;
    void onPlayerLeft(const std::string& player)override;
    void onNewHost(const std::string& player)override;
    void onReconfigure(const std::string& player)override;
    void onReconfigureFailed(const std::string& player)override;
    void onRoomClosed(const std::string& player)override;
    void onLobbyMessage(const std::string& player, const MultiplayerLobbyData& data)override;
    void onPlayerAddedToSession(const std::string& player)override{}
    void onPlayerQuitSession(const std::string& player, bool isHost)override{}
    void onSessionReadyToStart()override;
    void onSessionNotReadyToStart()override;
    void onSessionStart(const MultiplayerSessionStartInfo& multiplayerSessionStartInfo)override;
    void onSessionUpdate(const playerToData& otherPlayersSessionData, const std::vector<MultiplayerSessionData>& aiPlayersSessionData, bool isHost)override{}
    void onError(const std::string& message)override;

    MultiplayerSessionStartInfo getMultiplayerSessionStartInfo()const{return mMultiplayerSessionStartInfo;}

    bool getIsLobbyEntered()const{return mIsLobbyEntered;}

#if defined(__ANDROID__)
    //for UI only
    void reloadTextures()override;
#endif

protected:

    void doInitData(LoaderListener* loaderListener)override;
    void doClearData()override;

    void customFrameRenderingQueued()override;

private:

    MultiplayerSessionStartInfo mMultiplayerSessionStartInfo;

    CommonIncludes::shared_ptr<MultiplayerController> mMultiplayerController;
    CommonIncludes::shared_ptr<UIMainMenuMulti> mUIMainMenuMulti;

    bool mIsEnterFromBaseMenu;

    bool mIsLobbyEntered;

};
#endif
#endif