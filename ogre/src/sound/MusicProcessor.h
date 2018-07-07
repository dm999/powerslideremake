
#ifndef MUSICPROCESSOR
#define MUSICPROCESSOR

#ifndef NO_OPENAL

#include <string>

#include "../GameState.h"

#include "../includes/CommonIncludes.h"

namespace sf
{
    class Music;
}

class MusicProcessor
{
public:

    void init(const GameState& gameState);
    void deinit();

    void play();
    void stop();

private:

    CommonIncludes::shared_ptr<sf::Music> mMusic;
    Ogre::DataStreamPtr mTrack0;
};

#endif
#endif