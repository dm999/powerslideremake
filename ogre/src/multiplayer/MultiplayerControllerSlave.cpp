#include "../pcheader.h"

#include "MultiplayerControllerSlave.h"

#include "jsonxx.h"

MultiplayerControllerSlave::MultiplayerControllerSlave(MultiplayerControllerEvents* events, size_t broadcastInterval)
    : MultiplayerController(events, broadcastInterval)
{
}

bool MultiplayerControllerSlave::startLobbySlave(std::string ip, uint16_t port, std::string userName, std::string roomName)
{
    Ogre::LogManager::getSingleton().logMessage(Ogre::LML_NORMAL, "[MultiplayerControllerSlave::startLobbySlave]");

    bool res = true;

    try{
        mLobby = multislider::shared_ptr<multislider::Lobby>(new multislider::Lobby(ip, port));

        typedef std::vector<multislider::RoomInfo> roomV;
        const roomV& rooms = mLobby->getRooms();

        bool roomFound = false;
        size_t index = 0;
        for(size_t q = 0; q < rooms.size(); ++q)
        {
            if(rooms[q].getName() == roomName)
            {
                roomFound = true;
                index = q;
                break;
            }
        }

        if(roomFound)
        {
            multislider::Lobby::Status status = mLobby->joinRoom(userName, rooms[index], this);

            if(status == multislider::Lobby::SUCCESS)
            {
                Ogre::LogManager::getSingleton().logMessage(Ogre::LML_NORMAL, "[MultiplayerControllerSlave::startLobbySlave]: room joined [" + Ogre::String(rooms[index].getName()) + "]");

            }
            else
            {
                res = false;
                if(mEvents)
                {
                    mEvents->onError("room not joined");
                }
                Ogre::LogManager::getSingleton().logMessage(Ogre::LML_CRITICAL, "[MultiplayerControllerSlave::startLobbySlave]: room not joined");
            }
        }
        else
        {
            res = false;
            if(mEvents)
            {
                mEvents->onError("room not found");
            }
            Ogre::LogManager::getSingleton().logMessage(Ogre::LML_CRITICAL, "[MultiplayerControllerSlave::startLobbySlave]: room not found");
        }

    }catch(const std::runtime_error& err)
    {
        res = false;
        Ogre::LogManager::getSingleton().logMessage(Ogre::LML_CRITICAL, "[MultiplayerControllerSlave::startLobbySlave]: " + Ogre::String(err.what()));
    }

    return res;
}

bool MultiplayerControllerSlave::saySessionReady(const std::string& playerCharacter, bool isReady)
{
    Ogre::LogManager::getSingleton().logMessage(Ogre::LML_NORMAL, "[MultiplayerControllerSlave::saySessionReady]");

    bool res = true;

    try{

        mLobby->say(fillLobbyReadyMessage(playerCharacter, isReady).json(), true);

        mReadySent = true;
    }catch(const std::runtime_error& err)
    {
        res = false;
        Ogre::LogManager::getSingleton().logMessage(Ogre::LML_CRITICAL, "[MultiplayerControllerSlave::saySessionReady]: " + Ogre::String(err.what()));
    }

    return res;
}

void MultiplayerControllerSlave::onMessage(multislider::Lobby* lobby, const multislider::RoomInfo & room, const std::string & sender, const std::string & message)
{
    if (!message.empty())
    {
        Ogre::LogManager::getSingleton().logMessage(Ogre::LML_NORMAL, "[MultiplayerControllerSlave::onMessage]: message [" + Ogre::String(message) + "]");

        std::string playerCharacter;
        bool isReady = parseLobbyReadyMessage(message, playerCharacter);

        if(isReady)
        {
            addReadyPlayer(sender, playerCharacter);

            if(mEvents)
            {
                mEvents->onPlayerReady(sender);
            }
        }
        else
        {
            removeReadyPlayer(sender);

            if(mEvents)
            {
                mEvents->onPlayerNotReady(sender);
            }
        }
    }
}

void MultiplayerControllerSlave::onSessionStart(multislider::Lobby* lobby, const multislider::RoomInfo & room, multislider::SessionPtr session, const std::string & sessionData)
{
    Ogre::LogManager::getSingleton().logMessage(Ogre::LML_NORMAL, "[MultiplayerControllerSlave::onSessionStart]");

    std::vector<std::string> aiSkins;

    try{
        if (!lobby->isHost())
        {
            Ogre::LogManager::getSingleton().logMessage(Ogre::LML_NORMAL, "[MultiplayerControllerSlave::onSessionStart]: message [" + Ogre::String(sessionData) + "]");

            mAIDataTimestamp = 0;

            mAIPlayersSessionData.clear();
            mReadyPlayers.clear();

            for(size_t q = 0; q < room.getReservedPlayersNumber(); ++q)
            {
                mAIPlayersSessionData.push_back(MultiplayerSessionData());
            }

            jsonxx::Object jsonObjectBase;
            jsonObjectBase.parse(sessionData);


            //humandata
            {
                jsonxx::Array jsonArray = jsonObjectBase.get<jsonxx::Array>("humandata");

                for(size_t q = 0; q < jsonArray.size(); ++q)
                {
                    jsonxx::Object jsonObject = jsonArray.get<jsonxx::Object>(q);
                    if(jsonObject.has<jsonxx::String>("name") && jsonObject.has<jsonxx::String>("cartype"))
                    {
                        mReadyPlayers.insert(std::make_pair(jsonObject.get<jsonxx::String>("name"), jsonObject.get<jsonxx::String>("cartype")));
                    }
                }
            }

            //aidata
            {
                jsonxx::Array jsonArray = jsonObjectBase.get<jsonxx::Array>("aidata");

                for(size_t q = 0; q < jsonArray.size(); ++q)
                {
                    jsonxx::Object jsonObject = jsonArray.get<jsonxx::Object>(q);
                    if(jsonObject.has<jsonxx::String>("cartype"))
                    {
                        aiSkins.push_back(jsonObject.get<jsonxx::String>("cartype"));
                    }
                }
            }

            //hostdata
            {
            }
        }

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

                Ogre::LogManager::getSingleton().logMessage(Ogre::LML_NORMAL, "[MultiplayerControllerSlave::onSessionStart]: player added to list [" + Ogre::String(players[q] + "]"));
            }
            else//execute event for current player
            {
                if(mEvents)
                {
                    mEvents->onSessionStart(MultiplayerSessionStartInfo(room.getReservedPlayersNumber(), players, q, mLobby->isHost(), aiSkins, mReadyPlayers));
                }
            }
        }


        mSession = session;
        mSession->startup(this, 5 * 1000);
        mSessionStarted = true;
    }catch(const std::runtime_error& err)
    {
        Ogre::LogManager::getSingleton().logMessage(Ogre::LML_CRITICAL, "[MultiplayerControllerSlave::onSessionStart]: " + Ogre::String(err.what()));
    }
}