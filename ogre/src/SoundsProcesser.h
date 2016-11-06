
#ifndef SOUNDSPROCESSER
#define SOUNDSPROCESSER

#include <vector>

#include "OpenAL/OpenALState.h"

#include "OgreInclude.h"

#include "CommonIncludes.h"

#include "LinearController.h"

#include "lua/DMLuaManager.h"

#include "loaders/PFLoader.h"

class OpenALSource;

class SoundsProcesser
{
public:

    SoundsProcesser();
    ~SoundsProcesser();

    void initSoundSystem();
    void deInitSoundSystem();

    void setListenerPos(const Ogre::Vector3& pos);
    void setListenerGain(ALfloat gain);

    void setRemappers(const std::vector<size_t>& remapSounds, const std::vector<size_t>& remapSoundsCollision);

    void initSounds(const PFLoader& mPFLoaderData);
    void deinitSounds();
    void stopSounds();
    void stopSoundSurfaces();

    void playCrash1();
    void playCrash2();

    void playBeforeStart1();
    void playBeforeStart2();
    void playBeforeStart3();

    void playSurface(size_t surfaceNumber, Ogre::Real lateralVel);
    void playSurfaceCrash(size_t surfaceNumber);

private:


    void updatePosition(const Ogre::Vector3& pos);

    void resetInternalStatus();

    OpenALState mState;

    CommonIncludes::shared_ptr<OpenALSource> mCrash1;
    CommonIncludes::shared_ptr<OpenALSource> mCrash2;

    static const int mSurfacesCount = 7;
    CommonIncludes::shared_ptr<OpenALSource> mSurface[mSurfacesCount];
    CommonIncludes::shared_ptr<OpenALSource> mSurfaceCrash[mSurfacesCount];

    std::vector<size_t> mRemapSounds;
    std::vector<size_t> mRemapSoundsCollision;

    size_t mPrevSurface;

    LinearController<float> mSurfaceSoundGain;


    CommonIncludes::shared_ptr<OpenALSource> mBeforeStart1;
    CommonIncludes::shared_ptr<OpenALSource> mBeforeStart2;
    CommonIncludes::shared_ptr<OpenALSource> mBeforeStart3;

    bool mBeforeStartPlayedOnce1;
    bool mBeforeStartPlayedOnce2;
    bool mBeforeStartPlayedOnce3;

};

#endif