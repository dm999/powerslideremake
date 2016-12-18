#ifndef MULTIPLAYERCONTROLLERMASTER_H
#define MULTIPLAYERCONTROLLERMASTER_H

#include "MultiplayerController.h"

class MultiplayerControllerMaster : public MultiplayerController
{
public:
    MultiplayerControllerMaster(MultiplayerControllerEvents* events, size_t broadcastInterval);
    virtual ~MultiplayerControllerMaster(){}

    bool startLobbyMaster(std::string ip, uint16_t port, std::string userName, std::string roomName, uint32_t playersLimits, uint32_t aiAmount)override;
    bool startLobbySlave(std::string ip, uint16_t port, std::string userName, std::string roomName)override{assert(false);return false;}

    void setAISkins(const std::vector<std::string>& aiSkins){mAISkins = aiSkins;}
    bool saySessionReady(const std::string& playerCharacter, bool isReady)override;

    // Lobby callback
    void onMessage(multislider::Lobby* lobby, const multislider::RoomInfo & room, const std::string & sender, const std::string & message)override;
    void onSessionStart(multislider::Lobby* lobby, const multislider::RoomInfo & room, multislider::SessionPtr session, const std::string & sessionData)override;

    void startSession();

    void reconfigureSession(size_t aiAmount);

private:

    std::vector<std::string> mAISkins;
};

#endif