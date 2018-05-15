
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

void TrialGhost::init()
{
    mTimedPositions.clear();
    mTimedPositionsPrev.clear();
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
                res = mTimedPositionsPrev[q + 0].second;
                //res.chassisPos = Tools::lerp(mTimedPositionsPrev[q + 0].second.chassisPos, mTimedPositionsPrev[q + 1].second.chassisPos, (time - timeA) / (timeB - timeA));
                //res.chassisRot = Tools::lerp(mTimedPositionsPrev[q + 0].second.chassisRot, mTimedPositionsPrev[q + 0].second.chassisRot, (time - timeA) / (timeB - timeA));
                break;
            }
        }
    }

    return res;
}

void TrialGhost::swapData()
{
    mTimedPositionsPrev = mTimedPositions;
}

void TrialGhost::clearCurrent()
{
    mTimedPositions.clear();
}