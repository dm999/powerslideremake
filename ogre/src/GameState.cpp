
#include "pcheader.h"

#include "GameState.h"

#define GAMEVERSION "1.20.0"

GameState::GameState() :
    mVersion(GAMEVERSION),
    mOriginalDataInited(false),
    mAiOpponentsAmount(1),
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
    mMultiplayerAmountAI(0),
    mMultiplayerBroadcastInterval(350)
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
            else
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
    setRaceParameters(trackName, aiStrength, mSTRPowerslide.getLapsCount(trackName));
}

void GameState::setRaceParameters(const std::string& trackName, AIStrength aiStrength, size_t lapsCount)
{
    mTrackName = trackName;

    mAIStrength = aiStrength;

    if(mAIStrength > Easy && mTrackName == "speedway track")
    {
        mTrackName = "speedway night track";
    }

    mBackgroundColor = mSTRPowerslide.getTrackSkyColor(mTrackName);
    mLapsCount = lapsCount;
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

std::string GameState::getTrackNameAsOriginal() const
{
    std::string res = mTrackName;

    if(res == "speedway night track")
        res = "speedway track";

    return res;
}

PSAICar& GameState::getAICar(size_t index)
{
    if(index >= mAIMax)
        assert(false && "GameState::getAICar");

    return mPSCar[index];
}

const PSPlayerCar& GameState::getPlayerCar()const
{
    return mPSPlayerCar;
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

    for(std::map<std::string, size_t>::const_iterator i = mMultiplayerCarHumanIndexes.begin(), j = mMultiplayerCarHumanIndexes.end();
        i != j; ++i)
    {
        ret.push_back((*i).first);
    }

    return ret;
}

PSMultiplayerCar& GameState::getMultiplayerCarHuman(size_t index)
{
    if(index >= mMultiplayerMaxHumans)
        assert(false && "GameState::getMultiplayerCarHuman");

    return mMultiplayerCarHuman[index];
}

PSMultiplayerCar& GameState::getMultiplayerCarHuman(const std::string& playerName)
{
    std::map<std::string, size_t>::iterator i = mMultiplayerCarHumanIndexes.find(playerName);

    assert(i != mMultiplayerCarHumanIndexes.end() && "GameState::getMultiplayerCarHuman");

    return mMultiplayerCarHuman[(*i).second];
}

void GameState::addMultiplayerCarHuman(const std::string& playerName)
{
    if(mMultiplayerCarHumanIndexes.find(playerName) == mMultiplayerCarHumanIndexes.end())
    {
        size_t amount = mMultiplayerCarHumanIndexes.size();
        mMultiplayerCarHumanIndexes.insert(std::make_pair(playerName, amount));
    }
}

void GameState::clearMultiplayerCarsHuman()
{
    mMultiplayerCarHumanIndexes.clear();
}

void GameState::removeMultiplayerCarHuman(const std::string& playerName)
{
    if(mMultiplayerCarHumanIndexes.find(playerName) != mMultiplayerCarHumanIndexes.end())
    {
        mMultiplayerCarHumanIndexes.erase(playerName);
    }
}