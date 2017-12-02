
#include "PhysicsWheels.h"

#include "Physics.h"
#include "PhysicsVehicle.h"
#include "../tools/PhysicsTools.h"
#include "../mesh/StaticMeshProcesser.h"

PhysicsWheels::PhysicsWheels(const InitialVehicleSetup& initialVehicleSetup, 
                        Physics* physics,
                        const StaticMeshProcesser* const meshProcesser)
    :   mInitialVehicleSetup(initialVehicleSetup),
        mPhysics(physics),
        mMeshProcesser(meshProcesser)
{}

PhysicsWheels::~PhysicsWheels()
{
    for(int q = 0; q < InitialVehicleSetup::mWheelsAmount; ++q)
    {
        if(mWheel[q].get())
            mPhysics->removeCollisionObject(mWheel[q].get());
    }
}

void PhysicsWheels::init(const Ogre::Vector3& chassisPos, Ogre::SceneNode *wheelNodes[InitialVehicleSetup::mWheelsAmount])
{
    for(int q = 0; q < InitialVehicleSetup::mWheelsAmount; ++q)
    {
        mWheelNodes[q] = wheelNodes[q];
        mWheelShape[q] = CommonIncludes::shared_ptr<btSphereShape>(new btSphereShape(mInitialVehicleSetup.mWheelRadius[q]));
        mWheel[q] = CommonIncludes::shared_ptr<btCollisionObject>(new btCollisionObject());
        mWheel[q]->setCollisionShape(mWheelShape[q].get());
        mWheel[q]->getWorldTransform().setOrigin(PhysicsTools::convert(mInitialVehicleSetup.mChassisPos + mInitialVehicleSetup.mChassisRot * mInitialVehicleSetup.mConnectionPointWheel[q]));
        mWheel[q]->getWorldTransform().setRotation(PhysicsTools::convert(mInitialVehicleSetup.mChassisRot));

        mPhysics->addKinematicObject(mWheel[q].get());
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
    }

}

void PhysicsWheels::reposition(const Ogre::Vector3& chassisPos, const Ogre::Quaternion& chassisRot)
{
    for(int q = 0; q < InitialVehicleSetup::mWheelsAmount; ++q)
    {
        if(mWheel[q].get())
        {
            Ogre::Vector3 connectionPoint = mInitialVehicleSetup.mConnectionPointWheel[q];
            connectionPoint.y -= mSuspensionHeight[q];
            //mWheelNodes[q]->setPosition(mWheelsSuspensionGlobal[q]);
            mWheelNodes[q]->setPosition(chassisPos + chassisRot * connectionPoint);
            mWheel[q]->getWorldTransform().setOrigin(PhysicsTools::convert(chassisPos + chassisRot * connectionPoint));
        }
    }
}

void PhysicsWheels::rerotation(const Ogre::Vector3& chassisPos, const Ogre::Quaternion& chassisRot)
{
    for(int q = 0; q < InitialVehicleSetup::mWheelsAmount; ++q)
    {
        if(mWheel[q].get())
        {
            Ogre::Vector3 connectionPoint = mInitialVehicleSetup.mConnectionPointWheel[q];
            connectionPoint.y -= mSuspensionHeight[q];
            //mWheelNodes[q]->setPosition(mWheelsSuspensionGlobal[q]);
            mWheelNodes[q]->setPosition(chassisPos + chassisRot * connectionPoint);
            mWheelNodes[q]->setOrientation(chassisRot);
            mWheel[q]->getWorldTransform().setOrigin(PhysicsTools::convert(chassisPos + chassisRot * connectionPoint));
            mWheel[q]->getWorldTransform().setRotation(PhysicsTools::convert(chassisRot));
        }
    }

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
            Ogre::Vector3 tangent = vehicle.findTangent(normalisedImpulseRot, mWheelsSuspensionPoint[q]);
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

        mMeshProcesser->collideSphere(mWheelsSuspensionPoint2Global[q], mInitialVehicleSetup.mWheelRadius[q], tol,
                                        averagedSuspensionPos, averageLen);

        if(mWheel[q].get())
        {
            Ogre::Vector3 worldNormal;
            Ogre::Real distance = 0.0f;
            const btCollisionObject * staticObj;
            Ogre::Vector3 pointOnStatic;
            int triIndex;
            std::pair<int, int> address;
            if(mPhysics->findCollision(mWheel[q].get(), staticObj, pointOnStatic, triIndex, worldNormal, distance))
            {
                if(mPhysics->isRigidBodyStatic(staticObj, address))
                {
                    char terrainType = mMeshProcesser->getTerrainType(address, triIndex, pointOnStatic);
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
                            Ogre::Real dotPStatic = pointOnStatic.dotProduct(worldNormal);
                            suspHeight = (dotPWheels - dotPStatic + mInitialVehicleSetup.mWheelRadius[q]) / projUp;
                        }

                        suspHeight = calcSuspensionLength(suspHeight, q);

                        mSuspensionHeight[q] = suspHeight;

                        Ogre::Vector3 suspLocal = mInitialVehicleSetup.mConnectionPointWheel[q];
                        suspLocal.y -= suspHeight;
                        mWheelsSuspensionRot[q] = carRotQ * suspLocal;
                        mWheelsSuspensionGlobal[q] = carPos + mWheelsSuspensionRot[q];

                        mWheelsImpulseTangent[q] = vehicle.findTangent(worldNormal, mWheelsImpulseLinear[q]);

                        distance = -distance;
                        if(projUp < 0.0f)
                            projUp = 0.0f;
                        Ogre::Real distWeight = (1.0f - projUp) * mInitialVehicleSetup.mMaxTravel;
                        distance -= (distWeight * -0.7f);

                        Ogre::Real suspWeight;
                        if(q >= 2)//front wheels
                        {
                            suspWeight = mInitialVehicleSetup.mFrontSuspension;
                        }
                        else
                        {
                            suspWeight = 1.0f;
                        }

                        if(distance < 0.0f)distance = 0.0f;

                        Ogre::Real risingDampWeight = 1.0f;
                        Ogre::Real impulseProj = -mWheelsImpulseLinear[q].dotProduct(worldNormal);
                        if(impulseProj < 0.0f)
                        {
                            impulseProj = -impulseProj;
                            risingDampWeight = -mInitialVehicleSetup.mRisingDamp;
                        }

                        Ogre::Real d_dv = mInitialVehicleSetup.mWheelUnderGroundDDV.getPoint(distance);
                        Ogre::Real v_dv = mInitialVehicleSetup.mWheelUnderGroundVDV.getPoint(impulseProj);
                        Ogre::Real d_d = mInitialVehicleSetup.mWheelUnderGroundDD.getPoint(distance);
                        Ogre::Real v_v = mInitialVehicleSetup.mWheelUnderGroundVV.getPoint(impulseProj);

                        Ogre::Real resultedImpulse = (v_dv * d_dv * risingDampWeight) + 
                            (d_d * mSpringVal[q]) + 
                            (v_v * risingDampWeight);
                        resultedImpulse *= suspWeight;

                        if(resultedImpulse < 0.0f) resultedImpulse = 0.0f;
                        if(resultedImpulse > 100.0f) resultedImpulse = 100.0f;

                        vehicle.adjustImpulseInc(mWheelsSuspensionRot[q], worldNormal * resultedImpulse);
                    }
                }
            }
        }
    }
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