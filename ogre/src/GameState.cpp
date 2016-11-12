
#include "pcheader.h"

#include "GameState.h"

GameState::GameState() :
    mOriginalDataInited(false),
    mLLTObject(NULL),
    mGlobalLight(NULL),
    mShadowLight(NULL),
    mIsRaceStarted(false),
    mIsMirrorEnabled(false),
    mLapsCount(1),
    mListenerGain(1.0f),
    mIsGamePaused(false),
    mIsMultiplayerEnabled(false),
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

void GameState::initOriginalData(   const std::string& trackName, 
                                    const std::string& de2FileName, 
                                    const std::string& originalPathData, 
                                    const std::string& originalPathCommon)
{
    mTrackName = trackName;
    mDE2FileName = de2FileName;

    mPFLoaderData.init(originalPathData, "data.pf");
    mPFLoaderGameshell.init(originalPathData, "gameshell.pf");
    mPFLoaderStore.init(originalPathCommon, "store.pf");
    mSTRPowerslide.parse(mPFLoaderStore);
    mSTRRacecrud.parse(mPFLoaderStore);

    mBackgroundColor = mSTRPowerslide.getTrackSkyColor(mDE2FileName);
    mLapsCount = mSTRPowerslide.getLapsCount(mDE2FileName);

    mOriginalDataInited = true;
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

std::string GameState::getSkinByCarEnum(GameCars carEnum)
{
    std::string res = "base";

    switch(carEnum)
    {
        case Warthog_0 :
        res = "frantic";
        break;
        case Warthog_1 :
        res = "cyber";
        break;
        case Warthog_2 :
        res = "radiation";
        break;
        case Warthog_3 :
        res = "zig";
        break;
        case Warthog_4 :
        res = "zag";
        break;
        case Warthog_5 :
        res = "beryl";
        break;
        case Warthog_6 :
        res = "stig";
        break;


        //TODO: add others
    }

    return res;
}

std::string GameState::getDashSkinByCarEnum(GameCars carEnum)
{
    std::string res = "car0_0s.bmp";

    switch(carEnum)
    {
        case Warthog_0 :
        res = "car0_0s.bmp";
        break;
        case Warthog_1 :
        res = "car0_1s.bmp";
        break;
        case Warthog_2 :
        res = "car0_2s.bmp";
        break;
        case Warthog_3 :
        res = "car0_3s.bmp";
        break;
        case Warthog_4 :
        res = "car0_4s.bmp";
        break;
        case Warthog_5 :
        res = "car0_5s.bmp";
        break;
        case Warthog_6 :
        res = "car0_6s.bmp";
        break;
        case Heaver_0 :
        res = "car1_0s.bmp";
        break;
        case Heaver_1 :
        res = "car1_1s.bmp";
        break;
        case Heaver_2 :
        res = "car1_2s.bmp";
        break;
        case Heaver_3 :
        res = "car1_3s.bmp";
        break;
        case Heaver_4 :
        res = "car1_4s.bmp";
        break;
        case Heaver_5 :
        res = "car1_5s.bmp";
        break;
        case Heaver_6 :
        res = "car1_6s.bmp";
        break;
        case Orc_0 :
        res = "car2_0s.bmp";
        break;
        case Orc_1 :
        res = "car2_1s.bmp";
        break;
        case Orc_2 :
        res = "car2_2s.bmp";
        break;
        case Orc_3 :
        res = "car2_3s.bmp";
        break;
        case Orc_4 :
        res = "car2_4s.bmp";
        break;
        case Orc_5 :
        res = "car2_5s.bmp";
        break;
        case Saber_0 :
        res = "car3_0s.bmp";
        break;
        case Saber_1 :
        res = "car3_1s.bmp";
        break;
        case Saber_2 :
        res = "car3_2s.bmp";
        break;
        case Saber_3 :
        res = "car3_3s.bmp";
        break;
        case Saber_4 :
        res = "car3_4s.bmp";
        break;
        case Saber_5 :
        res = "car3_5s.bmp";
        break;
        case Skee_0 :
        res = "car4_0s.bmp";
        break;
        case Skee_1 :
        res = "car4_1s.bmp";
        break;
        case Skee_2 :
        res = "car4_2s.bmp";
        break;
        case Pickup_0 :
        res = "car5_0s.bmp";
        break;
        case Pickup_1 :
        res = "car5_1s.bmp";
        break;
        case Pickup_2 :
        res = "car5_2s.bmp";
        break;
    }

    return res;
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