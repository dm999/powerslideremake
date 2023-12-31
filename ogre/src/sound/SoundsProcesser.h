
#ifndef SOUNDSPROCESSER
#define SOUNDSPROCESSER

#include <vector>

#include "../includes/OgreInclude.h"

#include "../includes/CommonIncludes.h"

#include "../tools/LinearController.h"

#include "../lua/DMLuaManager.h"

#include "../loaders/PFLoader.h"

#include "../physics/TerrainData.h"

class SoundSource;

class SoundsProcesser
{
public:

    SoundsProcesser();
    ~SoundsProcesser();

    void setListenerPos(const Ogre::Vector3& pos, const Ogre::Vector3& up, const Ogre::Vector3& direction);
    void setListenerGain(float gain);
    float getListenerGain()const;

    void setTerrainData(const std::vector<TerrainData>& terrainData);

    //raceing sounds
    void initSounds(const PFLoader& mPFLoaderData);
    void deinitSounds();
    void stopSounds();
    void stopSoundSurfaces();

    //menu sounds
    void initSoundsUI(const PFLoader& mPFLoaderShell);
    void deinitSoundsUI();
    void stopSoundsUI();

    void playCrash1();
    void playCrash2();

    void playBeforeStart1();
    void playBeforeStart2();
    void playBeforeStart3();

    void playSurface(size_t surfaceNumber, Ogre::Real lateralVel, Ogre::Real speedometerVelocity);
    void playSurfaceCrash(size_t surfaceNumber);

    void playCheatBurn();
    void playCheatBomb();
    void playExplosion(const Ogre::Vector3& pos);

    void playUIOver();
    void playUIDown();

    void setVolume(float gain); // 0.0 - 1.0

private:


    void resetInternalStatus();

    void playRoll(Ogre::Real lateralVel, Ogre::Real speedometerVelocity);

    //OpenALState mState;

    CommonIncludes::shared_ptr<SoundSource> mCrash1;
    CommonIncludes::shared_ptr<SoundSource> mCrash2;

    static const int mSurfacesCount = 7;
    CommonIncludes::shared_ptr<SoundSource> mSurface[mSurfacesCount];
    CommonIncludes::shared_ptr<SoundSource> mSurfaceCrash[mSurfacesCount];
    CommonIncludes::shared_ptr<SoundSource> mRoll;

    std::vector<TerrainData> mTerrainData;

    size_t mPrevSurface;

    LinearController<float> mSurfaceSoundGain;
    LinearController<float> mSurfaceSoundPitch;


    CommonIncludes::shared_ptr<SoundSource> mBeforeStart1;
    CommonIncludes::shared_ptr<SoundSource> mBeforeStart2;
    CommonIncludes::shared_ptr<SoundSource> mBeforeStart3;

    bool mBeforeStartPlayedOnce1;
    bool mBeforeStartPlayedOnce2;
    bool mBeforeStartPlayedOnce3;

    CommonIncludes::shared_ptr<SoundSource> mBurn;
    CommonIncludes::shared_ptr<SoundSource> mBomb;
    CommonIncludes::shared_ptr<SoundSource> mExplosion;

    CommonIncludes::shared_ptr<SoundSource> mUIOver;
    CommonIncludes::shared_ptr<SoundSource> mUIDown;

    float mMasterGain;

};
#endif
