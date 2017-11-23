
#include "PhysicsVehicle.h"

#include "BulletCollision/CollisionShapes/btSphereShape.h"
#include "BulletCollision/CollisionDispatch/btCollisionObject.h"

#include "Physics.h"
#include "../tools/PhysicsTools.h"
#include "../mesh/StaticMeshProcesser.h"

PhysicsVehicle::PhysicsVehicle(Physics* physics, 
                               StaticMeshProcesser * meshProesser,
                               const InitialVehicleSetup& initialVehicleSetup, 
                               Ogre::SceneNode *wheelNodes[InitialVehicleSetup::mWheelsAmount], Ogre::SceneNode *chassis)
: mPhysics(physics),
    mMeshProcesser(meshProesser),
    mChassis(chassis),
    mInitialVehicleSetup(initialVehicleSetup)
{
    for(int q = 0; q < InitialVehicleSetup::mWheelsAmount; ++q)
    {
        mWheelNodes[q] = wheelNodes[q];
    }

    //wheels
    for(int q = 0; q < InitialVehicleSetup::mWheelsAmount; ++q)
    {
        mWheelShape[q] = CommonIncludes::shared_ptr<btSphereShape>(new btSphereShape(mInitialVehicleSetup.mWheelRadius[q]));
        mWheel[q] = CommonIncludes::shared_ptr<btCollisionObject>(new btCollisionObject());
        mWheel[q]->setCollisionShape(mWheelShape[q].get());
        mWheel[q]->getWorldTransform().setOrigin(PhysicsTools::convert(mInitialVehicleSetup.mChassisPos + mInitialVehicleSetup.mChassisRot * mInitialVehicleSetup.mConnectionPointWheel[q]));
        mWheel[q]->getWorldTransform().setRotation(PhysicsTools::convert(mInitialVehicleSetup.mChassisRot));

        //mPhysics->addKinematicObject(mWheel[q].get());
    }
    //wheels END

    //roof
    for(int q = 0; q < InitialVehicleSetup::mRoofsAmount; ++q)
    {
        mRoofShape[q] = CommonIncludes::shared_ptr<btSphereShape>(new btSphereShape(mInitialVehicleSetup.mRoofRadius[q]));
        mRoof[q] = CommonIncludes::shared_ptr<btCollisionObject>(new btCollisionObject());
        mRoof[q]->setCollisionShape(mRoofShape[q].get());
        mRoof[q]->getWorldTransform().setOrigin(PhysicsTools::convert(mInitialVehicleSetup.mChassisPos + mInitialVehicleSetup.mChassisRot * mInitialVehicleSetup.mRoofPos[q]));
        mRoof[q]->getWorldTransform().setRotation(PhysicsTools::convert(mInitialVehicleSetup.mChassisRot));

        mPhysics->addKinematicObject(mRoof[q].get());
    }
    //roof END

    //body
    mBodyShape = CommonIncludes::shared_ptr<btSphereShape>(new btSphereShape(mInitialVehicleSetup.mBodyRadius));
    mBody = CommonIncludes::shared_ptr<btCollisionObject>(new btCollisionObject());

    mBody->setCollisionShape(mBodyShape.get());
    mBody->getWorldTransform().setOrigin(PhysicsTools::convert(mInitialVehicleSetup.mChassisPos + mInitialVehicleSetup.mChassisRot * mInitialVehicleSetup.mBodyBasePos));
    mBody->getWorldTransform().setRotation(PhysicsTools::convert(mInitialVehicleSetup.mChassisRot));

    mPhysics->addKinematicObject(mBody.get());
    //body END

    mImpulseLinear = mInitialVehicleSetup.mInitialImpulseLinear;
    mImpulseLinearInc = mInitialVehicleSetup.mInitialImpulseLinearInc;
    mImpulseRot = mInitialVehicleSetup.mInitialImpulseRot;
    mImpulseRotInc = mInitialVehicleSetup.mInitialImpulseRotInc;

    for(int q = 0; q < InitialVehicleSetup::mRoofsAmount; ++q)
    {
        mRoofGlobal[q] = mChassis->getPosition();
        mRoofGlobalPrev[q] = mChassis->getPosition();
    }

    mBodyGlobal = Ogre::Vector3::ZERO;
}

PhysicsVehicle::~PhysicsVehicle()
{
    for(int q = 0; q < InitialVehicleSetup::mWheelsAmount; ++q)
    {
        //mPhysics->removeCollisionObject(mWheel[q].get());
    }

    for(int q = 0; q < InitialVehicleSetup::mRoofsAmount; ++q)
    {
        mPhysics->removeCollisionObject(mRoof[q].get());
    }

    mPhysics->removeCollisionObject(mBody.get());
}

void PhysicsVehicle::timeStep()
{
    initStep();

    //do AI
    //do steering adj
    //do suspension init

    integrate();

    Ogre::Vector3 posDiff = mImpulseLinear * mInitialVehicleSetup.mChassisInvMass;
    reposition(posDiff);

    Ogre::Real linearImpulseMod = mImpulseLinear.length();

    mVehicleVelocityMod = linearImpulseMod * mInitialVehicleSetup.mChassisInvMass;

    if(mVehicleVelocityMod < 0.0001f) mVehicleVelocityMod = 0.0001f;

    Ogre::Real airDensTransCoeff = -1.0f * mInitialVehicleSetup.mAirDensityTranslation * linearImpulseMod + 1.0f;
    mImpulseLinear *= airDensTransCoeff;

    Ogre::Real rotImpulseMod = mImpulseRot.length();
    if(rotImpulseMod > 0.00001f)
    {
        Ogre::Vector3 normalisedImpulseRot = mImpulseRot.normalisedCopy();
        Ogre::Real momentProj = momentOfInertiaProj(normalisedImpulseRot);

        Ogre::Real rotAngle = 1.0f / momentProj * rotImpulseMod;

        Ogre::Matrix3 carRot;
        mChassis->getOrientation().ToRotationMatrix(carRot);

        Ogre::Vector3 rotMatrixAxisY = carRot.GetColumn(1);
        Ogre::Vector3 rotMatrixAxisZ = carRot.GetColumn(2);
        createRotMatrix(rotMatrixAxisY, normalisedImpulseRot, rotAngle);
        createRotMatrix(rotMatrixAxisZ, normalisedImpulseRot, rotAngle);
        Ogre::Vector3 rotMatrixAxisX = rotMatrixAxisY.crossProduct(rotMatrixAxisZ);
        carRot.SetColumn(0, rotMatrixAxisX);
        carRot.SetColumn(1, rotMatrixAxisY);
        carRot.SetColumn(2, rotMatrixAxisZ);
        rerotation(Ogre::Quaternion (carRot));

        mImpulseRot *= mInitialVehicleSetup.mAirDensityRot;
    }


    mImpulseLinearInc.y += mInitialVehicleSetup.mChassisMass * (-mInitialVehicleSetup.mGravityForce);

    integrate();

    calcWheelRoofImpulses();

    //do wheels
    processRoof();
    processBody();
    //do flip restore
    //do transmission
    //do physics

    //mImpulseLinearInc.y -= mInitialVehicleSetup.mChassisMass * (-mInitialVehicleSetup.mGravityForce);
    //mImpulseLinearInc.x += mInitialVehicleSetup.mChassisMass * mInitialVehicleSetup.mGravityForce;
    //mImpulseLinearInc.y += mInitialVehicleSetup.mChassisMass * mInitialVehicleSetup.mGravityForce;
    //mImpulseLinearInc.z += mInitialVehicleSetup.mChassisMass * mInitialVehicleSetup.mGravityForce;

}

void PhysicsVehicle::initStep()
{
    for(int q = 0; q < InitialVehicleSetup::mRoofsAmount; ++q)
    {
        mRoofGlobalPrev[q] = mRoofGlobal[q];
    }

    mBodyGlobalPrev = mBodyGlobal;
}

void PhysicsVehicle::reposition(const Ogre::Vector3& posDiff)
{
    for(int q = 0; q < InitialVehicleSetup::mRoofsAmount; ++q)
    {
        mRoof[q]->getWorldTransform().setOrigin(mRoof[q]->getWorldTransform().getOrigin() + PhysicsTools::convert(posDiff));
    }

    for(int q = 0; q < InitialVehicleSetup::mWheelsAmount; ++q)
    {
        mWheelNodes[q]->translate(posDiff);
        mWheel[q]->getWorldTransform().setOrigin(mWheel[q]->getWorldTransform().getOrigin() + PhysicsTools::convert(posDiff));
    }

    mChassis->translate(posDiff);

    mBody->getWorldTransform().setOrigin(mBody->getWorldTransform().getOrigin() + PhysicsTools::convert(posDiff));
}

void PhysicsVehicle::rerotation(const Ogre::Quaternion& rot)
{
    Ogre::Vector3 chassisPos = mChassis->getPosition();

    for(int q = 0; q < InitialVehicleSetup::mRoofsAmount; ++q)
    {
        mRoof[q]->getWorldTransform().setOrigin(PhysicsTools::convert(chassisPos + rot * mInitialVehicleSetup.mRoofPos[q]));
        mRoof[q]->getWorldTransform().setRotation(PhysicsTools::convert(rot));
    }

    for(int q = 0; q < InitialVehicleSetup::mWheelsAmount; ++q)
    {
        mWheelNodes[q]->setPosition(chassisPos + rot * mInitialVehicleSetup.mConnectionPointWheel[q]);
        mWheelNodes[q]->setOrientation(rot);
        mWheel[q]->getWorldTransform().setOrigin(PhysicsTools::convert(mWheelNodes[q]->getPosition()));
        mWheel[q]->getWorldTransform().setRotation(PhysicsTools::convert(rot));
    }

    mChassis->setOrientation(rot);

    mBody->getWorldTransform().setOrigin(PhysicsTools::convert(chassisPos + rot * mInitialVehicleSetup.mBodyBasePos));
    mBody->getWorldTransform().setRotation(PhysicsTools::convert(rot));
}

void PhysicsVehicle::integrate()
{
    mImpulseLinear += mImpulseLinearInc;
    mImpulseLinearInc = Ogre::Vector3::ZERO;

    mImpulseRot += mImpulseRotInc;
    mImpulseRotInc = Ogre::Vector3::ZERO;
}

Ogre::Real PhysicsVehicle::momentOfInertiaProj(const Ogre::Vector3& axis)const
{
    Ogre::Real ret = 0.0f;

    Ogre::Matrix3 carRot;
    mChassis->getOrientation().ToRotationMatrix(carRot);
    Ogre::Vector3 tmpRes = carRot * axis;

    ret = mInitialVehicleSetup.mMomentOfInertia.dotProduct(Ogre::Vector3(tmpRes * tmpRes));

    return ret;
}

Ogre::Vector3 PhysicsVehicle::findTangent(const Ogre::Vector3& normal, const Ogre::Vector3& input) const
{
    //40E8C0
    Ogre::Real dot = input.dotProduct(normal);
    return input - dot * normal;
}

void PhysicsVehicle::calcWheelRoofImpulses()
{
    mImpulseRotPrev = mImpulseRot;
    Ogre::Vector3 normalisedImpulseRot = mImpulseRot.normalisedCopy();
    Ogre::Real momentProj = momentOfInertiaProj(normalisedImpulseRot);

    Ogre::Real recipMomentProj = 1.0f / momentProj;

    if(mImpulseRot.length() == 0.0f)
    {
        //do calc wheel impulses

        for(int q = 0; q < InitialVehicleSetup::mRoofsAmount; ++q)
        {
            mRoofImpulseLinear[q] = mImpulseLinear;
        }
    }
    else
    {
        Ogre::Real force = mInitialVehicleSetup.mChassisMass * recipMomentProj;

        //do calc wheel impulses

        for(int q = 0; q < InitialVehicleSetup::mRoofsAmount; ++q)
        {
            Ogre::Vector3 roofRot = mChassis->getOrientation() * mInitialVehicleSetup.mRoofPos[q];
            Ogre::Vector3 tangent = findTangent(normalisedImpulseRot, roofRot);
            if(tangent.x != 0.0f || tangent.y != 0.0f || tangent.z != 0.0f)
            {
                mRoofImpulseLinear[q] = tangent.crossProduct(mImpulseRotPrev) * force + mImpulseLinear;
            }
            else
            {
                mRoofImpulseLinear[q] = mImpulseLinear;
            }
        }

    }
}

void PhysicsVehicle::processRoof()
{
    for(int q = 0; q < InitialVehicleSetup::mRoofsAmount; ++q)
    {
        Ogre::Vector3 roofRot = mChassis->getOrientation() * mInitialVehicleSetup.mRoofPos[q];
        mRoofGlobal[q] = mChassis->getPosition() + roofRot;

        Ogre::Matrix3 carRot;
        mChassis->getOrientation().ToRotationMatrix(carRot);
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

                    Ogre::Vector3 tangent = findTangent(worldNormal, mRoofImpulseLinear[q]);
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

                    adjustImpulseInc(roofRot, velocityTangent);
                }
            }
        }
    }
}

void PhysicsVehicle::processBody()
{
    Ogre::Vector3 bodyRot = mChassis->getOrientation() * mInitialVehicleSetup.mBodyBasePos;

    mBodyGlobal = mChassis->getPosition() + bodyRot;

    Ogre::Matrix3 carRot;
    mChassis->getOrientation().ToRotationMatrix(carRot);
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

                Ogre::Vector3 tangent = findTangent(worldNormal, deriveImpulse);
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

                adjustImpulseInc(bodyRot, velocityTangent);
            }
        }
    }
}

void PhysicsVehicle::adjustImpulseInc(const Ogre::Vector3& rotAxis, const Ogre::Vector3& impulse)
{
    mImpulseRotInc += impulse.crossProduct(rotAxis);
    mImpulseLinearInc += impulse;
}

void PhysicsVehicle::createRotMatrix(Ogre::Vector3& matAxis, const Ogre::Vector3& normalisedImpulse, Ogre::Real angle) const
{
    Ogre::Vector3 normalisedImpulseProj = normalisedImpulse * matAxis.dotProduct(normalisedImpulse);
    Ogre::Vector3 diff = matAxis - normalisedImpulseProj;
    Ogre::Vector3 cross = normalisedImpulse.crossProduct(diff);
    Ogre::Vector3 cosAngle(Ogre::Math::Cos(angle));
    Ogre::Vector3 sinAngle(Ogre::Math::Sin(angle));
    matAxis = cosAngle * diff - sinAngle * cross + normalisedImpulseProj;
    if(matAxis.squaredLength() > 0.0f) matAxis.normalise();
}