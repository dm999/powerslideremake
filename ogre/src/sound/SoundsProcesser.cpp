
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

    mSurfaceSoundPitch.addPoint(0.0f, 0.0f);
    mSurfaceSoundPitch.addPoint(200.0f, 0.5f);
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

float SoundsProcesser::getListenerGain()const
{
    return sf::Listener::getGlobalVolume() / 100.0f;
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
    mBeforeStart1->setPitch(2.0f);
    mBeforeStart2->setPitch(2.0f);
    mBeforeStart3->setPitch(2.0f);

    for(int q = 0; q < mSurfacesCount; ++q)
    {
        std::string fileName = "8bsurf" + Conversions::DMToString(q) + ".its";
        mSurface[q].reset(new SoundSource("data/sfx/surfaces", fileName, mPFLoaderData));
        mSurface[q]->setLooping(true);
        //mSurface[q]->setPitch(0.5f);
        mSurface[q]->setRelativeToListener(true);

        if(q < (mSurfacesCount - 1))
        {
            std::string fileNameCrash = "8bsurfcrash" + Conversions::DMToString(q) + ".its";
            mSurfaceCrash[q].reset(new SoundSource("data/sfx/surfaces", fileNameCrash, mPFLoaderData));
            mSurfaceCrash[q]->setPitch(0.5f);
            mSurfaceCrash[q]->setRelativeToListener(true);

        }
    }

    mRoll.reset(new SoundSource("data/sfx/surfaces", "8broll0.its", mPFLoaderData));
    mRoll->setLooping(true);
    mRoll->setPitch(0.5f);
    mRoll->setRelativeToListener(true);

    mBurn.reset(new SoundSource("data/sfx/misc", "gun.its", mPFLoaderData));
    mBurn->setRelativeToListener(true);
    mBurn->setPitch(0.5f);

    mBomb.reset(new SoundSource("data/sfx/misc", "grenade.its", mPFLoaderData));
    mBomb->setRelativeToListener(true);
    mBomb->setPitch(0.5f);

    mExplosion.reset(new SoundSource("data/sfx/misc", "esh.its", mPFLoaderData));
    mExplosion->setPitch(0.5f);
    mExplosion->setAttenuation(0.05f);
    mExplosion->setReferenceDistance(30.0f);
    //mExplosion->setRelativeToListener(true);

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

    mRoll.reset();

    mBurn.reset();
    mBomb.reset();
    mExplosion.reset();
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

    if(mBurn.get() && mBurn->getSourceState() == SoundSource::Playing)
    {
        mBurn->stopPlaying();
    }

    if(mBomb.get() && mBomb->getSourceState() == SoundSource::Playing)
    {
        mBomb->stopPlaying();
    }

    if(mExplosion.get() && mExplosion->getSourceState() == SoundSource::Playing)
    {
        mExplosion->stopPlaying();
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

    if (mRoll.get() && mRoll->getSourceState() == SoundSource::Playing)
    {
        mRoll->stopPlaying();
    }
}

void SoundsProcesser::initSoundsUI(const PFLoader& mPFLoaderShell)
{
    stopSoundsUI();

    mUIOver.reset(new SoundSource("data/gameshell", "over.its", mPFLoaderShell));
    mUIOver->setPitch(2.0f);
    mUIOver->setRelativeToListener(true);

    mUIDown.reset(new SoundSource("data/gameshell", "down.its", mPFLoaderShell));
    mUIDown->setPitch(2.0f);
    mUIDown->setRelativeToListener(true);
}

void SoundsProcesser::deinitSoundsUI()
{
    stopSoundsUI();

    mUIOver.reset();
    mUIDown.reset();
}

void SoundsProcesser::stopSoundsUI()
{
    if(mUIOver.get() && mUIOver->getSourceState() == SoundSource::Playing)
    {
        mUIOver->stopPlaying();
    }

    if(mUIDown.get() && mUIDown->getSourceState() == SoundSource::Playing)
    {
        mUIDown->stopPlaying();
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

void SoundsProcesser::playSurface(size_t surfaceNumber, Ogre::Real lateralVel, Ogre::Real speedometerVelocity)
{
    size_t remappedSurfaceNumber = mTerrainData[surfaceNumber].mSoundIndex;

    bool isRoll = mTerrainData[surfaceNumber].mSoundIndex3 == 0;

    if (mRoll.get() && mRoll->getSourceState() == SoundSource::Playing)
    {
        mRoll->stopPlaying();
    }

    if (isRoll)
    {
        playRoll(lateralVel, speedometerVelocity);
    }
    else
    {
        if (remappedSurfaceNumber < mSurfacesCount)
        {
            if (mPrevSurface != remappedSurfaceNumber && mSurface[mPrevSurface].get() && mSurface[mPrevSurface]->getSourceState() == SoundSource::Playing)
            {
                mSurface[mPrevSurface]->stopPlaying();
            }

            mPrevSurface = remappedSurfaceNumber;

            if (mSurface[remappedSurfaceNumber].get() && mSurface[remappedSurfaceNumber]->getSourceState() != SoundSource::Playing)
            {
                mSurface[remappedSurfaceNumber]->startPlaying();
            }

            mSurface[remappedSurfaceNumber]->setGain(mSurfaceSoundGain.getVal(lateralVel) * mMasterGain);
        }
    }
}

void SoundsProcesser::playRoll(Ogre::Real lateralVel, Ogre::Real speedometerVelocity)
{
    if (mSurface[mPrevSurface].get() && mSurface[mPrevSurface]->getSourceState() == SoundSource::Playing)
    {
        mSurface[mPrevSurface]->stopPlaying();
    }

    if (mRoll.get() && mRoll->getSourceState() != SoundSource::Playing)
    {
        mRoll->startPlaying();
    }

    mRoll->setPitch(mSurfaceSoundPitch.getVal(speedometerVelocity));
    mRoll->setGain(mSurfaceSoundGain.getVal(lateralVel) * mMasterGain);
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

void SoundsProcesser::playCheatBurn()
{
    if(mBurn.get()/* && mBurn->getSourceState() != SoundSource::Playing*/)
    {
        mBurn->startPlaying();
    }
}

void SoundsProcesser::playCheatBomb()
{
    if(mBomb.get()/* && mBomb->getSourceState() != SoundSource::Playing*/)
    {
        mBomb->startPlaying();
    }
}

void SoundsProcesser::playExplosion(const Ogre::Vector3& pos)
{
    if(mExplosion.get()/* && mExplosion->getSourceState() != SoundSource::Playing*/)
    {
        mExplosion->setPosition(pos.x, pos.y, pos.z);
        mExplosion->startPlaying();
    }
}

void SoundsProcesser::playUIOver()
{
    if(mUIOver.get() && mUIOver->getSourceState() != SoundSource::Playing)
    {
        mUIOver->startPlaying();
    }
}

void SoundsProcesser::playUIDown()
{
    if(mUIDown.get() && mUIDown->getSourceState() != SoundSource::Playing)
    {
        mUIDown->startPlaying();
    }
}

void SoundsProcesser::setVolume(float gain)
{
    mMasterGain = gain;

    if(mCrash1.get())
        mCrash1->setGain(mMasterGain);

    if(mCrash2.get())
        mCrash2->setGain(mMasterGain);

    if(mBeforeStart1.get())
        mBeforeStart1->setGain(mMasterGain);

    if(mBeforeStart2.get())
        mBeforeStart2->setGain(mMasterGain);

    if(mBeforeStart3.get())
        mBeforeStart3->setGain(mMasterGain);

    for(int q = 0; q < mSurfacesCount; ++q)
    {
        if(mSurface[q].get())
        {
            mSurface[q]->setGain(mMasterGain);
        }

        if(mSurfaceCrash[q].get())
        {
            mSurfaceCrash[q]->setGain(mMasterGain);
        }
    }

    if (mRoll.get())
        mRoll->setGain(mMasterGain);

    if(mBurn.get())
        mBurn->setGain(mMasterGain);

    if(mBomb.get())
        mBomb->setGain(mMasterGain);

    if(mExplosion.get())
        mExplosion->setGain(mMasterGain);


    if(mUIOver.get())
        mUIOver->setGain(mMasterGain);

    if(mUIDown.get())
        mUIDown->setGain(mMasterGain);
}
