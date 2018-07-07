#ifndef NO_OPENAL

#include "MusicProcessor.h"

#include <SFML/Audio.hpp>

#include "../tools/OgreTools.h"

void MusicProcessor::init(const std::string& dataDir)
{
    mTrack0 = getReadableFile(dataDir, "track_0.ogg");
    mMusic = CommonIncludes::shared_ptr<sf::Music>(new sf::Music());
    mMusic->setLoop(true);
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
        if(mTrack0.get() && mTrack0->isReadable())
        {
            mBuf.resize(mTrack0->size());
            mTrack0->read(&mBuf[0], mTrack0->size());
            mTrack0->close();

            if (!mMusic->openFromMemory(&mBuf[0], mBuf.size()))
                return;

            mMusic->play();
        }
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