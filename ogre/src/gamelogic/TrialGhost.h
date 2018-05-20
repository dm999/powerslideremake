#ifndef TRIALGHOST_H
#define TRIALGHOST_H

#include <vector>
#include <utility>

#include "../includes/OgreInclude.h"

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

    typedef std::vector<std::pair<Ogre::Real, GhostPos> > GhostData;

    TrialGhost() : mFileName("hiscores.dat"){}

    void init(const std::string& dataDir, const std::string& trackName);
    void storePoint(const GhostPos& pos, Ogre::Real time);
    GhostPos getInterpolatedPoint(Ogre::Real time);
    bool lapFinished(Ogre::Real bestTime, const std::string& dataDir, bool isSupercar);
    bool isVisible()const{return mIsGhostVisible;}

private:

    void swapData();
    void clearCurrent();

    void saveBinToFile(const std::string& dataDir);
    void loadBinFromFile(const std::string& dataDir);

    void load();//for debug
    void save();//for debug

    const std::string mFileName;
    std::string mTrackName;

    Ogre::Real mGhostBestLapTime;
    bool mIsGhostVisible;

    GhostData mTimedPositions;
    GhostData mTimedPositionsPrev;

    Ogre::Real mPrevTimeRecord;
    Ogre::Real mWeightedTimeRecord;
    std::vector<Ogre::Real> mTimeIncWeightingRecord;

    Ogre::Real mPrevTimePlayback;
    Ogre::Real mWeightedTimePlayback;
    std::vector<Ogre::Real> mTimeIncWeighting;
};

#endif