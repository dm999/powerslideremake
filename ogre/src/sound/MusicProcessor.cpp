#include "MusicProcessor.h"

#include <SFML/Audio.hpp>

#include "../tools/OgreTools.h"

void MusicProcessor::initTrack(const std::string& track, bool isLooped)
{
    stop();

    Ogre::DataStreamPtr file = getReadableFile(mDataDir, track);

    float vol = getVolume();

    mMusic = std::make_shared<sf::Music>();

    setVolume(vol);

    mBuf.clear();
    if(file.get() && file->isReadable())
    {
        mBuf.resize(file->size());
        file->read(&mBuf[0], file->size());
        file->close();
    }

    mMusic->setLoop(isLooped);

    if(!mBuf.empty())
    {
        mMusic->openFromMemory(&mBuf[0], mBuf.size());
    }
}

void MusicProcessor::deinit()
{
    stop();
    mMusic.reset();
}

void MusicProcessor::play()
{
    if(mMasterGain > 0.0f)
    {
        if(mMusic.get())
        {
            mMusic->play();
        }
    }
}

void MusicProcessor::pause()
{
    if(mMusic.get())
    {
        mMusic->pause();
    }
}

void MusicProcessor::stop()
{
    if(mMusic.get())
    {
        mMusic->stop();
    }
}

bool MusicProcessor::isPaused() const
{
    bool ret = false;

    if(mMusic.get())
    {
        ret = mMusic->getStatus() == sf::SoundSource::Paused;
    }

    return ret;
}

void MusicProcessor::setVolume(float vol)
{
    mMasterGain = vol;

    if(mMusic.get())
    {
        mMusic->setVolume(mMasterGain * 100.0f);

        if(mMasterGain == 0.0f)
        {
            stop();
        }
        else
        {
            if(mMusic->getStatus() == sf::SoundSource::Stopped)
            {
                play();
            }
        }
    }
}

float MusicProcessor::getVolume() const
{
    return mMasterGain;
}
