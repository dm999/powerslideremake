
#ifndef NO_OPENAL

#include <SFML/Audio.hpp>

#if defined(__ANDROID__)
    #include <android/log.h>

    #define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO, "OGRE", __VA_ARGS__))
    #define LOGE(...) ((void)__android_log_print(ANDROID_LOG_ERROR, "OGRE", __VA_ARGS__)) 
#endif

#include "SoundsProcesser.h"

#include "SoundSource.h"

SoundsProcesser::SoundsProcesser() :
    mPrevSurface(0),
    mBeforeStartPlayedOnce1(false),
    mBeforeStartPlayedOnce2(false),
    mBeforeStartPlayedOnce3(false)
{
    mSurfaceSoundGain.addPoint(0.0f, 0.0f);
    mSurfaceSoundGain.addPoint(100.0f, 1.0f);
}

SoundsProcesser::~SoundsProcesser()
{
}

void SoundsProcesser::setListenerPos(const Ogre::Vector3& pos, const Ogre::Vector3& up, const Ogre::Vector3& direction)
{
    sf::Listener::setPosition(pos.x, pos.y, pos.z);
    sf::Listener::setDirection(direction.x, direction.y, direction.z);
    sf::Listener::setUpVector(up.x, up.y, up.z);
}

void SoundsProcesser::setListenerGain(float gain)
{
    sf::Listener::setGlobalVolume(gain * 100.0f);
}

void SoundsProcesser::initSounds(const PFLoader& mPFLoaderData)
{
    stopSounds();

    mCrash1.reset(new SoundSource("data/sfx/crashes", "8bcrash1.its", mPFLoaderData));
    mCrash2.reset(new SoundSource("data/sfx/crashes", "8bcrash2.its", mPFLoaderData));
    mCrash1->setPitch(0.5f);
    mCrash2->setPitch(0.5f);
    mCrash1->setRelativeToListener(true);
    mCrash2->setRelativeToListener(true);

    mBeforeStart1.reset(new SoundSource("data/sfx/beeps", "8bst1.its", mPFLoaderData));
    mBeforeStart2.reset(new SoundSource("data/sfx/beeps", "8bst2.its", mPFLoaderData));
    mBeforeStart3.reset(new SoundSource("data/sfx/beeps", "8bst3.its", mPFLoaderData));
    mBeforeStart1->setRelativeToListener(true);
    mBeforeStart2->setRelativeToListener(true);
    mBeforeStart3->setRelativeToListener(true);
    mBeforeStart1->setPitch(1.1f);
    mBeforeStart2->setPitch(1.1f);
    mBeforeStart3->setPitch(1.1f);

    for(int q = 0; q < mSurfacesCount; ++q)
    {
        std::string fileName = "8bsurf" + Conversions::DMToString(q) + ".its";
        mSurface[q].reset(new SoundSource("data/sfx/surfaces", fileName, mPFLoaderData));
        mSurface[q]->setLooping(true);
        mSurface[q]->setRelativeToListener(true);

        if(q < (mSurfacesCount - 1))
        {
            std::string fileNameCrash = "8bsurfcrash" + Conversions::DMToString(q) + ".its";
            mSurfaceCrash[q].reset(new SoundSource("data/sfx/surfaces", fileNameCrash, mPFLoaderData));
            mSurfaceCrash[q]->setPitch(0.5f);
            mSurfaceCrash[q]->setRelativeToListener(true);

        }
    }
}

void SoundsProcesser::deinitSounds()
{
    stopSounds();

    mCrash1.reset();
    mCrash2.reset();

    mBeforeStart1.reset();
    mBeforeStart2.reset();
    mBeforeStart3.reset();

    for(int q = 0; q < mSurfacesCount; ++q)
    {
        mSurface[q].reset();
        mSurfaceCrash[q].reset();
    }
}

void SoundsProcesser::stopSounds()
{
    if(mCrash1.get() && mCrash1->getSourceState() == SoundSource::Playing)
    {
        mCrash1->stopPlaying();
    }

    if(mCrash2.get() && mCrash2->getSourceState() == SoundSource::Playing)
    {
        mCrash2->stopPlaying();
    }

    if(mBeforeStart1.get() && mBeforeStart1->getSourceState() == SoundSource::Playing)
    {
        mBeforeStart1->stopPlaying();
    }

    if(mBeforeStart2.get() && mBeforeStart2->getSourceState() == SoundSource::Playing)
    {
        mBeforeStart2->stopPlaying();
    }

    if(mBeforeStart3.get() && mBeforeStart3->getSourceState() == SoundSource::Playing)
    {
        mBeforeStart3->stopPlaying();
    }

    stopSoundSurfaces();

}

void SoundsProcesser::stopSoundSurfaces()
{
    for(int q = 0; q < mSurfacesCount; ++q)
    {
        if(mSurface[q].get() && mSurface[q]->getSourceState() == SoundSource::Playing)
        {
            mSurface[q]->stopPlaying();
        }

        if(mSurfaceCrash[q].get() && mSurfaceCrash[q]->getSourceState() == SoundSource::Playing)
        {
            mSurfaceCrash[q]->stopPlaying();
        }
    }
}

void SoundsProcesser::setTerrainData(const std::vector<TerrainData>& terrainData)
{
    assert(terrainData.size() == TerrainData::mTerrainsAmount);

    mTerrainData = terrainData;

    resetInternalStatus();
}

void SoundsProcesser::resetInternalStatus()
{
    mBeforeStartPlayedOnce1 = false;
    mBeforeStartPlayedOnce2 = false;
    mBeforeStartPlayedOnce3 = false;
}

void SoundsProcesser::playCrash1()
{
    if(mCrash1.get() && mCrash1->getSourceState() != SoundSource::Playing && mCrash2.get() && mCrash2->getSourceState() != SoundSource::Playing)
    {
        mCrash1->startPlaying();
    }
}

void SoundsProcesser::playCrash2()
{
    if(mCrash2.get() && mCrash2->getSourceState() != SoundSource::Playing)
    {
        mCrash2->startPlaying();
    }
}

void SoundsProcesser::playBeforeStart1()
{
    if(!mBeforeStartPlayedOnce1 && mBeforeStart1.get() && mBeforeStart1->getSourceState() != SoundSource::Playing)
    {
        mBeforeStart1->startPlaying();
    }

    if(mBeforeStart1.get() && mBeforeStart1->getSourceState() == SoundSource::Playing)
    {
        mBeforeStartPlayedOnce1 = true;
    }
}

void SoundsProcesser::playBeforeStart2()
{
    if(!mBeforeStartPlayedOnce2 && mBeforeStart2.get() && mBeforeStart2->getSourceState() != SoundSource::Playing)
    {
        mBeforeStart2->startPlaying();
    }

    if(mBeforeStart2.get() && mBeforeStart2->getSourceState() == SoundSource::Playing)
    {
        mBeforeStartPlayedOnce2 = true;
    }
}

void SoundsProcesser::playBeforeStart3()
{
    if(!mBeforeStartPlayedOnce3 && mBeforeStart3.get() && mBeforeStart3->getSourceState() != SoundSource::Playing)
    {
        mBeforeStart3->startPlaying();
    }

    if(mBeforeStart3.get() && mBeforeStart3->getSourceState() == SoundSource::Playing)
    {
        mBeforeStartPlayedOnce3 = true;
    }
}

void SoundsProcesser::playSurface(size_t surfaceNumber, Ogre::Real lateralVel)
{
    size_t remappedSurfaceNumber = mTerrainData[surfaceNumber].mSoundIndex;

    if(remappedSurfaceNumber < mSurfacesCount)
    {
        if(mPrevSurface != remappedSurfaceNumber && mSurface[mPrevSurface].get() && mSurface[mPrevSurface]->getSourceState() == SoundSource::Playing)
        {
            mSurface[mPrevSurface]->stopPlaying();
        }

        mPrevSurface = remappedSurfaceNumber;

        if(mSurface[remappedSurfaceNumber].get() && mSurface[remappedSurfaceNumber]->getSourceState() != SoundSource::Playing)
        {
            mSurface[remappedSurfaceNumber]->startPlaying();
        }

        mSurface[remappedSurfaceNumber]->setGain(mSurfaceSoundGain.getVal(lateralVel));
    }
}

void SoundsProcesser::playSurfaceCrash(size_t surfaceNumber)
{
    size_t remappedSurfaceNumber = mTerrainData[surfaceNumber].mSoundIndex2;

    if(remappedSurfaceNumber < (mSurfacesCount - 1))
    {
        bool isPlaying = false;

        for(int q = 0; q < mSurfacesCount; ++q)
        {
            if(mSurfaceCrash[q].get() && mSurfaceCrash[q]->getSourceState() == SoundSource::Playing)
            {
                isPlaying = true;
                break;
            }
        }

        if(!isPlaying)
        {
            if(mSurfaceCrash[remappedSurfaceNumber].get())
            {
                mSurfaceCrash[remappedSurfaceNumber]->startPlaying();
            }
        }
    }
}

#endif