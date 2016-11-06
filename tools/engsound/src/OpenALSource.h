
#ifndef OPENALWAVFILE
#define OPENALWAVFILE

#include "OpenALInclude.h"
#include <string>
#include <vector>
#include <cassert>

//http://pielot.org/2010/12/openal-on-android/
typedef struct {
    char  riff[4];//'RIFF'
    unsigned int riffSize;
    char  wave[4];//'WAVE'
    char  fmt[4];//'fmt '
    unsigned int fmtSize;
    unsigned short format;
    unsigned short channels;
    unsigned int samplesPerSec;
    unsigned int bytesPerSec;
    unsigned short blockAlign;
    unsigned short bitsPerSample;
    char  data[4];//'data'
    unsigned int dataSize;
}BasicWAVEHeader;

#ifdef NDEBUG
#define ENABLE_AL_CHECK 0
#else
#define ENABLE_AL_CHECK 1
#endif

#if ENABLE_AL_CHECK
#define AL_CHECK(glFunc) \
    { \
        glFunc; \
        int e = alGetError(); \
        if (e != AL_NO_ERROR) \
        { \
            const char * errorString = ""; \
            switch(e) \
            { \
                case AL_INVALID_NAME:       errorString = "AL_INVALID_NAME";        break; \
                case AL_INVALID_ENUM:       errorString = "AL_INVALID_ENUM";       break; \
                case AL_INVALID_VALUE:      errorString = "AL_INVALID_VALUE";   break; \
                case AL_INVALID_OPERATION:  errorString = "AL_INVALID_OPERATION";       break; \
                case AL_OUT_OF_MEMORY:      errorString = "AL_OUT_OF_MEMORY";       break; \
                default:                                                            break; \
            } \
            printf("OpenAL error 0x%04X %s for %s", e, errorString, #glFunc); \
            assert(false); \
        } \
    }
#else
#   define AL_CHECK(glFunc) { glFunc; }
#endif

class OpenALSource
{
public:

    OpenALSource(const std::string& fileName, float amplifyFactor = 1.0f);

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
    void setLooping(ALboolean looping);

private:

    void readWAV(const std::string& fileName);
    void amplifyRawData(float factor);
    void createBufferFromRawWave();
    void createSource();

    BasicWAVEHeader mHeader;
    std::vector<unsigned char> mRawData;

    ALuint mBuffer;
    ALuint mFormat;
    ALuint mSource;
};

#endif