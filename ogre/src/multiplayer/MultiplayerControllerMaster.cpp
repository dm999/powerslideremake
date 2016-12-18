#include "../pcheader.h"

#include "MultiplayerControllerMaster.h"

#include "jsonxx.h"

MultiplayerControllerMaster::MultiplayerControllerMaster(MultiplayerControllerEvents* events, size_t broadcastInterval)
    : MultiplayerController(events, broadcastInterval)
{
}

bool MultiplayerControllerMaster::startLobbyMaster(std::string ip, uint16_t port, std::string userName, std::string roomName, uint32_t playersLimits, uint32_t aiAmount)
{

    bool res = true;

    Ogre::LogManager::getSingleton().logMessage(Ogre::LML_NORMAL, "[MultiplayerControllerMaster::startLobbyMaster]");

    try{

        mLobby = multislider::shared_ptr<multislider::Lobby>(new multislider::Lobby(ip, port));

        multislider::Lobby::Status status = mLobby->createRoom(userName, roomName, "", playersLimits + aiAmount, aiAmount, this);
        if(status == multislider::Lobby::SUCCESS)
        {
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

bool MultiplayerControllerMaster::saySessionReady(const std::string& playerCharacter, bool isReady)
{
    Ogre::LogManager::getSingleton().logMessage(Ogre::LML_NORMAL, "[MultiplayerControllerMaster::saySessionReady]");

    bool res = true;

    try{

        mLobby->say(fillLobbyReadyMessage(playerCharacter, isReady).json(), true);

        mReadySent = true;
    }catch(const std::runtime_error& err)
    {
        res = false;
        Ogre::LogManager::getSingleton().logMessage(Ogre::LML_CRITICAL, "[MultiplayerControllerMaster::saySessionReady]: " + Ogre::String(err.what()));
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
            std::string playerCharacter;
            bool isReady = parseLobbyReadyMessage(message, playerCharacter);

            if(isReady)
            {
                setPlayerReady(sender, playerCharacter);

                if(mEvents)
                {
                    mEvents->onPlayerReady(sender);
                }
            }
            else
            {
                resetPlayerReady(sender);

                if(mEvents)
                {
                    mEvents->onPlayerNotReady(sender);
                }
            }

            checkAllPlayersReadyOrNot();

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
                    mEvents->onSessionStart(MultiplayerSessionStartInfo(room.getReservedPlayersNumber(), players, q, mLobby->isHost(), mAISkins, mAllPlayers));
                }
            }
        }


        mSession = session;
        mSession->startup(this, 5 * 1000);
        mSessionStarted = true;
    }catch(const std::runtime_error& err)
    {
        Ogre::LogManager::getSingleton().logMessage(Ogre::LML_CRITICAL, "[MultiplayerControllerMaster::onSessionStart]: " + Ogre::String(err.what()));
    }
}

void MultiplayerControllerMaster::startSession()
{
    const bool isHost = mLobby->isHost();

    if (isHost)
    {
        if (checkAllPlayersReady())
        {

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
            }

            Ogre::LogManager::getSingleton().logMessage(Ogre::LML_NORMAL, "[MultiplayerControllerMaster::startSession]: start session");
            mLobby->startSession(jsonObject.json());
        }
    }
}

void MultiplayerControllerMaster::reconfigureSession(size_t aiAmount)
{
    size_t totalNumber = 12 - aiAmount;
    mLobby->reconfigure(totalNumber, aiAmount);
}