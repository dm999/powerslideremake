#ifndef TRIALGHOST_H
#define TRIALGHOST_H

#include <vector>
#include <utility>

#include "../includes/OgreInclude.h"
#include "../tools/PausedTimer.h"

struct GhostPos
{
    GhostPos() : chassisPos(Ogre::Vector3::ZERO), chassisRot(Ogre::Quaternion::IDENTITY){}
    GhostPos(const Ogre::Vector3& pos, const Ogre::Quaternion& rot) : chassisPos(pos), chassisRot(rot){}
    Ogre::Vector3 chassisPos;
    Ogre::Quaternion chassisRot;
};

class TrialGhost
{
public:

    void init();
    void storePoint(const GhostPos& pos, Ogre::Real time);
    GhostPos getInterpolatedPoint(Ogre::Real time);
    void lapFinished();
    void swapData();
    void clearCurrent();

private:

    std::vector<std::pair<Ogre::Real, GhostPos> > mTimedPositions;
    std::vector<std::pair<Ogre::Real, GhostPos> > mTimedPositionsPrev;

    PausedTimer mLapTimer;

};

#endif