
#ifndef GAMESTATE
#define GAMESTATE

#include "OgreInclude.h"

#include "PSPlayerCar.h"
#include "PSAICar.h"
#include "PSMultiplayerCar.h"
#include "Tools.h"
#include "PausedTimer.h"

enum AIStrength
{
    Easy    = 0,
    Medium  = 1,
    Hard    = 2,
    Insane  = 3
};


class GameState
{
public:

    GameState();
    ~GameState();

    /**
     * init data to read from *.pf and *.str files
     * originalPathData - should contain data.pf, gameshell.pf
     * originalPathCommon - should contain store.pf
     */
    void initOriginalData(  const std::string& trackName, 
                            const std::string& de2FileName, 
                            const std::string& originalPathData, 
                            const std::string& originalPathCommon);

    const PFLoader& getPFLoaderData() const;
    const PFLoader& getPFLoaderGameshell() const;
    const PFLoader& getPFLoaderStore() const;
    const STRPowerslide& getSTRPowerslide() const;
    const STRRacecrud& getSTRRacecrud() const;

    std::string getTrackName() const {return mTrackName;}
    std::string getDE2FileName() const {return mDE2FileName;}

    void setPlayerCharacterName(const std::string& name){mPlayerCharacterName = name;}
    std::string getPlayerCharacterName() const {return mPlayerCharacterName;}

    const PSAICar& getAICar(size_t index) const;
    PSAICar& getAICar(size_t index);
    const PSPlayerCar& getPlayerCar() const;
    PSPlayerCar& getPlayerCar();

    int getMaxAI()const{return mAIMax;}

    void setAICount(size_t opponentsAmount);
    size_t getAICount()const{return mAiOpponentsAmount;}

    void setAIStrength(AIStrength aiStrength) {mAIStrength = aiStrength;}
    AIStrength getAIStrength()const{return mAIStrength;}

    Ogre::Vector3 getPLayerCarPrevVel()const{return mPlayerCarPrevVel;}
    void setPLayerCarPrevVel(const Ogre::Vector3& vel){mPlayerCarPrevVel = vel;}

    void setLLTObject(Ogre::ManualObject* lltObject){mLLTObject = lltObject;}
    Ogre::ManualObject* getLLTObject(){return mLLTObject;}

    std::vector<LightEclusion>& getExclusions(){return mLightExclusions;}
    const std::vector<LightEclusion>& getExclusions()const{return mLightExclusions;}

    std::vector<Ogre::Matrix4>& getTrackPositions(){return mTrackPositions;}

    std::vector<Particle>& getParticles(){return mParticles;}
    const std::vector<Particle>& getParticles()const{return mParticles;}

    void setGlobalLight(Ogre::Light* light);
    Ogre::Light* getGlobalLight(){return mGlobalLight;}

    void setShadowLight(Ogre::Light* light);
    Ogre::Light* getShadowLight(){return mShadowLight;}

    void setRaceStarted(bool isStarted){mIsRaceStarted = isStarted;}
    bool getRaceStarted()const{return mIsRaceStarted;}

    unsigned long getBeforeStartTimerTime(){return mBeforeStartTimer.getMilliseconds();}
    void resetBeforeStartTimer(){mBeforeStartTimer.reset();}

    Ogre::SceneNode* getArrowNode(){return mArrowNode;}
    void setArrowNode(Ogre::SceneNode* node){mArrowNode = node;}

    Ogre::ColourValue getBackgroundColor()const{return mBackgroundColor;}

    void setMirrorEnabled(bool enabled){mIsMirrorEnabled = enabled;}
    bool getMirrorEnabled()const{return mIsMirrorEnabled;}

    size_t getLapsCount()const{return mLapsCount;}

    void setListenerGain(Ogre::Real gain){mListenerGain = gain;}
    Ogre::Real getListenerGain()const{return mListenerGain;}

    void setGamePaused();
    void resetGamePaused();
    bool isGamePaused()const{return mIsGamePaused;}

    //multiplayer
    void setMultiplayerEnabled(bool option){mIsMultiplayerEnabled = option;}
    bool isMultiplayerEnabled()const{return mIsMultiplayerEnabled;}

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

    void setMultiplayerPlayersLimits(size_t option){mMultiplayerPlayersLimits = option;}
    size_t getMultiplayerPlayersLimits()const{return mMultiplayerPlayersLimits;}

    void setMultiplayerBroadcastInterval(size_t option){mMultiplayerBroadcastInterval = option;}
    size_t getMultiplayerBroadcastInterval()const{return mMultiplayerBroadcastInterval;}

    //int getMaxMultiplayer()const{return mMultiplayerMax;}

    PSMultiplayerCar& getMultiplayerCarAI(size_t index);
    void setMultiplayerCountAI(size_t multiplayerAmountAI);
    size_t getMultiplayerCountAI()const{return mMultiplayerAmountAI;}

    std::vector<std::string> getMultiplayerCarHumanNames() const;
    PSMultiplayerCar& getMultiplayerCarHuman(const std::string& playerName);
    void addMultiplayerCarHuman(const std::string& playerName);
    void clearMultiplayerCarsHuman();
    void removeMultiplayerCarHuman(const std::string& playerName);

private:

    std::string mTrackName;
    std::string mDE2FileName;
    std::string mPlayerCharacterName;

    bool mOriginalDataInited;
    PFLoader mPFLoaderData;
    PFLoader mPFLoaderGameshell;
    PFLoader mPFLoaderStore;
    STRPowerslide mSTRPowerslide;
    STRRacecrud mSTRRacecrud;

    static const int mAIMax = 11;

    PSPlayerCar mPSPlayerCar;
    size_t mAiOpponentsAmount;
    AIStrength mAIStrength;
    PSAICar mPSCar[mAIMax];
    Ogre::Vector3 mPlayerCarPrevVel;

    Ogre::ManualObject* mLLTObject;

    std::vector<LightEclusion> mLightExclusions;

    std::vector<Ogre::Matrix4> mTrackPositions;

    std::vector<Particle> mParticles;

    Ogre::Light* mGlobalLight;
    Ogre::Light* mShadowLight;

    bool mIsRaceStarted;
    PausedTimer mBeforeStartTimer;

    Ogre::SceneNode* mArrowNode;

    Ogre::ColourValue mBackgroundColor;

    bool mIsMirrorEnabled;

    size_t mLapsCount;

    Ogre::Real mListenerGain; //master volume

    bool mIsGamePaused;

    //multiplayer
    bool mIsMultiplayerEnabled;
    bool mIsMultiplayerMaster;
    std::string mMultiplayerRoomName;
    std::string mMultiplayerUserName;
    std::string mMultiplayerServerIP;
    size_t mMultiplayerServerPort;
    size_t mMultiplayerPlayersLimits;
    size_t mMultiplayerBroadcastInterval;

    static const int mMultiplayerMaxAI = 11;

    size_t mMultiplayerAmountAI;
    PSMultiplayerCar mMultiplayerCarAI[mMultiplayerMaxAI];

    std::map<std::string, PSMultiplayerCar> mMultiplayerCarHuman;
};

#endif