
#include "PhysicsRoofs.h"

#include "Physics.h"
#include "PhysicsVehicle.h"
#include "../tools/PhysicsTools.h"
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
    for(int q = 0; q < InitialVehicleSetup::mRoofsAmount; ++q)
    {
        if(mRoof[q].get())
            mPhysics->removeCollisionObject(mRoof[q].get());
    }
}

void PhysicsRoofs::init(const Ogre::Vector3& chassisPos)
{
    for(int q = 0; q < InitialVehicleSetup::mRoofsAmount; ++q)
    {
        mRoofShape[q] = CommonIncludes::shared_ptr<btSphereShape>(new btSphereShape(mInitialVehicleSetup.mRoofRadius[q]));
        mRoof[q] = CommonIncludes::shared_ptr<btCollisionObject>(new btCollisionObject());
        mRoof[q]->setCollisionShape(mRoofShape[q].get());
        mRoof[q]->getWorldTransform().setOrigin(PhysicsTools::convert(mInitialVehicleSetup.mChassisPos + mInitialVehicleSetup.mChassisRot * mInitialVehicleSetup.mRoofPos[q]));
        mRoof[q]->getWorldTransform().setRotation(PhysicsTools::convert(mInitialVehicleSetup.mChassisRot));

        mPhysics->addKinematicObject(mRoof[q].get());
    }

    for(int q = 0; q < InitialVehicleSetup::mRoofsAmount; ++q)
    {
        mRoofGlobal[q] = chassisPos;
        mRoofGlobalPrev[q] = chassisPos;
    }
}

void PhysicsRoofs::initStep()
{
    for(int q = 0; q < InitialVehicleSetup::mRoofsAmount; ++q)
    {
        mRoofGlobalPrev[q] = mRoofGlobal[q];
    }
}

void PhysicsRoofs::reposition(const Ogre::Vector3& posDiff)
{
    for(int q = 0; q < InitialVehicleSetup::mRoofsAmount; ++q)
    {
        if(mRoof[q].get())
            mRoof[q]->getWorldTransform().setOrigin(mRoof[q]->getWorldTransform().getOrigin() + PhysicsTools::convert(posDiff));
    }
}

void PhysicsRoofs::rerotation(const Ogre::Vector3& chassisPos, const Ogre::Quaternion& rot)
{
    for(int q = 0; q < InitialVehicleSetup::mRoofsAmount; ++q)
    {
        if(mRoof[q].get())
        {
            mRoof[q]->getWorldTransform().setOrigin(PhysicsTools::convert(chassisPos + rot * mInitialVehicleSetup.mRoofPos[q]));
            mRoof[q]->getWorldTransform().setRotation(PhysicsTools::convert(rot));
        }
    }

}

void PhysicsRoofs::calcImpulses(const Ogre::Vector3& impulseRot, const Ogre::Vector3& impulseRotPrev, const Ogre::Vector3& normalisedImpulseRot,
                               const Ogre::Vector3& impulseLinear,
                               Ogre::Real recipMomentProj,
                               const Ogre::Quaternion& carRot,
                               const PhysicsVehicle& vehicle)
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
        Ogre::Real force = mInitialVehicleSetup.mChassisMass * recipMomentProj;

        for(int q = 0; q < InitialVehicleSetup::mRoofsAmount; ++q)
        {
            Ogre::Vector3 roofRot = carRot * mInitialVehicleSetup.mRoofPos[q];
            Ogre::Vector3 tangent = vehicle.findTangent(normalisedImpulseRot, roofRot);
            if(tangent.x != 0.0f || tangent.y != 0.0f || tangent.z != 0.0f)
            {
                mRoofImpulseLinear[q] = tangent.crossProduct(impulseRotPrev) * force + impulseLinear;
            }
            else
            {
                mRoofImpulseLinear[q] = impulseLinear;
            }
        }

    }
}

void PhysicsRoofs::process(const Ogre::SceneNode& chassis, PhysicsVehicle& vehicle)
{
    for(int q = 0; q < InitialVehicleSetup::mRoofsAmount; ++q)
    {
        if(mRoof[q].get())
        {
            Ogre::Vector3 roofRot = chassis.getOrientation() * mInitialVehicleSetup.mRoofPos[q];
            mRoofGlobal[q] = chassis.getPosition() + roofRot;

            Ogre::Matrix3 carRot;
            chassis.getOrientation().ToRotationMatrix(carRot);
            Ogre::Vector3 matrixYColumn = carRot.GetColumn(1);
            roofRot -= matrixYColumn * 1.2f;

            Ogre::Vector3 worldNormal;
            Ogre::Real distance = 0.0f;
            const btCollisionObject * staticObj;
            Ogre::Vector3 pointOnStatic;
            int triIndex;
            std::pair<int, int> address;
            if(mPhysics->findCollision(mRoof[q].get(), staticObj, pointOnStatic, triIndex, worldNormal, distance))
            {
                if(mPhysics->isRigidBodyStatic(staticObj, address))
                {
                    char terrainType = mMeshProcesser->getTerrainType(address, triIndex, pointOnStatic);
                    if(terrainType != -1)
                    {
                        assert(terrainType >= 0);
                        assert(terrainType < TerrainData::mTerrainsAmount);

                        const TerrainData& terrain = mMeshProcesser->getTerrainData(terrainType);

                        Ogre::Vector3 tangent = vehicle.findTangent(worldNormal, mRoofImpulseLinear[q]);
                        Ogre::Vector3 velocityTangent = tangent * mInitialVehicleSetup.mChassisInvMass * -33.0f;
                        Ogre::Real impulseProj = -mRoofImpulseLinear[q].dotProduct(worldNormal);

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

                        Ogre::Real resultedImpulse = v_dv * d_dv + v_v + d_d;
                        if(resultedImpulse > 100.0f) resultedImpulse = 100.f;

                        velocityTangent += worldNormal * resultedImpulse;

                        vehicle.adjustImpulseInc(roofRot, velocityTangent);
                    }
                }
            }
        }
    }
}