#include "../pcheader.h"

#include "MultiplayerController.h"

#include "jsonxx.h"

#include "../tools/Tools.h"

#include "../tools/Conversions.h"

namespace{
    Ogre::NameGenerator nameGenNodes("Scene/Mutiplayer/Node/Name");
}

MultiplayerController::MultiplayerController(Events* events, size_t broadcastInterval)
    : mEvents(events), mReadySent(false), mSessionStarted(false), mStartHappened(false), mGameFinished(false),
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
    mGameFinished = false;
}

void MultiplayerController::clearLobby()
{
    mLobby.reset();
}

void MultiplayerController::onJoined(multislider::Lobby* lobby, const multislider::RoomInfo & room)
{
}

void MultiplayerController::addReadyPlayer(const std::string& playerName, const std::string& characterName)
{
    Ogre::LogManager::getSingleton().logMessage(Ogre::LML_NORMAL, "[MultiplayerController::addReadyPlayer]");

    mReadyPlayers.insert(std::make_pair(playerName, characterName));
}

void MultiplayerController::removePlayerFromLobby(const std::string& playerName)
{
    Ogre::LogManager::getSingleton().logMessage(Ogre::LML_NORMAL, "[MultiplayerController::removePlayerFromLobby]");

    mReadyPlayers.erase(playerName);
}

bool MultiplayerController::checkAllPlayersReady()const
{
    bool res = false;

    if(mReadyPlayers.size() == mPlayersLimits) res = true;

    return res;
}

void MultiplayerController::onRoomUpdate(multislider::Lobby* lobby, const multislider::RoomInfo & room, const std::string & sender, uint8_t flags)
{
    Ogre::LogManager::getSingleton().logMessage(Ogre::LML_NORMAL, "[MultiplayerController::onRoomUpdate]");

    if(mEvents)
    {
        if(flags & FLAG_IS_EJECTED)
        {
            mEvents->onPlayerEjected(sender);
        }

        if(flags & FLAG_JOINED)
        {
            mEvents->onPlayerJoined(sender);
        }

        if(flags & FLAG_LEFT)
        {
            removePlayerFromLobby(sender);
            mEvents->onPlayerLeft(sender);
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

void MultiplayerController::onMessage(multislider::Lobby* lobby, const multislider::RoomInfo & room, const std::string & sender, const std::string & message)
{
    Ogre::LogManager::getSingleton().logMessage(Ogre::LML_NORMAL, "[MultiplayerController::onMessage]");

    const bool isHost = lobby->isHost();

    if (!message.empty()) {

        Ogre::LogManager::getSingleton().logMessage(Ogre::LML_NORMAL, "[MultiplayerController::onMessage]: message [" + Ogre::String(message) + "]");

        if (isHost)
        {
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

            if (checkAllPlayersReady())
            {
                jsonxx::Object jsonObject;

                //humandata
                {
                    jsonxx::Array jsonArray;
                    for(std::map<std::string, std::string>::const_iterator i = mReadyPlayers.begin(), j = mReadyPlayers.end();
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

                Ogre::LogManager::getSingleton().logMessage(Ogre::LML_NORMAL, "[MultiplayerController::onMessage]: start session");
                lobby->startSession(jsonObject.json());
            }
        }
    }
}

void MultiplayerController::onLeft(multislider::Lobby* lobby, const multislider::RoomInfo & room, uint8_t flags)
{
}

void MultiplayerController::onSessionStart(multislider::Lobby* lobby, const multislider::RoomInfo & room, multislider::SessionPtr session, const std::string & sessionData)
{
    Ogre::LogManager::getSingleton().logMessage(Ogre::LML_NORMAL, "[MultiplayerController::onSessionStart]");

    try{
        if (!lobby->isHost())
        {
            Ogre::LogManager::getSingleton().logMessage(Ogre::LML_NORMAL, "[MultiplayerController::onSessionStart]: message [" + Ogre::String(sessionData) + "]");

            mAIDataTimestamp = 0;

            mAIPlayersSessionData.clear();
            mAISkins.clear();
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
                        mAISkins.push_back(jsonObject.get<jsonxx::String>("cartype"));
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

                Ogre::LogManager::getSingleton().logMessage(Ogre::LML_NORMAL, "[MultiplayerController::onSessionStart]: player added to list [" + Ogre::String(players[q] + "]"));
            }
            else
            {
                if(mEvents)
                {
                    mEvents->onSessionStart(room.getReservedPlayersNumber(), players, q, mLobby->isHost(), mAISkins, mReadyPlayers);
                }
            }
        }


        mSession = session;
        mSession->startup(this, 5 * 1000);
        mSessionStarted = true;
    }catch(const std::runtime_error& err)
    {
        Ogre::LogManager::getSingleton().logMessage(Ogre::LML_CRITICAL, "[MultiplayerController::onSessionStart]: " + Ogre::String(err.what()));
    }
}

void MultiplayerController::onStart(multislider::Session* session)
{
    Ogre::LogManager::getSingleton().logMessage(Ogre::LML_NORMAL, "[MultiplayerController::onStart]");

    mStartHappened = true;
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


    if(mEvents)
    {
        mEvents->onPlayerQuitSession(playerName, isHost);
    }

    mOtherPlayersSessionData.erase(playerName);

    if(isHost)
    {
        mAIPlayersSessionData.clear();
    }
}

bool MultiplayerController::startLobbyMaster(std::string ip, uint16_t port, std::string userName, std::string roomName, uint32_t playersLimits, uint32_t aiAmount)
{

    bool res = true;

    Ogre::LogManager::getSingleton().logMessage(Ogre::LML_NORMAL, "[MultiplayerController::startLobbyMaster]");

    try{

        mPlayersLimits = playersLimits;

        mLobby = multislider::shared_ptr<multislider::Lobby>(new multislider::Lobby(ip, port));

        multislider::Lobby::Status status = mLobby->createRoom(userName, roomName, "", mPlayersLimits + aiAmount, aiAmount, this);
        if(status == multislider::Lobby::SUCCESS)
        {
            Ogre::LogManager::getSingleton().logMessage(Ogre::LML_NORMAL, "[MultiplayerController::startLobbyMaster]: room created [" + Ogre::String(roomName) + "]");
        }
        else
        {
            res = false;
            if(mEvents)
            {
                mEvents->onError("room not created");
            }
            Ogre::LogManager::getSingleton().logMessage(Ogre::LML_NORMAL, "[MultiplayerController::startLobbyMaster]: room not created");
        }
    }catch(const std::runtime_error& err)
    {
        res = false;
        Ogre::LogManager::getSingleton().logMessage(Ogre::LML_CRITICAL, "[MultiplayerController::startLobbyMaster]: " + Ogre::String(err.what()));
    }

    return res;
}

bool MultiplayerController::startLobbySlave(std::string ip, uint16_t port, std::string userName, std::string roomName)
{
    Ogre::LogManager::getSingleton().logMessage(Ogre::LML_NORMAL, "[MultiplayerController::startLobbySlave]");

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
                Ogre::LogManager::getSingleton().logMessage(Ogre::LML_NORMAL, "[MultiplayerController::startLobbySlave]: room joined [" + Ogre::String(rooms[index].getName()) + "]");

            }
            else
            {
                res = false;
                if(mEvents)
                {
                    mEvents->onError("room not joined");
                }
                Ogre::LogManager::getSingleton().logMessage(Ogre::LML_CRITICAL, "[MultiplayerController::startLobbySlave]: room not joined");
            }
        }
        else
        {
            res = false;
            if(mEvents)
            {
                mEvents->onError("room not found");
            }
            Ogre::LogManager::getSingleton().logMessage(Ogre::LML_CRITICAL, "[MultiplayerController::startLobbySlave]: room not found");
        }

    }catch(const std::runtime_error& err)
    {
        res = false;
        Ogre::LogManager::getSingleton().logMessage(Ogre::LML_CRITICAL, "[MultiplayerController::startLobbySlave]: " + Ogre::String(err.what()));
    }

    return res;
}

bool MultiplayerController::startSessionMaster(const std::vector<std::string>& aiSkins, const std::string& playerCharacter)
{
    Ogre::LogManager::getSingleton().logMessage(Ogre::LML_NORMAL, "[MultiplayerController::startSessionMaster]");

    bool res = true;

    try{

        mAISkins = aiSkins;

        mReadyPlayers.clear();

        mLobby->say(fillLobbyReadyMessage(playerCharacter).json(), true);

        mReadySent = true;
    }catch(const std::runtime_error& err)
    {
        res = false;
        Ogre::LogManager::getSingleton().logMessage(Ogre::LML_CRITICAL, "[MultiplayerController::startSessionMaster]: " + Ogre::String(err.what()));
    }

    return res;
}

bool MultiplayerController::startSessionSlave(const std::string& playerCharacter)
{
    Ogre::LogManager::getSingleton().logMessage(Ogre::LML_NORMAL, "[MultiplayerController::startSessionSlave]");

    bool res = true;

    try{

        mAISkins.clear();

        mLobby->say(fillLobbyReadyMessage(playerCharacter).json(), false);

        mReadySent = true;
    }catch(const std::runtime_error& err)
    {
        res = false;
        Ogre::LogManager::getSingleton().logMessage(Ogre::LML_CRITICAL, "[MultiplayerController::startSessionSlave]: " + Ogre::String(err.what()));
    }

    return res;
}

void MultiplayerController::receiveSessionData()
{
    //Ogre::LogManager::getSingleton().logMessage(Ogre::LML_NORMAL, "[MultiplayerController::receiveSessionData]");

    if(mSessionStarted)
    {
        try{
            mSession->receive();
            //mSession->keepAlive();
        }catch(multislider::RuntimeError& e)
        {
            Ogre::LogManager::getSingleton().logMessage(Ogre::LML_CRITICAL, "[MultiplayerController::receiveSessionData]: " + Ogre::String(e.what()));
        }catch(...)
        {
            Ogre::LogManager::getSingleton().logMessage(Ogre::LML_CRITICAL, "[MultiplayerController::receiveSessionData]: Unknown exception");
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
    if(mSessionStarted && !mGameFinished && mSession)
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

jsonxx::Object MultiplayerController::fillLobbyReadyMessage(const std::string& characterName)
{
    jsonxx::Object jsonObject;

    jsonObject << "status" << true;
    jsonObject << "cartype" << characterName;

    return jsonObject;
}