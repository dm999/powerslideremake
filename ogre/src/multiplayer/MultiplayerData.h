#ifndef MULTIPLAYERDATA_H
#define MULTIPLAYERDATA_H

#include "MultiSlider.h"
#include "CommonIncludes.h"

#include "../includes/OgreInclude.h"


enum MultiplayerLobbyDataType
{
    lobbyDataRegular = 0,
    lobbyDataLapTime = 1,
    lobbyDataLapTimeAI = 2
};

/**
 * Data packet during lobby
 */
struct MultiplayerLobbyData
{
    MultiplayerLobbyDataType mDataType;

    //regular lobby data

    //master & slave data
    bool mIsReady;
    std::string mCharacterName;  // defines car as well
    std::string mPlayerMessage;  // player said something

    //master data only
    std::string mTrackName;
    size_t mAICount;
    size_t mAIStrength;
    size_t mLapsCount;


    //race total time data
    Ogre::Real mRaceTotalTime;

    MultiplayerLobbyData(){}

    //lobby message constructor
    MultiplayerLobbyData(bool isReady, 
        const std::string& characterName, const std::string& playerMessage,
        const std::string& trackName,
        size_t aiCount,
        size_t aiStrength,
        size_t lapsCount
    ) :
        mDataType(lobbyDataRegular),
        mIsReady(isReady),
        mCharacterName(characterName),
        mPlayerMessage(playerMessage),
        mTrackName(trackName),
        mAICount(aiCount),
        mAIStrength(aiStrength),
        mLapsCount(lapsCount)
    {}

    //race message constructor
    MultiplayerLobbyData(MultiplayerLobbyDataType dataType, Ogre::Real raceTotalTime) :
        mDataType(dataType),
        mRaceTotalTime(raceTotalTime)
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
    bool isAcc;
    bool isBrake;
    Ogre::Real steering;

    Ogre::uint64 dataUpdateTimestamp;//other client update timestamp

    size_t currentLap;
    Ogre::Real lapPosition;

    MultiplayerSessionData() :
    pos(Ogre::Vector3::ZERO),
    vel(Ogre::Vector3::ZERO),
    velang(Ogre::Vector3::ZERO),
    isAcc(false), isBrake(false), steering(0.0f),
    dataUpdateTimestamp(0),
    currentLap(0), lapPosition(0.0f)
    {}
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
        const std::map<std::string, std::string>& playersSkins,
        const std::string& trackName,
        size_t aiStrength, size_t lapsCount) :
        mPlayers(players), mPlayerIndex(playerIndex),
        mIsHost(isHost), mAISkins(aiSkins), mPlayersSkins(playersSkins),
        mTrackName(trackName),
        mAIStrength(aiStrength),
        mLapsCount(lapsCount)
    {}

    std::vector<std::string> mPlayers;
    size_t mPlayerIndex;
    bool mIsHost;
    std::vector<std::string> mAISkins;//size == aiCount
    std::map<std::string, std::string> mPlayersSkins;

    std::string mTrackName;
    size_t mAIStrength;
    size_t mLapsCount;
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