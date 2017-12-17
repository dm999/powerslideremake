
#include "PhysicsWheels.h"

#include "Physics.h"
#include "PhysicsVehicle.h"
#include "../mesh/StaticMeshProcesser.h"

PhysicsWheels::PhysicsWheels(const InitialVehicleSetup& initialVehicleSetup, 
                        Physics* physics,
                        const StaticMeshProcesser* const meshProcesser)
    :   mInitialVehicleSetup(initialVehicleSetup),
        mPhysics(physics),
        mMeshProcesser(meshProcesser)
{}

PhysicsWheels::~PhysicsWheels()
{}

void PhysicsWheels::init(const Ogre::Vector3& chassisPos, Ogre::SceneNode *wheelNodes[InitialVehicleSetup::mWheelsAmount])
{
    for(int q = 0; q < InitialVehicleSetup::mWheelsAmount; ++q)
    {
        mWheelNodes[q] = wheelNodes[q];
    }

    for(int q = 0; q < InitialVehicleSetup::mWheelsAmount; ++q)
    {
        //mWheelsSuspensionGlobal[q] = chassisPos;
        //mWheelsSuspensionGlobalPrev[q] = chassisPos;
        mWheelsSuspensionPointGlobal[q] = chassisPos;
        mSuspensionHeight[q] = mInitialVehicleSetup.mSuspensionDataWheel[q].x;
        mSuspensionHeightPrev[q] = mSuspensionHeight[q];
        mSpringVal[q] = 1.0f;
        mSteering[q] = mInitialVehicleSetup.mSuspensionDataWheel[q].y;
        mVelocity[q] = mInitialVehicleSetup.mSuspensionDataWheel[q].z;
        mIsCollided[q] = false;
        mWheelRotationalAngle[q] = 0.0f;
        mTerrainIndex[q] = -1;
    }
}

void PhysicsWheels::initStep()
{
    Ogre::Matrix3 rotMatrix;
    mInitialVehicleSetup.mCarRot.ToRotationMatrix(rotMatrix);
    Ogre::Vector3 rotAxisY = rotMatrix.GetColumn(1);

    for(int q = 0; q < InitialVehicleSetup::mWheelsAmount; ++q)
    {
        mGlobalPos[q] = mInitialVehicleSetup.mCarGlobalPos + mInitialVehicleSetup.mCarRot * mInitialVehicleSetup.mConnectionPointWheel[q];

        Ogre::Real maxTravel = mInitialVehicleSetup.mMaxTravel * 0.5f;
        Ogre::Vector3 rotVal = maxTravel * rotAxisY;

        mWheelsSuspensionPointGlobalPrev[q] = mWheelsSuspensionPointGlobal[q];
        mWheelsSuspensionPointGlobal[q] = mGlobalPos[q] - rotVal;
        mWheelsSuspensionPoint2Global[q] = mWheelsSuspensionPointGlobal[q] - rotVal;
        mWheelsSuspensionPoint[q] = mWheelsSuspensionPoint2Global[q] - mInitialVehicleSetup.mCarGlobalPos;

        //mWheelsSuspensionGlobalPrev[q] = mWheelsSuspensionGlobal[q];
        mSuspensionHeightPrev[q] = mSuspensionHeight[q];

        mWheelsImpulseTangent[q] = Ogre::Vector3::ZERO;
        mWheelsAveragedNormal[q] = Ogre::Vector3::UNIT_Y;
        mWheelsImpulseResulted[q] = 0.0f;

        mTerrainIndex[q] = -1;
        mIsCollided[q] = false;
    }

}

void PhysicsWheels::reposition()
{
    for(int q = 0; q < InitialVehicleSetup::mWheelsAmount; ++q)
    {
        Ogre::Vector3 connectionPoint = mInitialVehicleSetup.mConnectionPointWheel[q];
        connectionPoint.y -= mSuspensionHeight[q];
        //mWheelNodes[q]->setPosition(mWheelsSuspensionGlobal[q]);
        mWheelNodes[q]->setPosition(mInitialVehicleSetup.mCarGlobalPos + mInitialVehicleSetup.mCarRot * connectionPoint);
    }
}

void PhysicsWheels::rerotation()
{
    for(int q = 0; q < InitialVehicleSetup::mWheelsAmount; ++q)
    {

        Ogre::Quaternion finalRot = mInitialVehicleSetup.mCarRot;

        Ogre::Vector3 connectionPoint = mInitialVehicleSetup.mConnectionPointWheel[q];
        connectionPoint.y -= mSuspensionHeight[q];
        //mWheelNodes[q]->setPosition(mWheelsSuspensionGlobal[q]);
        mWheelNodes[q]->setPosition(mInitialVehicleSetup.mCarGlobalPos + mInitialVehicleSetup.mCarRot * connectionPoint);

        mWheelRotationalAngle[q] += mVelocity[q] * mInitialVehicleSetup.mChassisInvMass * 1.27f / mInitialVehicleSetup.mWheelRadius[q];
        if(mWheelRotationalAngle[q] <= -Ogre::Math::TWO_PI)
        {
            mWheelRotationalAngle[q] += Ogre::Math::TWO_PI;
        }
        else
        {
            mWheelRotationalAngle[q] -= Ogre::Math::TWO_PI;
        }

        Ogre::Quaternion rotDrive;
        rotDrive.FromAngleAxis(Ogre::Radian(-mWheelRotationalAngle[q]), Ogre::Vector3(1.0f, 0.0f, 0.0f));

        if(q >= 2)//front
        {
            Ogre::Quaternion turnWheel;
            turnWheel.FromAngleAxis(Ogre::Degree(mSteering[q] * -45.0f), Ogre::Vector3(0.0f, 1.0f, 0.0f));

            finalRot = finalRot * turnWheel * rotDrive;
        }
        else
        {
            finalRot = finalRot * rotDrive;
        }

        mWheelNodes[q]->setOrientation(finalRot);
    }

}

Ogre::Real PhysicsWheels::calcVelocity(Ogre::Real vehicleVelocityMod, Ogre::Real throttle, Ogre::Real breaks)
{

    throttle *= 1.3f;

    const Ogre::Real handBreak = 0.0f;
    Ogre::Real breaksHand = handBreak * 2.6f;

    if(vehicleVelocityMod < 0.4f)
    {
        breaksHand *= 0.5f;
        if(breaksHand > breaks)
            breaks = breaksHand;

        if(vehicleVelocityMod < 0.05f)
        {
            breaks *= vehicleVelocityMod * 20.0f;
            breaksHand *= vehicleVelocityMod * 20.0f;
        }

    }

    for(int q = 0; q < InitialVehicleSetup::mWheelsAmount; ++q)
    {
        Ogre::Real breakFactor = 0.0f;
        if(q >= 2)//front
        {
            breakFactor = breaks;
            breakFactor *= mInitialVehicleSetup.mFrontBreak;
        }
        else
        {
            breakFactor = breaks + breaksHand;
            breakFactor *= mInitialVehicleSetup.mBackBreak;
        }

        if(mVelocity[q] < 0.0f)
        {
            mVelocity[q] += breakFactor;
        }
        else
        {
            mVelocity[q] -= breakFactor;
        }
    }

    Ogre::Real averageVel = (mVelocity[0] + mVelocity[1]) * 0.5f;

    //differential
    mVelocity[0] =  ( 1.0f - mInitialVehicleSetup.mDiffSlip) * averageVel +
                    mInitialVehicleSetup.mDiffSlip * mVelocity[0];

    mVelocity[1] =  ( 1.0f - mInitialVehicleSetup.mDiffSlip) * averageVel +
                    mInitialVehicleSetup.mDiffSlip * mVelocity[1];

    return averageVel;
}

void PhysicsWheels::calcVelocityMore(Ogre::Real power, int gear)
{
    if(gear > 0)//forward
    {
        for(int q = 0; q < InitialVehicleSetup::mWheelsAmount; ++q)
        {
            if(q >= 2)//front
            {
                mVelocity[q] += (power - mInitialVehicleSetup.mTransmissionDrag * mVelocity[q]) *
                                mInitialVehicleSetup.mFrontWheelDrive;
            }
            else
            {
                mVelocity[q] += (power - mInitialVehicleSetup.mTransmissionDrag * mVelocity[q]) *
                                (1.0f - mInitialVehicleSetup.mFrontWheelDrive);
            }
        }
    }

    if(gear == -1)//reverse
    {
        for(int q = 0; q < InitialVehicleSetup::mWheelsAmount; ++q)
        {
            if(q >= 2)//front
            {
                mVelocity[q] -= (power + mInitialVehicleSetup.mTransmissionDrag * mVelocity[q]) *
                                mInitialVehicleSetup.mFrontWheelDrive;
            }
            else
            {
                mVelocity[q] -=  (power + mInitialVehicleSetup.mTransmissionDrag * mVelocity[q]) *
                                (1.0f - mInitialVehicleSetup.mFrontWheelDrive);
            }
        }
    }

    for(int q = 0; q < InitialVehicleSetup::mWheelsAmount; ++q)
    {
        if(mIsCollided[q] && mTerrainIndex[q] != -1)
        {
            const TerrainData& terrain = mMeshProcesser->getTerrainData(mTerrainIndex[q]);

            if(q >= 2)//front
            {
                mVelocity[q] -= mVelocity[q] * terrain.mRollResistance * 0.4f;
            }
            else
            {
                mVelocity[q] -= mVelocity[q] * terrain.mRollResistance * 0.6f;
            }
        }
    }
}

bool PhysicsWheels::getWheelCollision(size_t index) const
{
    return mIsCollided[index];
}

char PhysicsWheels::getWheelTerainIndex(size_t index) const
{
    return mTerrainIndex[index];
}

Ogre::Real PhysicsWheels::getWheelVelocity(size_t index) const
{
    return mVelocity[index];
}

void PhysicsWheels::calcImpulses(const Ogre::Vector3& impulseRot, const Ogre::Vector3& impulseRotPrev, const Ogre::Vector3& normalisedImpulseRot,
                    const Ogre::Vector3& impulseLinear,
                    Ogre::Real recipMomentProj,
                    const PhysicsVehicle& vehicle)
{
    if(impulseRot.length() == 0.0f)
    {
        for(int q = 0; q < InitialVehicleSetup::mWheelsAmount; ++q)
        {
            mWheelsImpulseLinear[q] = impulseLinear;
        }
    }
    else
    {
        Ogre::Real force = mInitialVehicleSetup.mChassisMass * recipMomentProj;

        for(int q = 0; q < InitialVehicleSetup::mWheelsAmount; ++q)
        {
            Ogre::Vector3 tangent = PhysicsVehicle::findTangent(normalisedImpulseRot, mWheelsSuspensionPoint[q]);
            if(tangent.x != 0.0f || tangent.y != 0.0f || tangent.z != 0.0f)
            {
                mWheelsImpulseLinear[q] = tangent.crossProduct(impulseRotPrev) * force + impulseLinear;
            }
            else
            {
                mWheelsImpulseLinear[q] = impulseLinear;
            }
        }
    }
}

void PhysicsWheels::process(PhysicsVehicle& vehicle)
{

    Ogre::Matrix3 carRot;
    mInitialVehicleSetup.mCarRot.ToRotationMatrix(carRot);
    Ogre::Vector3 matrixXColumn = carRot.GetColumn(0);
    Ogre::Vector3 matrixYColumn = carRot.GetColumn(1);
    Ogre::Vector3 matrixZColumn = -carRot.GetColumn(2);//original data is left hand

    //for(int q = 0; q < InitialVehicleSetup::mWheelsAmount; ++q)
    for(int q = InitialVehicleSetup::mWheelsAmount - 1; q >= 0; --q)
    {

        const Ogre::Real tol = 0.00001f;
        Ogre::Vector3 averagedSuspensionPos = mWheelsSuspensionPointGlobal[q] + mWheelsSuspensionPointGlobalPrev[q];
        averagedSuspensionPos *= 0.5f;

        Ogre::Vector3 diffPos = mWheelsSuspensionPointGlobal[q] - mWheelsSuspensionPointGlobalPrev[q];
        Ogre::Real averageLen = diffPos.length() * 0.5f - (mInitialVehicleSetup.mMaxTravel * -0.5f) + mInitialVehicleSetup.mWheelRadius[q];

        size_t foundIndex;
        Ogre::Real distance;
        bool isSphereCollided = mMeshProcesser->collideSphere(mWheelsSuspensionPoint2Global[q], mInitialVehicleSetup.mWheelRadius[q], tol,
                                        averagedSuspensionPos, averageLen,
                                        foundIndex, distance);

        if(isSphereCollided)
        {
            const FoundCollision& collision = mMeshProcesser->getCollision(foundIndex);
            Ogre::Vector3 pointOnStaticA, pointOnStaticB, pointOnStaticC;
            mMeshProcesser->getGeoverts(collision, pointOnStaticA, pointOnStaticC, pointOnStaticB);

            Ogre::Vector2 pointOnStaticTextureA, pointOnStaticTextureB, pointOnStaticTextureC;
            mMeshProcesser->getGeovertsTexture(collision, pointOnStaticTextureA, pointOnStaticTextureC, pointOnStaticTextureB);


            Ogre::Vector3 worldNormal = collision.mNormal;
            worldNormal.z = -worldNormal.z;//original data is left hand

            Ogre::Real projUp = matrixYColumn.dotProduct(worldNormal);
            Ogre::Real suspHeight;
            if(projUp <= 0.0f)
            {
                suspHeight = mSuspensionHeightPrev[q];
            }
            else
            {
                Ogre::Real dotPWheels = mGlobalPos[q].dotProduct(worldNormal);
                Ogre::Real dotPStatic = pointOnStaticA.dotProduct(worldNormal);
                suspHeight = (dotPWheels - (dotPStatic + mInitialVehicleSetup.mWheelRadius[q])) / projUp;
            }

            suspHeight = calcSuspensionLength(suspHeight, q);

            mSuspensionHeight[q] = suspHeight;

            Ogre::Vector3 suspLocal = mInitialVehicleSetup.mConnectionPointWheel[q];
            suspLocal.y -= suspHeight;
            mWheelsSuspensionRot[q] = mInitialVehicleSetup.mCarRot * suspLocal;
            mWheelsSuspensionGlobal[q] = mInitialVehicleSetup.mCarGlobalPos + mWheelsSuspensionRot[q];

            const Ogre::Image * terrainMap = mMeshProcesser->getTerrainMap(mMeshProcesser->getTerrainName(collision));
            Ogre::Vector2 texCoords = PhysicsVehicle::findTexCoordinates(worldNormal, 
                mWheelsSuspensionGlobal[q],
                pointOnStaticA, pointOnStaticB, pointOnStaticC,
                pointOnStaticTextureA, pointOnStaticTextureB, pointOnStaticTextureC
                );
            mTerrainIndex[q] = mMeshProcesser->getTerrainType(terrainMap, texCoords);

            Ogre::Vector3 averagedNormal;
            Ogre::Real finalDistance = averageCollisionNormal(matrixYColumn, q, averagedNormal);

            mWheelsImpulseTangent[q] = PhysicsVehicle::findTangent(worldNormal, mWheelsImpulseLinear[q]);

            Ogre::Real suspWeight;
            if(q >= 2)//front wheels
            {
                suspWeight = mInitialVehicleSetup.mFrontSuspension;
            }
            else
            {
                suspWeight = 1.0f;
            }

            if(finalDistance < 0.0f)finalDistance = 0.0f;

            Ogre::Real risingDampWeight = 1.0f;
            Ogre::Real impulseProj = -mWheelsImpulseLinear[q].dotProduct(averagedNormal);
            if(impulseProj < 0.0f)
            {
                impulseProj = -impulseProj;
                risingDampWeight = -mInitialVehicleSetup.mRisingDamp;
            }

            Ogre::Real d_dv = mInitialVehicleSetup.mWheelUnderGroundDDV.getPoint(finalDistance);
            Ogre::Real v_dv = mInitialVehicleSetup.mWheelUnderGroundVDV.getPoint(impulseProj);
            Ogre::Real d_d = mInitialVehicleSetup.mWheelUnderGroundDD.getPoint(finalDistance);
            Ogre::Real v_v = mInitialVehicleSetup.mWheelUnderGroundVV.getPoint(impulseProj);

            Ogre::Real resultedImpulse = (v_dv * d_dv * risingDampWeight) + 
                (d_d * mSpringVal[q]) + 
                (v_v * risingDampWeight);
            resultedImpulse *= suspWeight;

            if(resultedImpulse < 0.0f) resultedImpulse = 0.0f;
            if(resultedImpulse > 100.0f) resultedImpulse = 100.0f;

            mWheelsAveragedNormal[q] = averagedNormal;
            mWheelsImpulseResulted[q] = resultedImpulse;

            Ogre::Vector3 impulseInc = averagedNormal * resultedImpulse;

            vehicle.adjustImpulseInc(mWheelsSuspensionRot[q], impulseInc);

            if(q >= 2)//front
            {
                Ogre::Real xAxisDot = matrixXColumn.dotProduct(impulseInc);
                Ogre::Real zAxisDot = matrixZColumn.dotProduct(impulseInc);

                if(q == 2)//right
                {
                    vehicle.mSteeringAdditionalParam -= (zAxisDot - xAxisDot) * 0.02f;
                }
                else//left
                {
                    vehicle.mSteeringAdditionalParam -= (zAxisDot + xAxisDot) * -0.02f;
                }
            }


            mIsCollided[q] = true;
        }
        else
        {
            mSuspensionHeight[q] = calcSuspensionLength(mInitialVehicleSetup.mMaxTravel * 2.0f, q);
        }
    }
}

Ogre::Real PhysicsWheels::averageCollisionNormal(const Ogre::Vector3& matrixYColumn, size_t wheelIndex, Ogre::Vector3& averagedNormal) const
{
    const Ogre::Real wheelRad = mInitialVehicleSetup.mWheelRadius[wheelIndex] * 4.0f;
    const Ogre::Real wheelRadSqr = wheelRad * wheelRad;
    const Ogre::Real wheelRadSqrMul = mInitialVehicleSetup.mWheelRadius[wheelIndex] * mInitialVehicleSetup.mWheelRadius[wheelIndex] * 15.8f;

    Ogre::Real finalDistance = 0.0f;

    averagedNormal = Ogre::Vector3::ZERO;

    const std::vector<size_t>& arrayOfCollisions = mMeshProcesser->getArrayOfCollisions();
    for(size_t q = 0; q < arrayOfCollisions.size(); ++q)
    {
        const FoundCollision& collision = mMeshProcesser->getCollision(arrayOfCollisions[q]);

        Ogre::Real distance = -collision.mDistance;

        Ogre::Vector3 worldNormal = collision.mNormal;
        worldNormal.z = -worldNormal.z;//original data is left hand

        Ogre::Real projUp = matrixYColumn.dotProduct(worldNormal);
        if(projUp < 0.0f) projUp = 0.0f;

        Ogre::Real distWeight = (1.0f - projUp) * mInitialVehicleSetup.mMaxTravel;
        distance -= (distWeight * -0.7f);

        if(distance > finalDistance) finalDistance = distance;

        Ogre::Vector3 pointOnStaticA, pointOnStaticB, pointOnStaticC;
        mMeshProcesser->getGeoverts(collision, pointOnStaticA, pointOnStaticC, pointOnStaticB);

        Ogre::Vector3 A_sub_C = pointOnStaticA - pointOnStaticC;
        Ogre::Vector3 B_sub_A = pointOnStaticB - pointOnStaticA;
        Ogre::Vector3 C_sub_B = pointOnStaticC - pointOnStaticB;

        Ogre::Real diff1 = A_sub_C.dotProduct(B_sub_A);
        Ogre::Real diff2 = C_sub_B.dotProduct(B_sub_A);
        Ogre::Real diff3 = A_sub_C.dotProduct(C_sub_B);

        Ogre::Real diff1m = -diff1;
        Ogre::Real diff2m = -diff2;
        Ogre::Real diff3m = -diff3;

        Ogre::Real dot;
        Ogre::Real valA;
        Ogre::Vector3 valB;

        if(diff1m < diff2m)
        {
            if(diff1m <= diff3m)//diff1m is minimum
            {
                dot = C_sub_B.dotProduct(C_sub_B);
                valA = diff2 * diff2 / dot;
                valB = B_sub_A;
            }
            else//diff3m is minimum
            {
                dot = B_sub_A.dotProduct(B_sub_A);
                valA = diff1 * diff1 / dot;
                valB = A_sub_C;
            }
        }
        else
        {
            if(diff3m <= diff2m)//diff3m is minimum
            {
                dot = B_sub_A.dotProduct(B_sub_A);
                valA = diff1 * diff1 / dot;
                valB = A_sub_C;
            }
            else//diff2m is minimum
            {
                dot = A_sub_C.dotProduct(A_sub_C);
                valA = diff3 * diff3 / dot;
                valB = C_sub_B;
            }
        }

        Ogre::Real len = Ogre::Math::Sqrt(valB.squaredLength() - valA);

        Ogre::Real valWheel;

        if(dot < wheelRadSqr) valWheel = Ogre::Math::Sqrt(dot);
        else valWheel = wheelRad;

        if(len > wheelRad) len = wheelRad;

        Ogre::Real lenValWheel = len * valWheel;

        Ogre::Real impulseProj = -mWheelsImpulseLinear[wheelIndex].dotProduct(worldNormal);
        if(impulseProj < 0.0f) impulseProj = 0.0f;

        Ogre::Real impulseProjMul = 8.0f - impulseProj * -0.2f;

        if(wheelRadSqr < wheelRadSqrMul) impulseProjMul = 8.0f;

        Ogre::Real weight = lenValWheel * impulseProjMul;

        averagedNormal += worldNormal * weight;
    }

    Ogre::Real averageLenSqr = averagedNormal.squaredLength();
    if(averageLenSqr <= 0.0f) averagedNormal = Ogre::Vector3::UNIT_Y;
    else
    {
        averagedNormal.normalise();
    }

    return finalDistance;
}

float PhysicsWheels::calcSuspensionLength(float len, size_t wheelIndex)
{
    float res = 0.0f;

    if(len > 0.0f)
    {
        if(len < mInitialVehicleSetup.mMaxTravel)
        {
            res = len;
            mSpringVal[wheelIndex] *= 0.95f;
            if(mSpringVal[wheelIndex] < 1.0f)
                mSpringVal[wheelIndex] = 1.0f;
        }
        else
        {
            res = mInitialVehicleSetup.mMaxTravel;
            mSpringVal[wheelIndex] = 1.0f;
        }
    }
    else
    {
        res = 0.0f;
        mSpringVal[wheelIndex] *= 1.015f;
            if(mSpringVal[wheelIndex] > 2.0f)
                mSpringVal[wheelIndex] = 2.0f;
    }

    return res;
}

void PhysicsWheels::calcPhysics(PhysicsVehicle& vehicle, Ogre::Real throttle, Ogre::Real breaks, Ogre::Real tractionScale)
{
    Ogre::Matrix3 carRot;
    mInitialVehicleSetup.mCarRot.ToRotationMatrix(carRot);
    Ogre::Vector3 matrixYColumn = carRot.GetColumn(1);
    Ogre::Vector3 matrixZColumn = carRot.GetColumn(2);

    Ogre::Real turnFinish = Ogre::Math::Abs(mSteering[3] * 1.0526316f);
    if(turnFinish < 0.0f) turnFinish = 0.0f;
    if(turnFinish <= mInitialVehicleSetup.mTurnFinish)
        turnFinish /= mInitialVehicleSetup.mTurnFinish;
    else
        turnFinish = 1.0f;

    Ogre::Real lowFrontTraction = (1.0f - mInitialVehicleSetup.mOversteer) * mInitialVehicleSetup.mLowFrontTraction * -0.11f;

    Ogre::Real rearTraction = mInitialVehicleSetup.mLowRearTraction - lowFrontTraction;
    Ogre::Real frontTraction = lowFrontTraction + mInitialVehicleSetup.mLowFrontTraction;

    //d.polubotko: use handbrake if need
    breaks *= 0.17f;

    Ogre::Real rearTractionDiff = (mInitialVehicleSetup.mHighRearTraction - mInitialVehicleSetup.mLowRearTraction) * turnFinish;
    Ogre::Real frontTractionDiff = (mInitialVehicleSetup.mHighFrontTraction - mInitialVehicleSetup.mLowFrontTraction) * turnFinish;

    rearTraction += rearTractionDiff - breaks;
    frontTraction += frontTractionDiff + breaks;

    for(int q = InitialVehicleSetup::mWheelsAmount - 1; q >= 0; --q)
    {
        if(mIsCollided[q])
        {
            const TerrainData& terrain = mMeshProcesser->getTerrainData(mTerrainIndex[q]);

            Ogre::Vector3 impulseProj = PhysicsVehicle::findTangent(mWheelsAveragedNormal[q], 
                -matrixZColumn);//original data is left hand
            impulseProj.normalise();

            Ogre::Real cosSteer = Ogre::Math::Cos(mSteering[q]);
            Ogre::Real sinSteer = Ogre::Math::Sin(mSteering[q]);

            Ogre::Real suspensionTraction = mWheelsImpulseResulted[q] * 4.0f /
                                            (mInitialVehicleSetup.mChassisMass * mInitialVehicleSetup.mGravityVelocity);

            Ogre::Real suspensionTractionSpline = mInitialVehicleSetup.mSuspensionTraction.getPoint(suspensionTraction);

            Ogre::Vector3 velocityWithSteering = impulseProj.crossProduct(mWheelsAveragedNormal[q]);
            velocityWithSteering *= sinSteer;
            velocityWithSteering += cosSteer * impulseProj;

            Ogre::Vector3 wheelVelocity = velocityWithSteering * mVelocity[q];

            Ogre::Vector3 velocity = wheelVelocity - mWheelsImpulseTangent[q] * mInitialVehicleSetup.mChassisInvMass * 33.0f;

            Ogre::Real traction;

            if(q >= 2)//front
            {
                traction = frontTraction * suspensionTractionSpline;
            }
            else
            {
                traction = rearTraction * suspensionTractionSpline;
            }

            traction *= tractionScale;

            Ogre::Real velMod = velocity.length();
            velocity.normalise();

            Ogre::Real velocitySpline = mInitialVehicleSetup.mVelocitySpline[terrain.mVelocityIndex].getPoint(velMod);

            Ogre::Vector3 velocityProj = velocityWithSteering * velocityWithSteering.dotProduct(velocity);
            Ogre::Vector3 velocityProjDiff = velocity - velocityProj;
            //Ogre::Vector3 velocityProjWeight = (velocityProj * 6.0f - velocityProjDiff * -0.7f) * velMod;

            Ogre::Real velocitySpline2 = mInitialVehicleSetup.mVelocitySpline[terrain.mVelocityIndex].getPoint(velocityProj.length() * velMod);
            Ogre::Real velocitySpline3 = mInitialVehicleSetup.mVelocitySpline[terrain.mVelocityIndex].getPoint(velocityProjDiff.length() * velMod);

            Ogre::Vector3 velocityProjWeighted = velocityProj *
                                                (terrain.mLatitudinalGripMultiplier * velocitySpline2 +
                                                terrain.mCombinedMultiplier * velocitySpline) *
                                                traction;

            Ogre::Real finalTraction =  (velocitySpline3 * terrain.mParameter +
                                        velocitySpline * terrain.mParameter2) *
                                        traction;

            Ogre::Real accel;
            if(q >= 2)//front
            {
                accel = 1.15f - throttle * 0.15f;
            }
            else
            {
                accel = 1.1f - throttle * 0.1f;
            }

            Ogre::Vector3 velocityProjDiffWeight = velocityProjDiff * finalTraction * accel;

            //d.polubotko: TODO for user car only

            if(velMod > 0.0f)
            {
                Ogre::Vector3 finalVelocity = velocityProjWeighted + velocityProjDiffWeight;
                Ogre::Real finalVelocityDot = velocityWithSteering.dotProduct(finalVelocity);

                if(q >= 2)//front
                {
                    vehicle.mSteeringAdditionalParam -= matrixYColumn.crossProduct(velocityWithSteering).dotProduct(finalVelocity) * -0.02f;
                }

                vehicle.adjustImpulseInc(mWheelsSuspensionRot[q], finalVelocity);

                mVelocity[q] -= mInitialVehicleSetup.mWheelVelocitySub * finalVelocityDot;
            }
        }
    }
}

void PhysicsWheels::setSteering(Ogre::Real value)
{
    mSteering[2] = value;
    mSteering[3] = value;
}