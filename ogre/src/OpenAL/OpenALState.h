
#ifndef OPENALSTATE
#define OPENALSTATE

#include "OpenALInclude.h"

class OpenALState
{
public:

    OpenALState();
    ~OpenALState();

    void init();
    void deInit();

private:

    ALCdevice* mDevice;
    ALCcontext* mContext;

};

#endif