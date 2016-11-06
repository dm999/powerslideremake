
#include "OpenALState.h"

OpenALState::OpenALState() :
    mDevice(0),
    mContext(0)
{
}

OpenALState::~OpenALState()
{
}

void OpenALState::init()
{
     const ALint context_attribs[] = { ALC_FREQUENCY, 22050, 0 };

    if(!mDevice)
    {
        mDevice = alcOpenDevice(0);
    }

    if(!mContext)
    {
        mContext = alcCreateContext(mDevice, context_attribs);
        alcMakeContextCurrent(mContext);
    }
}

void OpenALState::deInit()
{
    // Cleaning up
    alcMakeContextCurrent(0);

    if(mContext)
    {
        alcDestroyContext(mContext);
        mContext = 0;
    }

    if(mDevice)
    {
        alcCloseDevice(mDevice);
        mDevice = 0;
    }
}