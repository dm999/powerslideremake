
#include "TrialGhost.h"

#include "../tools/Tools.h"

#include "../pscar/PSBaseGraphicsVehicle.h"

namespace{
    const size_t maxRecordedSamples = 10000;
}

GhostPos::GhostPos() 
    : chassisPos(Ogre::Vector3::ZERO), chassisRot(Ogre::Quaternion::IDENTITY)
{}

GhostPos::GhostPos(PSBaseGraphicsVehicle& playerCar)
    :   chassisPos(playerCar.getModelNode()->getPosition()),
        chassisRot(playerCar.getModelNode()->getOrientation())
{
    for(size_t q = 0; q < InitialVehicleSetup::mWheelsAmount; ++q)
    {
        wheelPos[q] = playerCar.getWheelNodes()[q]->getPosition();
        wheelRot[q] = playerCar.getWheelNodes()[q]->getOrientation();
    }
}

GhostPos GhostPos::lerp(const GhostPos& other, Ogre::Real min, Ogre::Real max, Ogre::Real cur) const
{
    GhostPos ret;

    Ogre::Real step = (cur - min) / (max - min);

    ret.chassisRot = Ogre::Quaternion::Slerp(step, chassisRot, other.chassisRot);

    for(size_t q = 0; q < InitialVehicleSetup::mWheelsAmount; ++q)
    {
        ret.wheelRot[q] = Ogre::Quaternion::Slerp(step, wheelRot[q], other.wheelRot[q]);
    }

    return ret;
}

void TrialGhost::init()
{
    mTimedPositions.clear();
    mTimedPositionsPrev.clear();
    
    mPosSpline.clear();

    for(size_t q = 0; q < InitialVehicleSetup::mWheelsAmount; ++q)
    {
        mPosSplineWheels[q].clear();
    }
}

void TrialGhost::storePoint(const GhostPos& pos, Ogre::Real time)
{
    if(mTimedPositions.size() < maxRecordedSamples)
    {
        mTimedPositions.push_back(std::make_pair(time, pos));
    }
}

GhostPos TrialGhost::getInterpolatedPoint(Ogre::Real time)
{
    GhostPos res;

    if(!mTimedPositionsPrev.empty())
    {
        res = mTimedPositionsPrev[0].second;

        for(size_t q = 0; q < mTimedPositionsPrev.size() - 1; ++q)
        {
            Ogre::Real timeA = mTimedPositionsPrev[q + 0].first;
            Ogre::Real timeB = mTimedPositionsPrev[q + 1].first;

            if(time >= timeA && time < timeB)
            {
                res = mTimedPositionsPrev[q + 0].second.lerp(mTimedPositionsPrev[q + 1].second, timeA, timeB, time);

                Ogre::Real step = Ogre::Math::Clamp(time / mTimedPositionsPrev[mTimedPositionsPrev.size() - 1].first, 0.0f, 1.0f);

                res.chassisPos = mPosSpline.interpolate(step);

                for(size_t w = 0; w < InitialVehicleSetup::mWheelsAmount; ++w)
                {
                    res.wheelPos[w] = mPosSplineWheels[w].interpolate(step);
                }

                break;
            }
        }
    }

    return res;
}

void TrialGhost::swapData()
{
    mTimedPositionsPrev = mTimedPositions;

    mPosSpline.clear();

    for(size_t q = 0; q < InitialVehicleSetup::mWheelsAmount; ++q)
    {
        mPosSplineWheels[q].clear();
    }

    for(size_t q = 0; q < mTimedPositionsPrev.size(); ++q)
    {
        mPosSpline.addPoint(mTimedPositionsPrev[q].second.chassisPos);

        for(size_t w = 0; w < InitialVehicleSetup::mWheelsAmount; ++w)
        {
            mPosSplineWheels[w].addPoint(mTimedPositionsPrev[q].second.wheelPos[w]);
        }
    }
}

void TrialGhost::clearCurrent()
{
    mTimedPositions.clear();
}