#ifndef MULTIPLAYERCONTROLLERSLAVE_H
#define MULTIPLAYERCONTROLLERSLAVE_H

#include "MultiplayerController.h"

class MultiplayerControllerSlave : public MultiplayerController
{
public:
    MultiplayerControllerSlave(MultiplayerControllerEvents* events, size_t broadcastInterval);
    virtual ~MultiplayerControllerSlave(){}

    bool startLobbyMaster(std::string ip, uint16_t port, std::string userName, std::string roomName, uint32_t playersLimits, uint32_t aiAmount, const std::string& version)override{assert(false); return false;}
    bool startLobbySlave(std::string ip, uint16_t port, std::string userName, std::string roomName)override;

    void switchedToMainMenu()override{}

    // Lobby callback
    void onMessage(multislider::Lobby* lobby, const multislider::RoomInfo & room, const std::string & sender, const std::string & message)override;
    void onRoomUpdate(multislider::Lobby* lobby, const multislider::RoomInfo & room, const std::string & sender, uint8_t flags)override;
    void onSessionStart(multislider::Lobby* lobby, const multislider::RoomInfo & room, multislider::SessionPtr session, const std::string & sessionData)override;

private:
};

#endif