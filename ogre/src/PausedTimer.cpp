#include "pcheader.h"

#include "PausedTimer.h"

PausedTimer::PausedTimer() : mTimeLengthBeforePause(0), mPauseLength(0), mIsPaused(false)
{
}

unsigned long PausedTimer::getMilliseconds()
{
    unsigned long res = 0;

    if(mIsPaused)
    {
        res = mTimeLengthBeforePause;
    }
    else
    {
        res = mTimer.getMilliseconds() - mPauseLength;
    }

    return res;
}

void PausedTimer::reset()
{
    mTimer.reset();
    mIsPaused = false;
    mPauseLength = 0;
    mTimeLengthBeforePause = 0;
}

void PausedTimer::pause()
{
    if(!mIsPaused)
    {
        mTimeLengthBeforePause = mTimer.getMilliseconds() - mPauseLength;
        mIsPaused = true;
    }
}

void PausedTimer::resume()
{
    if(mIsPaused)
    {
        mPauseLength = mTimer.getMilliseconds() - mTimeLengthBeforePause;
        mIsPaused = false;
    }
}