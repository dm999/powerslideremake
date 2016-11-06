

#include "OpenALSource.h"

#include <string.h>

#ifdef _MSC_VER
    #pragma warning(disable:4996)
#endif

OpenALSource::OpenALSource(const std::string& fileName, float amplifyFactor) :
    mBuffer(0),
    mFormat(0),
    mSource(0)
{
    readWAV(fileName.c_str());
    if(amplifyFactor != 1.0f)
    {
        amplifyRawData(amplifyFactor);
    }
    createBufferFromRawWave();
    createSource();
}

OpenALSource::~OpenALSource()
{
    clearALData();
}

void OpenALSource::clearALData()
{
    if(mSource)
    {
        AL_CHECK(alDeleteSources(1, &mSource));
        mSource = 0;
    }

    if(mBuffer)
    {
        AL_CHECK(alDeleteBuffers(1, &mBuffer));
        mBuffer = 0;
    }
}

void OpenALSource::readWAV(const std::string& fileName){

    FILE* file = fopen(fileName.c_str(),"rb");

    if (file)
    {

        if (fread(&mHeader, sizeof(BasicWAVEHeader), 1, file))
        {
            if (!(//these things *must* be valid with this basic header
            memcmp("RIFF",mHeader.riff,4) ||
            memcmp("WAVE",mHeader.wave,4) ||
            memcmp("fmt ",mHeader.fmt,4)  ||
            memcmp("data",mHeader.data,4)
            ))
            {
                mRawData.clear();
                mRawData.resize(mHeader.dataSize);

                fread(&mRawData[0],mHeader.dataSize,1,file);
            }
        }

        fclose(file);
    }
}

void OpenALSource::amplifyRawData(float factor)
{
    if(!mRawData.empty())
    {
        for(size_t q = 0; q < mRawData.size(); ++q)
        {
            unsigned short data = mRawData[q];
            data = static_cast<unsigned short>(data * factor);
            if(data > 0xFF)
            {
                data = 0xFF;
            }

            mRawData[q] = static_cast<unsigned char>(data);
        }
    }
}

void OpenALSource::createBufferFromRawWave()
{
    if(!mRawData.empty())
    {
        bool correctHeader = false;

        switch (mHeader.bitsPerSample){
            
            case 8:
            mFormat = (mHeader.channels == 1) ? AL_FORMAT_MONO8 : AL_FORMAT_STEREO8;
            correctHeader = true;
            break;

            case 16:
            mFormat = (mHeader.channels == 1) ? AL_FORMAT_MONO16 : AL_FORMAT_STEREO16;
            correctHeader = true;
            break;
        }

        if(correctHeader)
        {
            AL_CHECK(alGenBuffers(1, &mBuffer));
            AL_CHECK(alBufferData(mBuffer, mFormat, &mRawData[0], mHeader.dataSize, mHeader.samplesPerSec));
        }
    }
}

void OpenALSource::createSource()
{
    if(mBuffer)
    {
        AL_CHECK(alGenSources(1, &mSource ));
        AL_CHECK(alSourcei(mSource, AL_BUFFER, mBuffer));
    }
}

void OpenALSource::startPlaying()
{
    if(mSource)
    {
        AL_CHECK(alSourcePlay(mSource));
    }
}

void OpenALSource::pausePlaying()
{
    if(mSource)
    {
        AL_CHECK(alSourcePause(mSource));
    }
}

void OpenALSource::stopPlaying()
{
    if(mSource)
    {
        AL_CHECK(alSourceStop(mSource));
    }
}

int OpenALSource::getSourceState()const
{
    int ret = AL_INITIAL;
    if(mSource)
    {
        AL_CHECK(alGetSourcei(mSource, AL_SOURCE_STATE, &ret));
    }

    return ret;
}

void OpenALSource::setPitch(ALfloat pitch)
{
    if(mSource)
    {
        AL_CHECK(alSourcef(mSource, AL_PITCH, pitch));
    }
}

void OpenALSource::setMinGain(ALfloat gain)
{
    if(mSource)
    {
        AL_CHECK(alSourcef(mSource, AL_MIN_GAIN, gain));
    }
}

void OpenALSource::setMaxGain(ALfloat gain)
{
    if(mSource)
    {
        AL_CHECK(alSourcef(mSource, AL_MAX_GAIN, gain));
    }
}

void OpenALSource::setGain(ALfloat gain)
{
    if(mSource)
    {
        AL_CHECK(alSourcef(mSource, AL_GAIN, gain));
    }
}

void OpenALSource::setLooping(ALboolean looping)
{
    if(mSource)
    {
        AL_CHECK(alSourcei(mSource, AL_LOOPING, looping));
    }
}