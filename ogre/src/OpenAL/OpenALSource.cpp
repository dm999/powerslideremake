#include "OpenALSource.h"

#include "../OgreInclude.h"

#include "../loaders/ITSLoader.h"

#include <string.h>

#ifdef _MSC_VER
    #pragma warning(disable:4996)
#endif

OpenALSource::OpenALSource(const std::string& subFolder, const std::string& fileName, const PFLoader& mPFLoaderData, float amplifyFactor) :
    mBuffer(0),
    mFormat(0),
    mSource(0)
{
    readITS(subFolder, fileName, mPFLoaderData);
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

void OpenALSource::readITS(const std::string& subFolder, const std::string& fileName, const PFLoader& mPFLoaderData)
{
    ITSLoader itsLoader;
    mHeader.dataSize = itsLoader.load(mPFLoaderData, subFolder, fileName, mRawData);
    mHeader.bitsPerSample = 8;
    mHeader.channels = 1;
    mHeader.samplesPerSec = 22050;
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
            mRawData.clear();
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

ALfloat OpenALSource::getGain()const
{
    ALfloat res = 0.0f;

    if(mSource)
    {
        AL_CHECK(alGetSourcef(mSource, AL_GAIN, &res));
    }

    return res;
}

void OpenALSource::setLooping(ALboolean looping)
{
    if(mSource)
    {
        AL_CHECK(alSourcei(mSource, AL_LOOPING, looping));
    }
}

void OpenALSource::setPosition(ALfloat x, ALfloat y, ALfloat z)
{
    if(mSource)
    {
        AL_CHECK(alSource3f(mSource, AL_POSITION, x, y, z));
    }
}

void OpenALSource::setReferenceDistance(ALfloat dist)
{
    if(mSource)
    {
        AL_CHECK(alSourcef(mSource, AL_REFERENCE_DISTANCE, dist));
    }
}

void OpenALSource::setMaxDistance(ALfloat dist)
{
    if(mSource)
    {
        AL_CHECK(alSourcef(mSource, AL_MAX_DISTANCE, dist));
    }
}