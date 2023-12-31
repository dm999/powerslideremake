
#ifndef LAPUTILS_H
#define LAPUTILS_H

#include "../includes/OgreInclude.h"
#include "../tools/PausedTimer.h"

struct LLTDataSegment
{
    Ogre::Vector3 posA, posB;
    Ogre::Real segmentLength;
};

class LapUtils
{
public:

    class Events
    {
    public:
        virtual void onLapFinished() = 0;
    };

    LapUtils();

    void checkCheckPoints(const Ogre::Vector3& carPos, bool isSpeedCheatsUsed);

    Ogre::Quaternion getArrowOrientation(const Ogre::Vector3& carPos, Ogre::Vector3 carDir)const;

    Ogre::Real getTotalTime()const {return mTotalTime;}
    Ogre::Real getLastLapTime()const {return mLastLapTime;}
    Ogre::Real getLapTime()const {return mLapTime;}
    Ogre::Real getBestLapTime()const {return mBestLapTime;}
    Ogre::Real getLapPosition()const {return mLapPosition;}
    size_t getCurrentLap()const{return mCurrentLap;}

    void setData(   const std::vector<Ogre::Vector3>& pos, 
                    const std::vector<Ogre::Vector3>& dirs, 
                    const std::vector<Ogre::Real>& weights, 
                    const std::vector<bool>& ischecks,
                    Ogre::SceneManager* sceneMgr,
                    bool isLuge,
                    bool isDebug);

    void resetLapTimer(){mLapTimer.reset(); mAfterFinishLinePassTimer.reset();}
    void pauseLapTimer(){mLapTimer.pause();}
    void resumeLapTimer(){mLapTimer.resume();}
    const PausedTimer& getLapTimer() const{return mLapTimer;}//for time trial

    Ogre::Real getAfterFinishLinePassTime();

    void setEvents(Events* events){mEvents = events;}

private:

    bool mIsDebugLLT;
    Ogre::SceneNode * mDebugSphereNode;

    //size_t findLapPosition(const Ogre::Vector3& carPos);
    void calcLapTime(size_t minIndex, bool isSpeedCheatsUsed);

    size_t getClosestSegment(const Ogre::Vector3& carPos, Ogre::Vector3& pointInLineRes);
    size_t getRelativeClosestSegment(const Ogre::Vector3& carPos);

    PausedTimer mLapTimer;

    Ogre::Timer mAfterFinishLinePassTimer;

    Ogre::Real mTotalTime;
    Ogre::Real mLastLapTime;
    Ogre::Real mLapTime;
    Ogre::Real mBestLapTime;
    Ogre::Real mLapPosition;
    bool mIsLapPosInited;

    size_t mCurrentLap;

    std::vector<Ogre::Vector3> mPositions;
    std::vector<Ogre::Vector3> mDirs;
    std::vector<Ogre::Real> mWeights;
    std::vector<bool> mIsCheckpoints;

    bool mPrevClosestSegmentInited;
    size_t mPrevClosestSegmentIndex;
    std::vector<LLTDataSegment> mLLTDataSegments;

    std::vector<bool> mIsPassed;

    Ogre::SimpleSpline mSpline;
    Ogre::Vector3 mAheadPointForArrow;
    Ogre::SceneNode * mDebugSphereArrowNode;

    bool mIsLuge;

    Events* mEvents;

};

#endif