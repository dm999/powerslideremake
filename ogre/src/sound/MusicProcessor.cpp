#ifndef NO_OPENAL

#include "MusicProcessor.h"

#include <SFML/Audio.hpp>

#include "../tools/OgreTools.h"

void MusicProcessor::init(const GameState& gameState)
{
    mTrack0 = getReadableFile("track_0.ogg", gameState.getDataDir());
    mMusic = CommonIncludes::shared_ptr<sf::Music>(new sf::Music());
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
        //if (!mMusic->openFromStream(mTrack0.get()))
            //return;

        //mMusic->play();
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