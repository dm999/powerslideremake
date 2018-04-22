

#include "GameState.h"

#include "PowerslideRemakeConfig.h"

#include "tools/Conversions.h"

GameState::GameState() :
    mVersion(GAMEVERSION),
    mPlayerName("Rasputin"),
    mGameLevel(Easy),
    mTrackName("desert track"),
    mLapsCount(3),
    mTransmissionType(trAuto),
    mIsKMPh(true),
    mIsCastShadows(true),
    mResolution("800 x 600"),
    mIsVsync(false),
    mIsFullscreen(true),
    mCameraPos(CameraPosition_ChassisB),
    mOriginalDataInited(false),
    mAiOpponentsAmount(3),
    mAIStrength(Easy),
    mLLTObject(NULL),
    mGlobalLight(NULL),
    mShadowLight(NULL),
    mIsRaceStarted(false),
    mIsRaceFinished(false),
    mBackgroundColor(0.91372550f, 0.78823531f, 0.52156866f),
    mIsMirrorEnabled(true),
    mListenerGain(1.0f),
    mIsGamePaused(false),
    mInputType(itKeyboard),
    mIsMultiplayerMaster(false),
    mMultiplayerRoomName("Powerslide"),
    mMultiplayerUserName(mPlayerName),
    mMultiplayerServerIP(""),
    mMultiplayerServerPort(8800),
    mMultiplayerAmountAI(0),
    mMultiplayerBroadcastInterval(350),
    mDataDir("")
{
}

GameState::~GameState()
{
}

void GameState::initOriginalData()
{
    Ogre::LogManager::getSingleton().logMessage(Ogre::LML_NORMAL, "[GameState::initOriginalData]: " + Ogre::String(mVersion.c_str()));

    bool isLoaded = mPFLoaderData.init("data.pf", mDataDir);

    if(isLoaded)
    {
        isLoaded = mPFLoaderGameshell.init("gameshell.pf", mDataDir);

        if(isLoaded)
        {
            isLoaded = mPFLoaderStore.init("store.pf", mDataDir);

            if(isLoaded)
            {
                mSTRPowerslide.parse(mPFLoaderStore);
                mSTRRacecrud.parse(mPFLoaderStore);
                mSTRRacetimes.parse(mPFLoaderStore);

                mPlayerSettings.parse(mDataDir);
                mPlayerName = mPlayerSettings.getValue("", "player name", mPlayerName.c_str());
                setAICount(mPlayerSettings.getIntValue("", "num opponents"));
                mResolution = mPlayerSettings.getValue("", "resolution", mResolution);
                mIsVsync = mPlayerSettings.getIntValue("", "vsync", static_cast<int>(mIsVsync));
                mIsFullscreen = mPlayerSettings.getIntValue("", "fullscreen", static_cast<int>(mIsFullscreen));
                mIsCastShadows = mPlayerSettings.getIntValue("", "shadows", static_cast<int>(mIsCastShadows));
                mIsMirrorEnabled = mPlayerSettings.getIntValue("", "mirror", static_cast<int>(mIsMirrorEnabled));
                mIsKMPh = mPlayerSettings.getIntValue("", "speedo", static_cast<int>(mIsKMPh));
                mTransmissionType = static_cast<TransmissionType>(mPlayerSettings.getIntValue("", "transmission", static_cast<int>(mTransmissionType)));
                mInputType = static_cast<InputType>(mPlayerSettings.getIntValue("", "input", static_cast<int>(mInputType)));
                mCameraPos = static_cast<CameraPositions>(mPlayerSettings.getIntValue("", "camera setting", static_cast<int>(mCameraPos)));
                loadPlayerData();
                setRaceParameters(mPlayerSettings.getValue("", "track choice", mTrackName.c_str()), mGameLevel);

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

void GameState::loadPlayerData()
{
    const std::string section = mPlayerName + " parameters";
    const std::string level = mPlayerSettings.getValue(section, "Level", Conversions::DMToString(static_cast<AIStrength>(mGameLevel)));
    Ogre::uint32 levelData;
    Conversions::DMFromString(level, levelData);
    mGameLevel = static_cast<AIStrength>(levelData);

    setRaceParameters("desert track", mGameLevel);
    mPSPlayerCar.setCharacterName("frantic");
}

void GameState::savePlayerData()
{
    STRPlayerSettings::GlobalData globalData;
    globalData.playerName = mPlayerName;
    globalData.numOpponents = mAiOpponentsAmount;

    Ogre::RenderSystem * rs = Ogre::Root::getSingletonPtr()->getRenderSystem();
    Ogre::ConfigOptionMap configOpts = rs->getConfigOptions();
    Ogre::ConfigOption videoMode = configOpts["Video Mode"];
    globalData.resolution = videoMode.currentValue;

    globalData.vsync = static_cast<Ogre::RenderWindow*>(Ogre::Root::getSingletonPtr()->getRenderTarget("Powerslide Remake"))->isVSyncEnabled();
    globalData.fullscreen = static_cast<Ogre::RenderWindow*>(Ogre::Root::getSingletonPtr()->getRenderTarget("Powerslide Remake"))->isFullScreen();
    globalData.shadows = mIsCastShadows;
    globalData.mirror = mIsMirrorEnabled;
    globalData.kmph = mIsKMPh;
    globalData.transmission = mTransmissionType;
    globalData.input = mInputType;
    globalData.cameraPos = mCameraPos;

    globalData.track = mTrackName;

    STRPlayerSettings::PlayerData playerData;
    playerData.level = mGameLevel;

    mPlayerSettings.save(mDataDir, globalData, playerData);

    if(mPlayerSettings.getIsSaved())
    {
        Ogre::LogManager::getSingleton().logMessage(Ogre::LML_NORMAL, "[GameState::savePlayerData]: save sucess");
    }
    else
    {
        Ogre::LogManager::getSingleton().logMessage(Ogre::LML_NORMAL, "[GameState::savePlayerData]: save error");
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

const STRRacetimes& GameState::getSTRRacetimes() const
{
    assert(mOriginalDataInited);
    return mSTRRacetimes;
}

std::string GameState::getTrackNameAsOriginal() const
{
    std::string res = mTrackName;

    if(res == "speedway night track")
        res = "speedway track";

    return res;
}

const PSAICar& GameState::getAICar(size_t index)const
{
    if(index >= mAIMax)
        assert(false && "GameState::getAICar");

    return mPSCar[index];
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