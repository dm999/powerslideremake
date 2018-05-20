
#include "TrialGhost.h"

#include "../tools/Tools.h"
#include "../tools/SectionsFile.h"

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

    ret.chassisPos = Tools::lerp(chassisPos, other.chassisPos, step);
    ret.chassisRot = Ogre::Quaternion::Slerp(step, chassisRot, other.chassisRot);
    //ret.chassisRot = Tools::lerp(chassisRot, other.chassisRot, step);

    for(size_t q = 0; q < InitialVehicleSetup::mWheelsAmount; ++q)
    {
        ret.wheelPos[q] = Tools::lerp(wheelPos[q], other.wheelPos[q], step);
        ret.wheelRot[q] = Ogre::Quaternion::Slerp(step, wheelRot[q], other.wheelRot[q]);
        //ret.wheelRot[q] = Tools::lerp(wheelRot[q], other.wheelRot[q], step);
    }

    return ret;
}

void TrialGhost::init(const std::string& dataDir, const std::string& trackName)
{
    mTimedPositions.clear();
    mTimedPositionsPrev.clear();
    mGhostBestLapTime = 0.0f;
    mIsGhostVisible = false;

    mTimeIncWeightingRecord.clear();
    mPrevTimeRecord = 0.0f;
    mWeightedTimeRecord = 0.0f;

    mTimeIncWeighting.clear();
    mPrevTimePlayback = 0.0f;
    mWeightedTimePlayback = 0.0f;

    mTrackName = trackName;

    //load();
    loadBinFromFile(dataDir);
}

void TrialGhost::storePoint(const GhostPos& pos, Ogre::Real time)
{
    if(mTimedPositions.size() < maxRecordedSamples)
    {
        Ogre::Real timeDiff = time - mPrevTimeRecord;
        mPrevTimeRecord = time;

        Ogre::Real weightedTimeInc = 0.0f;

        mTimeIncWeightingRecord.push_back(timeDiff);

        const size_t timeWeightWindow = 10;

        while(mTimeIncWeightingRecord.size() > timeWeightWindow)
        {
            mTimeIncWeightingRecord.erase(mTimeIncWeightingRecord.begin());
        }

        for(size_t q = 0; q < mTimeIncWeightingRecord.size(); ++q)
        {
            weightedTimeInc += mTimeIncWeightingRecord[q];
        }
        weightedTimeInc /= mTimeIncWeightingRecord.size();
        mWeightedTimeRecord += weightedTimeInc;
        mWeightedTimeRecord -= (mWeightedTimeRecord - time) / 10.0f;

        mTimedPositions.push_back(std::make_pair(mWeightedTimeRecord, pos));
    }
}

GhostPos TrialGhost::getInterpolatedPoint(Ogre::Real time)
{
    GhostPos res;

    Ogre::Real timeDiff = time - mPrevTimePlayback;
    mPrevTimePlayback = time;

    Ogre::Real weightedTimeInc = 0.0f;

    mTimeIncWeighting.push_back(timeDiff);

    const size_t timeWeightWindow = 10;

    while(mTimeIncWeighting.size() > timeWeightWindow)
    {
        mTimeIncWeighting.erase(mTimeIncWeighting.begin());
    }

    for(size_t q = 0; q < mTimeIncWeighting.size(); ++q)
    {
        weightedTimeInc += mTimeIncWeighting[q];
    }
    weightedTimeInc /= mTimeIncWeighting.size();
    mWeightedTimePlayback += weightedTimeInc;
    mWeightedTimePlayback -= (mWeightedTimePlayback - time) / 10.0f;

    if(!mTimedPositionsPrev.empty())
    {
        res = mTimedPositionsPrev[0].second;

        for(size_t q = 0; q < mTimedPositionsPrev.size() - 1; ++q)
        {
            Ogre::Real timeA = mTimedPositionsPrev[q + 0].first;
            Ogre::Real timeB = mTimedPositionsPrev[q + 1].first;

            if(mWeightedTimePlayback >= timeA && mWeightedTimePlayback < timeB)
            {
                res = mTimedPositionsPrev[q + 0].second.lerp(mTimedPositionsPrev[q + 1].second, timeA, timeB, mWeightedTimePlayback);

                break;
            }
        }
    }

    return res;
}

bool TrialGhost::lapFinished(Ogre::Real bestTime, const std::string& dataDir)
{
    bool isBestBeaten = false;

    if(mGhostBestLapTime == 0.0f)
    {
        isBestBeaten = true;
        mGhostBestLapTime = bestTime;
        swapData();
        //save();
        saveBinToFile(dataDir);
    }
    else
    {
        if(mGhostBestLapTime > bestTime)
        {
            isBestBeaten = true;
            mGhostBestLapTime = bestTime;
            swapData();
            //save();
            saveBinToFile(dataDir);
        }
    }
    clearCurrent();
    mIsGhostVisible = true;

    return isBestBeaten;
}

void TrialGhost::swapData()
{
    mTimedPositionsPrev = mTimedPositions;
}

void TrialGhost::clearCurrent()
{
    mTimedPositions.clear();

    mTimeIncWeightingRecord.clear();
    mPrevTimeRecord = 0.0f;
    mWeightedTimeRecord = 0.0f;

    mTimeIncWeighting.clear();
    mPrevTimePlayback = 0.0f;
    mWeightedTimePlayback = 0.0f;
}

void TrialGhost::save()
{
    FILE * f = fopen("test.txt", "w");
    if(f)
    {
        fprintf(f, "%f\n", mGhostBestLapTime);
        fprintf(f, "%d\n", mTimedPositionsPrev.size());
        for(size_t q = 0; q < mTimedPositionsPrev.size(); ++q)
        {
            fprintf(f, "%f\n", mTimedPositionsPrev[q].first);
            fprintf(f, "%f %f %f\n", mTimedPositionsPrev[q].second.chassisPos.x, mTimedPositionsPrev[q].second.chassisPos.y, mTimedPositionsPrev[q].second.chassisPos.z);
            fprintf(f, "%f %f %f %f\n", mTimedPositionsPrev[q].second.chassisRot.x, mTimedPositionsPrev[q].second.chassisRot.y, mTimedPositionsPrev[q].second.chassisRot.z, mTimedPositionsPrev[q].second.chassisRot.w);
            for(size_t w = 0; w < InitialVehicleSetup::mWheelsAmount; ++w)
            {
                fprintf(f, "%f %f %f\n", mTimedPositionsPrev[q].second.wheelPos[w].x, mTimedPositionsPrev[q].second.wheelPos[w].y, mTimedPositionsPrev[q].second.wheelPos[w].z);
                fprintf(f, "%f %f %f %f\n", mTimedPositionsPrev[q].second.wheelRot[w].x, mTimedPositionsPrev[q].second.wheelRot[w].y, mTimedPositionsPrev[q].second.wheelRot[w].z, mTimedPositionsPrev[q].second.wheelRot[w].w);
            }
        }
        fclose(f);
    }
}

void TrialGhost::load()
{
    FILE * f = fopen("test.txt", "r");
    if(f)
    {
        mTimedPositionsPrev.clear();

        size_t amount = 0;
        fscanf(f, "%f\n", &mGhostBestLapTime);
        fscanf(f, "%d\n", &amount);

        for(size_t q = 0; q < amount; ++q)
        {
            GhostPos pos;
            Ogre::Real time;
            fscanf(f, "%f\n", &time);
            fscanf(f, "%f %f %f\n", &pos.chassisPos.x, &pos.chassisPos.y, &pos.chassisPos.z);
            fscanf(f, "%f %f %f %f\n", &pos.chassisRot.x, &pos.chassisRot.y, &pos.chassisRot.z, &pos.chassisRot.w);
            for(size_t w = 0; w < InitialVehicleSetup::mWheelsAmount; ++w)
            {
                fscanf(f, "%f %f %f\n", &pos.wheelPos[w].x, &pos.wheelPos[w].y, &pos.wheelPos[w].z);
                fscanf(f, "%f %f %f %f\n", &pos.wheelRot[w].x, &pos.wheelRot[w].y, &pos.wheelRot[w].z, &pos.wheelRot[w].w);
            }

            mTimedPositionsPrev.push_back(std::make_pair(time, pos));
        }
        fclose(f);
        mIsGhostVisible = true;
    }
}

void TrialGhost::saveBinToFile(const std::string& dataDir)
{
    SectionsFile sectionFile;
    sectionFile.load(dataDir, mFileName);
    sectionFile.updateSection(mTrackName, mGhostBestLapTime, mTimedPositionsPrev);
    sectionFile.save(dataDir, mFileName);
}

void TrialGhost::loadBinFromFile(const std::string& dataDir)
{
    SectionsFile sectionFile;
    sectionFile.load(dataDir, mFileName);
    if(sectionFile.isSectionExists(mTrackName))
    {
        mGhostBestLapTime = sectionFile.getSectionTime(mTrackName);
        mTimedPositionsPrev = sectionFile.getSectionData(mTrackName);
        mIsGhostVisible = true;
    }
}