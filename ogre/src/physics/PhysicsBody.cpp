
#include "PhysicsBody.h"

#include "Physics.h"
#include "PhysicsVehicle.h"
#include "../tools/PhysicsTools.h"
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
    if(mBody.get())
        mPhysics->removeCollisionObject(mBody.get());
}

void PhysicsBody::init()
{
    mBodyShape = CommonIncludes::shared_ptr<btSphereShape>(new btSphereShape(mInitialVehicleSetup.mBodyRadius));
    mBody = CommonIncludes::shared_ptr<btCollisionObject>(new btCollisionObject());

    mBody->setCollisionShape(mBodyShape.get());
    mBody->getWorldTransform().setOrigin(PhysicsTools::convert(mInitialVehicleSetup.mChassisPos + mInitialVehicleSetup.mChassisRot * mInitialVehicleSetup.mBodyBasePos));
    mBody->getWorldTransform().setRotation(PhysicsTools::convert(mInitialVehicleSetup.mChassisRot));

    mPhysics->addKinematicObject(mBody.get());

    mBodyGlobal = Ogre::Vector3::ZERO;
}

void PhysicsBody::initStep()
{
    mBodyGlobalPrev = mBodyGlobal;
}

void PhysicsBody::reposition(const Ogre::Vector3& posDiff)
{
    if(mBody.get())
        mBody->getWorldTransform().setOrigin(mBody->getWorldTransform().getOrigin() + PhysicsTools::convert(posDiff));
}

void PhysicsBody::rerotation(const Ogre::Vector3& chassisPos, const Ogre::Quaternion& rot)
{
    if(mBody.get())
    {
        mBody->getWorldTransform().setOrigin(PhysicsTools::convert(chassisPos + rot * mInitialVehicleSetup.mBodyBasePos));
        mBody->getWorldTransform().setRotation(PhysicsTools::convert(rot));
    }
}

void PhysicsBody::process(const Ogre::SceneNode& chassis, PhysicsVehicle& vehicle)
{
    if(mBody.get())
    {
        Ogre::Vector3 bodyRot = chassis.getOrientation() * mInitialVehicleSetup.mBodyBasePos;

        mBodyGlobal = chassis.getPosition() + bodyRot;

        Ogre::Matrix3 carRot;
        chassis.getOrientation().ToRotationMatrix(carRot);
        Ogre::Vector3 matrixYColumn = carRot.GetColumn(1);
        bodyRot -= matrixYColumn * 1.2f;

        Ogre::Vector3 diffBodyPos = mBodyGlobal - mBodyGlobalPrev;

        Ogre::Vector3 deriveImpulse = diffBodyPos * mInitialVehicleSetup.mChassisMass;

        Ogre::Vector3 worldNormal;
        Ogre::Real distance = 0.0f;
        const btCollisionObject * staticObj;
        Ogre::Vector3 pointOnStatic;
        int triIndex;
        std::pair<int, int> address;
        if(mPhysics->findCollision(mBody.get(), staticObj, pointOnStatic, triIndex, worldNormal, distance))
        {
            if(mPhysics->isRigidBodyStatic(staticObj, address))
            {
                char terrainType = mMeshProcesser->getTerrainType(address, triIndex, pointOnStatic);
                if(terrainType != -1)
                {
                    assert(terrainType >= 0);
                    assert(terrainType < TerrainData::mTerrainsAmount);

                    const TerrainData& terrain = mMeshProcesser->getTerrainData(terrainType);

                    //std::string terrainMap = mMeshProcesser->getBatchByAddress(address).mTerrainMap;
                    //terrainMap = terrainMap.substr(0, terrainMap.length() - 4);
                    //mWheelFrontLColliderIndex = terrainMap;
                    //mWheelFrontLColliderIndex = Conversions::DMToString(triIndex);
                    //Ogre::Vector2 texCoord = processer.getTextureCoordinateInTriangle(address, triIndex, cp.getPositionWorldOnB());
                    //mWheelFrontLColliderIndex = Conversions::DMToString(texCoord.x);
                    //mWheelFrontLColliderString = terrainMap + " " + Conversions::DMToString((size_t)terrainType);
                    //mWheelFrontLColliderIndex = terrainType;

                    Ogre::Vector3 tangent = vehicle.findTangent(worldNormal, deriveImpulse);
                    Ogre::Vector3 velocityTangent = tangent * mInitialVehicleSetup.mChassisInvMass * -33.0f;
                    Ogre::Real impulseProj = -deriveImpulse.dotProduct(worldNormal);

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

                    vehicle.adjustImpulseInc(bodyRot, velocityTangent);
                }
            }
        }
    }
}