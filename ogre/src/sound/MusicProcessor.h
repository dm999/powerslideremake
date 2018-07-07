
#ifndef MUSICPROCESSOR
#define MUSICPROCESSOR

#ifndef NO_OPENAL

#include <string>

#include "../includes/CommonIncludes.h"

namespace sf
{
    class Music;
}

class MusicProcessor
{
public:

    void init(const std::string& dataDir);
    void deinit();

    void play();
    void stop();

private:

    CommonIncludes::shared_ptr<sf::Music> mMusic;
    Ogre::DataStreamPtr mTrack0;
    std::vector<Ogre::uint8> mBuf;
};

#endif
#endif