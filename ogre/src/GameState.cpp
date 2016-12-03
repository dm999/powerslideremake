
#include "pcheader.h"

#include "GameState.h"

GameState::GameState() :
    mOriginalDataInited(false),
    mLLTObject(NULL),
    mGlobalLight(NULL),
    mShadowLight(NULL),
    mIsRaceStarted(false),
    mIsRaceFinished(false),
    mIsMirrorEnabled(false),
    mLapsCount(1),
    mListenerGain(1.0f),
    mIsGamePaused(false),
    mIsMultiplayerMaster(false),
    mMultiplayerRoomName("Powerslide"),
    mMultiplayerUserName("Rasputin"),
    mMultiplayerServerIP(""),
    mMultiplayerServerPort(8800),
    mMultiplayerPlayersLimits(2),
    mMultiplayerAmountAI(0),
    mMultiplayerBroadcastInterval(150)
{
}

GameState::~GameState()
{
}

void GameState::initOriginalData(const std::string& originalPathData, const std::string& originalPathCommon)
{
    bool isLoaded = mPFLoaderData.init(originalPathData, "data.pf");

    if(isLoaded)
    {
        isLoaded = mPFLoaderGameshell.init(originalPathData, "gameshell.pf");

        if(isLoaded)
        {
            isLoaded = mPFLoaderStore.init(originalPathCommon, "store.pf");

            if(isLoaded)
            {
                mSTRPowerslide.parse(mPFLoaderStore);
                mSTRRacecrud.parse(mPFLoaderStore);

                mOriginalDataInited = true;
            }
            {
                Ogre::LogManager::getSingleton().logMessage(Ogre::LML_CRITICAL, "[CRITICAL]: Unable to locate store.pf file");
            }
        }
        else
        {
            Ogre::LogManager::getSingleton().logMessage(Ogre::LML_CRITICAL, "[CRITICAL]: Unable to locate gameshell.pf file");
        }
    }
    else
    {
        Ogre::LogManager::getSingleton().logMessage(Ogre::LML_CRITICAL, "[CRITICAL]: Unable to locate data.pf file");
    }
}

void GameState::setRaceParameters(const std::string& trackName, AIStrength aiStrength)
{
    mTrackName = trackName;

    mAIStrength = aiStrength;

    if(mAIStrength > Easy && mTrackName == "speedway track")
    {
        mTrackName = "speedway night track";
    }

    mBackgroundColor = mSTRPowerslide.getTrackSkyColor(mTrackName);
    mLapsCount = mSTRPowerslide.getLapsCount(mTrackName);
}

const PFLoader& GameState::getPFLoaderData() const
{
    assert(mOriginalDataInited);
    return mPFLoaderData;
}

const PFLoader& GameState::getPFLoaderGameshell() const
{
    assert(mOriginalDataInited);
    return mPFLoaderGameshell;
}

const PFLoader& GameState::getPFLoaderStore() const
{
    assert(mOriginalDataInited);
    return mPFLoaderStore;
}

const STRPowerslide& GameState::getSTRPowerslide() const
{
    assert(mOriginalDataInited);
    return mSTRPowerslide;
}

const STRRacecrud& GameState::getSTRRacecrud() const
{
    assert(mOriginalDataInited);
    return mSTRRacecrud;
}

PSAICar& GameState::getAICar(size_t index)
{
    if(index >= mAIMax)
        assert(false && "GameState::getAICar");

    return mPSCar[index];
}

PSPlayerCar& GameState::getPlayerCar()
{
    return mPSPlayerCar;
}

void GameState::setAICount(size_t opponentsAmount)
{
    mAiOpponentsAmount = Ogre::Math::Clamp<size_t>(opponentsAmount, 0, mAIMax);
}

void GameState::setGlobalLight(Ogre::Light* light)
{
    mGlobalLight = light;
}

void GameState::setShadowLight(Ogre::Light* light)
{
    mShadowLight = light;
}

void GameState::setGamePaused()
{
    if(!mIsGamePaused)
    {
        mIsGamePaused = true;
        Ogre::ControllerManager::getSingleton().setTimeFactor(0.0f);
        mBeforeStartTimer.pause();
        mPSPlayerCar.getLapUtils().pauseLapTimer();
        for(size_t q = 0; q < mAiOpponentsAmount; ++q)
        {
            mPSCar[q].getLapUtils().pauseLapTimer();
        }
    }
}

void GameState::resetGamePaused()
{
    mIsGamePaused = false;
    Ogre::ControllerManager::getSingleton().setTimeFactor(1.0f);
    mBeforeStartTimer.resume();
    mPSPlayerCar.getLapUtils().resumeLapTimer();
    for(size_t q = 0; q < mAiOpponentsAmount; ++q)
    {
        mPSCar[q].getLapUtils().resumeLapTimer();
    }
}

PSMultiplayerCar& GameState::getMultiplayerCarAI(size_t index)
{
    if(index >= mMultiplayerMaxAI)
        assert(false && "GameState::getMultiplayerCarAI");

    return mMultiplayerCarAI[index];
}

void GameState::setMultiplayerCountAI(size_t multiplayerAmountAI)
{
    mMultiplayerAmountAI = Ogre::Math::Clamp<size_t>(multiplayerAmountAI, 0, mMultiplayerMaxAI);
}

std::vector<std::string> GameState::getMultiplayerCarHumanNames() const
{
    std::vector<std::string> ret;

    for(std::map<std::string, PSMultiplayerCar>::const_iterator i = mMultiplayerCarHuman.begin(), j = mMultiplayerCarHuman.end();
        i != j; ++i)
    {
        ret.push_back((*i).first);
    }

    return ret;
}

PSMultiplayerCar& GameState::getMultiplayerCarHuman(const std::string& playerName)
{
    std::map<std::string, PSMultiplayerCar>::iterator i = mMultiplayerCarHuman.find(playerName);

    assert(i != mMultiplayerCarHuman.end() && "GameState::getMultiplayerCarHuman");

    return (*i).second;
}

void GameState::addMultiplayerCarHuman(const std::string& playerName)
{
    if(mMultiplayerCarHuman.find(playerName) == mMultiplayerCarHuman.end())
    {
        mMultiplayerCarHuman.insert(std::make_pair(playerName, PSMultiplayerCar()));
    }
}

void GameState::clearMultiplayerCarsHuman()
{
    mMultiplayerCarHuman.clear();
}

void GameState::removeMultiplayerCarHuman(const std::string& playerName)
{
    if(mMultiplayerCarHuman.find(playerName) != mMultiplayerCarHuman.end())
    {
        mMultiplayerCarHuman.erase(playerName);
    }
}