
#ifndef PAUSEDTIMER_H
#define PAUSEDTIMER_H

#include "../includes/OgreInclude.h"

class PausedTimer
{
public:

    PausedTimer();

    void pause();
    void resume();

    unsigned long getMilliseconds() const;
    void reset();

private:

    mutable Ogre::Timer mTimer;
    unsigned long mTimeLengthBeforePause;
    unsigned long mPauseLength;
    bool mIsPaused;

};

#endif