#if defined(__ANDROID__)
    #include <android/log.h>

    #define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO, "OGRE", __VA_ARGS__))
    #define LOGE(...) ((void)__android_log_print(ANDROID_LOG_ERROR, "OGRE", __VA_ARGS__)) 
#endif

#include <SFML/Audio.hpp>

#include "SoundSource.h"

#include "../includes/OgreInclude.h"

#include "../loaders/ITSLoader.h"

#include <string.h>

#ifdef _MSC_VER
    #pragma warning(disable:4996)
#endif

SoundSource::SoundSource(const std::string& subFolder, const std::string& fileName, const PFLoader& mPFLoaderData) :
    mSoundBuffer(new sf::SoundBuffer()),
    mSound(new sf::Sound(*mSoundBuffer.get()))
{
    readITS(subFolder, fileName, mPFLoaderData);
    mSoundBuffer->loadFromSamples(&mRawData[0], mRawData.size(), mHeader.channels, mHeader.samplesPerSec);
}

void SoundSource::readITS(const std::string& subFolder, const std::string& fileName, const PFLoader& mPFLoaderData)
{
    ITSLoader itsLoader;
    mHeader.dataSize = itsLoader.load(mPFLoaderData, subFolder, fileName, mRawData);
    mHeader.bitsPerSample = 8;
    mHeader.channels = 1;
    mHeader.samplesPerSec = 22050;
}

void SoundSource::startPlaying()
{
    mSound->play();
    //if(mSource)
    {
        //AL_CHECK(alSourcePlay(mSource));
    }
}

void SoundSource::pausePlaying()
{
    mSound->pause();
    //if(mSource)
    {
        //AL_CHECK(alSourcePause(mSource));
    }
}

void SoundSource::stopPlaying()
{
    mSound->stop();
    //if(mSource)
    {
        //AL_CHECK(alSourceStop(mSource));
    }
}

SoundSource::SoundSourceStatus SoundSource::getSourceState()const
{
    sf::Sound::Status status = mSound->getStatus();
    SoundSourceStatus ret = Stopped;
    if(status == sf::Sound::Stopped)
        ret = Stopped;
    if(status == sf::Sound::Paused)
        ret = Paused;
    if(status == sf::Sound::Playing)
        ret = Playing;
    //if(mSource)
    {
        //AL_CHECK(alGetSourcei(mSource, AL_SOURCE_STATE, &ret));
    }

    return ret;
}

void SoundSource::setRelativeToListener(bool isRelative)
{
    mSound->setRelativeToListener(isRelative);
}

void SoundSource::setPitch(float pitch)
{
    mSound->setPitch(pitch);
    //if(mSource)
    {
        //AL_CHECK(alSourcef(mSource, AL_PITCH, pitch));
    }
}

void SoundSource::setMinGain(float gain)
{
    //if(mSource)
    {
        //AL_CHECK(alSourcef(mSource, AL_MIN_GAIN, gain));
    }
}

void SoundSource::setMaxGain(float gain)
{
    //if(mSource)
    {
        //AL_CHECK(alSourcef(mSource, AL_MAX_GAIN, gain));
    }
}

void SoundSource::setGain(float gain)
{
    mSound->setVolume(gain * 100.0f);
    //if(mSource)
    {
        //AL_CHECK(alSourcef(mSource, AL_GAIN, gain));
    }
}

float SoundSource::getGain()const
{
    float res = 0.0f;

    res = mSound->getVolume() / 100.0f;

    //if(mSource)
    {
        //AL_CHECK(alGetSourcef(mSource, AL_GAIN, &res));
    }

    return res;
}

void SoundSource::setLooping(bool looping)
{
    mSound->setLoop(looping);
    //if(mSource)
    {
        //AL_CHECK(alSourcei(mSource, AL_LOOPING, looping));
    }
}

void SoundSource::setPosition(float x, float y, float z)
{
    mSound->setPosition(x, y, z);
    //if(mSource)
    {
        //AL_CHECK(alSource3f(mSource, AL_POSITION, x, y, z));
    }
}

void SoundSource::setReferenceDistance(float dist)
{
    mSound->setMinDistance(dist);
    //if(mSource)
    {
        //AL_CHECK(alSourcef(mSource, AL_REFERENCE_DISTANCE, dist));
    }
}

void SoundSource::setMaxDistance(float dist)
{
    //if(mSource)
    {
        //AL_CHECK(alSourcef(mSource, AL_MAX_DISTANCE, dist));
    }
}