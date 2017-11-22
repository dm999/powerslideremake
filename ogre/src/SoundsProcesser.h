
#ifndef SOUNDSPROCESSER
#define SOUNDSPROCESSER

#ifndef NO_OPENAL
#include <vector>

#include "OpenAL/OpenALState.h"

#include "includes/OgreInclude.h"

#include "includes/CommonIncludes.h"

#include "tools/LinearController.h"

#include "lua/DMLuaManager.h"

#include "loaders/PFLoader.h"

#include "physics/TerrainData.h"

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

    void setTerrainData(const std::vector<TerrainData>& terrainData);

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

    std::vector<TerrainData> mTerrainData;

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
#endif