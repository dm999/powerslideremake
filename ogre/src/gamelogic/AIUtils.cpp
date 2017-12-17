
#include "AIUtils.h"

#include "../GameState.h"

#include "../tools/OgreTools.h"
#include "../tools/Tools.h"
#include "../tools/Randomizer.h"

#include "../pscar/PSAICar.h"

#include "../physics/PhysicsVehicleAI.h"

#include "../physics/InitialVehicleSetup.h"

AIUtils::AIUtils() :
        mAIDistanceLength(0.0f),
        mPrevPos(Ogre::Vector3::ZERO),
        mIsReverseEnabled(false)
{
}

void AIUtils::setAIData(const AIWhole& aiWhole, Ogre::SceneManager* sceneMgr, bool isDebugAI)
{
    mAIWhole = aiWhole;
    mIsPrevClosestSplineIndexInited = false;

    mPrevRot = Ogre::Vector3::ZERO;

    mAIDistanceLength = 0.0f;
    mPrevPos = Ogre::Vector3::ZERO;
    mTimerAIStuck.reset();
    mIsReverseEnabled = false;

    //original data is left hand, convert for AI
    for(size_t q = 0; q < aiWhole.aiData.size(); ++q)
    {
        mAIWhole.aiData[q].pos.z = -mAIWhole.aiData[q].pos.z;
        mAIWhole.aiData[q].tangent.z = -mAIWhole.aiData[q].tangent.z;
        mAIWhole.aiData[q].magic.z = -mAIWhole.aiData[q].magic.z;
    }
}

void AIUtils::performAICorrection(PSAICar* aiCar, PhysicsVehicleAI* physicsAICar, const GameState& gameState, const InitialVehicleSetup& initialVehicleSetup)
{

    float steeringVal;
    float accelerationVal;
    float breaksVal;

    Ogre::Vector3 carPos = aiCar->getModelNode()->getPosition();

    if(!mIsPrevClosestSplineIndexInited)
    {
        //original data is left hand, convert for AI
        mPrevClosestSplineIndex = getClosestSplinePoint(Ogre::Vector3(carPos.x, carPos.y, -carPos.z));
        mIsPrevClosestSplineIndexInited = true;
    }

    calcFeatures(aiCar, physicsAICar, gameState, initialVehicleSetup);
    inference(steeringVal, accelerationVal);
    adjustInferenceResults(steeringVal, accelerationVal, breaksVal, gameState.getTrackName() == "mineshaft");

    if(gameState.getRaceStarted())
    {
        if(gameState.isGamePaused())
        {
            mTimerAIStuck.reset();
        }

        //steering
        Ogre::Vector3 carDir = aiCar->getForwardAxis();
        Ogre::Vector3 carDirOriginal = carDir;
        carDir.y = 0.0f;
        
        Ogre::Real lastDistance = carDir.dotProduct(carPos - mPrevPos);
        mPrevPos = carPos;
        mAIDistanceLength += Ogre::Math::Abs(lastDistance);

        physicsAICar->setSteering(steeringVal);
        physicsAICar->setAcceleration(accelerationVal);
        physicsAICar->setBreaks(breaksVal);


        const Ogre::Real stuckDistanceLimit = 20.0f;
        const unsigned long stuckTimeLimit = 2000; //ms
        const unsigned long reverseTimeLimit = 2000; //ms
#if 0
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
            if(accelerationVal < 0.0f)
            {
                aiCar->setBrake(true);
            }
            else
            {
                Ogre::Real aiSpeed = aiCar->getAlignedVelocity();
                if(aiSpeed < (aiMaxSpeed * mSpeedCoeff * accelerationVal))
                {
                    aiCar->setAcceleration(true);
                }
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
#endif
    }
}

void AIUtils::raceStarted()
{
    mTimerAIStuck.reset();
}

void AIUtils::calcFeatures(PSAICar* aiCar, PhysicsVehicleAI* physicsAICar, const GameState& gameState, const InitialVehicleSetup& initialVehicleSetup)
{
    float feature3 = mAIWhole.slotMatrix[18][0];

    if(mAIWhole.hackType == 1)
    {
        feature3 += 1.0f;
    }
    else
    {
        feature3 += 0.25f;
    }

    float feature4 = mAIWhole.slotMatrix[19][0] + 1.0f;

    Ogre::Vector3 carPos = aiCar->getModelNode()->getPosition();
    carPos.z = -carPos.z;//original data is left hand

    Ogre::Matrix3 carRot;
    aiCar->getModelNode()->getOrientation().ToRotationMatrix(carRot);
    Ogre::Vector3 carRotV[3];//original data is left hand
    carRotV[0] = Ogre::Vector3(carRot[0][0], carRot[1][0], -carRot[2][0]);
    carRotV[1] = Ogre::Vector3(carRot[0][1], carRot[1][1], -carRot[2][1]);
    carRotV[2] = Ogre::Vector3(-carRot[0][2], -carRot[1][2], carRot[2][2]);

    Ogre::Vector3 carLinearImpulse = aiCar->getLinearImpulse();
    carLinearImpulse.z = -carLinearImpulse.z;//original data is left hand


    size_t closestSplineIndex = getRelativeClosestSplinePoint(carPos);

    float frac = 0.0f;
    size_t splineFracIndex = getFracIndex(closestSplineIndex, carPos, frac);

    SplineFeatures splineFeatures = getSplineFeatures(
        physicsAICar,
        carPos, 
        carLinearImpulse,
        initialVehicleSetup,
        frac, splineFracIndex, 
        fabs(feature3), fabs(feature4)
        );

    if(mAIWhole.hackType == 1)
    {
        mAIWhole.slotMatrix[2][0]   = splineFeatures.out10;
        mAIWhole.slotMatrix[3][0]   = splineFeatures.out11;
        mAIWhole.slotMatrix[8][0]   = carRotV[0].dotProduct(carLinearImpulse) * initialVehicleSetup.mChassisInvMass;
        mAIWhole.slotMatrix[9][0]   = carRotV[1].dotProduct(carLinearImpulse) * initialVehicleSetup.mChassisInvMass;
        mAIWhole.slotMatrix[10][0]  = carRotV[2].dotProduct(carLinearImpulse) * initialVehicleSetup.mChassisInvMass;
    }
    else
    {
        float impulseLen = carLinearImpulse.length();
        float velocity = initialVehicleSetup.mChassisInvMass * impulseLen * 0.333f;

        mAIWhole.slotMatrix[2][0] = splineFeatures.out10 - velocity;
        mAIWhole.slotMatrix[3][0] = splineFeatures.out11 - velocity;

        Ogre::Vector3 valA, valB;
        if(impulseLen == 0.0f)
        {
            valA = Ogre::Vector3::ZERO;
            valB = Ogre::Vector3::ZERO;
        }
        else
        {
            valA = carLinearImpulse / impulseLen;
            valB = valA.crossProduct(carRotV[1]);
        }

        mAIWhole.slotMatrix[8][0]   = atan2(carRotV[2].dotProduct(valB), carRotV[2].dotProduct(valA));
        mAIWhole.slotMatrix[9][0]   = atan2(splineFeatures.out12.dotProduct(valB), splineFeatures.out12.dotProduct(valA));
        mAIWhole.slotMatrix[10][0]  = atan2(splineFeatures.out13.dotProduct(valB), splineFeatures.out13.dotProduct(valA));
    }

    mAIWhole.slotMatrix[0][0] = atan2(carRotV[0].dotProduct(splineFeatures.out6), carRotV[2].dotProduct(splineFeatures.out6)) * splineFeatures.out8;
    mAIWhole.slotMatrix[1][0] = atan2(carRotV[0].dotProduct(splineFeatures.out7), carRotV[2].dotProduct(splineFeatures.out7)) * splineFeatures.out9;
    
    float v34 = 1.0f / sqrt(carRotV[2].z * carRotV[2].z + (-carRotV[2].x * -carRotV[2].x));
    float v57 = v34 * carRotV[2].z;
    float v59 = v34 * -carRotV[2].x;
    mAIWhole.slotMatrix[4][0] = carRotV[1].x * v57 + carRotV[1].z * v59;
    mAIWhole.slotMatrix[5][0] = carRotV[2].y;
    mAIWhole.slotMatrix[6][0] = carRotV[1].y;
    mAIWhole.slotMatrix[7][0] = carRotV[2].dotProduct(mPrevRot) * 15.0f;

    mAIWhole.slotMatrix[11][0] = 1.0f;
    mAIWhole.slotMatrix[12][0] = Randomizer::GetInstance().GetRandomFloat(-1.0f, 1.0f);

    //overtake logic
    bool isNeedToOvertake = false;
    int closestOtherCarIndex = 0;

    std::vector<const PSBaseCar*> cars;
    cars.reserve(GameState::mRaceGridCarsMax);

    for(size_t q = 0; q < gameState.getAICount(); ++q)
    {
        cars.push_back(&gameState.getAICar(q));
    }
    cars.push_back(&gameState.getPlayerCar());

    float minSqDist = std::numeric_limits<float>::max();

    for(size_t q = 0; q < cars.size(); ++q)
    {
        Ogre::Vector3 otherPos = cars[q]->getModelNode()->getPosition();
        otherPos.z = -otherPos.z;//original data is left hand

        Ogre::Vector3 posDiff = otherPos - carPos;

        if(carRotV[2].dotProduct(posDiff) > 0.0f)
        {
            float sqDist = posDiff.squaredLength();

            if(sqDist < minSqDist && sqDist != 0.0f)
            {
                isNeedToOvertake = true;
                closestOtherCarIndex = q;
                minSqDist = sqDist;
            }
        }
    }

    if(isNeedToOvertake)
    {
        Ogre::Vector3 otherPos = cars[closestOtherCarIndex]->getModelNode()->getPosition();
        otherPos.z = -otherPos.z;//original data is left hand

        Ogre::Vector3 posDiff = otherPos - carPos;

        mAIWhole.slotMatrix[13][0] = atan2(carRotV[0].dotProduct(posDiff), carRotV[2].dotProduct(posDiff));
        
        mAIWhole.slotMatrix[15][0] = mAIWhole.slotMatrix[13][0] + Ogre::Math::PI;
        if(mAIWhole.slotMatrix[15][0] > Ogre::Math::PI)
            mAIWhole.slotMatrix[15][0] -= Ogre::Math::TWO_PI;
        mAIWhole.slotMatrix[15][0] *= 0.5f;

        float diffLen = posDiff.length();
        if(diffLen <= 0.0f) diffLen = 0.0001f;

        mAIWhole.slotMatrix[14][0] = 25.0f / diffLen;
        mAIWhole.slotMatrix[13][0] *= Ogre::Math::Pow(mAIWhole.slotMatrix[14][0], mAIWhole.multiplier.x);
        mAIWhole.slotMatrix[15][0] *= Ogre::Math::Pow(mAIWhole.slotMatrix[14][0], mAIWhole.multiplier.x);
    }
    else
    {
        mAIWhole.slotMatrix[13][0] = 0.0f;
        mAIWhole.slotMatrix[14][0] = 0.0f;
        mAIWhole.slotMatrix[15][0] = 0.0f;
    }

    mPrevRot = carRotV[0];
}

void AIUtils::adjustInferenceResults(float& steering, float& acceleration, float& breaks, bool isMineshafted) const
{
    if(acceleration > mAIWhole.accelerationCoeff)
        acceleration = mAIWhole.accelerationCoeff;

    if(isMineshafted)
    {
        if(mPrevClosestSplineIndex > 46 && mPrevClosestSplineIndex <= 50)
            acceleration = 1.0f;
    }

    if(acceleration > 1.0f) acceleration = 1.0f;
    if(acceleration >= 0.0f) breaks = 0.0f;

    if(acceleration < 0.0f)
    {
        if(mAIWhole.hackType <= 2)
        {
            breaks = 0.0f;//go by inertia
        }
        else
        {
            breaks = -0.5f - acceleration;
            breaks = Ogre::Math::Clamp(breaks, 0.0f, 1.0f);
        }

        acceleration = 0.0f;
    }


    steering = Ogre::Math::Clamp(steering, -0.95f, 0.95f);
}

void AIUtils::inference(float& steering, float& acceleration)
{
    mulSlotMatrix(20, mAIWhole.slotMatrix.size() - 1);
    mulSlotMatrix(16, 19);

    acceleration = mAIWhole.slotMatrix[16][0];
    steering = mAIWhole.slotMatrix[17][0];
}

void AIUtils::mulSlotMatrix(size_t fromRow, size_t toRow)
{
    for(size_t q = fromRow; q <= toRow; ++q)
    {
        Ogre::Vector3 partACoeffs(
            mAIWhole.slotMatrix[q][2] + mAIWhole.slotMatrix[q][5],
            mAIWhole.slotMatrix[q][8] + mAIWhole.slotMatrix[q][11],
            mAIWhole.slotMatrix[q][14] + mAIWhole.slotMatrix[q][17]
            );

        Ogre::Vector2 partBCoeffs(
            mAIWhole.slotMatrix[q][20] + mAIWhole.slotMatrix[q][23],
            mAIWhole.slotMatrix[q][26] + mAIWhole.slotMatrix[q][29]
            );

        Ogre::Vector3 partAFeatures(
            mAIWhole.slotMatrix[mAIWhole.remapper[q][0]][0],
            mAIWhole.slotMatrix[mAIWhole.remapper[q][1]][0],
            mAIWhole.slotMatrix[mAIWhole.remapper[q][2]][0]
            );

        Ogre::Vector2 partBFeatures(
            mAIWhole.slotMatrix[mAIWhole.remapper[q][3]][0],
            mAIWhole.slotMatrix[mAIWhole.remapper[q][4]][0]
        );

        float activation = mAIWhole.activation[mAIWhole.remapper[q][5]][0];

        float rowRes = (partACoeffs.dotProduct(partAFeatures) + partBCoeffs.dotProduct(partBFeatures)) * activation;

        float multiplier = -1.0f;
        if(rowRes >= 0.0f)multiplier = 1.0f;
        else rowRes = -rowRes;

        if(rowRes == 0.0f)rowRes = 0.0001f;

        //index selection
        if(rowRes < 20.0f)
        {
            double integer;
            float frac = static_cast<float>(modf(rowRes, &integer));
            size_t index = static_cast<size_t>(integer);

            const float activationMin = mAIWhole.activation[mAIWhole.remapper[q][5]][index + 9];
            const float activationMax = mAIWhole.activation[mAIWhole.remapper[q][5]][index + 10];

            mAIWhole.slotMatrix[q][0] = Tools::lerp(activationMin, activationMax, frac) * multiplier;
        }
        else
        {
            mAIWhole.slotMatrix[q][0] = mAIWhole.activation[mAIWhole.remapper[q][5]][29] * multiplier;
        }

        mAIWhole.slotMatrix[q][5]   = (mAIWhole.slotMatrix[q][0] * mAIWhole.slotMatrix[q][3] * mAIWhole.slotMatrix[mAIWhole.remapper[q][0]][0] + mAIWhole.slotMatrix[q][5]) * mAIWhole.slotMatrix[q][4];
        mAIWhole.slotMatrix[q][11]  = (mAIWhole.slotMatrix[q][0] * mAIWhole.slotMatrix[q][9] * mAIWhole.slotMatrix[mAIWhole.remapper[q][1]][0] + mAIWhole.slotMatrix[q][11]) * mAIWhole.slotMatrix[q][10];
        mAIWhole.slotMatrix[q][17]  = (mAIWhole.slotMatrix[q][0] * mAIWhole.slotMatrix[q][15] * mAIWhole.slotMatrix[mAIWhole.remapper[q][2]][0] + mAIWhole.slotMatrix[q][17]) * mAIWhole.slotMatrix[q][16];
        mAIWhole.slotMatrix[q][23]  = (mAIWhole.slotMatrix[q][0] * mAIWhole.slotMatrix[q][21] * mAIWhole.slotMatrix[mAIWhole.remapper[q][3]][0] + mAIWhole.slotMatrix[q][23]) * mAIWhole.slotMatrix[q][22];
        mAIWhole.slotMatrix[q][29]  = (mAIWhole.slotMatrix[q][0] * mAIWhole.slotMatrix[q][27] * mAIWhole.slotMatrix[mAIWhole.remapper[q][4]][0] + mAIWhole.slotMatrix[q][29]) * mAIWhole.slotMatrix[q][28];
    }
}

size_t AIUtils::getClosestSplinePoint(const Ogre::Vector3& carPos) const
{
    size_t ret = 0;

    float minDist = std::numeric_limits<float>::max();
    for(size_t q = 0; q < mAIWhole.aiData.size(); ++q)
    {
        float dist = carPos.squaredDistance(mAIWhole.aiData[q].pos);
        if(dist < minDist)
        {
            ret = q;
            minDist = dist;
        }
    }

    return ret;
}

size_t AIUtils::getRelativeClosestSplinePoint(const Ogre::Vector3& carPos)
{
    size_t ret = mPrevClosestSplineIndex;

    size_t indexCur = mPrevClosestSplineIndex;
    size_t indexNext = mPrevClosestSplineIndex + 1;
    size_t indexPrev = mPrevClosestSplineIndex - 1;

    if(indexNext >= mAIWhole.aiData.size()) indexNext = 0;
    if(indexCur == 0) indexPrev = mAIWhole.aiData.size() - 1;

    float distCur = carPos.squaredDistance(mAIWhole.aiData[indexCur].pos);
    float distNext = carPos.squaredDistance(mAIWhole.aiData[indexNext].pos);
    float distPrev = carPos.squaredDistance(mAIWhole.aiData[indexPrev].pos);

    if(distNext > distCur)
    {
        while(distPrev < distCur)
        {
            distCur = distPrev;
            ret = indexPrev;
            
            if(indexPrev == 0) indexPrev = mAIWhole.aiData.size() - 1;
            else --indexPrev;

            distPrev = carPos.squaredDistance(mAIWhole.aiData[indexPrev].pos);
        }
    }
    else
    {
        do
        {
            distCur = distNext;
            ret = indexNext;
            
            ++indexNext;
            if(indexNext >= mAIWhole.aiData.size()) indexNext = 0;

            distNext = carPos.squaredDistance(mAIWhole.aiData[indexNext].pos);
        }while(distNext < distCur);
    }

    mPrevClosestSplineIndex = ret;

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

    if(diffClosestVal <= 0.0f)
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
    PhysicsVehicleAI* physicsAICar,
    const Ogre::Vector3& carPos, 
    const Ogre::Vector3& impulseLinear, 
    const InitialVehicleSetup& initialVehicleSetup,
    float frac, size_t fracIndex, 
    float feature3, float feature4) const
{
    SplineFeatures ret;

    const size_t splinePoints = mAIWhole.aiData.size();

    double integer;

    if(mAIWhole.hack1 > 0.0 || mAIWhole.hack2 > 0.0)
    {
        float v17 = frac - 0.050000001f;

        float fracMod = static_cast<float>(modf(v17, &integer));
        size_t fracInt = static_cast<size_t>(integer);

        float fracModSquare = fracMod * fracMod;
        float fracModCube = fracMod * fracMod * fracMod;

        float coeff1 = fracModCube * -0.5f  + fracMod * -0.5f       + fracModSquare;
        float coeff2 = fracModCube * 1.5f   + fracModSquare * -2.5f + 1.0f;
        float coeff3 = fracModCube * -1.5f  + fracMod * 0.5f        + fracModSquare + fracModSquare;
        float coeff4 = fracModCube * 0.5f   + fracModSquare * -0.5f;

        const AIData& p1 = mAIWhole.aiData[(fracIndex + fracInt  + splinePoints - 1) % splinePoints];
        const AIData& p2 = mAIWhole.aiData[(fracIndex + fracInt) % splinePoints];
        const AIData& p3 = mAIWhole.aiData[(fracIndex + fracInt + 1) % splinePoints];
        const AIData& p4 = mAIWhole.aiData[(fracIndex + fracInt + 2) % splinePoints];

        Ogre::Vector4 px(p1.pos.x, p2.pos.x, p3.pos.x, p4.pos.x);
        Ogre::Vector4 py(p1.pos.y, p2.pos.y, p3.pos.y, p4.pos.y);
        Ogre::Vector4 pz(p1.pos.z, p2.pos.z, p3.pos.z, p4.pos.z);

        Ogre::Vector4 co(coeff1, coeff2, coeff3, coeff4);

        Ogre::Vector3 tmpRes;
        tmpRes.x = px.dotProduct(co);
        tmpRes.y = py.dotProduct(co);
        tmpRes.z = pz.dotProduct(co);

        Ogre::Vector3 tmpDiff = tmpRes - carPos;

        if(mAIWhole.hack1 > 0.0)//mineshafted
        {
            float multiplier = 0.0f;

            float sqLen = tmpDiff.squaredLength();

            if(sqLen <= 4900.0f) multiplier = 1.0f;
            else multiplier = 70.0f / sqrt(sqLen);

            float multiplier2 = multiplier * initialVehicleSetup.mChassisMass * physicsAICar->getLinearVelocity() * mAIWhole.hack1 * 45.0f;

            Ogre::Vector3 impulseInc = physicsAICar->getLinearImpulseInc();

            impulseInc.x += tmpDiff.x * multiplier2;
            impulseInc.z += tmpDiff.z * multiplier2;
            physicsAICar->setLinearImpulseInc(impulseInc);
        }
        if(mAIWhole.hack2 > 0.0)//dam, city
        {
            Ogre::Vector4 tx(p1.tangent.x, p2.tangent.x, p3.tangent.x, p4.tangent.x);
            Ogre::Vector4 tz(p1.tangent.z, p2.tangent.z, p3.tangent.z, p4.tangent.z);

            Ogre::Real linVel = physicsAICar->getLinearVelocity();
            if(linVel < 0.00001f)
                linVel = 0.00001f;
            physicsAICar->setLinearVelocity(linVel);

            Ogre::Vector2 tmpRes2;
            tmpRes2.x = tx.dotProduct(co);
            tmpRes2.y = tz.dotProduct(co);

            Ogre::Vector3 impulseInc = physicsAICar->getLinearImpulseInc();

            tmpRes2 -= Ogre::Vector2(impulseLinear.x, impulseLinear.z) / (initialVehicleSetup.mChassisMass * linVel);


            float multiplier = 0.0f;

            if(tmpRes2.dotProduct(Ogre::Vector2(tmpDiff.x, tmpDiff.z)) <= 0.0f)
            {
                multiplier = initialVehicleSetup.mChassisMass * linVel * mAIWhole.hack2 / 
                    (Ogre::Vector2(tmpDiff.x, tmpDiff.z).squaredLength() * mAIWhole.hackMultiplier + 1.0f);
            }
            else
            {
                multiplier = initialVehicleSetup.mChassisMass * linVel * mAIWhole.hack2;
            }

            impulseInc.x += tmpRes2.x * multiplier;
            impulseInc.z += tmpRes2.y * multiplier;
            physicsAICar->setLinearImpulseInc(impulseInc);
        }
    }

    float f3Frac = feature3 + frac;
    float f4Frac = feature4 + frac;

    float f3FracMod = static_cast<float>(modf(f3Frac, &integer));
    size_t f3FracInt = static_cast<size_t>(integer);

    float f4FracMod = static_cast<float>(modf(f4Frac, &integer));
    size_t f4FracInt = static_cast<size_t>(integer);

    float f3FracModSquare = f3FracMod * f3FracMod;
    float f3FracModQube = f3FracMod * f3FracMod * f3FracMod;

    float f4FracModSquare = f4FracMod * f4FracMod;
    float f4FracModQube = f4FracMod * f4FracMod * f4FracMod;

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