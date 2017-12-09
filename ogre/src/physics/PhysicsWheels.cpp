
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
    }
}

void PhysicsWheels::initStep(const Ogre::Vector3& chassisPos, const Ogre::Quaternion& chassisRot)
{
    Ogre::Matrix3 rotMatrix;
    chassisRot.ToRotationMatrix(rotMatrix);
    Ogre::Vector3 rotAxisY = rotMatrix.GetColumn(1);

    for(int q = 0; q < InitialVehicleSetup::mWheelsAmount; ++q)
    {
        mGlobalPos[q] = chassisPos + chassisRot * mInitialVehicleSetup.mConnectionPointWheel[q];

        Ogre::Real maxTravel = mInitialVehicleSetup.mMaxTravel * 0.5f;
        Ogre::Vector3 rotVal = maxTravel * rotAxisY;

        mWheelsSuspensionPointGlobalPrev[q] = mWheelsSuspensionPointGlobal[q];
        mWheelsSuspensionPointGlobal[q] = mGlobalPos[q] - rotVal;
        mWheelsSuspensionPoint2Global[q] = mWheelsSuspensionPointGlobal[q] - rotVal;
        mWheelsSuspensionPoint[q] = mWheelsSuspensionPoint2Global[q] - chassisPos;

        //mWheelsSuspensionGlobalPrev[q] = mWheelsSuspensionGlobal[q];
        mSuspensionHeightPrev[q] = mSuspensionHeight[q];

        mIsCollided[q] = false;
    }

}

void PhysicsWheels::reposition(const Ogre::Vector3& chassisPos, const Ogre::Quaternion& chassisRot)
{
    for(int q = 0; q < InitialVehicleSetup::mWheelsAmount; ++q)
    {
        Ogre::Vector3 connectionPoint = mInitialVehicleSetup.mConnectionPointWheel[q];
        connectionPoint.y -= mSuspensionHeight[q];
        //mWheelNodes[q]->setPosition(mWheelsSuspensionGlobal[q]);
        mWheelNodes[q]->setPosition(chassisPos + chassisRot * connectionPoint);
    }
}

void PhysicsWheels::rerotation(const Ogre::Vector3& chassisPos, const Ogre::Quaternion& chassisRot)
{
    for(int q = 0; q < InitialVehicleSetup::mWheelsAmount; ++q)
    {
        Ogre::Vector3 connectionPoint = mInitialVehicleSetup.mConnectionPointWheel[q];
        connectionPoint.y -= mSuspensionHeight[q];
        //mWheelNodes[q]->setPosition(mWheelsSuspensionGlobal[q]);
        mWheelNodes[q]->setPosition(chassisPos + chassisRot * connectionPoint);
        mWheelNodes[q]->setOrientation(chassisRot);
    }

}

bool PhysicsWheels::isAnyCollided() const
{
    bool ret = false;

    for(int q = 0; q < InitialVehicleSetup::mWheelsAmount; ++q)
    {
        ret |= mIsCollided[q];
    }

    return ret;
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

void PhysicsWheels::process(const Ogre::SceneNode& chassis, PhysicsVehicle& vehicle)
{

    Ogre::Matrix3 carRot;
    Ogre::Quaternion carRotQ = chassis.getOrientation();
    carRotQ.ToRotationMatrix(carRot);
    Ogre::Vector3 matrixYColumn = carRot.GetColumn(1);
    Ogre::Vector3 carPos = chassis.getPosition();

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
            /*
            Ogre::Vector3 worldNormal;
            Ogre::Real distance = 0.0f;
            const btCollisionObject * staticObj;
            Ogre::Vector3 pointOnStatic;
            int triIndex;
            std::pair<int, int> address;
            */
            //if(mPhysics->findCollision(mWheel[q].get(), staticObj, pointOnStatic, triIndex, worldNormal, distance))
            //if(mPhysics->isRigidBodyStatic(staticObj, address))

            const FoundCollision& collision = mMeshProcesser->getCollision(foundIndex);
            Ogre::Vector3 pointOnStaticA, pointOnStaticB, pointOnStaticC;
            mMeshProcesser->getGeoverts(collision, pointOnStaticA, pointOnStaticC, pointOnStaticB);

            Ogre::Vector3 worldNormal = collision.mNormal;
            worldNormal.z = -worldNormal.z;//original data is left hand

            char terrainType = 0; //mMeshProcesser->getTerrainType(address, triIndex, pointOnStatic);
            if(terrainType != -1)
            {
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
                mWheelsSuspensionRot[q] = carRotQ * suspLocal;
                mWheelsSuspensionGlobal[q] = carPos + mWheelsSuspensionRot[q];

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

                vehicle.adjustImpulseInc(mWheelsSuspensionRot[q], averagedNormal * resultedImpulse);
            }

            mIsCollided[q] = true;
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