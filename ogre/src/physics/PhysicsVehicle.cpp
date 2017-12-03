
#include "PhysicsVehicle.h"

#include "Physics.h"
#include "../tools/PhysicsTools.h"
#include "../mesh/StaticMeshProcesser.h"

PhysicsVehicle::PhysicsVehicle(Physics* physics, 
                               StaticMeshProcesser * meshProesser,
                               const InitialVehicleSetup& initialVehicleSetup, 
                               Ogre::SceneNode *wheelNodes[InitialVehicleSetup::mWheelsAmount],
                               Ogre::SceneNode *chassis)
: mPhysics(physics),
    mMeshProcesser(meshProesser),
    mChassis(chassis),
    mInitialVehicleSetup(initialVehicleSetup),
    mPhysicsWheels(initialVehicleSetup, physics, meshProesser),
    mPhysicsRoofs(initialVehicleSetup, physics, meshProesser),
    mPhysicsBody(initialVehicleSetup, physics, meshProesser)
{

    mPhysicsWheels.init(chassis->getPosition(), wheelNodes);
    mPhysicsRoofs.init(chassis->getPosition());
    mPhysicsBody.init();
    mCoreBaseGlobal = chassis->getPosition();
    mCoreBaseGlobalPrev = mCoreBaseGlobal;

    mImpulseLinear = mInitialVehicleSetup.mInitialImpulseLinear;
    mImpulseLinearInc = mInitialVehicleSetup.mInitialImpulseLinearInc;
    mImpulseRot = mInitialVehicleSetup.mInitialImpulseRot;
    mImpulseRotInc = mInitialVehicleSetup.mInitialImpulseRotInc;

    //find collision distance
    mMaxCollisionDistance = 0.0f;
    for(int q = 0; q < InitialVehicleSetup::mWheelsAmount; ++q)
    {
        Ogre::Real tmpDist = mInitialVehicleSetup.mConnectionPointWheel[q].length() + mInitialVehicleSetup.mWheelRadius[q];
        if(tmpDist > mMaxCollisionDistance) mMaxCollisionDistance = tmpDist;

        Ogre::Vector3 tmpPoint = mInitialVehicleSetup.mConnectionPointWheel[q];
        tmpPoint.y -= mInitialVehicleSetup.mMaxTravel;
        tmpDist = tmpPoint.length() + mInitialVehicleSetup.mWheelRadius[q];
        if(tmpDist > mMaxCollisionDistance) mMaxCollisionDistance = tmpDist;
    }

    for(int q = 0; q < InitialVehicleSetup::mRoofsAmount; ++q)
    {
        Ogre::Real tmpDist = mInitialVehicleSetup.mRoofPos[q].length() + mInitialVehicleSetup.mRoofRadius[q];
        if(tmpDist > mMaxCollisionDistance) mMaxCollisionDistance = tmpDist;
    }
}

PhysicsVehicle::~PhysicsVehicle()
{
}

void PhysicsVehicle::timeStep()
{
    mPhysicsWheels.initStep(mChassis->getPosition(), mChassis->getOrientation());
    mPhysicsRoofs.initStep();
    mPhysicsBody.initStep();
    mCoreBaseGlobalPrev = mCoreBaseGlobal;

    //do AI
    //do steering adj
    //do suspension init

    integrate();

    Ogre::Matrix3 carRot;
    Ogre::Vector3 posDiff = mImpulseLinear * mInitialVehicleSetup.mChassisInvMass;

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

        mChassis->getOrientation().ToRotationMatrix(carRot);

        Ogre::Vector3 rotMatrixAxisY = carRot.GetColumn(1);
        Ogre::Vector3 rotMatrixAxisZ = carRot.GetColumn(2);
        createRotMatrix(rotMatrixAxisY, normalisedImpulseRot, rotAngle);
        createRotMatrix(rotMatrixAxisZ, normalisedImpulseRot, rotAngle);
        Ogre::Vector3 rotMatrixAxisX = rotMatrixAxisY.crossProduct(rotMatrixAxisZ);
        carRot.SetColumn(0, rotMatrixAxisX);
        carRot.SetColumn(1, rotMatrixAxisY);
        carRot.SetColumn(2, rotMatrixAxisZ);

        mImpulseRot *= mInitialVehicleSetup.mAirDensityRot;
    }

    //do falloff check
    mCoreBaseGlobal = mChassis->getPosition() + mChassis->getOrientation() * mInitialVehicleSetup.mCoreBase;
    mMeshProcesser->performCollisionDetection(mChassis->getPosition(), mCoreBaseGlobalPrev, mMaxCollisionDistance);

    mImpulseLinearInc.y += mInitialVehicleSetup.mChassisMass * (-mInitialVehicleSetup.mGravityForce);

    integrate();

    calcWheelRoofImpulses();

    //mPhysicsWheels.process(*mChassis, *this);
    //mPhysicsRoofs.process(*mChassis, *this);
    mPhysicsBody.process(*mChassis, *this);
    //do flip restore
    //do transmission
    //do physics

    //mImpulseLinearInc.y -= mInitialVehicleSetup.mChassisMass * (-mInitialVehicleSetup.mGravityForce);
    //mImpulseLinearInc.x += mInitialVehicleSetup.mChassisMass * mInitialVehicleSetup.mGravityForce;
    //mImpulseLinearInc.y += mInitialVehicleSetup.mChassisMass * mInitialVehicleSetup.mGravityForce;
    //mImpulseLinearInc.z += mInitialVehicleSetup.mChassisMass * mInitialVehicleSetup.mGravityForce;

    reposition(posDiff);
    rerotation(Ogre::Quaternion (carRot));
}

void PhysicsVehicle::reposition(const Ogre::Vector3& posDiff)
{
    mChassis->translate(posDiff);
    Ogre::Vector3 chassisPos = mChassis->getPosition();
    const Ogre::Quaternion& chassisRot = mChassis->getOrientation();

    mPhysicsWheels.reposition(chassisPos, chassisRot);
}

void PhysicsVehicle::rerotation(const Ogre::Quaternion& rot)
{
    Ogre::Vector3 chassisPos = mChassis->getPosition();
    mChassis->setOrientation(rot);

    mPhysicsWheels.rerotation(chassisPos, rot);
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

Ogre::Vector3 PhysicsVehicle::findTangent(const Ogre::Vector3& normal, const Ogre::Vector3& input)
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

    mPhysicsWheels.calcImpulses(mImpulseRot, mImpulseRotPrev, normalisedImpulseRot, 
                                mImpulseLinear, 
                                recipMomentProj,
                                *this);

    mPhysicsRoofs.calcImpulses(mImpulseRot, mImpulseRotPrev, normalisedImpulseRot, 
                                mImpulseLinear, 
                                recipMomentProj,
                                mChassis->getOrientation(),
                                *this);

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