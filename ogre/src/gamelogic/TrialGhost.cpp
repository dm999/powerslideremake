
#include "TrialGhost.h"

#include "../tools/Tools.h"

void TrialGhost::init()
{
    mTimedPositions.clear();
    mTimedPositionsPrev.clear();
    mIsSwapped = true;
    mLapTimer.reset();
}

void TrialGhost::storePoint(const GhostPos& pos)
{
    if(mIsSwapped)
    {
        mTimedPositions.push_back(std::make_pair(mLapTimer.getMilliseconds() / 1000.0f, pos));
    }
}

GhostPos TrialGhost::getInterpolatedPoint(Ogre::Real time)
{
    GhostPos res;

    time = mLapTimer.getMilliseconds() / 1000.0f;

    if(!mTimedPositionsPrev.empty())
    {
        res = mTimedPositionsPrev[0].second;

        for(size_t q = 0; q < mTimedPositionsPrev.size() - 1; ++q)
        {
            Ogre::Real timeA = mTimedPositionsPrev[q + 0].first;
            Ogre::Real timeB = mTimedPositionsPrev[q + 1].first;

            if(time >= timeA && time < timeB)
            {
                res = mTimedPositionsPrev[q + 0].second;
                //res.chassisPos = Tools::lerp(mTimedPositionsPrev[q + 0].second.chassisPos, mTimedPositionsPrev[q + 1].second.chassisPos, (time - timeA) / (timeB - timeA));
                //res.chassisRot = Tools::lerp(mTimedPositionsPrev[q + 0].second.chassisRot, mTimedPositionsPrev[q + 0].second.chassisRot, (time - timeA) / (timeB - timeA));
                break;
            }
        }
    }

    return res;
}

void TrialGhost::lapFinished()
{
    mLapTimer.reset();
    mIsSwapped = false;
}

void TrialGhost::swapData()
{
    mTimedPositionsPrev = mTimedPositions;
    mTimedPositions.clear();
    mIsSwapped = true;
}