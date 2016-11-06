#ifndef MULTIPLAYERCONTROLLER_H
#define MULTIPLAYERCONTROLLER_H

#include "multislider/MultiSlider.h"

#include "multislider/CommonIncludes.h"

#include "../OgreInclude.h"
#include "../GameCars.h"

struct MultiplayerSessionData
{
    Ogre::Vector3 pos;
    Ogre::Quaternion rot;
    Ogre::Vector3 vel;
    Ogre::Vector3 velang;
    Ogre::Vector3 wheelpos[4];
    bool isAcc;
    bool isBrake;
    bool isLeft;
    bool isRight;

    uint64_t dataUpdateTimestamp;//server side time

    size_t currentLap;
    Ogre::Real lapPosition;

    MultiplayerSessionData() :
    pos(Ogre::Vector3::ZERO),
    vel(Ogre::Vector3::ZERO),
    velang(Ogre::Vector3::ZERO),
    isAcc(false), isBrake(false), isLeft(false), isRight(false),
    dataUpdateTimestamp(0),
    currentLap(0), lapPosition(0.0f)
    {
        wheelpos[0] = Ogre::Vector3::ZERO;
        wheelpos[1] = Ogre::Vector3::ZERO;
        wheelpos[2] = Ogre::Vector3::ZERO;
        wheelpos[3] = Ogre::Vector3::ZERO;
    }
};

class MultiplayerController : public multislider::Lobby::Callback, public multislider::SessionCallback
{
public:

    typedef std::map<std::string, MultiplayerSessionData> playerToData;

    class Events
    {
    public:
        virtual void onPlayerEjected(const std::string& player) = 0;
        virtual void onPlayerJoined(const std::string& player) = 0;
        virtual void onPlayerLeft(const std::string& player) = 0;
        virtual void onNewHost(const std::string& player) = 0;
        virtual void onRoomClosed(const std::string& player) = 0;
        virtual void onPlayerReady(const std::string& player) = 0;
        virtual void onPlayerAddedToSession(const std::string& player) = 0;
        virtual void onPlayerQuitSession(const std::string& player, bool isHost) = 0;
        virtual void onSessionStart(uint32_t aiAmount, const std::vector<std::string>& players, size_t playerIndex, bool isHost, const std::vector<GameCars>& aiSkins, const std::map<std::string, GameCars>& playersSkins) = 0;
        virtual void onSessionUpdate(const playerToData& otherPlayersSessionData, const std::vector<MultiplayerSessionData>& aiPlayersSessionData, bool isHost) = 0;
        virtual void onError(const std::string& message) = 0;
    };

    Events* mEvents;

private:
    //-------------------------------------------------------
    // Lobby callback

    void onJoined(multislider::Lobby* lobby, const multislider::RoomInfo & room)override;
    void onRoomUpdate(multislider::Lobby* lobby, const multislider::RoomInfo & room, const std::string & sender, uint8_t flags)override;
    void onMessage(multislider::Lobby* lobby, const multislider::RoomInfo & room, const std::string & sender, const std::string & message)override;
    void onSessionStart(multislider::Lobby* lobby, const multislider::RoomInfo & room, multislider::SessionPtr session, const std::string & sessionData)override;
    void onLeft(multislider::Lobby* lobby, const multislider::RoomInfo & room, uint8_t flags)override;

    //-------------------------------------------------------
    // Session callback

    void onStart(multislider::Session* session)override;
    void onUpdate(multislider::Session* session, const multislider::SessionData & data, const multislider::PlayerData & sharedData)override;
    void onQuit(multislider::Session* session, const std::string & playerName, bool byTimeout) throw ()override;

    void addReadyPlayer(const std::string& playerName, const GameCars& playerSkin);
    void removePlayerFromLobby(const std::string& playerName);
    bool checkAllPlayersReady()const;

    static bool parseLobbyReadyMessage(const std::string& message, GameCars& playerSkin);
    static jsonxx::Object fillLobbyReadyMessage(const GameCars& playerSkin);

    static void parseDataPacket(MultiplayerSessionData& data, const jsonxx::Object& jsonObject);
    static jsonxx::Object fillDataPacket(const MultiplayerSessionData& data);


    multislider::shared_ptr<multislider::Lobby> mLobby;
    multislider::SessionPtr mSession;

    uint32_t mPlayersLimits;
    std::map<std::string, GameCars> mReadyPlayers;//used: write in master, read in slaves
    bool mReadySent;
    bool mSessionStarted;
    bool mStartHappened;//to synchronize race events
    bool mGameFinished;

    playerToData mOtherPlayersSessionData;

    std::vector<MultiplayerSessionData> mAIPlayersSessionData;
    uint64_t mAIDataTimestamp;

    Ogre::Timer mBroadcastTimer;

    std::vector<GameCars> mAISkins;

    size_t mBroadcastInterval; // ms

public:

    MultiplayerController(Events* events, size_t broadcastInterval);
    ~MultiplayerController();

    void clearSessionAndLobby();

    bool startSessionMaster(std::string ip, uint16_t port, std::string userName, std::string roomName, uint32_t playersLimits, uint32_t aiAmount, const std::vector<GameCars>& aiSkins, const GameCars& playerSkin);
    bool startSessionSlave(std::string ip, uint16_t port, std::string userName, std::string roomName, const GameCars& playerSkin);

    void receiveSessionData();

    void updateSessionData(const MultiplayerSessionData& dataHuman, const std::vector<MultiplayerSessionData>& dataAI, float spf);

    bool getReadySent()const {return mReadySent;};

    bool getStartHappen()const{return mStartHappened;}

    size_t getSessionPing()const;

};

#endif