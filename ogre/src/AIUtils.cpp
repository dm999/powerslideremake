#include "pcheader.h"

#include "AIUtils.h"

#include "OgreTools.h"

#include "PSAICar.h"

//#include "KochanekBartelsSpline.h"

AIUtils::AIUtils() :
        mIsDebugAI(false),
        mSpeedCoeff(1.0f),
        mAIDistanceLength(0.0f),
        mPrevPos(Ogre::Vector3::ZERO),
        mIsReverseEnabled(false)
{
}

void AIUtils::setAIData(const std::vector<AIData>& aiData, Ogre::SceneManager* sceneMgr, bool isDebugAI)
{
    //std::vector<Ogre::Vector3> splinePoints;

    mAIDistanceLength = 0.0f;
    mPrevPos = Ogre::Vector3::ZERO;
    mTimerAIStuck.reset();
    mIsReverseEnabled = false;

    mSpline.clear();

    for(size_t q = 0; q < aiData.size() - 1; ++q)
    {
        AIDataSegment segment;
        segment.posA = aiData[q + 0].pos;
        segment.posB = aiData[q + 1].pos;
        segment.dirA = aiData[q + 0].dir;
        segment.dirB = aiData[q + 1].dir;
        segment.segmentLength = segment.posA.distance(segment.posB);
        mAIDataSegments.push_back(segment);

        //splinePoints.push_back(aiData[q].pos);
        mSpline.addPoint(aiData[q].pos);

    }

    //splinePoints.push_back(aiData[aiData.size() - 1].pos);
    //splinePoints.push_back(aiData[0].pos);
    mSpline.addPoint(aiData[aiData.size() - 1].pos);
    mSpline.addPoint(aiData[0].pos);

    //mSpline.reset(new KochanekBartelsSpline(splinePoints));

    mPrevClosestSegmentInited = false;
    mPrevClosestSegmentIndex = 0;

    mDebugSphereNode = NULL;

    mIsDebugAI = isDebugAI;

    if(mIsDebugAI)
    {
        Ogre::Real scale = 0.2f;
        Ogre::Entity * debugSphere = sceneMgr->createEntity(Ogre::SceneManager::PT_SPHERE);
        debugSphere->setMaterialName("BaseWhiteNoLighting");
        mDebugSphereNode = sceneMgr->getRootSceneNode()->createChildSceneNode();
        mDebugSphereNode->attachObject(debugSphere);
        mDebugSphereNode->setPosition(aiData[0].pos);
        mDebugSphereNode->setScale(scale, scale, scale);
        debugSphere->setCastShadows(false);
    }
}

void AIUtils::clear()
{
    mAIDataSegments.clear();
}

void AIUtils::performAICorrection(PSAICar* aiCar, bool isGamePaused)
{

    if(isGamePaused)
    {
        mTimerAIStuck.reset();
    }

    //steering
    Ogre::Vector3 carDir = aiCar->getForwardAxis();
    Ogre::Vector3 carDirOriginal = carDir;
    carDir.y = 0.0f;
    Ogre::Vector3 carPos = aiCar->getModelNode()->getPosition();
    
    Ogre::Real lastDistance = carDir.dotProduct(carPos - mPrevPos);
    mPrevPos = carPos;
    mAIDistanceLength += Ogre::Math::Abs(lastDistance);

    //size_t towardPointIndex;
    Ogre::Vector3 towardDir;
    Ogre::Vector3 towardPoint = getTowardPoint(carPos, towardDir);

    Ogre::Real speedCoeff = carDirOriginal.dotProduct(towardDir);
    speedCoeff = Ogre::Math::Clamp(speedCoeff, 0.2f, 1.0f);

    if(mIsDebugAI)
    {
        mDebugSphereNode->setPosition(towardPoint);
    }
    //Ogre::Vector3 dirToPointHint = mAIData[towardPointIndex].dir;
    //dirToPointHint.y = 0.0f;

    Ogre::Vector3 dirToPoint = towardPoint - carPos;
    dirToPoint.normalise();
    dirToPoint.y = 0.0f;
    //dirToPoint = (dirToPoint + dirToPointHint) / 2.0f;

    Ogre::Real proj = dirToPoint.dotProduct(carDir);

    aiCar->setSteerLeft(false);
    aiCar->setSteerRight(false);


    //left or right
    if(proj < 0.99f)
    {
        Ogre::Vector3 dirToPointProj = dirToPoint;
        dirToPointProj.y = 0.0f;

        Ogre::Real steerAngle = GetSignedAngle(carDir, dirToPoint);
        Ogre::Real steerImpulse = Ogre::Math::Abs(steerAngle) / 2.0f;
        steerImpulse = Ogre::Math::Clamp(steerImpulse, 0.0f, 30.0f);

        if(steerAngle < 0.0f)
        {
            aiCar->setSteeringUmpulse(steerImpulse);
            aiCar->setSteerLeft(true);
        }
        else
        {
            aiCar->setSteeringUmpulse(steerImpulse);
            aiCar->setSteerRight(true);
        }
    }


    //speed
    Ogre::Real aiSpeed = aiCar->getAlignedVelocity();
    //Ogre::Real aiSpeed = aiCar->getLinearVelocity().length();

    aiCar->setAcceleration(false);
    aiCar->setBrake(false);

    const Ogre::Real stuckDistanceLimit = 20.0f;
    const unsigned long stuckTimeLimit = 2000; //ms
    const unsigned long reverseTimeLimit = 2000; //ms

    if(mTimerAIStuck.getMilliseconds() > stuckTimeLimit && !mIsReverseEnabled)
    {
        if(mAIDistanceLength < stuckDistanceLimit)
        {
            mIsReverseEnabled = true;
            mTimerReverse.reset();
        }
        mTimerAIStuck.reset();
        mAIDistanceLength = 0.0f;
    }

    if(!mIsReverseEnabled)
    {
        if(aiSpeed >100.0f && speedCoeff < 0.5f)
        {
            aiCar->setBrake(true);
        }

        if(aiSpeed < (200.0f * mSpeedCoeff/* * speedCoeff*/))
        {
            aiCar->setAcceleration(true);
        }
    }

    if(mIsReverseEnabled)
    {
        aiCar->setBrake(true);

        if(mTimerReverse.getMilliseconds() > reverseTimeLimit)
        {
            mIsReverseEnabled = false;
            mTimerAIStuck.reset();
        }
    }

}

Ogre::Vector3 AIUtils::getTowardPoint(const Ogre::Vector3& carPos, Ogre::Vector3& towardDir)
{
    Ogre::Vector3 res;

    Ogre::Vector3 pointInLine;
    size_t segment = getClosestSegment(carPos, pointInLine);
    Ogre::Real distToStart = pointInLine.distance(mAIDataSegments[segment].posA);

    Ogre::Real frac = distToStart / mAIDataSegments[segment].segmentLength;

    //Ogre::Vector3 segmentDir = mAIDataSegments[segment].posB - mAIDataSegments[segment].posA;
    //segmentDir.normalise();
    //res = mAIDataSegments[segment].posA + segmentDir * segmentLength * frac;

    size_t nextSegment = segment;

    const Ogre::Real distFromCar = 0.5f;
    if(frac > (1.0f - distFromCar))
    {
        ++nextSegment;
        if(nextSegment == mAIDataSegments.size())
        {
            nextSegment = 0;
        }
        frac -= (1.0f - distFromCar);
    }
    else
    {
        frac += distFromCar;
    }


    //res = mSpline->ComputePoint(frac, nextSegment, nextSegment + 1);
    res = mSpline.interpolate(nextSegment, frac);

    towardDir = mAIDataSegments[nextSegment].dirB;

    /*
    Ogre::Vector3 nextSegmentDir = mAIDataSegments[nextSegment].posB - mAIDataSegments[nextSegment].posA;
    nextSegmentDir.normalise();
    Ogre::Real nextSegmentLength = mAIDataSegments[nextSegment].segmentLength;
    res = mAIDataSegments[nextSegment].posA + nextSegmentDir * nextSegmentLength * frac;*/

    return res;
}

size_t AIUtils::getClosestSegment(const Ogre::Vector3& carPos, Ogre::Vector3& pointInLineRes)
{
    size_t res = 0;

    Ogre::Real minDist = 10000.0f;

    if(!mPrevClosestSegmentInited)
    {
        for(size_t q = 0; q < mAIDataSegments.size(); ++q)
        {
            Ogre::Vector3 pointInLine = ProjectPointOnLine(carPos, mAIDataSegments[q].posA, mAIDataSegments[q].posB);
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

        int dataSegmentsLength = mAIDataSegments.size();

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
            Ogre::Vector3 pointInLine = ProjectPointOnLine(carPos, mAIDataSegments[closestRange[q]].posA, mAIDataSegments[closestRange[q]].posB);
            Ogre::Real dist = pointInLine.distance(carPos);

            if(dist < minDist)
            {
                pointInLineRes = pointInLine;
                minDist = dist;
                res = closestRange[q];
            }
        }
    }
    
    mPrevClosestSegmentIndex = res;

    return res;
}

void AIUtils::raceStarted()
{
    mTimerAIStuck.reset();
}

#if 0
Ogre::Vector3 AIUtils::getTowardPoint(const Ogre::Vector3& carPos, size_t& towardPointIndex)const
{

    // ---prev-----[car]-------cur---------ahead----

    size_t indexPrev, indexCur, indexAhead;
    getClosestSegment(carPos, indexPrev, indexCur, indexAhead);

    Ogre::Real curSegmentLength = mAIData[indexPrev].pos.distance(mAIData[indexCur].pos);
    Ogre::Real aheadSegmentLength = mAIData[indexCur].pos.distance(mAIData[indexAhead].pos);

    Ogre::Vector3 prevPos = mAIData[indexPrev].pos;
    Ogre::Vector3 curPos = mAIData[indexCur].pos;
    Ogre::Vector3 aheadPos = mAIData[indexAhead].pos;

    Ogre::Real toPrevPosDist = prevPos.distance(carPos);
    Ogre::Real toCurPosDist = curPos.distance(carPos);
    Ogre::Real toAheadPosDist = aheadPos.distance(carPos);


    Ogre::Real fracToCurSegment = toCurPosDist / toPrevPosDist;
    Ogre::Real fracToAheadSegment = toCurPosDist / toAheadPosDist;

    const Ogre::Real switchThreshold = 1.0f; // more far
    //const Ogre::Real switchThreshold = 0.5f;

    if(fracToCurSegment < switchThreshold)
    {
        towardPointIndex = indexAhead;
    }
    else
    {
        towardPointIndex = indexCur;
    }

    return mAIData[towardPointIndex].pos;
}

void AIUtils::getClosestSegment(const Ogre::Vector3& carPos, size_t& indexPrev, size_t& indexCur, size_t& indexAhead)const
{
    Ogre::Real minDist = 100000.0f;
    size_t minIndex = 0;

    for(size_t q = 0; q < mAIData.size(); ++q)
    {
        Ogre::Real dist = carPos.distance(mAIData[q].pos);
        if(dist < minDist)
        {
            minDist = dist;
            minIndex = q;
        }
    }

    size_t minIndexAhead = minIndex + 1;
    int minIndexPrev = minIndex - 1;

    if(minIndexAhead == mAIData.size()) minIndexAhead = 0;
    if(minIndexPrev < 0) minIndexPrev = mAIData.size() - 1;

    indexPrev = minIndexPrev;
    indexCur = minIndex;
    indexAhead = minIndexAhead;
}
#endif
