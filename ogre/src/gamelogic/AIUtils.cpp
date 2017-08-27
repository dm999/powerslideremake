
#include "AIUtils.h"

#include "../tools/OgreTools.h"
#include "../tools/Tools.h"

#include "../pscar/PSAICar.h"

//#include "KochanekBartelsSpline.h"

namespace
{
    const float aiMaxSpeed = 300.0f;
}

AIUtils::AIUtils() :
        mIsDebugAI(false),
        mSpeedCoeff(1.0f),
        mAIDistanceLength(0.0f),
        mPrevPos(Ogre::Vector3::ZERO),
        mIsReverseEnabled(false)
{
}

void AIUtils::setAIData(const AIWhole& aiWhole, Ogre::SceneManager* sceneMgr, bool isDebugAI)
{
    mAIWhole = aiWhole;

    mField17 = Ogre::Vector3::ZERO;

    //std::vector<Ogre::Vector3> splinePoints;

    mAIDistanceLength = 0.0f;
    mPrevPos = Ogre::Vector3::ZERO;
    mTimerAIStuck.reset();
    mIsReverseEnabled = false;

    mSpline.clear();

    for(size_t q = 0; q < aiWhole.aiData.size() - 1; ++q)
    {
        AIDataSegment segment;
        segment.posA = aiWhole.aiData[q + 0].pos;
        segment.posB = aiWhole.aiData[q + 1].pos;
        segment.tangentA = aiWhole.aiData[q + 0].tangent;
        segment.tangentB = aiWhole.aiData[q + 1].tangent;
        segment.magicA = aiWhole.aiData[q + 0].magic;
        segment.magicB = aiWhole.aiData[q + 1].magic;
        segment.segmentLength = segment.posA.distance(segment.posB);
        mAIDataSegments.push_back(segment);

        //splinePoints.push_back(aiData[q].pos);
        mSpline.addPoint(aiWhole.aiData[q].pos);

    }

    {
        AIDataSegment segment;
        segment.posA = aiWhole.aiData[aiWhole.aiData.size() - 1].pos;
        segment.posB = aiWhole.aiData[0].pos;
        segment.tangentA = aiWhole.aiData[aiWhole.aiData.size() - 1].tangent;
        segment.tangentB = aiWhole.aiData[0].tangent;
        segment.magicA = aiWhole.aiData[aiWhole.aiData.size() - 1].magic;
        segment.magicB = aiWhole.aiData[0].magic;
        segment.segmentLength = segment.posA.distance(segment.posB);
        mAIDataSegments.push_back(segment);
    }

    //splinePoints.push_back(aiWhole.aiData[aiData.size() - 1].pos);
    //splinePoints.push_back(aiWhole.aiData[0].pos);
    mSpline.addPoint(aiWhole.aiData[aiWhole.aiData.size() - 1].pos);
    mSpline.addPoint(aiWhole.aiData[0].pos);

    //original data is left hand, convert for AI
    for(size_t q = 0; q < aiWhole.aiData.size(); ++q)
    {
        mAIWhole.aiData[q].pos.z = -mAIWhole.aiData[q].pos.z;
        mAIWhole.aiData[q].tangent.z = -mAIWhole.aiData[q].tangent.z;
        mAIWhole.aiData[q].magic.z = -mAIWhole.aiData[q].magic.z;
    }

    //mSpline.reset(new KochanekBartelsSpline(splinePoints));

    mPrevClosestSegmentInited = false;
    mPrevClosestSegmentIndex = 0;

    mDebugSphereNode = NULL;

    mIsDebugAI = isDebugAI;

    if(mIsDebugAI)
    {
        Ogre::Real scale = 0.02f;
        Ogre::Entity * debugSphere = sceneMgr->createEntity(Ogre::SceneManager::PT_SPHERE);
        debugSphere->setMaterialName("BaseWhiteNoLighting");
        mDebugSphereNode = sceneMgr->getRootSceneNode()->createChildSceneNode();
        mDebugSphereNode->attachObject(debugSphere);
        mDebugSphereNode->setPosition(aiWhole.aiData[0].pos);
        mDebugSphereNode->setScale(scale, scale, scale);
        debugSphere->setCastShadows(false);
    }
}

void AIUtils::clear()
{
    mAIDataSegments.clear();
}

void AIUtils::performAICorrection(PSAICar* aiCar, bool isRaceStarted, bool isGamePaused)
{

    calcFeatures(aiCar);
    float steeringVal = inference();

    if(isRaceStarted)
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

            if(aiSpeed < (aiMaxSpeed * mSpeedCoeff/* * speedCoeff*/))
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
    //res = mSpline.interpolate(nextSegment, frac);
    res = Ogre::Math::lerp(mAIDataSegments[nextSegment].posA, mAIDataSegments[nextSegment].posB, frac);

    towardDir = mAIDataSegments[nextSegment].tangentB;

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


void AIUtils::calcFeatures(PSAICar* aiCar)
{
    const float psCarMass = 45.0f;
    const float psInvCarMass = 1.0f / psCarMass;


    float feature3 = mAIWhole.slotMatrix[18][0];

    if(mAIWhole.hackType == 1)feature3 += 1.0f;
    else feature3 += 0.25f;

    float feature4 = mAIWhole.slotMatrix[19][0] + 1.0f;

    Ogre::Vector3 carPos = aiCar->getModelNode()->getPosition();
    carPos.z = -carPos.z;//original data is left hand

    Ogre::Matrix3 carRot;
    aiCar->getModelNode()->getOrientation().ToRotationMatrix(carRot);
    Ogre::Vector3 carRotV[3];//original data is left hand
    carRotV[0] = Ogre::Vector3(carRot[0][0], carRot[1][0], -carRot[2][0]);
    carRotV[1] = Ogre::Vector3(carRot[0][1], carRot[1][1], -carRot[2][1]);
    carRotV[2] = Ogre::Vector3(-carRot[0][2], -carRot[1][2], carRot[2][2]);

    Ogre::Vector3 carLinearForce = aiCar->getLinearForce();
    carLinearForce.z = -carLinearForce.z;//original data is left hand


    size_t closestSplineIndex = getClosestSplinePoint(carPos);

    float frac = 0.0f;
    size_t splineFracIndex = getFracIndex(closestSplineIndex, carPos, frac);

    SplineFeatures splineFeatures = getSplineFeatures(
        carPos, 
        frac, splineFracIndex, 
        fabs(feature3), fabs(feature4)
        );

    if(mAIWhole.hackType == 1)
    {
        mAIWhole.slotMatrix[2][0]   = splineFeatures.out10;
        mAIWhole.slotMatrix[3][0]   = splineFeatures.out11;
        mAIWhole.slotMatrix[8][0]   = carRotV[0].dotProduct(carLinearForce) * psInvCarMass;
        mAIWhole.slotMatrix[9][0]   = carRotV[1].dotProduct(carLinearForce) * psInvCarMass;
        mAIWhole.slotMatrix[10][0]  = carRotV[2].dotProduct(carLinearForce) * psInvCarMass;
    }
    else
    {
        //d.polubotko: TODO
    }

    mAIWhole.slotMatrix[0][0] = atan2(carRotV[2].dotProduct(splineFeatures.out6), carRotV[0].dotProduct(splineFeatures.out6)) * splineFeatures.out8;
    mAIWhole.slotMatrix[1][0] = atan2(carRotV[2].dotProduct(splineFeatures.out7), carRotV[0].dotProduct(splineFeatures.out7)) * splineFeatures.out9;
    
    float v34 = 1.0f / sqrt(carRotV[2].z * carRotV[2].z + (-carRotV[2].x * -carRotV[2].x));
    float v57 = v34 * carRotV[2].z;
    float v59 = v34 * -carRotV[2].x;
    mAIWhole.slotMatrix[4][0] = carRotV[1].x * v57 + carRotV[1].z * v59;
    mAIWhole.slotMatrix[5][0] = carRotV[2].y;
    mAIWhole.slotMatrix[5][0] = carRotV[1].y;
    mAIWhole.slotMatrix[7][0] = carRotV[2].dotProduct(mField17) * 15.0f;

    mAIWhole.slotMatrix[11][0] = 1.0f;
    mAIWhole.slotMatrix[12][0] = Tools::randomSmallValue();

    if(false)
    {
        //d.polubotko: TODO overtake
    }
    else
    {
        mAIWhole.slotMatrix[13][0] = 0.0f;
        mAIWhole.slotMatrix[14][0] = 0.0f;
        mAIWhole.slotMatrix[15][0] = 0.0f;
    }

    mField17 = carRotV[0];
}

float AIUtils::inference()
{
    float res = 0.0f;

    return res;
}

size_t AIUtils::getClosestSplinePoint(const Ogre::Vector3& carPos) const
{
    size_t ret = 0;

    float minDist = std::numeric_limits<float>::max();
    for(size_t q = 0; q < mAIWhole.aiData.size(); ++q)
    {
        float dist = carPos.distance(mAIWhole.aiData[q].pos);
        if(dist < minDist)
        {
            ret = q;
            minDist = dist;
        }
    }

    return ret;
}

size_t AIUtils::getFracIndex(size_t closestSplineIndex, const Ogre::Vector3& carPos, float & frac) const
{
    size_t ret = 0;

    size_t closestSplineIndexPrev = closestSplineIndex - 1;
    if(closestSplineIndex == 0) closestSplineIndexPrev = mAIWhole.aiData.size() - 1;

    size_t closestSplineIndexNext = closestSplineIndex + 1;
    if(closestSplineIndexNext >= mAIWhole.aiData.size()) closestSplineIndexNext = 0;


    Ogre::Vector3 diffClosest = (carPos - mAIWhole.aiData[closestSplineIndex].pos) * mAIWhole.aiData[closestSplineIndex].tangent;
    float diffClosestVal = diffClosest.x + diffClosest.y + diffClosest.z;

    if(diffClosestVal < 0.0f)
    {
        Ogre::Vector3 diffPrev = (carPos - mAIWhole.aiData[closestSplineIndexPrev].pos) * mAIWhole.aiData[closestSplineIndexPrev].tangent;
        float diffPrevVal = diffPrev.x + diffPrev.y + diffPrev.z;
        if(diffPrevVal >= 0.0f)
        {
            ret = closestSplineIndexPrev;
            frac = diffPrevVal / (diffPrevVal - diffClosestVal);
        }
        else
        {
            ret = closestSplineIndexPrev;
            frac = 0.0f;
        }
    }
    else
    {
        Ogre::Vector3 diffNext = (carPos - mAIWhole.aiData[closestSplineIndexNext].pos) * mAIWhole.aiData[closestSplineIndexNext].tangent;
        float diffNextVal = diffNext.x + diffNext.y + diffNext.z;
        if(diffNextVal <= 0.0f)
        {
            ret = closestSplineIndex;
            frac = diffClosestVal / (diffClosestVal - diffNextVal);
        }
        else
        {
            ret = closestSplineIndexNext;
            frac = 0.0f;
        }
    }

    return ret;
}

AIUtils::SplineFeatures AIUtils::getSplineFeatures(
    const Ogre::Vector3& carPos, 
    float frac, size_t fracIndex, 
    float feature3, float feature4) const
{
    SplineFeatures ret;

    if(mAIWhole.hack1 > 0.0 || mAIWhole.hack2 > 0.0)
    {
        //d.polubotko: TODO physics hack
    }

    float f3Frac = feature3 + frac;
    float f4Frac = feature4 + frac;

    double integer;
    float f3FracMod = static_cast<float>(modf(f3Frac, &integer));
    size_t f3FracInt = static_cast<size_t>(integer);

    float f4FracMod = static_cast<float>(modf(f4Frac, &integer));
    size_t f4FracInt = static_cast<size_t>(integer);

    float f3FracModSquare = f3FracMod * f3FracMod;
    float f3FracModQube = f3FracMod * f3FracMod * f3FracMod;

    float f4FracModSquare = f4FracMod * f4FracMod;
    float f4FracModQube = f4FracMod * f4FracMod * f3FracMod;

    const size_t splinePoints = mAIWhole.aiData.size();

    const AIData& f3p1 = mAIWhole.aiData[(fracIndex + f3FracInt + splinePoints - 1) % splinePoints];
    const AIData& f3p2 = mAIWhole.aiData[(fracIndex + f3FracInt) % splinePoints];
    const AIData& f3p3 = mAIWhole.aiData[(fracIndex + f3FracInt + 1) % splinePoints];
    const AIData& f3p4 = mAIWhole.aiData[(fracIndex + f3FracInt + 2) % splinePoints];

    const AIData& f4p1 = mAIWhole.aiData[(fracIndex + f4FracInt + splinePoints - 1) % splinePoints];
    const AIData& f4p2 = mAIWhole.aiData[(fracIndex + f4FracInt) % splinePoints];
    const AIData& f4p3 = mAIWhole.aiData[(fracIndex + f4FracInt + 1) % splinePoints];
    const AIData& f4p4 = mAIWhole.aiData[(fracIndex + f4FracInt + 2) % splinePoints];

    float f3Coeff1 = f3FracModQube * -0.5f  + f3FracMod * -0.5f         + f3FracModSquare;
    float f3Coeff2 = f3FracModQube * 1.5f   + f3FracModSquare * -2.5f   + 1.0f;
    float f3Coeff3 = f3FracModQube * -1.5f  + f3FracMod * 0.5f          + f3FracModSquare + f3FracModSquare;
    float f3Coeff4 = f3FracModQube * 0.5f   + f3FracModSquare * -0.5f;

    float f4Coeff1 = f4FracModQube * -0.5f  + f4FracMod * -0.5f         + f4FracModSquare;
    float f4Coeff2 = f4FracModQube * 1.5f   + f4FracModSquare * -2.5f   + 1.0f;
    float f4Coeff3 = f4FracModQube * -1.5f  + f4FracMod * 0.5f          + f4FracModSquare + f4FracModSquare;
    float f4Coeff4 = f4FracModQube * 0.5f   + f4FracModSquare * -0.5f;

    //feature 3
    Ogre::Vector4 f3Px(f3p1.pos.x, f3p2.pos.x, f3p3.pos.x, f3p4.pos.x);
    Ogre::Vector4 f3Py(f3p1.pos.y, f3p2.pos.y, f3p3.pos.y, f3p4.pos.y);
    Ogre::Vector4 f3Pz(f3p1.pos.z, f3p2.pos.z, f3p3.pos.z, f3p4.pos.z);
    Ogre::Vector4 f3Tx(f3p1.tangent.x, f3p2.tangent.x, f3p3.tangent.x, f3p4.tangent.x);
    Ogre::Vector4 f3Ty(f3p1.tangent.y, f3p2.tangent.y, f3p3.tangent.y, f3p4.tangent.y);
    Ogre::Vector4 f3Tz(f3p1.tangent.z, f3p2.tangent.z, f3p3.tangent.z, f3p4.tangent.z);
    Ogre::Vector4 f3Mx(f3p1.magic.x, f3p2.magic.x, f3p3.magic.x, f3p4.magic.x);
    Ogre::Vector4 f3Mz(f3p1.magic.z, f3p2.magic.z, f3p3.magic.z, f3p4.magic.z);
    Ogre::Vector4 f3C(f3Coeff1, f3Coeff2, f3Coeff3, f3Coeff4);

    ret.out6.x = f3Px.dotProduct(f3C);
    ret.out6.y = f3Py.dotProduct(f3C);
    ret.out6.z = f3Pz.dotProduct(f3C);
    ret.out6 -= carPos;
    ret.out8 = f3Mx.dotProduct(f3C);
    ret.out10 = f3Mz.dotProduct(f3C);

    if(mAIWhole.hackType != 1)
    {
        ret.out12.x = f3Tx.dotProduct(f3C);
        ret.out12.y = f3Ty.dotProduct(f3C);
        ret.out12.z = f3Tz.dotProduct(f3C);
    }


    //feature 4
    Ogre::Vector4 f4Px(f4p1.pos.x, f4p2.pos.x, f4p3.pos.x, f4p4.pos.x);
    Ogre::Vector4 f4Py(f4p1.pos.y, f4p2.pos.y, f4p3.pos.y, f4p4.pos.y);
    Ogre::Vector4 f4Pz(f4p1.pos.z, f4p2.pos.z, f4p3.pos.z, f4p4.pos.z);
    Ogre::Vector4 f4Tx(f4p1.tangent.x, f4p2.tangent.x, f4p3.tangent.x, f4p4.tangent.x);
    Ogre::Vector4 f4Ty(f4p1.tangent.y, f4p2.tangent.y, f4p3.tangent.y, f4p4.tangent.y);
    Ogre::Vector4 f4Tz(f4p1.tangent.z, f4p2.tangent.z, f4p3.tangent.z, f4p4.tangent.z);
    Ogre::Vector4 f4My(f4p1.magic.y, f4p2.magic.y, f4p3.magic.y, f4p4.magic.y);
    Ogre::Vector4 f4Mz(f4p1.magic.z, f4p2.magic.z, f4p3.magic.z, f4p4.magic.z);
    Ogre::Vector4 f4C(f4Coeff1, f4Coeff2, f4Coeff3, f4Coeff4);

    ret.out7.x = f4Px.dotProduct(f4C);
    ret.out7.y = f4Py.dotProduct(f4C);
    ret.out7.z = f4Pz.dotProduct(f4C);
    ret.out7 -= carPos;
    ret.out9 = f4My.dotProduct(f4C);
    ret.out11 = f4Mz.dotProduct(f4C);

    if(mAIWhole.hackType != 1)
    {
        ret.out13.x = f4Tx.dotProduct(f4C);
        ret.out13.y = f4Ty.dotProduct(f4C);
        ret.out13.z = f4Tz.dotProduct(f4C);
    }

    return ret;
}