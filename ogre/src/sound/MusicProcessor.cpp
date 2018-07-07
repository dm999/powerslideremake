#ifndef NO_OPENAL

#include "MusicProcessor.h"

#include <SFML/Audio.hpp>

#include "../tools/OgreTools.h"

void MusicProcessor::initTrack(const std::string& track)
{
    stop();

    Ogre::DataStreamPtr file = getReadableFile(mDataDir, track);

    float vol = getVolume();

    mMusic = CommonIncludes::shared_ptr<sf::Music>(new sf::Music());

    setVolume(vol);

    mBuf.clear();
    if(file.get() && file->isReadable())
    {
        mBuf.resize(file->size());
        file->read(&mBuf[0], file->size());
        file->close();
    }

    if(track == "track_0.ogg")
    {
        mMusic->setLoop(true);
    }
}

void MusicProcessor::deinit()
{
    stop();
    mMusic.reset();
}

void MusicProcessor::play()
{
    if(mMusic.get())
    {
        if(!mBuf.empty())
        {
            if (!mMusic->openFromMemory(&mBuf[0], mBuf.size()))
                return;

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

void MusicProcessor::setVolume(float vol)
{
    if(mMusic.get())
    {
        mMusic->setVolume(vol * 100.0f);
    }
}

float MusicProcessor::getVolume() const
{
    float ret = 1.0f;

    if(mMusic.get())
    {
        ret = mMusic->getVolume() * 0.01f;
    }

    return ret;
}

#endif