
#ifndef LAPUTILS_H
#define LAPUTILS_H

#include "OgreInclude.h"
#include "PausedTimer.h"

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

    void checkCheckPoints(const Ogre::Vector3& carPos);

    Ogre::Quaternion getArrowOrientation(const Ogre::Vector3& carPos, Ogre::Vector3 carDir)const;

    Ogre::Real getLastLapTime()const {return mLastLapTime;}
    Ogre::Real getLapTime()const {return mLapTime;}
    Ogre::Real getLapPosition()const {return mLapPosition;}
    size_t getCurrentLap()const{return mCurrentLap;}

    void setData(   const std::vector<Ogre::Vector3>& pos, 
                    const std::vector<Ogre::Vector3>& dirs, 
                    const std::vector<Ogre::Real>& weights, 
                    const std::vector<bool>& ischecks,
                    Ogre::SceneManager* sceneMgr,
                    bool isDebug);

    void resetLapTimer(){mLapTimer.reset(); mAfterFinishLinePassTimer.reset();}
    void pauseLapTimer(){mLapTimer.pause();}
    void resumeLapTimer(){mLapTimer.resume();}

    Ogre::Real getAfterFinishLinePassTime();

    void setEvents(Events* events){mEvents = events;}

private:

    bool mIsDebugLLT;
    Ogre::SceneNode * mDebugSphereNode;

    //size_t findLapPosition(const Ogre::Vector3& carPos);
    void calcLapTime(size_t minIndex);

    size_t getClosestSegment(const Ogre::Vector3& carPos, Ogre::Vector3& pointInLineRes);

    PausedTimer mLapTimer;

    Ogre::Timer mAfterFinishLinePassTimer;

    Ogre::Real mLastLapTime;
    Ogre::Real mLapTime;
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

    Events* mEvents;

};

#endif