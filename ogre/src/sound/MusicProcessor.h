
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

    void init(const std::string& dataDir){mDataDir = dataDir;}
    void initTrack(const std::string& track);
    void deinit();

    void play();
    void pause();
    void stop();

private:

    std::string mDataDir;
    CommonIncludes::shared_ptr<sf::Music> mMusic;
    std::vector<Ogre::uint8> mBuf;
};

#endif
#endif