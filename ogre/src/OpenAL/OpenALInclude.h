
#ifndef OPENALINCLUDE
#define OPENALINCLUDE

#include <AL/al.h>
#include <AL/alc.h>

#include <stdio.h>
#include <cassert>

#ifdef NDEBUG
#define ENABLE_AL_CHECK 0
#else
#define ENABLE_AL_CHECK 1
#endif

#if !defined(__ANDROID__)
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
#else
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
                char msgBuf[4096]; \
                sprintf(msgBuf, "OpenAL error 0x%04X %s for %s", e, errorString, #glFunc); \
                LOGE(msgBuf); \
            } \
        }
#endif

#endif