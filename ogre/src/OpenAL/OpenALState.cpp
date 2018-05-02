#if defined(__ANDROID__)
    #include <android/log.h>

    #define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO, "OGRE", __VA_ARGS__))
    #define LOGE(...) ((void)__android_log_print(ANDROID_LOG_ERROR, "OGRE", __VA_ARGS__)) 
#endif

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
#if defined(__ANDROID__)
    LOGI("OpenALState[init]: Begin"); 
#endif

     const ALint context_attribs[] = { ALC_FREQUENCY, 22050, 0 };

    if(!mDevice)
    {
        mDevice = alcOpenDevice(0);
#if defined(__ANDROID__)
        if(!mDevice)
            LOGE("OpenALState[init]: Error create audio device"); 
#endif
    }

    if(!mContext)
    {
        mContext = alcCreateContext(mDevice, context_attribs);
        alcMakeContextCurrent(mContext);
#if defined(__ANDROID__)
        if(!mContext)
            LOGE("OpenALState[init]: Error create audio context"); 
#endif
    }

    AL_CHECK(alDistanceModel(AL_EXPONENT_DISTANCE));

#if defined(__ANDROID__)
    LOGI("OpenALState[init]: End"); 
#endif
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