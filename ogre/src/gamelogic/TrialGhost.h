#ifndef TRIALGHOST_H
#define TRIALGHOST_H

#include <vector>
#include <utility>

#include "../includes/OgreInclude.h"
#include "../tools/PausedTimer.h"

#include "../physics/InitialVehicleSetup.h"

class PSBaseGraphicsVehicle;

struct GhostPos
{
    GhostPos();
    GhostPos(PSBaseGraphicsVehicle& playerCar);
    Ogre::Vector3 chassisPos;
    Ogre::Quaternion chassisRot;

    Ogre::Vector3 wheelPos[InitialVehicleSetup::mWheelsAmount];//RR, RL, FR, FL
    Ogre::Quaternion wheelRot[InitialVehicleSetup::mWheelsAmount];//RR, RL, FR, FL

    GhostPos lerp(const GhostPos& other, Ogre::Real min, Ogre::Real max, Ogre::Real cur) const;
};

class TrialGhost
{
public:

    void init();
    void storePoint(const GhostPos& pos, Ogre::Real time);
    GhostPos getInterpolatedPoint(Ogre::Real time);
    void swapData();
    void clearCurrent();

private:

    std::vector<std::pair<Ogre::Real, GhostPos> > mTimedPositions;
    std::vector<std::pair<Ogre::Real, GhostPos> > mTimedPositionsPrev;
    Ogre::SimpleSpline mPosSpline;
    Ogre::SimpleSpline mPosSplineWheels[InitialVehicleSetup::mWheelsAmount];

};

#endif