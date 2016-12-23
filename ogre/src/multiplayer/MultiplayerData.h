#ifndef MULTIPLAYERDATA_H
#define MULTIPLAYERDATA_H

#include "multislider/MultiSlider.h"

#include "multislider/CommonIncludes.h"

#include "../includes/OgreInclude.h"

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
        uint32_t aiAmount,
        const std::vector<std::string>& players,
        size_t playerIndex, bool isHost,
        const std::vector<std::string>& aiSkins,
        const std::map<std::string, std::string>& playersSkins,
        const std::string& trackName) :
        mAIAmount(aiAmount), mPlayers(players), mPlayerIndex(playerIndex),
        mIsHost(isHost), mAISkins(aiSkins), mPlayersSkins(playersSkins),
        mTrackName(trackName)
    {}

    uint32_t mAIAmount;
    std::vector<std::string> mPlayers;
    size_t mPlayerIndex;
    bool mIsHost;
    std::vector<std::string> mAISkins;
    std::map<std::string, std::string> mPlayersSkins;
    std::string mTrackName;
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
    virtual void onRoomClosed(const std::string& player) = 0;
    virtual void onPlayerReady(const std::string& player) = 0;
    virtual void onPlayerNotReady(const std::string& player) = 0;
    virtual void onPlayerAddedToSession(const std::string& player) = 0;
    virtual void onPlayerQuitSession(const std::string& player, bool isHost) = 0;
    virtual void onSessionReadyToStart() = 0;
    virtual void onSessionNotReadyToStart() = 0;
    virtual void onSessionStart(const MultiplayerSessionStartInfo& multiplayerSessionStartInfo) = 0;
    virtual void onSessionUpdate(const playerToData& otherPlayersSessionData, const std::vector<MultiplayerSessionData>& aiPlayersSessionData, bool isHost) = 0;
    virtual void onError(const std::string& message) = 0;
};

#endif