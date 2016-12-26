#ifndef MULTIPLAYERDATA_H
#define MULTIPLAYERDATA_H

#include "multislider/MultiSlider.h"

#include "multislider/CommonIncludes.h"

#include "../includes/OgreInclude.h"

/**
 * Data packet during lobby
 */
struct MultiplayerLobbyData
{
    //master & slave data
    bool mIsReady;
    std::string mCharacterName;  // defines car as well
    std::string mPlayerMessage;  // player said something

    //master data only
    std::string mTrackName;
    size_t mAICount;
    size_t mAIStrength;
    size_t mLapsCount;

    MultiplayerLobbyData(){}

    MultiplayerLobbyData(bool isReady, 
        const std::string& characterName, const std::string& playerMessage,
        const std::string& trackName,
        size_t aiCount,
        size_t aiStrength,
        size_t lapsCount
    ) :
    mIsReady(isReady),
    mCharacterName(characterName),
    mPlayerMessage(playerMessage),
    mTrackName(trackName),
    mAICount(aiCount),
    mAIStrength(aiStrength),
    mLapsCount(lapsCount)
    {}
};

/**
 * Data packet during racing session
 */
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

/**
 * Used for switch between multi menu mode to multi race mode:
 * multiplayer controller execute onSessionStart when GameModeSwitcher is in menu mode
 * this structure used to pass data into racing mode
 */
struct MultiplayerSessionStartInfo
{
    MultiplayerSessionStartInfo(){}

    MultiplayerSessionStartInfo(
        const std::vector<std::string>& players,
        size_t playerIndex, bool isHost,
        const std::vector<std::string>& aiSkins,
        const std::map<std::string, std::string>& playersSkins) :
        mPlayers(players), mPlayerIndex(playerIndex),
        mIsHost(isHost), mAISkins(aiSkins), mPlayersSkins(playersSkins)
    {}

    std::vector<std::string> mPlayers;
    size_t mPlayerIndex;
    bool mIsHost;
    std::vector<std::string> mAISkins;//size == aiCount
    std::map<std::string, std::string> mPlayersSkins;
};

typedef std::map<std::string, MultiplayerSessionData> playerToData;

class MultiplayerControllerEvents
{
public:
    virtual void onRoomEnter(const std::string& roomName, const std::string& player, const std::vector<std::string>& players) = 0;
    virtual void onPlayerEjected(const std::string& player) = 0;
    virtual void onPlayerJoined(const std::string& player) = 0;
    virtual void onPlayerLeft(const std::string& player) = 0;
    virtual void onNewHost(const std::string& player) = 0;
    virtual void onReconfigure(const std::string& player) = 0;
    virtual void onReconfigureFailed(const std::string& player) = 0;
    virtual void onRoomClosed(const std::string& player) = 0;
    virtual void onLobbyMessage(const std::string& player, const MultiplayerLobbyData& data) = 0;
    virtual void onPlayerAddedToSession(const std::string& player) = 0;
    virtual void onPlayerQuitSession(const std::string& player, bool isHost) = 0;
    
    //only for master
    virtual void onSessionReadyToStart() = 0;
    virtual void onSessionNotReadyToStart() = 0;

    virtual void onSessionStart(const MultiplayerSessionStartInfo& multiplayerSessionStartInfo) = 0;
    virtual void onSessionUpdate(const playerToData& otherPlayersSessionData, const std::vector<MultiplayerSessionData>& aiPlayersSessionData, bool isHost) = 0;
    virtual void onError(const std::string& message) = 0;
};

#endif