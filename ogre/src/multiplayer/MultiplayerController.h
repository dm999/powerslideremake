#ifndef MULTIPLAYERCONTROLLER_H
#define MULTIPLAYERCONTROLLER_H

#include "MultiplayerData.h"

class MultiplayerController : public multislider::Lobby::Callback, public multislider::SessionCallback
{
public:

    MultiplayerController(MultiplayerControllerEvents* events, size_t broadcastInterval);
    ~MultiplayerController();

    void setEvents(MultiplayerControllerEvents* events){mEvents = events;}

    void clearSession();
    void clearLobby();

    virtual bool startLobbyMaster(std::string ip, uint16_t port, std::string userName, std::string roomName, uint32_t playersLimits, uint32_t aiAmount, const std::string& version) = 0;
    virtual bool startLobbySlave(std::string ip, uint16_t port, std::string userName, std::string roomName) = 0;

    bool sendLobbyMessage(bool isReady, const std::string& characterName, const std::string& playerMessage, const std::string& trackName, size_t aiCount, size_t aiStrength, size_t lapsCount);

    void receiveData();

    void updateSessionData(const MultiplayerSessionData& dataHuman, const std::vector<MultiplayerSessionData>& dataAI, float spf);

    bool getStartHappen()const{return mStartHappened;}

    size_t getSessionPing()const;

protected:

    // Lobby callback
    void onJoined(multislider::Lobby* lobby, const multislider::RoomInfo & room)override;
    void onLeft(multislider::Lobby* lobby, const multislider::RoomInfo & room, uint8_t flags)override{}

    // Session callback
    void onStart(multislider::Session* session)override;
    void onUpdate(multislider::Session* session, const multislider::SessionData & data, const multislider::PlayerData & sharedData)override;
    void onQuit(multislider::Session* session, const std::string & playerName, bool byTimeout) throw ()override;


    static void parseLobbyMessage(const std::string& message, MultiplayerLobbyData& data);
    static std::string fillLobbyMessage(const MultiplayerLobbyData& data);

    MultiplayerControllerEvents* mEvents;

    multislider::shared_ptr<multislider::Lobby> mLobby;
    multislider::SessionPtr mSession;

    bool mSessionStarted;

    playerToData mOtherPlayersSessionData;

    std::vector<MultiplayerSessionData> mAIPlayersSessionData;

private:

    static void parseDataPacket(MultiplayerSessionData& data, const jsonxx::Object& jsonObject);
    jsonxx::Object fillDataPacket(const MultiplayerSessionData& data);

    bool mStartHappened;//to synchronize race events

    Ogre::Timer mBroadcastTimer;

    Ogre::Timer mStartSessionTimer;//used to measure time from session start

    size_t mBroadcastInterval; // ms

};

#endif