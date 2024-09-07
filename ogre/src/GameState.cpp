

#include "GameState.h"

#include "PowerslideRemakeConfig.h"

#include "tools/Conversions.h"

#ifdef _MSC_VER
#pragma warning(disable: 4800)
#endif

GameState::GameState() :
    mVersion(GAMEVERSION),
    mBuildDate(GAMEBUILDDATE),
    mPlayerName("Rasputin"),
    mTrackName("desert track"),
    mIsSafeRun(false),
    mIsSandblasterTrack(true),
    mIsSpeedwayTrack(false),
    mIsMineshaftedTrack(false),
    mIsStuntTrack(false),
    mIsLugeTrack(false),
    mIsFoxnhound1Track(false),
    mIsFoxnhound2Track(false),
    mLapsCount(3),
    mTransmissionType(trAuto),
    mIsKMPh(true),
    mIsCastShadows(true),
    mResolution("800 x 600"),
    mIsVsync(false),
    mIsFullscreen(false),
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
    mGamma(1.0f),
    mDoUpscale(false),
    mIsMirrorEnabled(true),
    mIsGhostEnabled(true),
    mAdvancedLightingPlayer(true),
    mAttenuationPlayer(false),
    mAdvancedLightingAI(false),
    mSoundsGain(1.0f),
    mMusicGain(1.0f),
    mIsGamePaused(false),
    mInputType(itKeyboard),
    mIsGhostVisible(true),
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
    setDefaultKeyCodeMappers();
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

                //mSTRHiscores.remove("desert track", 0);
                //mSTRHiscores.save(mDataDir);

                mPlayerSettings.parse(mDataDir);
                mSoundsGain = static_cast<float>(mPlayerSettings.getIntValue("", "sound fx volume", 9)) / 9.0f;
                mMusicGain = static_cast<float>(mPlayerSettings.getIntValue("", "sound cd volume", 9)) / 9.0f;
                mPlayerName = mPlayerSettings.getValue("", "player name", mPlayerName.c_str());
                setAICount(mPlayerSettings.getIntValue("", "num opponents", mAIMin));
                if (!mIsSafeRun)
                {
                    mResolution = mPlayerSettings.getValue("", "resolution", mResolution);
                    mIsVsync = mPlayerSettings.getIntValue("", "vsync", static_cast<int>(mIsVsync));
                    mIsFullscreen = mPlayerSettings.getIntValue("", "fullscreen", static_cast<int>(mIsFullscreen));
                }
                mIsCastShadows = mPlayerSettings.getIntValue("", "shadows", static_cast<int>(mIsCastShadows));
                mIsMirrorEnabled = mPlayerSettings.getIntValue("", "mirror", static_cast<int>(mIsMirrorEnabled));
                mIsGhostEnabled = mPlayerSettings.getIntValue("", "ghost", static_cast<int>(mIsGhostEnabled));
                mAdvancedLightingPlayer = mPlayerSettings.getIntValue("", "adv lighting player", static_cast<int>(mAdvancedLightingPlayer));
                mAttenuationPlayer = mPlayerSettings.getIntValue("", "attenuation player", static_cast<int>(mAttenuationPlayer));
                mAdvancedLightingAI = mPlayerSettings.getIntValue("", "adv lighting ai", static_cast<int>(mAdvancedLightingAI));
                mGamma = mPlayerSettings.getFloatValue("", "gamma", mGamma);
                mDoUpscale = mPlayerSettings.getIntValue("", "bicubic upscale", static_cast<int>(mDoUpscale));
                mIsKMPh = mPlayerSettings.getIntValue("", "speedo", static_cast<int>(mIsKMPh));
                mTransmissionType = static_cast<TransmissionType>(mPlayerSettings.getIntValue("", "transmission", static_cast<int>(mTransmissionType)));
                mInputType = static_cast<InputType>(mPlayerSettings.getIntValue("", "input", static_cast<int>(mInputType)));
                mCameraPos = static_cast<CameraPositions>(mPlayerSettings.getIntValue("", "camera setting", static_cast<int>(mCameraPos)));
                {
                    std::vector<std::string> keyMapperStr = mPlayerSettings.getArrayValue("", "key mappers");
                    std::vector<std::string> keyMapperStr2 = mPlayerSettings.getArrayValue("", "key mappers 2");
                    std::vector<std::string> mouseMapperStr = mPlayerSettings.getArrayValue("", "mouse mappers");

                    for (size_t q = 0; q < keyMapperStr.size(); ++q)
                    {
                        size_t val;
                        Conversions::DMFromString(keyMapperStr[q], val);
                        if (q < InputKeyMapping::kmEmpty)
                        {
                            mInputKeysKeyboard[q] = OIS::KeyCode(val);
                        }
                    }

                    for (size_t q = 0; q < keyMapperStr2.size(); ++q)
                    {
                        size_t val;
                        Conversions::DMFromString(keyMapperStr2[q], val);
                        if (q < InputKeyMapping::kmEmpty)
                        {
                            mInputKeysKeyboard2[q] = OIS::KeyCode(val);
                        }
                    }


                    for (size_t q = 0; q < mouseMapperStr.size(); ++q)
                    {
                        size_t val;
                        Conversions::DMFromString(mouseMapperStr[q], val);
                        if (q < InputKeyMapping::kmEmpty)
                        {
                            mInputKeysMouse[q] = OIS::MouseButtonID(val);
                        }
                    }
                }
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
    mPlayerData.level = static_cast<AIStrength>(Ogre::Math::Clamp<Ogre::uint32>(levelData, Easy, UltraInsane));

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
    globalData.ghost = mIsGhostEnabled;
    globalData.adv_lightinig_player = mAdvancedLightingPlayer;
    globalData.attenuation_player = mAttenuationPlayer;
    globalData.adv_lightinig_ai = mAdvancedLightingAI;
    globalData.gamma = mGamma;
    globalData.bicubic_upscale = mDoUpscale;
    globalData.kmph = mIsKMPh;
    globalData.transmission = mTransmissionType;
    globalData.input = mInputType;
    globalData.cameraPos = mCameraPos;
    globalData.fxVolume = mSoundsGain;
    globalData.musicVolume = mMusicGain;
    {
        for (size_t q = 0; q < InputKeyMapping::kmEmpty; ++q)
        {
            globalData.keyMappers.push_back(Conversions::DMToString(mInputKeysKeyboard[q]));
            globalData.keyMappers2.push_back(Conversions::DMToString(mInputKeysKeyboard2[q]));
            globalData.mouseMappers.push_back(Conversions::DMToString(mInputKeysMouse[q]));
        }
    }

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

    mIsSandblasterTrack = false;
    mIsSpeedwayTrack = false;
    mIsMineshaftedTrack = false;
    mIsStuntTrack = false;
    mIsLugeTrack = false;
    mIsFoxnhound1Track = false;
    mIsFoxnhound2Track = false;

    if (mTrackName == "desert track")
        mIsSandblasterTrack = true;

    if(mTrackName == "speedway night track" || mTrackName == "speedway track")
        mIsSpeedwayTrack = true;

    if(mTrackName == "mineshaft")
        mIsMineshaftedTrack = true;

    if(mTrackName == "stunt track")
        mIsStuntTrack = true;

    if (mTrackName == "luge track")
        mIsLugeTrack = true;

    if(mTrackName == "Foxnhound1 track")
        mIsFoxnhound1Track = true;

    if(mTrackName == "Foxnhound2 track")
        mIsFoxnhound2Track = true;

    mBackgroundColor = mSTRPowerslide.getTrackSkyColor(mTrackName);
    mBackgroundColor.r = Ogre::Math::Pow(mBackgroundColor.r, 1.0f / mGamma);
    mBackgroundColor.g = Ogre::Math::Pow(mBackgroundColor.g, 1.0f / mGamma);
    mBackgroundColor.b = Ogre::Math::Pow(mBackgroundColor.b, 1.0f / mGamma);

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

void GameState::setDefaultKeyCodeMappers()
{
    //arrows
    mInputKeysKeyboard[InputKeyMapping::kmLeft] = OIS::KC_LEFT;
    mInputKeysKeyboard[InputKeyMapping::kmRight] = OIS::KC_RIGHT;
    mInputKeysKeyboard[InputKeyMapping::kmUp] = OIS::KC_UP;
    mInputKeysKeyboard[InputKeyMapping::kmDown] = OIS::KC_DOWN;

    //gears
    mInputKeysKeyboard[InputKeyMapping::kmGearUp] = OIS::KC_A;
    mInputKeysKeyboard[InputKeyMapping::kmGearDown] = OIS::KC_Z;

    //view
    mInputKeysKeyboard[InputKeyMapping::kmView] = OIS::KC_V;

    //ghost visibility in game
    mInputKeysKeyboard[InputKeyMapping::kmGhost] = OIS::KC_G;

    //handbreak
    mInputKeysKeyboard[InputKeyMapping::kmHandBreak] = OIS::KC_LCONTROL;

    //F
    mInputKeysKeyboard[InputKeyMapping::kmBurn] = OIS::KC_F1;
    mInputKeysKeyboard[InputKeyMapping::kmBomb] = OIS::KC_F2;
    mInputKeysKeyboard[InputKeyMapping::kmNitro] = OIS::KC_F3;
    mInputKeysKeyboard[InputKeyMapping::kmDropCam] = OIS::KC_F4;
    mInputKeysKeyboard[InputKeyMapping::kmSticky] = OIS::KC_F5;
    mInputKeysKeyboard[InputKeyMapping::kmSpider] = OIS::KC_F6;
    mInputKeysKeyboard[InputKeyMapping::kmICBM] = OIS::KC_F7;
    mInputKeysKeyboard[InputKeyMapping::kmApollo] = OIS::KC_F8;
    mInputKeysKeyboard[InputKeyMapping::kmLunar] = OIS::KC_F9;
    mInputKeysKeyboard[InputKeyMapping::kmJump] = OIS::KC_F10;
    mInputKeysKeyboard[InputKeyMapping::kmHover] = OIS::KC_F11;
    mInputKeysKeyboard[InputKeyMapping::kmGlider] = OIS::KC_F12;

    for (size_t q = 0; q < InputKeyMapping::kmEmpty; ++q)
    {
        mInputKeysKeyboard2[q] = mInputKeysKeyboard[q];
        mInputKeysMouse[q] = static_cast<OIS::MouseButtonID>(-1);
    }

    mInputKeysKeyboard2[InputKeyMapping::kmUp] = OIS::KC_UNASSIGNED;
    mInputKeysKeyboard2[InputKeyMapping::kmDown] = OIS::KC_UNASSIGNED;
    mInputKeysMouse[InputKeyMapping::kmUp] = OIS::MB_Left;
    mInputKeysMouse[InputKeyMapping::kmDown] = OIS::MB_Right;
}

bool GameState::checkKeyCode(OIS::KeyCode code, InputKeyMapping index) const
{
    bool ret = false;

    if (index < InputKeyMapping::kmEmpty)
    {
        const OIS::KeyCode * mapper = NULL;
        if (mInputType == itKeyboard)
        {
            mapper = mInputKeysKeyboard;
        }
        if (mInputType == itMouse)
        {
            mapper = mInputKeysKeyboard2;
        }

        if (mapper)
        {
            ret = mapper[index] == code;
        }
    }

    return ret;
}

bool GameState::checkKeyCode(OIS::MouseButtonID id, InputKeyMapping index) const
{
    bool ret = false;

    if (index < InputKeyMapping::kmEmpty)
    {
        ret = mInputKeysMouse[index] == id;
    }

    return ret;
}

OIS::KeyCode GameState::getKeyCode(InputKeyMapping index) const
{
    OIS::KeyCode ret = OIS::KC_UNASSIGNED;

    if (index < InputKeyMapping::kmEmpty)
    {
        const OIS::KeyCode * mapper = NULL;
        if (mInputType == itKeyboard)
        {
            mapper = mInputKeysKeyboard;
        }
        if (mInputType == itMouse)
        {
            mapper = mInputKeysKeyboard2;
        }

        if (mapper)
        {
            ret = mapper[index];
        }
    }

    return ret;
}

OIS::MouseButtonID GameState::getMouseID(InputKeyMapping index) const
{
    OIS::MouseButtonID ret = OIS::MB_Left;

    if (index < InputKeyMapping::kmEmpty)
    {
        ret = mInputKeysMouse[index];
    }

    return ret;
}

void GameState::setKeyCode(OIS::KeyCode code, InputKeyMapping index)
{
    if (index < InputKeyMapping::kmEmpty)
    {
        OIS::KeyCode * mapper = NULL;
        if (mInputType == itKeyboard)
        {
            mapper = mInputKeysKeyboard;
        }
        if (mInputType == itMouse)
        {
            mapper = mInputKeysKeyboard2;
        }

        if (mapper)
        {
            mapper[index] = code;
        }
    }
}

void GameState::setKeyCode(OIS::MouseButtonID id, InputKeyMapping index)
{
    if (index < InputKeyMapping::kmEmpty)
    {
        mInputKeysMouse[index] = id;
    }
}

Ogre::Real GameState::getGamma() const
{ 
    Ogre::Real gamma;
    Conversions::DMFromString(Conversions::DMToString(mGamma, 3, 1), gamma);
    return gamma;
}

void GameState::setGamma(Ogre::Real gamma)
{ 
    mGamma = gamma;
    mBackgroundColor = mSTRPowerslide.getTrackSkyColor(mTrackName);
    mBackgroundColor.r = Ogre::Math::Pow(mBackgroundColor.r, 1.0f / mGamma);
    mBackgroundColor.g = Ogre::Math::Pow(mBackgroundColor.g, 1.0f / mGamma);
    mBackgroundColor.b = Ogre::Math::Pow(mBackgroundColor.b, 1.0f / mGamma);
}

bool GameState::loadLUTs()
{
    bool success = true;

    //x2
    {
        Ogre::DataStreamPtr pStream = Ogre::ResourceGroupManager::getSingleton().openResource("x2/S0_LSB_HD_LUT_D_x2_4bit_int8.bin", "LUTs");
        if(pStream.get() && pStream->isReadable())
        {
            uint16_t amount;
            pStream->read(&amount, sizeof(uint16_t));
            mLutsX2.LSB_HD_D.resize(amount);
            pStream->read(mLutsX2.LSB_HD_D.data(), amount);
        }
        else success = false;

        pStream->close();
    }

    {
        Ogre::DataStreamPtr pStream = Ogre::ResourceGroupManager::getSingleton().openResource("x2/S0_LSB_HD_LUT_H_x2_4bit_int8.bin", "LUTs");
        if(pStream.get() && pStream->isReadable())
        {
            uint16_t amount;
            pStream->read(&amount, sizeof(uint16_t));
            mLutsX2.LSB_HD_H.resize(amount);
            pStream->read(mLutsX2.LSB_HD_H.data(), amount);
        }
        else success = false;

        pStream->close();
    }

    {
        Ogre::DataStreamPtr pStream = Ogre::ResourceGroupManager::getSingleton().openResource("x2/S0_MSB_HDB_LUT_B_x2_4bit_int8.bin", "LUTs");
        if(pStream.get() && pStream->isReadable())
        {
            uint16_t amount;
            pStream->read(&amount, sizeof(uint16_t));
            mLutsX2.MSB_HDB_B.resize(amount);
            pStream->read(mLutsX2.MSB_HDB_B.data(), amount);
        }
        else success = false;

        pStream->close();
    }

    {
        Ogre::DataStreamPtr pStream = Ogre::ResourceGroupManager::getSingleton().openResource("x2/S0_MSB_HDB_LUT_D_x2_4bit_int8.bin", "LUTs");
        if(pStream.get() && pStream->isReadable())
        {
            uint16_t amount;
            pStream->read(&amount, sizeof(uint16_t));
            mLutsX2.MSB_HDB_D.resize(amount);
            pStream->read(mLutsX2.MSB_HDB_D.data(), amount);
        }
        else success = false;

        pStream->close();
    }


    {
        Ogre::DataStreamPtr pStream = Ogre::ResourceGroupManager::getSingleton().openResource("x2/S0_MSB_HDB_LUT_H_x2_4bit_int8.bin", "LUTs");
        if(pStream.get() && pStream->isReadable())
        {
            uint16_t amount;
            pStream->read(&amount, sizeof(uint16_t));
            mLutsX2.MSB_HDB_H.resize(amount);
            pStream->read(mLutsX2.MSB_HDB_H.data(), amount);
        }
        else success = false;

        pStream->close();
    }


    //X4
    {
        Ogre::DataStreamPtr pStream = Ogre::ResourceGroupManager::getSingleton().openResource("x4/S1_LSB_HD_LUT_D_x2_4bit_int8.bin", "LUTs");
        if(pStream.get() && pStream->isReadable())
        {
            uint16_t amount;
            pStream->read(&amount, sizeof(uint16_t));
            mLutsX4.LSB_HD_D.resize(amount);
            pStream->read(mLutsX4.LSB_HD_D.data(), amount);
        }
        else success = false;

        pStream->close();
    }

    {
        Ogre::DataStreamPtr pStream = Ogre::ResourceGroupManager::getSingleton().openResource("x4/S1_LSB_HD_LUT_H_x2_4bit_int8.bin", "LUTs");
        if(pStream.get() && pStream->isReadable())
        {
            uint16_t amount;
            pStream->read(&amount, sizeof(uint16_t));
            mLutsX4.LSB_HD_H.resize(amount);
            pStream->read(mLutsX4.LSB_HD_H.data(), amount);
        }
        else success = false;

        pStream->close();
    }

    {
        Ogre::DataStreamPtr pStream = Ogre::ResourceGroupManager::getSingleton().openResource("x4/S1_MSB_HDB_LUT_B_x2_4bit_int8.bin", "LUTs");
        if(pStream.get() && pStream->isReadable())
        {
            uint16_t amount;
            pStream->read(&amount, sizeof(uint16_t));
            mLutsX4.MSB_HDB_B.resize(amount);
            pStream->read(mLutsX4.MSB_HDB_B.data(), amount);
        }
        else success = false;

        pStream->close();
    }

    {
        Ogre::DataStreamPtr pStream = Ogre::ResourceGroupManager::getSingleton().openResource("x4/S1_MSB_HDB_LUT_D_x2_4bit_int8.bin", "LUTs");
        if(pStream.get() && pStream->isReadable())
        {
            uint16_t amount;
            pStream->read(&amount, sizeof(uint16_t));
            mLutsX4.MSB_HDB_D.resize(amount);
            pStream->read(mLutsX4.MSB_HDB_D.data(), amount);
        }
        else success = false;

        pStream->close();
    }


    {
        Ogre::DataStreamPtr pStream = Ogre::ResourceGroupManager::getSingleton().openResource("x4/S1_MSB_HDB_LUT_H_x2_4bit_int8.bin", "LUTs");
        if(pStream.get() && pStream->isReadable())
        {
            uint16_t amount;
            pStream->read(&amount, sizeof(uint16_t));
            mLutsX4.MSB_HDB_H.resize(amount);
            pStream->read(mLutsX4.MSB_HDB_H.data(), amount);
        }
        else success = false;

        pStream->close();
    }

    return success;
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