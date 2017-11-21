
#include "PhysicsVehicle.h"

#include "BulletCollision/CollisionShapes/btSphereShape.h"
#include "BulletCollision/CollisionDispatch/btCollisionObject.h"

#include "Physics.h"
#include "../tools/PhysicsTools.h"
#include "../mesh/StaticMeshProcesser.h"

PhysicsVehicle::PhysicsVehicle(Physics* physics, 
                               StaticMeshProcesser * meshProesser,
                               const InitialVehicleSetup& initialVehicleSetup, 
                               Ogre::SceneNode *wheelNodes[mWheelsAmount], Ogre::SceneNode *chassis)
: mPhysics(physics),
    mMeshProcesser(meshProesser),
    mChassis(chassis),
    mInitialVehicleSetup(initialVehicleSetup)
{
    for(int q = 0; q < mWheelsAmount; ++q)
    {
        mWheelNodes[q] = wheelNodes[q];
    }

    //wheels
    mWheelShapeFL = CommonIncludes::shared_ptr<btSphereShape>(new btSphereShape(mInitialVehicleSetup.mWheelRadiusFront));
    mWheelShapeFR = CommonIncludes::shared_ptr<btSphereShape>(new btSphereShape(mInitialVehicleSetup.mWheelRadiusFront));
    mWheelShapeRL = CommonIncludes::shared_ptr<btSphereShape>(new btSphereShape(mInitialVehicleSetup.mWheelRadiusBack));
    mWheelShapeRR = CommonIncludes::shared_ptr<btSphereShape>(new btSphereShape(mInitialVehicleSetup.mWheelRadiusBack));

    mWheelFL = CommonIncludes::shared_ptr<btCollisionObject>(new btCollisionObject());
    mWheelFR = CommonIncludes::shared_ptr<btCollisionObject>(new btCollisionObject());
    mWheelRL = CommonIncludes::shared_ptr<btCollisionObject>(new btCollisionObject());
    mWheelRR = CommonIncludes::shared_ptr<btCollisionObject>(new btCollisionObject());

    mWheelFL->setCollisionShape(mWheelShapeFL.get());
    mWheelFL->getWorldTransform().setOrigin(PhysicsTools::convert(mInitialVehicleSetup.mChassisPos + mInitialVehicleSetup.mChassisRot * mInitialVehicleSetup.mConnectionPointFLWheel));
    mWheelFL->getWorldTransform().setRotation(PhysicsTools::convert(mInitialVehicleSetup.mChassisRot));

    mWheelFR->setCollisionShape(mWheelShapeFR.get());
    mWheelFR->getWorldTransform().setOrigin(PhysicsTools::convert(mInitialVehicleSetup.mChassisPos + mInitialVehicleSetup.mChassisRot * mInitialVehicleSetup.mConnectionPointFRWheel));
    mWheelFR->getWorldTransform().setRotation(PhysicsTools::convert(mInitialVehicleSetup.mChassisRot));

    mWheelRL->setCollisionShape(mWheelShapeRL.get());
    mWheelRL->getWorldTransform().setOrigin(PhysicsTools::convert(mInitialVehicleSetup.mChassisPos + mInitialVehicleSetup.mChassisRot * mInitialVehicleSetup.mConnectionPointRLWheel));
    mWheelRL->getWorldTransform().setRotation(PhysicsTools::convert(mInitialVehicleSetup.mChassisRot));

    mWheelRR->setCollisionShape(mWheelShapeRR.get());
    mWheelRR->getWorldTransform().setOrigin(PhysicsTools::convert(mInitialVehicleSetup.mChassisPos + mInitialVehicleSetup.mChassisRot * mInitialVehicleSetup.mConnectionPointRRWheel));
    mWheelRR->getWorldTransform().setRotation(PhysicsTools::convert(mInitialVehicleSetup.mChassisRot));

    //mPhysics->addCollisionObject(mWheelFL.get());
    //mPhysics->addCollisionObject(mWheelFR.get());
    //mPhysics->addCollisionObject(mWheelRL.get());
    //mPhysics->addCollisionObject(mWheelRR.get());
    //wheels END

    //body
    mBodyShape = CommonIncludes::shared_ptr<btSphereShape>(new btSphereShape(mInitialVehicleSetup.mBodyRadius));
    mBody = CommonIncludes::shared_ptr<btCollisionObject>(new btCollisionObject());

    mBody->setCollisionShape(mBodyShape.get());
    mBody->getWorldTransform().setOrigin(PhysicsTools::convert(mInitialVehicleSetup.mChassisPos + mInitialVehicleSetup.mChassisRot * mInitialVehicleSetup.mBodyBasePos));
    mBody->getWorldTransform().setRotation(PhysicsTools::convert(mInitialVehicleSetup.mChassisRot));

    mPhysics->addCollisionObject(mBody.get());
    //body END

    mImpulseLinear = mInitialVehicleSetup.mInitialImpulseLinear;
    mImpulseLinearInc = mInitialVehicleSetup.mInitialImpulseLinearInc;
    mImpulseRot = mInitialVehicleSetup.mInitialImpulseRot;
    mImpulseRotInc = mInitialVehicleSetup.mInitialImpulseRotInc;

    mBodyGlobal = Ogre::Vector3::ZERO;
}

PhysicsVehicle::~PhysicsVehicle()
{
    //mPhysics->removeCollisionObject(mWheelFL.get());
    //mPhysics->removeCollisionObject(mWheelFR.get());
    //mPhysics->removeCollisionObject(mWheelRL.get());
    //mPhysics->removeCollisionObject(mWheelRR.get());

    mPhysics->removeCollisionObject(mBody.get());
}

void PhysicsVehicle::timeStep()
{
    initStep();

    //do AI
    //do steering adj
    //do suspension init

    integrateLinear();
    integrateRot();

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

    integrateLinear();
    integrateRot();

    processBody();

    //mImpulseLinearInc.y -= mInitialVehicleSetup.mChassisMass * (-mInitialVehicleSetup.mGravityForce);
    //mImpulseLinearInc.x += mInitialVehicleSetup.mChassisMass * mInitialVehicleSetup.mGravityForce;
    //mImpulseLinearInc.y += mInitialVehicleSetup.mChassisMass * mInitialVehicleSetup.mGravityForce;
    //mImpulseLinearInc.z += mInitialVehicleSetup.mChassisMass * mInitialVehicleSetup.mGravityForce;

}

void PhysicsVehicle::initStep()
{
    mBodyGlobalPrev = mBodyGlobal;
}

void PhysicsVehicle::reposition(const Ogre::Vector3& posDiff)
{
    for(int q = 0; q < mWheelsAmount; ++q)
    {
        mWheelNodes[q]->translate(posDiff);
    }

    mChassis->translate(posDiff);

    mWheelFL->getWorldTransform().setOrigin(mWheelFL->getWorldTransform().getOrigin() + PhysicsTools::convert(posDiff));
    mWheelFR->getWorldTransform().setOrigin(mWheelFR->getWorldTransform().getOrigin() + PhysicsTools::convert(posDiff));
    mWheelRL->getWorldTransform().setOrigin(mWheelRL->getWorldTransform().getOrigin() + PhysicsTools::convert(posDiff));
    mWheelRR->getWorldTransform().setOrigin(mWheelRR->getWorldTransform().getOrigin() + PhysicsTools::convert(posDiff));

    mBody->getWorldTransform().setOrigin(mBody->getWorldTransform().getOrigin() + PhysicsTools::convert(posDiff));
}

void PhysicsVehicle::rerotation(const Ogre::Quaternion& rot)
{
    Ogre::Vector3 chassisPos = mChassis->getPosition();

    mWheelNodes[0]->setPosition(chassisPos + rot * mInitialVehicleSetup.mConnectionPointRRWheel);
    mWheelNodes[1]->setPosition(chassisPos + rot * mInitialVehicleSetup.mConnectionPointRLWheel);
    mWheelNodes[2]->setPosition(chassisPos + rot * mInitialVehicleSetup.mConnectionPointFRWheel);
    mWheelNodes[3]->setPosition(chassisPos + rot * mInitialVehicleSetup.mConnectionPointFLWheel);
    for(int q = 0; q < mWheelsAmount; ++q)
    {
        mWheelNodes[q]->setOrientation(rot);
    }

    mChassis->setOrientation(rot);


    mWheelFL->getWorldTransform().setOrigin(PhysicsTools::convert(mWheelNodes[3]->getPosition()));
    mWheelFR->getWorldTransform().setOrigin(PhysicsTools::convert(mWheelNodes[2]->getPosition()));
    mWheelRL->getWorldTransform().setOrigin(PhysicsTools::convert(mWheelNodes[1]->getPosition()));
    mWheelRR->getWorldTransform().setOrigin(PhysicsTools::convert(mWheelNodes[0]->getPosition()));

    mWheelFL->getWorldTransform().setRotation(PhysicsTools::convert(rot));
    mWheelFR->getWorldTransform().setRotation(PhysicsTools::convert(rot));
    mWheelRL->getWorldTransform().setRotation(PhysicsTools::convert(rot));
    mWheelRR->getWorldTransform().setRotation(PhysicsTools::convert(rot));

    mBody->getWorldTransform().setOrigin(PhysicsTools::convert(chassisPos + rot * mInitialVehicleSetup.mBodyBasePos));
    mBody->getWorldTransform().setRotation(PhysicsTools::convert(rot));
}

void PhysicsVehicle::integrateLinear()
{
    mImpulseLinear += mImpulseLinearInc;
    mImpulseLinearInc = Ogre::Vector3::ZERO;
}

void PhysicsVehicle::integrateRot()
{
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

void PhysicsVehicle::processBody()
{
    Ogre::Vector3 bodyRot = mChassis->getOrientation() * mInitialVehicleSetup.mBodyBasePos;

    mBodyGlobal = mChassis->getPosition() + mChassis->getOrientation() * mInitialVehicleSetup.mBodyBasePos;

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
                    assert(terrainType >= 0);
                    assert(terrainType <= 15);
                    Ogre::Real velocityMultiplier = mInitialVehicleSetup.mVelocitySpline[terrainType].getPoint(velocityMod);
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