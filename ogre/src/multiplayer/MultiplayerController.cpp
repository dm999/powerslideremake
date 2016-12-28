#include "../pcheader.h"

#include "MultiplayerController.h"

#include "jsonxx.h"

#include "../tools/Tools.h"

#include "../tools/Conversions.h"

namespace{
    Ogre::NameGenerator nameGenNodes("Scene/Mutiplayer/Node/Name");
}

MultiplayerController::MultiplayerController(MultiplayerControllerEvents* events, size_t broadcastInterval)
    : mEvents(events), mSessionStarted(false), mStartHappened(false),
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
    mSessionStarted = false;
    mStartHappened = false;
}

void MultiplayerController::clearLobby()
{
    mLobby.reset();
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

bool MultiplayerController::sendLobbyMessage(bool isReady, const std::string& characterName, const std::string& playerMessage, const std::string& trackName, size_t aiCount, size_t aiStrength, size_t lapsCount)
{
    Ogre::LogManager::getSingleton().logMessage(Ogre::LML_NORMAL, "[MultiplayerController::sendLobbyMessage]");

    bool res = true;

    try{

        MultiplayerLobbyData multiplayerLobbyData(isReady, characterName, playerMessage, trackName, aiCount, aiStrength, lapsCount);
        mLobby->say(fillLobbyMessage(multiplayerLobbyData), true);

    }catch(const std::runtime_error& err)
    {
        res = false;
        Ogre::LogManager::getSingleton().logMessage(Ogre::LML_CRITICAL, "[MultiplayerControllerSlave::sendLobbyMessage]: " + Ogre::String(err.what()));
    }

    return res;
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
            if(!(*i).second.data.empty())
            {
                jsonxx::Object jsonObject;
                jsonObject.parse((*i).second.data);
                parseDataPacket((*ii).second, jsonObject);
            }
        }
    }

    //ai data
    if (!mLobby->isHost())
    {
        if(!sharedData.data.empty())
        {
            //Ogre::LogManager::getSingleton().logMessage(Ogre::LML_NORMAL, "[MultiplayerController::onUpdate]: shared data " + Ogre::String(sharedData.data));

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

            mLobby->receive();
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
        mLobby->receive();
        Tools::Sleep(10);
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
        data.dataUpdateTimestamp = static_cast<uint64_t>(jsonObject.get<jsonxx::Number>("timestamp"));
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
    jsonObject << "timestamp" << mStartSessionTimer.getMilliseconds();
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
    jsonObject << "isAcc" << data.isAcc;
    jsonObject << "isBrake" << data.isBrake;
    jsonObject << "isLeft" << data.isLeft;
    jsonObject << "isRight" << data.isRight;
    jsonObject << "lap" << data.currentLap;
    jsonObject << "lappos" << data.lapPosition;

    //Ogre::LogManager::getSingleton().logMessage(Ogre::LML_NORMAL, "[MultiplayerController::updateSessionData]: x " + Ogre::String(Conversions::DMToString(data.pos.x)));

    return jsonObject;
}

void MultiplayerController::parseLobbyMessage(const std::string& message, MultiplayerLobbyData& data)
{
    data.mIsReady = false;
    data.mCharacterName = "frantic";
    data.mPlayerMessage = "";
    data.mTrackName = "";

    jsonxx::Object jsonObject;
    jsonObject.parse(message);
    if(jsonObject.has<jsonxx::Boolean>("isReady"))
    {
        data.mIsReady = jsonObject.get<jsonxx::Boolean>("isReady");
    }

    if(jsonObject.has<jsonxx::String>("characterName"))
    {
        data.mCharacterName = jsonObject.get<jsonxx::String>("characterName");
    }

    if(jsonObject.has<jsonxx::String>("playerMessage"))
    {
        data.mPlayerMessage = jsonObject.get<jsonxx::String>("playerMessage");
    }

    if(jsonObject.has<jsonxx::String>("trackName"))
    {
        data.mTrackName = jsonObject.get<jsonxx::String>("trackName");
    }

    if(jsonObject.has<jsonxx::Number>("aiCount"))
    {
        data.mAICount = static_cast<size_t>(jsonObject.get<jsonxx::Number>("aiCount"));
    }

    if(jsonObject.has<jsonxx::Number>("aiStrength"))
    {
        data.mAIStrength = static_cast<size_t>(jsonObject.get<jsonxx::Number>("aiStrength"));
    }

    if(jsonObject.has<jsonxx::Number>("lapsCount"))
    {
        data.mLapsCount = static_cast<size_t>(jsonObject.get<jsonxx::Number>("lapsCount"));
    }
}

std::string MultiplayerController::fillLobbyMessage(const MultiplayerLobbyData& data)
{
    jsonxx::Object jsonObject;

    jsonObject << "isReady" << data.mIsReady;
    jsonObject << "characterName" << data.mCharacterName;
    jsonObject << "playerMessage" << data.mPlayerMessage;
    jsonObject << "trackName" << data.mTrackName;
    jsonObject << "aiCount" << data.mAICount;
    jsonObject << "aiStrength" << data.mAIStrength;
    jsonObject << "lapsCount" << data.mLapsCount;

    return jsonObject.json();
}