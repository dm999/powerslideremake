
#ifndef GAMESTATE
#define GAMESTATE

#include "includes/OgreInclude.h"

#include "pscar/PSPlayerCar.h"
#include "pscar/PSAICar.h"
#include "pscar/PSMultiplayerCar.h"
#include "tools/Tools.h"
#include "tools/PausedTimer.h"

#include "loaders/PFLoader.h"

#include "OriginalSettings.h"

#include "physics/InitialVehicleSetup.h"

#include "gamelogic/Championship.h"

#include "Enums.h"

class GameState
{
public:

    GameState();
    ~GameState();

    std::string getVersion()const{return mVersion;}

    void setPlayerName(std::string option){mPlayerName = option;}
    std::string getPlayerName()const{return mPlayerName;}

    const STRPlayerSettings::PlayerData& getPlayerData()const{return mPlayerData;}
    STRPlayerSettings::PlayerData& getPlayerData(){return mPlayerData;}


    /**
     * init data to read from *.pf and *.str files
     */
    void initOriginalData();
    bool isOriginalDataInited()const {return mOriginalDataInited;}
    bool isPatchDataInited()const {return mPatchDataInited;}

    void loadPlayerData();
    void savePlayerData();

    void setRaceParameters(const std::string& trackName, AIStrength aiStrength);
    void setRaceParameters(const std::string& trackName, AIStrength aiStrength, size_t lapsCount);

    bool isSpeedwayTrack() const {return mIsSpeedwayTrack;}
    bool isMineshaftedTrack() const {return mIsMineshaftedTrack;}
    bool isStuntTrack() const {return mIsStuntTrack;}
    bool isFoxnhound1Track() const {return mIsFoxnhound1Track;}
    bool isFoxnhound2Track() const {return mIsFoxnhound2Track;}

    bool isSupercar() const;

    const PFLoader& getPFLoaderData() const;
    const PFLoader& getPFLoaderGameshell() const;
    const PFLoader& getPFLoaderStore() const;
    const PFLoader& getPFLoaderPatch() const;
    const STRPowerslide& getSTRPowerslide() const;
    const STRRacecrud& getSTRRacecrud() const;
    const STRRacetimes& getSTRRacetimes() const;
    const STRHiscores& getSTRHiscores() const;
    STRHiscores& getSTRHiscores();

    bool updateHiscores(const std::string& character, const std::string& playerName, Ogre::Real time);

    TransmissionType getTransmissionType() const {return mTransmissionType;}
    void setTransmissionType(TransmissionType type){mTransmissionType = type;}

    bool isKMPh() const {return mIsKMPh;}
    void setKMPh(bool isKMPh){mIsKMPh = isKMPh;}

    bool isCastShadows() const {return mIsCastShadows;}
    void setCastShadows(bool isCastShadows){mIsCastShadows = isCastShadows;}

    std::string getResolution() const {return mResolution;}
    bool isVsync() const {return mIsVsync;}
    bool isFullscreen() const {return mIsFullscreen;}

    CameraPositions getCameraPositionType() const {return mCameraPos;}
    void setCameraPositionType(CameraPositions type){mCameraPos = type;}

    std::string getTrackName() const {return mTrackName;}
    std::string getTrackNameAsOriginal() const;
    bool isAITrack() const;

    void setAICharacters(const std::vector<std::string>& aiCharacters){mAICharacters = aiCharacters;}
    std::vector<std::string> getAICharacters()const{return mAICharacters;}

    const PSAICar& getAICar(size_t index) const;
    PSAICar& getAICar(size_t index);
    const PSPlayerCar& getPlayerCar() const;
    PSPlayerCar& getPlayerCar();

    static const int mLapsMin = 1;
    static const int mLapsMax = 10;

    static const size_t mRaceGridCarsMax = 12;
    static const int mAIMin = 3;
    static const int mAIMax = 11;

    bool checkKeyCode(OIS::KeyCode code, InputKeyMapping index) const;
    OIS::KeyCode getKeyCode(InputKeyMapping index) const;
    void setDefaultKeyCodeMappers();

    void setAICount(size_t opponentsAmount);
    size_t getAICount()const{return mAiOpponentsAmount;}

    void setAICountInRace(size_t opponentsAmount);
    size_t getAICountInRace()const{return mAiOpponentsAmountInRace;}

    AIStrength getAIStrength()const{return mAIStrength;}

    Ogre::Vector3 getPLayerCarPrevVel()const{return mPlayerCarPrevVel;}
    void setPLayerCarPrevVel(const Ogre::Vector3& vel){mPlayerCarPrevVel = vel;}

    void setLLTObject(Ogre::ManualObject* lltObject){mLLTObject = lltObject;}
    Ogre::ManualObject* getLLTObject(){return mLLTObject;}

    std::vector<LightEclusion>& getExclusions(){return mLightExclusions;}
    const std::vector<LightEclusion>& getExclusions()const{return mLightExclusions;}

    std::vector<InitialVehicleSetup>& getInitialVehicleSetup(){return mInitialVehicleSetup;}

    std::vector<Particle>& getParticles(){return mParticles;}
    const std::vector<Particle>& getParticles()const{return mParticles;}

    void setGlobalLight(Ogre::Light* light);
    Ogre::Light* getGlobalLight(){return mGlobalLight;}

    void setShadowLight(Ogre::Light* light);
    Ogre::Light* getShadowLight(){return mShadowLight;}

    void setRaceStarted(bool isStarted){mIsRaceStarted = isStarted;}
    bool getRaceStarted()const{return mIsRaceStarted;}

    void setRaceFinished(bool isFinished){mIsRaceFinished= isFinished;}
    bool getRaceFinished()const{return mIsRaceFinished;}

    unsigned long getAfterFinishTimerTime(){return mTimerAfterFinish.getMilliseconds();}
    void resetAfterFinishTimer(){mTimerAfterFinish.reset();}

    unsigned long getBeforeStartTimerTime(){return mBeforeStartTimer.getMilliseconds();}
    void resetBeforeStartTimer(){mBeforeStartTimer.reset();}

    Ogre::SceneNode* getArrowNode(){return mArrowNode;}
    void setArrowNode(Ogre::SceneNode* node){mArrowNode = node;}

    Ogre::ColourValue getBackgroundColor()const{return mBackgroundColor;}

    void setMirrorEnabled(bool enabled){mIsMirrorEnabled = enabled;}
    bool getMirrorEnabled()const{return mIsMirrorEnabled;}

    void setGhostEnabled(bool enabled){mIsGhostEnabled = enabled;}
    bool getGhostEnabled()const{return mIsGhostEnabled;}

    void setAdvancedLightingPlayer(bool enabled){mAdvancedLightingPlayer = enabled;}
    bool getAdvancedLightingPlayer()const{return mAdvancedLightingPlayer;}

    void setAdvancedLightingAI(bool enabled){mAdvancedLightingAI = enabled;}
    bool getAdvancedLightingAI()const{return mAdvancedLightingAI;}

    size_t getLapsCount()const{return mLapsCount;}

    void setSoundsGain(Ogre::Real gain){mSoundsGain = gain;}
    Ogre::Real getSoundsGain()const{return mSoundsGain;}

    void setMusicGain(Ogre::Real gain){mMusicGain = gain;}
    Ogre::Real getMusicGain()const{return mMusicGain;}

    void setGamePaused();
    void resetGamePaused();
    bool isGamePaused()const{return mIsGamePaused;}

    InputType getInputType() const {return mInputType;}
    void setInputType(InputType type) {mInputType = type;}

    Championship& getChampionship() {return mChampionship;}

    void setSpeedCheatUsed(bool isUsed) {mIsSpeedCheatUsed = isUsed;}
    bool getSpeedCheatUsed() const{return mIsSpeedCheatUsed;}

    void setMultiplayerMaster(bool option){mIsMultiplayerMaster = option;}
    bool isMultiplayerMaster()const{return mIsMultiplayerMaster;}

    void setMultiplayerRoomName(std::string option){mMultiplayerRoomName = option;}
    std::string getMultiplayerRoomName()const{return mMultiplayerRoomName;}

    void setMultiplayerUserName(std::string option){mMultiplayerUserName = option;}
    std::string getMultiplayerUserName()const{return mMultiplayerUserName;}

    void setMultiplayerServerIP(std::string option){mMultiplayerServerIP = option;}
    std::string getMultiplayerServerIP()const{return mMultiplayerServerIP;}

    void setMultiplayerServerPort(size_t option){mMultiplayerServerPort = option;}
    size_t getMultiplayerServerPort()const{return mMultiplayerServerPort;}

    void setMultiplayerBroadcastInterval(size_t option){mMultiplayerBroadcastInterval = option;}
    size_t getMultiplayerBroadcastInterval()const{return mMultiplayerBroadcastInterval;}

    int getMaxMultiplayerAI()const{return mMultiplayerMaxAI;}
    PSMultiplayerCar& getMultiplayerCarAI(size_t index);
    void setMultiplayerCountAI(size_t multiplayerAmountAI);
    size_t getMultiplayerCountAI()const{return mMultiplayerAmountAI;}

    int getMaxMultiplayerHumans()const{return mMultiplayerMaxHumans;}
    std::vector<std::string> getMultiplayerCarHumanNames() const;
    PSMultiplayerCar& getMultiplayerCarHuman(size_t index);//only for data preparation in LLT
    PSMultiplayerCar& getMultiplayerCarHuman(const std::string& playerName);
    void addMultiplayerCarHuman(const std::string& playerName);
    void clearMultiplayerCarsHuman();
    void removeMultiplayerCarHuman(const std::string& playerName);

    const std::string& getDataDir()const {return mDataDir;}
#if defined(__ANDROID__)
    void setDataDir(const std::string& dir){ mDataDir = dir; }
#endif

private:

    std::string mVersion;

    std::string mPlayerName;

    STRPlayerSettings::PlayerData mPlayerData;

    std::string mTrackName;
    bool mIsSpeedwayTrack;
    bool mIsMineshaftedTrack;
    bool mIsStuntTrack;
    bool mIsFoxnhound1Track;
    bool mIsFoxnhound2Track;
    size_t mLapsCount;

    TransmissionType mTransmissionType;
    bool mIsKMPh;
    bool mIsCastShadows;
    std::string mResolution;
    bool mIsVsync;
    bool mIsFullscreen;
    CameraPositions mCameraPos;

    bool mOriginalDataInited;
    bool mPatchDataInited;
    PFLoader mPFLoaderData;
    PFLoader mPFLoaderGameshell;
    PFLoader mPFLoaderStore;
    PFLoader mPFLoaderPatch;
    STRPowerslide mSTRPowerslide;
    STRRacecrud mSTRRacecrud;
    STRRacetimes mSTRRacetimes;
    STRHiscores mSTRHiscores;
    STRPlayerSettings mPlayerSettings;

    PSPlayerCar mPSPlayerCar;
    size_t mAiOpponentsAmount;
    size_t mAiOpponentsAmountInRace;
    std::vector<std::string> mAICharacters;
    AIStrength mAIStrength;
    PSAICar mPSCar[mAIMax];
    Ogre::Vector3 mPlayerCarPrevVel;

    Ogre::ManualObject* mLLTObject;

    std::vector<LightEclusion> mLightExclusions;

    std::vector<InitialVehicleSetup> mInitialVehicleSetup;

    std::vector<Particle> mParticles;

    Ogre::Light* mGlobalLight;
    Ogre::Light* mShadowLight;

    bool mIsRaceStarted;
    PausedTimer mBeforeStartTimer;

    bool mIsRaceFinished;

    Ogre::Timer mTimerAfterFinish;

    Ogre::SceneNode* mArrowNode;

    Ogre::ColourValue mBackgroundColor;

    bool mIsMirrorEnabled;
    bool mIsGhostEnabled;
    bool mAdvancedLightingPlayer;
    bool mAdvancedLightingAI;

    Ogre::Real mSoundsGain; //0.0 - 1.0
    Ogre::Real mMusicGain; //0.0 - 1.0

    bool mIsGamePaused;

    InputType mInputType;

    OIS::KeyCode mInputKeysKeyboard[InputKeyMapping::kmEmpty];//0-3 arrows, 4-5 gears, 6 view, 7, hand brake, 8-11 F
    OIS::KeyCode mInputKeysMouse[InputKeyMapping::kmEmpty];

    bool mIsSpeedCheatUsed;

    Championship mChampionship;

    //multiplayer
    bool mIsMultiplayerMaster;
    std::string mMultiplayerRoomName;
    std::string mMultiplayerUserName;
    std::string mMultiplayerServerIP;
    size_t mMultiplayerServerPort;
    size_t mMultiplayerBroadcastInterval;

    static const int mMultiplayerMaxAI = 11;

    size_t mMultiplayerAmountAI;
    PSMultiplayerCar mMultiplayerCarAI[mMultiplayerMaxAI];

    static const int mMultiplayerMaxHumans = 11;
    PSMultiplayerCar mMultiplayerCarHuman[mMultiplayerMaxHumans];
    std::map<std::string, size_t> mMultiplayerCarHumanIndexes;

    std::string mDataDir;//for android
};

#endif