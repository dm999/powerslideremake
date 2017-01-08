#include "../pcheader.h"

#include "MultiplayerControllerMaster.h"

#include "jsonxx.h"

MultiplayerControllerMaster::MultiplayerControllerMaster(MultiplayerControllerEvents* events, size_t broadcastInterval)
    : MultiplayerController(events, broadcastInterval)
{
}

bool MultiplayerControllerMaster::startLobbyMaster(std::string ip, uint16_t port, std::string userName, std::string roomName, uint32_t playersLimits, uint32_t aiAmount, const std::string& version)
{

    bool res = true;

    Ogre::LogManager::getSingleton().logMessage(Ogre::LML_NORMAL, "[MultiplayerControllerMaster::startLobbyMaster]");

    try{

        mLobby = multislider::shared_ptr<multislider::Lobby>(new multislider::Lobby(ip, port));

        multislider::Lobby::Status status = mLobby->createRoom(userName, roomName, version, playersLimits + aiAmount, aiAmount, this);
        if(status == multislider::Lobby::SUCCESS)
        {
            addPlayer(userName);

            Ogre::LogManager::getSingleton().logMessage(Ogre::LML_NORMAL, "[MultiplayerControllerMaster::startLobbyMaster]: room created [" + Ogre::String(roomName) + "]");
        }
        else
        {
            res = false;
            if(mEvents)
            {
                mEvents->onError("room not created");
            }
            Ogre::LogManager::getSingleton().logMessage(Ogre::LML_NORMAL, "[MultiplayerControllerMaster::startLobbyMaster]: room not created");
        }
    }catch(const std::runtime_error& err)
    {
        res = false;
        Ogre::LogManager::getSingleton().logMessage(Ogre::LML_CRITICAL, "[MultiplayerControllerMaster::startLobbyMaster]: " + Ogre::String(err.what()));
    }

    return res;
}

std::vector<std::string> MultiplayerControllerMaster::getAllPlayersSkins()const
{
    std::vector<std::string> res;

    for(std::map<std::string, std::string>::const_iterator i = mAllPlayers.begin(), j = mAllPlayers.end();
        i != j; ++i)
    {
        res.push_back((*i).second);
    }

    return res;
}

void MultiplayerControllerMaster::onMessage(multislider::Lobby* lobby, const multislider::RoomInfo & room, const std::string & sender, const std::string & message)
{
    Ogre::LogManager::getSingleton().logMessage(Ogre::LML_NORMAL, "[MultiplayerControllerMaster::onMessage]");

    const bool isHost = lobby->isHost();

    if (!message.empty()) {

        Ogre::LogManager::getSingleton().logMessage(Ogre::LML_NORMAL, "[MultiplayerControllerMaster::onMessage]: message [" + Ogre::String(message) + "]");

        if (isHost)
        {
            MultiplayerLobbyData multiplayerLobbyData;
            parseLobbyMessage(message, multiplayerLobbyData);

            //only if message during lobby session
            if(multiplayerLobbyData.mDataType == lobbyDataRegular)
            {
                if(multiplayerLobbyData.mIsReady)
                {
                    setPlayerReady(sender, multiplayerLobbyData.mCharacterName);
                }
                else
                {
                    resetPlayerReady(sender);
                }
            }

            if(mEvents)
            {
                mEvents->onLobbyMessage(sender, multiplayerLobbyData);
            }

            //only if message during lobby session
            if(multiplayerLobbyData.mDataType == lobbyDataRegular)
                checkAllPlayersReadyOrNot();

        }
    }
}

void MultiplayerControllerMaster::onRoomUpdate(multislider::Lobby* lobby, const multislider::RoomInfo & room, const std::string & sender, uint8_t flags)
{
    Ogre::LogManager::getSingleton().logMessage(Ogre::LML_NORMAL, "[MultiplayerControllerMaster::onRoomUpdate]");

    if(mEvents)
    {
        if(flags & FLAG_IS_EJECTED)
        {
            removePlayer(sender);
            mEvents->onPlayerEjected(sender);
            checkAllPlayersReadyOrNot();
        }

        if(flags & FLAG_JOINED)
        {
            addPlayer(sender);
            mEvents->onPlayerJoined(sender);
            checkAllPlayersReadyOrNot();
        }

        if(flags & FLAG_LEFT)
        {
            removePlayer(sender);
            mEvents->onPlayerLeft(sender);
            checkAllPlayersReadyOrNot();
        }

        if(flags & FLAG_NEW_HOST)
        {
            mEvents->onNewHost(sender);
        }

        if(flags & FLAG_RECONFIGURED_BY_HOST)
        {
            mEvents->onReconfigure(sender);
        }

        if(flags & FLAG_RECONFIGURE_FAIL)
        {
            mEvents->onReconfigureFailed(sender);
        }

        if(flags & FLAG_ROOM_CLOSED_BY_HOST)
        {
            mEvents->onRoomClosed(sender);
        }
    }
}

void MultiplayerControllerMaster::onSessionStart(multislider::Lobby* lobby, const multislider::RoomInfo & room, multislider::SessionPtr session, const std::string & sessionData)
{
    Ogre::LogManager::getSingleton().logMessage(Ogre::LML_NORMAL, "[MultiplayerControllerMaster::onSessionStart]");

    try{

        //human
        mOtherPlayersSessionData.clear();

        const std::vector<std::string> players = room.getPlayers();
        for(size_t q = 0; q < players.size(); ++q)
        {
            if(players[q] != lobby->getPlayerName())//current player name
            {
                mOtherPlayersSessionData.insert(std::make_pair(players[q], MultiplayerSessionData()));

                if(mEvents)
                {
                    mEvents->onPlayerAddedToSession(players[q]);
                }

                Ogre::LogManager::getSingleton().logMessage(Ogre::LML_NORMAL, "[MultiplayerControllerMaster::onSessionStart]: player added to list [" + Ogre::String(players[q] + "]"));
            }
            else//execute event for current player
            {
                if(mEvents)
                {
                    MultiplayerSessionStartInfo sessionStartInfo(
                        players, q, 
                        mLobby->isHost(), 
                        mAISkins, mAllPlayers, "", 0, 0);
                    mEvents->onSessionStart(sessionStartInfo);
                }
            }
        }


        mSession = session;
        mSession->startup(this, 10000);
        mSessionStarted = true;
    }catch(const std::runtime_error& err)
    {
        Ogre::LogManager::getSingleton().logMessage(Ogre::LML_CRITICAL, "[MultiplayerControllerMaster::onSessionStart]: " + Ogre::String(err.what()));
    }
}

void MultiplayerControllerMaster::startSession(const std::string& trackName, size_t aiStrength, size_t lapsCount)
{
    const bool isHost = mLobby->isHost();

    if (isHost)
    {
        if (checkAllPlayersReady())
        {

            resetPlayersReady();

            jsonxx::Object jsonObject;

            //humandata
            {
                jsonxx::Array jsonArray;
                for(std::map<std::string, std::string>::const_iterator i = mAllPlayers.begin(), j = mAllPlayers.end();
                    i != j; ++i)
                {
                    jsonxx::Object jsonObjectHuman;
                    jsonObjectHuman << "name" << (*i).first;
                    jsonObjectHuman << "cartype" << (*i).second;
                    jsonArray << jsonObjectHuman;
                }
                jsonObject << "humandata" << jsonArray;
            }

            //aidata
            {
                jsonxx::Array jsonArray;
                for(size_t q = 0; q < mAISkins.size(); ++q)
                {
                    jsonxx::Object jsonObjectAI;
                    jsonObjectAI << "cartype" << mAISkins[q];
                    jsonArray << jsonObjectAI;
                }
                jsonObject << "aidata" << jsonArray;
            }

            //host info
            {
                jsonxx::Object jsonObjectHostInfo;
                jsonObjectHostInfo << "trackName" << trackName;
                jsonObjectHostInfo << "aiStrength" << aiStrength;
                jsonObjectHostInfo << "lapsCount" << lapsCount;
                jsonObject << "hostinfo" << jsonObjectHostInfo;
            }

            Ogre::LogManager::getSingleton().logMessage(Ogre::LML_NORMAL, "[MultiplayerControllerMaster::startSession]: start session");
            mLobby->startSession(jsonObject.json());
        }
    }
}

void MultiplayerControllerMaster::reconfigureSession(size_t aiAmount)
{
    mLobby->reconfigure(12, aiAmount);
}

void MultiplayerControllerMaster::addPlayer(const std::string& playerName)
{
    Ogre::LogManager::getSingleton().logMessage(Ogre::LML_NORMAL, "[MultiplayerControllerMaster::addPlayer]");

    mAllPlayers.insert(std::make_pair(playerName, "frantic"));
    mReadyPlayers.insert(std::make_pair(playerName, false));
}

void MultiplayerControllerMaster::removePlayer(const std::string& playerName)
{
    Ogre::LogManager::getSingleton().logMessage(Ogre::LML_NORMAL, "[MultiplayerControllerMaster::removePlayer]");

    mAllPlayers.erase(playerName);
    mReadyPlayers.erase(playerName);
}

void MultiplayerControllerMaster::setPlayerReady(const std::string& playerName, const std::string& characterName)
{
    Ogre::LogManager::getSingleton().logMessage(Ogre::LML_NORMAL, "[MultiplayerControllerMaster::setPlayerReady]");

    mAllPlayers[playerName] = characterName;
    mReadyPlayers[playerName] = true;
}

void MultiplayerControllerMaster::resetPlayerReady(const std::string& playerName)
{
    Ogre::LogManager::getSingleton().logMessage(Ogre::LML_NORMAL, "[MultiplayerControllerMaster::resetPlayerReady]");

    mReadyPlayers[playerName] = false;
}

void MultiplayerControllerMaster::resetPlayersReady()
{
    Ogre::LogManager::getSingleton().logMessage(Ogre::LML_NORMAL, "[MultiplayerControllerMaster::resetPlayersReady]");

    for(std::map<std::string, bool>::iterator i = mReadyPlayers.begin(), j = mReadyPlayers.end(); i != j; ++i)
    {
        (*i).second = false;
    }
}

bool MultiplayerControllerMaster::checkAllPlayersReady()const
{
    bool res = false;

    size_t readyAmount = 0;
    for(std::map<std::string, bool>::const_iterator i = mReadyPlayers.begin(), j = mReadyPlayers.end(); i != j; ++i)
    {
        if((*i).second)
        {
            ++readyAmount;
        }
    }

    if(readyAmount == mAllPlayers.size()) res = true;

    return res;
}

void MultiplayerControllerMaster::checkAllPlayersReadyOrNot()const
{
    if (checkAllPlayersReady())
    {
        if(mEvents)
        {
            mEvents->onSessionReadyToStart();
        }
    }
    else
    {
        if(mEvents)
        {
            mEvents->onSessionNotReadyToStart();
        }
    }
}