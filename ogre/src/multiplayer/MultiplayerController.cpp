#include "../pcheader.h"

#include "MultiplayerController.h"

#include "jsonxx.h"

#include "../tools/Tools.h"

#include "../tools/Conversions.h"

namespace{
    Ogre::NameGenerator nameGenNodes("Scene/Mutiplayer/Node/Name");
}

MultiplayerController::MultiplayerController(MultiplayerControllerEvents* events, size_t broadcastInterval)
    : mEvents(events), mReadySent(false), mSessionStarted(false), mStartHappened(false),
    mBroadcastInterval(broadcastInterval)
{
}

MultiplayerController::~MultiplayerController()
{
    clearSession();
    clearLobby();
}

void MultiplayerController::clearSession()
{
    mSession.reset();
    mReadySent = false;
    mSessionStarted = false;
    mStartHappened = false;
}

void MultiplayerController::clearLobby()
{
    mLobby.reset();
}

void MultiplayerController::addReadyPlayer(const std::string& playerName, const std::string& characterName)
{
    Ogre::LogManager::getSingleton().logMessage(Ogre::LML_NORMAL, "[MultiplayerController::addReadyPlayer]");

    mReadyPlayers.insert(std::make_pair(playerName, characterName));
}

void MultiplayerController::removeReadyPlayer(const std::string& playerName)
{
    Ogre::LogManager::getSingleton().logMessage(Ogre::LML_NORMAL, "[MultiplayerController::removeReadyPlayer]");
    mReadyPlayers.erase(playerName);
}

bool MultiplayerController::checkAllPlayersReady()const
{
    bool res = false;

    if(mReadyPlayers.size() == mPlayersLimits) res = true;

    return res;
}

void MultiplayerController::onJoined(multislider::Lobby* lobby, const multislider::RoomInfo & room)
{
    if(mEvents)
    {
        std::string player = lobby->getPlayerName();
        std::vector<std::string> players = room.getPlayers();
        std::vector<std::string> playersFiltered;
        for(size_t q = 0; q < players.size(); ++q)
        {
            if(players[q] != player)
                playersFiltered.push_back(players[q]);
        }

        mEvents->onRoomEnter(room.getName(), player, playersFiltered);
    }
}

void MultiplayerController::onRoomUpdate(multislider::Lobby* lobby, const multislider::RoomInfo & room, const std::string & sender, uint8_t flags)
{
    Ogre::LogManager::getSingleton().logMessage(Ogre::LML_NORMAL, "[MultiplayerController::onRoomUpdate]");

    if(mEvents)
    {
        if(flags & FLAG_IS_EJECTED)
        {
            //consider changes (remove from lobby!)
            removeReadyPlayer(sender);
            mEvents->onPlayerNotReady(sender);
            mEvents->onPlayerEjected(sender);
            checkAllPlayersReadyOrNot();
        }

        if(flags & FLAG_JOINED)
        {
            mEvents->onPlayerJoined(sender);
        }

        if(flags & FLAG_LEFT)
        {
            //consider changes (remove from lobby!)
            removeReadyPlayer(sender);
            mEvents->onPlayerNotReady(sender);
            mEvents->onPlayerLeft(sender);
            checkAllPlayersReadyOrNot();
        }

        if(flags & FLAG_NEW_HOST)
        {
            mEvents->onNewHost(sender);
        }

        if(flags & FLAG_RECONFIGURED_BY_HOST)
        {
        }

        if(flags & FLAG_RECONFIGURE_FAIL)
        {
        }

        if(flags & FLAG_ROOM_CLOSED_BY_HOST)
        {
            mEvents->onRoomClosed(sender);
        }
    }
}

void MultiplayerController::checkAllPlayersReadyOrNot()const
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

void MultiplayerController::onStart(multislider::Session* session)
{
    Ogre::LogManager::getSingleton().logMessage(Ogre::LML_NORMAL, "[MultiplayerController::onStart]");

    mStartSessionTimer.reset();
}

void MultiplayerController::onUpdate(multislider::Session* session, const multislider::SessionData & data, const multislider::PlayerData & sharedData)
{
    //Ogre::LogManager::getSingleton().logMessage(Ogre::LML_NORMAL, "[MultiplayerController::onUpdate]");

    //human data
    for(playerToData::iterator ii = mOtherPlayersSessionData.begin(), jj = mOtherPlayersSessionData.end(); 
        ii != jj ; ++ii)
    {
        multislider::SessionData::const_iterator i = data.find((*ii).first);
        if(i != data.end())
        {
            if(!(*i).second.data.empty() && (*i).second.timestamp > (*ii).second.dataUpdateTimestamp)
            {
                (*ii).second.dataUpdateTimestamp = (*i).second.timestamp;

                jsonxx::Object jsonObject;
                jsonObject.parse((*i).second.data);
                parseDataPacket((*ii).second, jsonObject);
            }
        }
    }

    //ai data
    if (!mLobby->isHost())
    {
        if(!sharedData.data.empty() && sharedData.timestamp > mAIDataTimestamp)
        {
            //Ogre::LogManager::getSingleton().logMessage(Ogre::LML_NORMAL, "[MultiplayerController::onUpdate]: shared data " + Ogre::String(sharedData.data));

            mAIDataTimestamp = sharedData.timestamp;
            jsonxx::Array jsonArray;
            jsonArray.parse(sharedData.data);
            if(mAIPlayersSessionData.size() == jsonArray.size())
            {
                for(size_t q = 0; q < jsonArray.size(); ++q)
                {
                    parseDataPacket(mAIPlayersSessionData[q], jsonArray.get<jsonxx::Object>(q));
                }
            }

            //Ogre::LogManager::getSingleton().logMessage(Ogre::LML_NORMAL, "[MultiplayerController::onUpdate]: time " + Ogre::String(Conversions::DMToString(mAIDataTimestamp)));
        }
    }

    if(mEvents)
    {
        mEvents->onSessionUpdate(mOtherPlayersSessionData, mAIPlayersSessionData, mLobby->isHost());
    }
}

void MultiplayerController::onQuit(multislider::Session* session, const std::string & playerName, bool byTimeout) throw ()
{

    bool isHost = mLobby->getRoomInfo().getHostName() == playerName;

    removeReadyPlayer(playerName);

    if(mEvents)
    {
        mEvents->onPlayerNotReady(playerName);
        mEvents->onPlayerQuitSession(playerName, isHost);
    }

    checkAllPlayersReadyOrNot();

    mOtherPlayersSessionData.erase(playerName);

    if(isHost)
    {
        mAIPlayersSessionData.clear();
    }
}

void MultiplayerController::receiveData()
{
    //Ogre::LogManager::getSingleton().logMessage(Ogre::LML_NORMAL, "[MultiplayerController::receiveData]");

    if(mSessionStarted)
    {
        if(!mStartHappened)
        {
            const size_t timeToWaitForOthersToLoad = 10000;//in ms
            if(mStartSessionTimer.getMilliseconds() > timeToWaitForOthersToLoad)
                mStartHappened = true;
        }

        try{
            mSession->receive();
            //mSession->keepAlive();
        }catch(multislider::RuntimeError& e)
        {
            Ogre::LogManager::getSingleton().logMessage(Ogre::LML_CRITICAL, "[MultiplayerController::receiveData]: " + Ogre::String(e.what()));
        }catch(...)
        {
            Ogre::LogManager::getSingleton().logMessage(Ogre::LML_CRITICAL, "[MultiplayerController::receiveData]: Unknown exception");
        }
    }
    else
    {
        if(mReadySent) // create / join room
        {
            mLobby->receive();
            Tools::Sleep(10);
        }
    }
}

void MultiplayerController::updateSessionData(const MultiplayerSessionData& dataHuman, const std::vector<MultiplayerSessionData>& dataAI, float spf)
{
    if(mSessionStarted && mSession)
    {

        if(mBroadcastTimer.getMilliseconds() > mBroadcastInterval)
        {
            mBroadcastTimer.reset();

            //https://github.com/hjiang/jsonxx

            std::string sharedData = "";

            if (mLobby->isHost())
            {
                jsonxx::Array jsonArray;
                for(size_t q = 0; q < dataAI.size(); ++q)
                {
                    jsonArray << fillDataPacket(dataAI[q]);
                }

                sharedData = jsonArray.json();
            }

            try{
                mSession->broadcast(fillDataPacket(dataHuman).json(), sharedData, false);
            }catch(multislider::RuntimeError& e)
            {
                Ogre::LogManager::getSingleton().logMessage(Ogre::LML_CRITICAL, "[MultiplayerController::updateSessionData]: " + Ogre::String(e.what()));
            }catch(...)
            {
                Ogre::LogManager::getSingleton().logMessage(Ogre::LML_CRITICAL, "[MultiplayerController::updateSessionData]: Unknown exception");
            }
        }
    }
}

size_t MultiplayerController::getSessionPing()const
{
    uint64_t res = 0;
    if(mSessionStarted && mSession)
    {
        res = mSession->getLastPing();
    }

    return static_cast<size_t>(res);
}

void MultiplayerController::parseDataPacket(MultiplayerSessionData& data, const jsonxx::Object& jsonObject)
{
    if(jsonObject.has<jsonxx::Number>("x"))
    {
        data.pos.x = static_cast<float>(jsonObject.get<jsonxx::Number>("x"));
        data.pos.y = static_cast<float>(jsonObject.get<jsonxx::Number>("y"));
        data.pos.z = static_cast<float>(jsonObject.get<jsonxx::Number>("z"));
        data.rot.x = static_cast<float>(jsonObject.get<jsonxx::Number>("rotx"));
        data.rot.y = static_cast<float>(jsonObject.get<jsonxx::Number>("roty"));
        data.rot.z = static_cast<float>(jsonObject.get<jsonxx::Number>("rotz"));
        data.rot.w = static_cast<float>(jsonObject.get<jsonxx::Number>("rotw"));
        data.vel.x = static_cast<float>(jsonObject.get<jsonxx::Number>("velx"));
        data.vel.y = static_cast<float>(jsonObject.get<jsonxx::Number>("vely"));
        data.vel.z = static_cast<float>(jsonObject.get<jsonxx::Number>("velz"));
        data.velang.x = static_cast<float>(jsonObject.get<jsonxx::Number>("velangx"));
        data.velang.y = static_cast<float>(jsonObject.get<jsonxx::Number>("velangy"));
        data.velang.z = static_cast<float>(jsonObject.get<jsonxx::Number>("velangz"));
        data.wheelpos[0].x = static_cast<float>(jsonObject.get<jsonxx::Number>("w0x"));
        data.wheelpos[0].y = static_cast<float>(jsonObject.get<jsonxx::Number>("w0y"));
        data.wheelpos[0].z = static_cast<float>(jsonObject.get<jsonxx::Number>("w0z"));
        data.wheelpos[1].x = static_cast<float>(jsonObject.get<jsonxx::Number>("w1x"));
        data.wheelpos[1].y = static_cast<float>(jsonObject.get<jsonxx::Number>("w1y"));
        data.wheelpos[1].z = static_cast<float>(jsonObject.get<jsonxx::Number>("w1z"));
        data.wheelpos[2].x = static_cast<float>(jsonObject.get<jsonxx::Number>("w2x"));
        data.wheelpos[2].y = static_cast<float>(jsonObject.get<jsonxx::Number>("w2y"));
        data.wheelpos[2].z = static_cast<float>(jsonObject.get<jsonxx::Number>("w2z"));
        data.wheelpos[3].x = static_cast<float>(jsonObject.get<jsonxx::Number>("w3x"));
        data.wheelpos[3].y = static_cast<float>(jsonObject.get<jsonxx::Number>("w3y"));
        data.wheelpos[3].z = static_cast<float>(jsonObject.get<jsonxx::Number>("w3z"));
        data.isAcc = jsonObject.get<jsonxx::Boolean>("isAcc");
        data.isBrake = jsonObject.get<jsonxx::Boolean>("isBrake");
        data.isLeft = jsonObject.get<jsonxx::Boolean>("isLeft");
        data.isRight = jsonObject.get<jsonxx::Boolean>("isRight");
        data.currentLap = static_cast<size_t>(jsonObject.get<jsonxx::Number>("lap"));
        data.lapPosition = static_cast<Ogre::Real>(jsonObject.get<jsonxx::Number>("lappos"));

        //Ogre::LogManager::getSingleton().logMessage(Ogre::LML_NORMAL, "[MultiplayerController::onUpdate]: x " + Ogre::String(Conversions::DMToString((*ii).second.pos.x)));
    }
}

jsonxx::Object MultiplayerController::fillDataPacket(const MultiplayerSessionData& data)
{
    jsonxx::Object jsonObject;
    jsonObject << "x" << data.pos.x;
    jsonObject << "y" << data.pos.y;
    jsonObject << "z" << data.pos.z;
    jsonObject << "rotx" << data.rot.x;
    jsonObject << "roty" << data.rot.y;
    jsonObject << "rotz" << data.rot.z;
    jsonObject << "rotw" << data.rot.w;
    jsonObject << "velx" << data.vel.x;
    jsonObject << "vely" << data.vel.y;
    jsonObject << "velz" << data.vel.z;
    jsonObject << "velangx" << data.velang.x;
    jsonObject << "velangy" << data.velang.y;
    jsonObject << "velangz" << data.velang.z;
    jsonObject << "w0x" << data.wheelpos[0].x;
    jsonObject << "w0y" << data.wheelpos[0].y;
    jsonObject << "w0z" << data.wheelpos[0].z;
    jsonObject << "w1x" << data.wheelpos[1].x;
    jsonObject << "w1y" << data.wheelpos[1].y;
    jsonObject << "w1z" << data.wheelpos[1].z;
    jsonObject << "w2x" << data.wheelpos[2].x;
    jsonObject << "w2y" << data.wheelpos[2].y;
    jsonObject << "w2z" << data.wheelpos[2].z;
    jsonObject << "w3x" << data.wheelpos[3].x;
    jsonObject << "w3y" << data.wheelpos[3].y;
    jsonObject << "w3z" << data.wheelpos[3].z;
    jsonObject << "isAcc" << data.isAcc;
    jsonObject << "isBrake" << data.isBrake;
    jsonObject << "isLeft" << data.isLeft;
    jsonObject << "isRight" << data.isRight;
    jsonObject << "lap" << data.currentLap;
    jsonObject << "lappos" << data.lapPosition;

    //Ogre::LogManager::getSingleton().logMessage(Ogre::LML_NORMAL, "[MultiplayerController::updateSessionData]: x " + Ogre::String(Conversions::DMToString(data.pos.x)));

    return jsonObject;
}

bool MultiplayerController::parseLobbyReadyMessage(const std::string& message, std::string& characterName)
{
    bool res = false;
    characterName = "frantic";

    jsonxx::Object jsonObject;
    jsonObject.parse(message);
    if(jsonObject.has<jsonxx::Boolean>("status"))
    {
        res = jsonObject.get<jsonxx::Boolean>("status");
    }

    if(jsonObject.has<jsonxx::String>("cartype"))
    {
        characterName = jsonObject.get<jsonxx::String>("cartype");
    }

    return res;
}

jsonxx::Object MultiplayerController::fillLobbyReadyMessage(const std::string& characterName, bool isReady)
{
    jsonxx::Object jsonObject;

    jsonObject << "status" << isReady;
    jsonObject << "cartype" << characterName;

    return jsonObject;
}