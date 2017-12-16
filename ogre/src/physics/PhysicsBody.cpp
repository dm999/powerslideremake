
#include "PhysicsBody.h"

#include "Physics.h"
#include "PhysicsVehicle.h"
#include "../mesh/StaticMeshProcesser.h"

PhysicsBody::PhysicsBody(const InitialVehicleSetup& initialVehicleSetup, 
                        Physics* physics,
                        const StaticMeshProcesser* const meshProcesser)
    :   mInitialVehicleSetup(initialVehicleSetup),
        mPhysics(physics),
        mMeshProcesser(meshProcesser)
{}

PhysicsBody::~PhysicsBody()
{
}

void PhysicsBody::init()
{
    mBodyGlobal = Ogre::Vector3::ZERO;
}

void PhysicsBody::initStep()
{
    mBodyGlobalPrev = mBodyGlobal;
}

bool PhysicsBody::process(PhysicsVehicle& vehicle)
{
    bool isCollided = false;

    Ogre::Vector3 bodyRot = mInitialVehicleSetup.mCarRot * mInitialVehicleSetup.mBodyBasePos;

    mBodyGlobal = mInitialVehicleSetup.mCarGlobalPos + bodyRot;

    Ogre::Matrix3 carRot;
    mInitialVehicleSetup.mCarRot.ToRotationMatrix(carRot);
    Ogre::Vector3 matrixYColumn = carRot.GetColumn(1);
    bodyRot -= matrixYColumn * 1.2f;

    Ogre::Vector3 diffBodyPos = mBodyGlobal - mBodyGlobalPrev;

    Ogre::Vector3 deriveImpulse = diffBodyPos * mInitialVehicleSetup.mChassisMass;

    /*
    Ogre::Vector3 worldNormal;
    Ogre::Real distance = 0.0f;
    const btCollisionObject * staticObj;
    Ogre::Vector3 pointOnStatic;
    int triIndex;
    std::pair<int, int> address;
    */
    //if(mPhysics->findCollision(mBody.get(), staticObj, pointOnStatic, triIndex, worldNormal, distance))
    //if(mPhysics->isRigidBodyStatic(staticObj, address))

    const Ogre::Real tol = 0.00001f;

    Ogre::Vector3 averagedBodyPos = mBodyGlobal + mBodyGlobalPrev;
    averagedBodyPos *= 0.5f;

    Ogre::Real averageLen = diffBodyPos.length() * 0.5f + mInitialVehicleSetup.mBodyRadius;

    size_t foundIndex;
    Ogre::Real distance;
    bool isSphereCollided = mMeshProcesser->collideSphere(mBodyGlobal, mInitialVehicleSetup.mBodyRadius, tol,
                                    averagedBodyPos, averageLen,
                                    foundIndex, distance);
    if(isSphereCollided)
    {
        isCollided = true;

        const FoundCollision& collision = mMeshProcesser->getCollision(foundIndex);

        Ogre::Vector3 worldNormal = collision.mNormal;
        worldNormal.z = -worldNormal.z;//original data is left hand

        Ogre::Vector3 pointOnStaticA, pointOnStaticB, pointOnStaticC;
        mMeshProcesser->getGeoverts(collision, pointOnStaticA, pointOnStaticC, pointOnStaticB);

        Ogre::Vector2 pointOnStaticTextureA, pointOnStaticTextureB, pointOnStaticTextureC;
        mMeshProcesser->getGeovertsTexture(collision, pointOnStaticTextureA, pointOnStaticTextureC, pointOnStaticTextureB);

        const Ogre::Image * terrainMap = mMeshProcesser->getTerrainMap(mMeshProcesser->getTerrainName(collision));
        Ogre::Vector2 texCoords = PhysicsVehicle::findTexCoordinates(worldNormal, 
            mBodyGlobal,
            pointOnStaticA, pointOnStaticB, pointOnStaticC,
            pointOnStaticTextureA, pointOnStaticTextureB, pointOnStaticTextureC
            );
        char terrainType = mMeshProcesser->getTerrainType(terrainMap, texCoords);

        if(terrainType != -1)
        {
            assert(terrainType >= 0);
            assert(terrainType < TerrainData::mTerrainsAmount);

            const TerrainData& terrain = mMeshProcesser->getTerrainData(terrainType);

            Ogre::Vector3 tangent = PhysicsVehicle::findTangent(worldNormal, deriveImpulse);
            Ogre::Vector3 velocityTangent = tangent * mInitialVehicleSetup.mChassisInvMass * -33.0f;
            Ogre::Real impulseProj = -deriveImpulse.dotProduct(worldNormal);

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
            distance += 2.7f;

            Ogre::Real d_dv = mInitialVehicleSetup.mWheelUnderGroundDDV.getPoint(distance);
            Ogre::Real v_dv = mInitialVehicleSetup.mWheelUnderGroundVDV.getPoint(impulseProj);
            Ogre::Real v_v = mInitialVehicleSetup.mWheelUnderGroundVV.getPoint(impulseProj);
            Ogre::Real d_d = mInitialVehicleSetup.mWheelUnderGroundDD.getPoint(distance);

            Ogre::Real resultedImpulse = splineMultiplier * v_dv * d_dv + splineMultiplier * v_v + d_d;
            if(resultedImpulse > 100.0f) resultedImpulse = 100.f;

            velocityTangent += worldNormal * resultedImpulse;

            vehicle.adjustImpulseInc(bodyRot, velocityTangent);
        }
    }

    return isCollided;
}