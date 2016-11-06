
#ifndef OPENALWAVFILE
#define OPENALWAVFILE

#include "OpenALInclude.h"
#include <string>
#include <vector>

#include "../loaders/PFLoader.h"

struct SoundHeader
{
    unsigned short channels;
    unsigned int samplesPerSec;
    unsigned short bitsPerSample;
    unsigned int dataSize;
};

class OpenALSource
{
public:

    OpenALSource(const std::string& fileName, const std::string& subFolder, const PFLoader& mPFLoaderData, float amplifyFactor = 1.0f);

    ~OpenALSource();

    void startPlaying();
    void pausePlaying();
    void stopPlaying();
    void clearALData();

    int getSourceState()const;

    void setPitch(ALfloat pitch);
    void setMinGain(ALfloat gain);
    void setMaxGain(ALfloat gain);
    void setGain(ALfloat gain);
    ALfloat getGain()const;
    void setLooping(ALboolean looping);
    void setPosition(ALfloat x, ALfloat y, ALfloat z);
    void setReferenceDistance(ALfloat dist);
    void setMaxDistance(ALfloat dist);

private:

    void readITS(const std::string& fileName, const std::string& subfolder, const PFLoader& mPFLoaderData);
    void amplifyRawData(float factor);
    void createBufferFromRawWave();
    void createSource();

    SoundHeader mHeader;
    std::vector<unsigned char> mRawData;

    ALuint mBuffer;
    ALuint mFormat;
    ALuint mSource;
};

#endif