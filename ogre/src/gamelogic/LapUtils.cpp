

#include "LapUtils.h"

#include "../tools/OgreTools.h"

LapUtils::LapUtils() : 
    mIsDebugLLT(false),
    mTotalTime(0.0f), mLastLapTime(0.0f), mLapTime(0.0f),
    mIsLapPosInited(false),
    mLapPosition(0.0f),
    mCurrentLap(0),
    mEvents(NULL)
{
}

void LapUtils::setData( const std::vector<Ogre::Vector3>& pos, 
                        const std::vector<Ogre::Vector3>& dirs, 
                        const std::vector<Ogre::Real>& weights, 
                        const std::vector<bool>& ischecks,
                        Ogre::SceneManager* sceneMgr,
                        bool isDebug)
{
    mIsDebugLLT = isDebug;

    mPositions = pos;
    mDirs = dirs;
    mWeights = weights;
    mIsCheckpoints = ischecks;

    mSpline.clear();

    mLLTDataSegments.clear();
    for(size_t q = 0; q < pos.size() - 1; ++q)
    {
        LLTDataSegment segment;
        segment.posA = pos[q + 0];
        segment.posB = pos[q + 1];
        segment.segmentLength = segment.posA.distance(segment.posB);
        mLLTDataSegments.push_back(segment);

        mSpline.addPoint(segment.posA);
    }
    {
        LLTDataSegment segment;
        segment.posA = pos[pos.size() - 1];
        segment.posB = pos[0];
        segment.segmentLength = segment.posA.distance(segment.posB);
        mLLTDataSegments.push_back(segment);

        mSpline.addPoint(segment.posA);
        mSpline.addPoint(segment.posB);
    }
    mPrevClosestSegmentInited = false;
    mPrevClosestSegmentIndex = 0;

    mIsPassed.resize(mIsCheckpoints.size());
    std::fill(mIsPassed.begin(), mIsPassed.end(), false);

    assert(mPositions.size() == mDirs.size());
    assert(mIsCheckpoints.size() == mDirs.size());
    assert(mWeights.size() == mDirs.size());
    assert(!mPositions.empty());

    mLapTimer.reset();
    mTotalTime = 0.0f;
    mLastLapTime = 0.0f;
    mLapTime = 0.0f;
    mLapPosition = 0.0f;
    mIsLapPosInited = false;
    mCurrentLap = 0;

    if(mIsDebugLLT)
    {
        Ogre::Real scale = 0.2f;
        Ogre::Entity * debugSphere = sceneMgr->createEntity(Ogre::SceneManager::PT_SPHERE);
        debugSphere->setMaterialName("BaseWhiteNoLighting");
        mDebugSphereNode = sceneMgr->getRootSceneNode()->createChildSceneNode();
        mDebugSphereNode->attachObject(debugSphere);
        mDebugSphereNode->setPosition(mPositions[0]);
        mDebugSphereNode->setScale(scale, scale, scale);
        debugSphere->setCastShadows(false);

        Ogre::Entity * debugSphereArrow = sceneMgr->createEntity(Ogre::SceneManager::PT_SPHERE);
        debugSphereArrow->setMaterialName("BaseWhiteNoLighting");
        mDebugSphereArrowNode = sceneMgr->getRootSceneNode()->createChildSceneNode();
        mDebugSphereArrowNode->attachObject(debugSphereArrow);
        mDebugSphereArrowNode->setPosition(mPositions[0]);
        mDebugSphereArrowNode->setScale(scale, scale, scale);
        debugSphereArrow->setCastShadows(false);
        
    }
}

void LapUtils::checkCheckPoints(const Ogre::Vector3& carPos)
{
    //size_t minIndex = findLapPosition(carPos);
    Ogre::Vector3 pointInLine;
    size_t segment = getClosestSegment(carPos, pointInLine);

    Ogre::Real distToStartPointInSegment = pointInLine.distance(mLLTDataSegments[segment].posA);
    Ogre::Real frac = distToStartPointInSegment / mLLTDataSegments[segment].segmentLength;

    size_t nextSegment = segment + 1;
    size_t nextnextSegment = segment + 4;

    if(nextSegment >= mWeights.size())
    {
        nextSegment = 0;
    }
    if(nextnextSegment >= mWeights.size())
    {
        nextnextSegment = nextnextSegment - mWeights.size();
    }
    
    if(segment != 0)
    {
        Ogre::Real nextLapPos = mWeights[segment] + (mWeights[nextSegment] - mWeights[segment]) * frac;
        if(Ogre::Math::Abs(nextLapPos - mLapPosition) < 0.5f || nextLapPos < 0.2f || !mIsLapPosInited)
        {
            mLapPosition = nextLapPos;
            mIsLapPosInited = true;
        }
    }
    else
    {
        Ogre::Real nextLapPos = mWeights[nextSegment] * frac;
        if(Ogre::Math::Abs(nextLapPos - mLapPosition) < 0.5f || nextLapPos < 0.2f || !mIsLapPosInited)
        {
            mLapPosition = nextLapPos;
            mIsLapPosInited = true;
        }
    }

    mAheadPointForArrow = mSpline.interpolate(nextnextSegment, frac);

    calcLapTime(segment);

    if(mIsDebugLLT)
    {
        //mDebugSphereNode->setPosition(mPositions[minIndex]);
        mDebugSphereNode->setPosition(pointInLine);
        mDebugSphereArrowNode->setPosition(mAheadPointForArrow);
    }
}

#if 0
size_t LapUtils::findLapPosition(const Ogre::Vector3& carPos)
{
    Ogre::Real minDist = 100000.0f;
    size_t minIndex = 0;

    Ogre::Real minDist2 = 100000.0f;
    size_t minIndex2 = 0;

    for(size_t q = 0; q < mPositions.size(); ++q)
    {
        Ogre::Real dist = carPos.distance(mPositions[q]);
        if(dist < minDist)
        {
            minDist2 = minDist;
            minDist = dist;
            minIndex2 = minIndex;
            minIndex = q;
        }
    }

    Ogre::Real frac = minDist / minDist2;

    mLapPosition = mWeights[minIndex] + (mWeights[minIndex2] - mWeights[minIndex]) * frac;

    if(mLapPosition > 0.9999f)
        mLapPosition = 0.9999f;

    return minIndex;
}
#endif

void LapUtils::calcLapTime(size_t minIndex)
{
    mLapTime = mLapTimer.getMilliseconds() / 1000.0f;

    bool previousPassed = true;

    if(minIndex > 0)
    {
        for(size_t q = 0; q < minIndex - 1; ++q)
        {
            if(mIsCheckpoints[q])
            {
                if(!mIsPassed[q])
                {
                    previousPassed = false;
                    break;
                }
            }
        }
    }

    if(previousPassed)
        mIsPassed[minIndex] = true;

    bool isAllPassed = true;

    for(size_t q = 0; q < mIsPassed.size(); ++q)
    {
        if(mIsCheckpoints[q])
        {
            if(!mIsPassed[q])
            {
                isAllPassed = false;
                break;
            }
        }
    }

    if(isAllPassed && minIndex == 0)
    {
        std::fill(mIsPassed.begin(), mIsPassed.end(), false);

        mLapTimer.reset();
        mAfterFinishLinePassTimer.reset();

        mLastLapTime = mLapTime;
        mTotalTime += mLastLapTime;

        ++mCurrentLap;

        if(mEvents)
        {
            mEvents->onLapFinished();
        }
    }

    if(mCurrentLap == 0 && minIndex == 0)
    {
        ++mCurrentLap;
    }
}

size_t LapUtils::getClosestSegment(const Ogre::Vector3& carPos, Ogre::Vector3& pointInLineRes)
{
    size_t res = 0;

    Ogre::Real minDist = 10000.0f;

    if(!mPrevClosestSegmentInited)
    {
        for(size_t q = 0; q < mLLTDataSegments.size(); ++q)
        {
            Ogre::Vector3 pointInLine = ProjectPointOnLine(carPos, mLLTDataSegments[q].posA, mLLTDataSegments[q].posB);
            Ogre::Real dist = pointInLine.distance(carPos);

            if(dist < minDist)
            {
                pointInLineRes = pointInLine;
                minDist = dist;
                res = q;
            }
        }

        mPrevClosestSegmentInited = true;
    }
    else
    {
        const int range = 5;
        const size_t rangeAmount = range * 2;
        std::vector<size_t> closestRange(rangeAmount);

        int dataSegmentsLength = mLLTDataSegments.size();

        size_t index = 0;
        for(int q = -range; q < range; ++q, ++index)
        {
            int rangeIndex = mPrevClosestSegmentIndex + q;
            
            if(rangeIndex < 0)
            {
                rangeIndex = dataSegmentsLength + rangeIndex;
            }
            if(rangeIndex >= dataSegmentsLength)
            {
                rangeIndex = rangeIndex - dataSegmentsLength;
            }

            closestRange[index] = rangeIndex;
        }

        for(size_t q = 0; q < rangeAmount; ++q)
        {
            Ogre::Vector3 pointInLine = ProjectPointOnLine(carPos, mLLTDataSegments[closestRange[q]].posA, mLLTDataSegments[closestRange[q]].posB);
            Ogre::Real dist = pointInLine.distance(carPos);

            if(dist < minDist)
            {
                pointInLineRes = pointInLine;
                minDist = dist;
                res = closestRange[q];
            }
        }

        //if we are too far away from prev point run global search
        if(minDist > 100.0f)
        {
            mPrevClosestSegmentInited = false;
        }
    }

    mPrevClosestSegmentIndex = res;

    return res;
}

/**
 * should be called after checkCheckPoints
 */
Ogre::Quaternion LapUtils::getArrowOrientation(const Ogre::Vector3& carPos, Ogre::Vector3 carDir)const
{
    Ogre::Quaternion res;

    Ogre::Vector3 dirToPoint = mAheadPointForArrow - carPos;
    dirToPoint.normalise();
    dirToPoint.y = 0.0f;

    carDir.y = 0.0f;

    Ogre::Real rotAngle = GetSignedAngle(carDir, dirToPoint);

    res.FromAngleAxis(Ogre::Degree(-rotAngle), Ogre::Vector3::UNIT_Y);

    return res;
}

Ogre::Real LapUtils::getAfterFinishLinePassTime()
{
    return mAfterFinishLinePassTimer.getMilliseconds() / 1000.0f;
}