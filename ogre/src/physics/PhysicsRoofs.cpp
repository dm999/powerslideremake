
#include "PhysicsRoofs.h"

#include "Physics.h"
#include "PhysicsVehicle.h"
#include "../mesh/StaticMeshProcesser.h"

PhysicsRoofs::PhysicsRoofs(const InitialVehicleSetup& initialVehicleSetup, 
                        Physics* physics,
                        const StaticMeshProcesser* const meshProcesser)
    :   mInitialVehicleSetup(initialVehicleSetup),
        mPhysics(physics),
        mMeshProcesser(meshProcesser)
{}

PhysicsRoofs::~PhysicsRoofs()
{
}

void PhysicsRoofs::init()
{
    Ogre::Vector3 carPos(mInitialVehicleSetup.mCarGlobalPos);
    carPos.z = -carPos.z;//original data is left hand

    for(int q = 0; q < InitialVehicleSetup::mRoofsAmount; ++q)
    {
        mRoofGlobal[q] = carPos;
        mRoofGlobalPrev[q] = carPos;
    }
}

void PhysicsRoofs::initStep()
{
    for(int q = 0; q < InitialVehicleSetup::mRoofsAmount; ++q)
    {
        mRoofGlobalPrev[q] = mRoofGlobal[q];
    }
}

void PhysicsRoofs::calcImpulses(const Ogre::Vector3& impulseRot, const Ogre::Vector3& impulseRotPrev, const Ogre::Vector3& normalisedImpulseRot,
                               const Ogre::Vector3& impulseLinear,
                               Ogre::Real recipMomentProj)
{


    if(impulseRot.length() == 0.0f)
    {
        for(int q = 0; q < InitialVehicleSetup::mRoofsAmount; ++q)
        {
            mRoofImpulseLinear[q] = impulseLinear;
        }
    }
    else
    {
        Ogre::Matrix3 carRot;
        mInitialVehicleSetup.mCarRot.ToRotationMatrix(carRot);

        Ogre::Matrix3 carRotPS;
        Ogre::Vector3 carRotV[3];//original data is left hand
        carRotV[0] = Ogre::Vector3(carRot[0][0], carRot[1][0], -carRot[2][0]);
        carRotV[1] = Ogre::Vector3(carRot[0][1], carRot[1][1], -carRot[2][1]);
        carRotV[2] = Ogre::Vector3(-carRot[0][2], -carRot[1][2], carRot[2][2]);
        carRotPS.FromAxes(carRotV[0], carRotV[1], carRotV[2]);

        for(int q = 0; q < InitialVehicleSetup::mRoofsAmount; ++q)
        {
            Ogre::Vector3 roofRot = carRotPS * mInitialVehicleSetup.mRoofPos[q];
            Ogre::Vector3 tangent = PhysicsVehicle::findTangent(normalisedImpulseRot, roofRot);
            if(tangent.x != 0.0f || tangent.y != 0.0f || tangent.z != 0.0f)
            {
                Ogre::Real recipAccel = mInitialVehicleSetup.mChassisMass * recipMomentProj;
                mRoofImpulseLinear[q] = tangent.crossProduct(impulseRotPrev) * recipAccel + impulseLinear;
            }
            else
            {
                mRoofImpulseLinear[q] = impulseLinear;
            }
        }
    }
}

bool PhysicsRoofs::process(PhysicsVehicle& vehicle)
{
    Ogre::Vector3 carPos = mInitialVehicleSetup.mCarGlobalPos;
    carPos.z = -carPos.z;//original data is left hand

    Ogre::Matrix3 carRot;
    mInitialVehicleSetup.mCarRot.ToRotationMatrix(carRot);

    Ogre::Matrix3 carRotPS;
    Ogre::Vector3 carRotV[3];//original data is left hand
    carRotV[0] = Ogre::Vector3(carRot[0][0], carRot[1][0], -carRot[2][0]);
    carRotV[1] = Ogre::Vector3(carRot[0][1], carRot[1][1], -carRot[2][1]);
    carRotV[2] = Ogre::Vector3(-carRot[0][2], -carRot[1][2], carRot[2][2]);
    carRotPS.FromAxes(carRotV[0], carRotV[1], carRotV[2]);

    bool isCollided = false;

    for(int q = 0; q < InitialVehicleSetup::mRoofsAmount; ++q)
    {

        Ogre::Vector3 roofRot = carRotPS * mInitialVehicleSetup.mRoofPos[q];
        mRoofGlobal[q] = carPos + roofRot;

        roofRot -= carRotV[1] * 1.2f;

        Ogre::Vector3 diffRoofPos = mRoofGlobal[q] - mRoofGlobalPrev[q];

        const Ogre::Real tol = 0.00001f;

        Ogre::Vector3 averagedRoofPos = mRoofGlobal[q] + mRoofGlobalPrev[q];
        averagedRoofPos *= 0.5f;

        Ogre::Real averageLen = diffRoofPos.length() * 0.5f + mInitialVehicleSetup.mRoofRadius[q];

        size_t foundIndex;
        Ogre::Real distance;
        bool isSphereCollided = mMeshProcesser->collideSphere(mRoofGlobal[q], mInitialVehicleSetup.mRoofRadius[q], tol,
                                        averagedRoofPos, averageLen,
                                        foundIndex, distance);
        if(isSphereCollided)
        {
            isCollided = true;

            const FoundCollision& collision = mMeshProcesser->getCollision(foundIndex);

            Ogre::Vector3 worldNormal = collision.mNormal;

            Ogre::Vector3 pointOnStaticA, pointOnStaticB, pointOnStaticC;
            mMeshProcesser->getGeoverts(collision, pointOnStaticA, pointOnStaticC, pointOnStaticB);

            Ogre::Vector2 pointOnStaticTextureA, pointOnStaticTextureB, pointOnStaticTextureC;
            mMeshProcesser->getGeovertsTexture(collision, pointOnStaticTextureA, pointOnStaticTextureC, pointOnStaticTextureB);

            const Ogre::Image * terrainMap = mMeshProcesser->getTerrainMap(mMeshProcesser->getTerrainName(collision));
            Ogre::Vector2 texCoords = PhysicsVehicle::findTexCoordinates(worldNormal, 
                mRoofGlobal[q],
                pointOnStaticA, pointOnStaticB, pointOnStaticC,
                pointOnStaticTextureA, pointOnStaticTextureB, pointOnStaticTextureC
                );
            char terrainType = mMeshProcesser->getTerrainType(terrainMap, texCoords);

            if(terrainType != -1)
            {
                assert(terrainType >= 0);
                assert(terrainType < TerrainData::mTerrainsAmount);

                const TerrainData& terrain = mMeshProcesser->getTerrainData(terrainType);

                Ogre::Vector3 tangent = PhysicsVehicle::findTangent(worldNormal, mRoofImpulseLinear[q]);
                Ogre::Vector3 velocityTangent = tangent * mInitialVehicleSetup.mChassisInvMass * -33.0f;
                Ogre::Real impulseProj = -mRoofImpulseLinear[q].dotProduct(worldNormal);

                Ogre::Real splineMultiplier = 1.0f;

                if(impulseProj < 0.0f)
                {
                    impulseProj = -impulseProj;
                    splineMultiplier = -mInitialVehicleSetup.mRisingDamp * 0.8f;
                }

                Ogre::Real velocityMod = velocityTangent.length();
                if(velocityMod > 0.0f)
                {
                    velocityTangent.normalise();
                    Ogre::Real velocityMultiplier = mInitialVehicleSetup.mVelocitySpline[terrain.mVelocityIndex].getPoint(velocityMod);
                    velocityTangent *= velocityMultiplier;
                }

                distance = -distance;
                distance += 1.7f;

                Ogre::Real d_dv = mInitialVehicleSetup.mWheelUnderGroundDDV.getPoint(distance);
                Ogre::Real v_dv = mInitialVehicleSetup.mWheelUnderGroundVDV.getPoint(impulseProj);
                Ogre::Real v_v = mInitialVehicleSetup.mWheelUnderGroundVV.getPoint(impulseProj);
                Ogre::Real d_d = mInitialVehicleSetup.mWheelUnderGroundDD.getPoint(distance);

                Ogre::Real resultedImpulse = splineMultiplier * v_dv * d_dv + splineMultiplier * v_v + d_d;
                if(resultedImpulse > 100.0f) resultedImpulse = 100.f;

                velocityTangent += worldNormal * resultedImpulse;

                vehicle.adjustImpulseInc(roofRot, velocityTangent);
            }
        }
    }

    return isCollided;
}