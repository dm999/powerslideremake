
#ifndef SOUNDSOURCE
#define SOUNDSOURCE

#ifndef NO_OPENAL

#include <string>
#include <vector>

#include "../loaders/PFLoader.h"

#include "../includes/CommonIncludes.h"

namespace sf
{
    class SoundBuffer;
    class Sound;
}

struct SoundHeader
{
    unsigned short channels;
    unsigned int samplesPerSec;
    unsigned short bitsPerSample;
    unsigned int dataSize;
};

class SoundSource
{
public:

    enum SoundSourceStatus
    {
        Stopped, ///< Sound is not playing
        Paused,  ///< Sound is paused
        Playing  ///< Sound is playing
    };

    SoundSource(const std::string& fileName, const std::string& subFolder, const PFLoader& mPFLoaderData);

    void startPlaying();
    void pausePlaying();
    void stopPlaying();

    SoundSourceStatus getSourceState()const;

    void setRelativeToListener(bool isRelative);

    void setPitch(float pitch);
    void setMinGain(float gain);
    void setMaxGain(float gain);
    void setGain(float gain);
    float getGain()const;
    void setLooping(bool looping);
    void setPosition(float x, float y, float z);
    void setReferenceDistance(float dist);
    void setMaxDistance(float dist);

private:

    CommonIncludes::shared_ptr<sf::SoundBuffer> mSoundBuffer;
    CommonIncludes::shared_ptr<sf::Sound> mSound;

    void readITS(const std::string& fileName, const std::string& subfolder, const PFLoader& mPFLoaderData);

    SoundHeader mHeader;
    std::vector<signed short> mRawData;
};

#endif
#endif