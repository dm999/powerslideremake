
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

SoundSource::SoundSource() :
    mSoundBuffer(std::make_shared<sf::SoundBuffer>()),
    mSound(std::make_shared<sf::Sound>(*mSoundBuffer.get()))
{}

SoundSource::SoundSource(const std::string& subFolder, const std::string& fileName, const PFLoader& mPFLoaderData) :
    mSoundBuffer(std::make_shared<sf::SoundBuffer>()),
    mSound(std::make_shared<sf::Sound>(*mSoundBuffer.get()))
{
    readITS(subFolder, fileName, mPFLoaderData);
    mSoundBuffer->loadFromSamples(&mRawData[0], mRawData.size(), mHeader.channels, mHeader.samplesPerSec);
}

void SoundSource::updateSamples(const std::vector<Ogre::int16> &samples, size_t sampleCount, size_t channels, size_t sampleRate)
{
    mSoundBuffer->loadFromSamples(&samples[0], sampleCount, channels, sampleRate);
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
}

void SoundSource::pausePlaying()
{
    mSound->pause();
}

void SoundSource::stopPlaying()
{
    mSound->stop();
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

    return ret;
}

void SoundSource::setRelativeToListener(bool isRelative)
{
    mSound->setRelativeToListener(isRelative);
}

void SoundSource::setPitch(float pitch)
{
    mSound->setPitch(pitch);
}

void SoundSource::setGain(float gain)
{
    mSound->setVolume(gain * 100.0f);
}

float SoundSource::getGain()const
{
    float res = 0.0f;

    res = mSound->getVolume() * 0.01f;

    return res;
}

void SoundSource::setLooping(bool looping)
{
    mSound->setLoop(looping);
}

void SoundSource::setPosition(float x, float y, float z)
{
    mSound->setPosition(x, y, z);
}

void SoundSource::setReferenceDistance(float dist)
{
    mSound->setMinDistance(dist);
}

void SoundSource::setAttenuation(float att)
{
    mSound->setAttenuation(att);
}