

#include "GameState.h"

#include "PowerslideRemakeConfig.h"

#include "tools/Conversions.h"

#ifdef _MSC_VER
#pragma warning(disable: 4800)
#endif

GameState::GameState() :
    mVersion(GAMEVERSION),
    mPlayerName("Rasputin"),
    mTrackName("desert track"),
    mIsSpeedwayTrack(false),
    mIsMineshaftedTrack(false),
    mIsStuntTrack(false),
    mIsFoxnhound1Track(false),
    mIsFoxnhound2Track(false),
    mLapsCount(3),
    mTransmissionType(trAuto),
    mIsKMPh(true),
    mIsCastShadows(true),
    mResolution("800 x 600"),
    mIsVsync(false),
    mIsFullscreen(true),
    mCameraPos(CameraPosition_ChassisB),
    mOriginalDataInited(false),
    mPatchDataInited(false),
    mAiOpponentsAmount(3),
    mAiOpponentsAmountInRace(3),
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
    mIsSpeedCheatUsed(false),
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

            mPatchDataInited = mPFLoaderPatch.init("patch.pf", mDataDir);

            if(isLoaded)
            {
                mSTRPowerslide.parse(mPFLoaderStore);
                mSTRRacecrud.parse(mPFLoaderStore);
                mSTRRacetimes.parse(mPFLoaderStore);
                mSTRHiscores.parse(mPFLoaderStore, mDataDir);

                mPlayerSettings.parse(mDataDir);
                mListenerGain = static_cast<float>(mPlayerSettings.getIntValue("", "sound fx volume", 9)) / 9.0f;
                mPlayerName = mPlayerSettings.getValue("", "player name", mPlayerName.c_str());
                setAICount(mPlayerSettings.getIntValue("", "num opponents", mAIMin));
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
                setRaceParameters(mPlayerSettings.getValue("", "track choice", mTrackName.c_str()), mPlayerData.level);
                if(!isAITrack())
                {
                    setAICountInRace(0);
                }
                else
                {
                    setAICountInRace(mAiOpponentsAmount);
                }
                mPSPlayerCar.setCharacterName(mPlayerSettings.getValue("", "character choice", "frantic"));

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
    const std::string level = mPlayerSettings.getValue(section, "Level", "0");
    Ogre::uint32 levelData;
    Conversions::DMFromString(level, levelData);
    if(mPlayerName == "megasaxon")levelData = 3;//http://www.mobygames.com/developer/sheet/view/developerId,47107/
    mPlayerData.level = static_cast<AIStrength>(Ogre::Math::Clamp<Ogre::uint32>(levelData, 0, 3));

    std::vector<std::string> fruits = mPlayerSettings.getArrayValue(section, "fruit");
    if(!fruits.empty())
    {
        for(size_t q = 0; q < fruits.size(); ++q)
        {
            bool fruitPred;
            Conversions::DMFromString(fruits[q], fruitPred);
            mPlayerData.fruit[q] = fruitPred;
        }
    }
    else
    {
        mPlayerData.fruit = std::vector<bool>(STRPlayerSettings::PlayerData::mFruitAmount, false);
    }

    setRaceParameters("desert track", mPlayerData.level);
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
    globalData.fxVolume = mListenerGain;

    globalData.track = mTrackName;
    globalData.character = mPSPlayerCar.getCharacterName();

    mPlayerSettings.save(mDataDir, globalData, mPlayerData);

    if(mPlayerSettings.getIsSaved())
    {
        Ogre::LogManager::getSingleton().logMessage(Ogre::LML_NORMAL, "[GameState::savePlayerData]: save sucess");
    }
    else
    {
        Ogre::LogManager::getSingleton().logMessage(Ogre::LML_CRITICAL, "[GameState::savePlayerData]: save error");
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
    if(mAIStrength == Easy && mTrackName == "speedway night track")
    {
        mTrackName = "speedway track";
    }

    mIsSpeedwayTrack = false;
    mIsMineshaftedTrack = false;
    mIsStuntTrack = false;
    mIsFoxnhound1Track = false;
    mIsFoxnhound2Track = false;

    if(mTrackName == "speedway night track" || mTrackName == "speedway track")
        mIsSpeedwayTrack = true;

    if(mTrackName == "mineshaft")
        mIsMineshaftedTrack = true;

    if(mTrackName == "stunt track")
        mIsStuntTrack = true;

    if(mTrackName == "Foxnhound1 track")
        mIsFoxnhound1Track = true;

    if(mTrackName == "Foxnhound2 track")
        mIsFoxnhound2Track = true;

    mBackgroundColor = mSTRPowerslide.getTrackSkyColor(mTrackName);
    mLapsCount = lapsCount;
}

bool GameState::isSupercar() const
{
    std::string carName = getSTRPowerslide().getCarFromCharacter(mPSPlayerCar.getCharacterName());
    bool ret = carName == "supercar";

    return ret;
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

const PFLoader& GameState::getPFLoaderPatch() const
{
    return mPFLoaderPatch;
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

const STRHiscores& GameState::getSTRHiscores() const
{
    assert(mOriginalDataInited);
    return mSTRHiscores;
}

STRHiscores& GameState::getSTRHiscores()
{
    assert(mOriginalDataInited);
    return mSTRHiscores;
}

bool GameState::updateHiscores(const std::string& character, const std::string& playerName, Ogre::Real time)
{
    bool isBestBeaten = false;

    if(time > 0.0f && !isSupercar())
    {
        isBestBeaten = mSTRHiscores.updateTrackTime(getTrackNameAsOriginal(), character, playerName, time);

        mSTRHiscores.save(mDataDir);
    }

    return isBestBeaten;
}

std::string GameState::getTrackNameAsOriginal() const
{
    std::string res = mTrackName;

    if(res == "speedway night track")
        res = "speedway track";

    return res;
}

bool GameState::isAITrack() const
{
    return 
        mTrackName != "stunt track"         &&
        mTrackName != "luge track"          &&
        mTrackName != "Foxnhound1 track"    &&
        mTrackName != "Foxnhound2 track";
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
    mAiOpponentsAmount = Ogre::Math::Clamp<size_t>(opponentsAmount, mAIMin, mAIMax);
}

void GameState::setAICountInRace(size_t opponentsAmount)
{
    mAiOpponentsAmountInRace = Ogre::Math::Clamp<size_t>(opponentsAmount, 0, mAIMax);
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
        for(size_t q = 0; q < mAiOpponentsAmountInRace; ++q)
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
    for(size_t q = 0; q < mAiOpponentsAmountInRace; ++q)
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