#ifndef NO_OPENAL

#include "MusicProcessor.h"

#include <SFML/Audio.hpp>

#include "../tools/OgreTools.h"

void MusicProcessor::initTrack(const std::string& track)
{
    stop();

    Ogre::DataStreamPtr file = getReadableFile(mDataDir, track);

    mMusic = CommonIncludes::shared_ptr<sf::Music>(new sf::Music());

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

#endif