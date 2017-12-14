
#include "PhysicsVehicle.h"

#include "Physics.h"
#include "../mesh/StaticMeshProcesser.h"

PhysicsVehicle::PhysicsVehicle(Physics* physics, 
                               StaticMeshProcesser * meshProesser,
                               InitialVehicleSetup& initialVehicleSetup, 
                               Ogre::SceneNode *wheelNodes[InitialVehicleSetup::mWheelsAmount],
                               Ogre::SceneNode *chassis)
: mPhysics(physics),
    mMeshProcesser(meshProesser),
    mChassis(chassis),
    mInitialVehicleSetup(initialVehicleSetup),
    mPhysicsWheels(initialVehicleSetup, physics, meshProesser),
    mPhysicsRoofs(initialVehicleSetup, physics, meshProesser),
    mPhysicsBody(initialVehicleSetup, physics, meshProesser),
    mCarEngine(initialVehicleSetup)
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

    mThrottle = 0.0f;
    mBreaks = 0.0f;

    mTurnOverValue = 0;
}

PhysicsVehicle::~PhysicsVehicle()
{
}

void PhysicsVehicle::timeStep()
{
    mPhysicsWheels.initStep();
    mPhysicsRoofs.initStep();
    mPhysicsBody.initStep();
    mCoreBaseGlobalPrev = mCoreBaseGlobal;

    //do AI
    mPhysicsWheels.setSteering(adjustSteering());

    integrate();

    //d.polubotko: TODO add velocity scale
    //d.polubotko: TODO adjust velocity scale for AI

    Ogre::Real linearImpulseMod = mImpulseLinear.length();
    mVehicleVelocityMod = linearImpulseMod * mInitialVehicleSetup.mChassisInvMass;
    if(mVehicleVelocityMod < 0.0001f) mVehicleVelocityMod = 0.0001f;

    mInitialVehicleSetup.mCarGlobalPos += mImpulseLinear * mInitialVehicleSetup.mChassisInvMass;
    Ogre::Real airDensTransCoeff = -1.0f * mInitialVehicleSetup.mAirDensityTranslation * linearImpulseMod + 1.0f;
    mImpulseLinear *= airDensTransCoeff;

    //do falloff check

    Ogre::Real rotImpulseMod = mImpulseRot.length();
    if(rotImpulseMod > 0.00001f)
    {
        Ogre::Matrix3 carRot;
        Ogre::Vector3 normalisedImpulseRot = mImpulseRot.normalisedCopy();
        Ogre::Real momentProj = momentOfInertiaProj(normalisedImpulseRot);

        Ogre::Real rotAngle = 1.0f / momentProj * rotImpulseMod;

        //d.polubotko: TODO adjust rotAngle for AI

        mInitialVehicleSetup.mCarRot.ToRotationMatrix(carRot);

        Ogre::Vector3 rotMatrixAxisY = carRot.GetColumn(1);
        Ogre::Vector3 rotMatrixAxisZ = carRot.GetColumn(2);
        createRotMatrix(rotMatrixAxisY, normalisedImpulseRot, rotAngle);
        createRotMatrix(rotMatrixAxisZ, normalisedImpulseRot, rotAngle);
        Ogre::Vector3 rotMatrixAxisX = rotMatrixAxisY.crossProduct(rotMatrixAxisZ);
        carRot.SetColumn(0, rotMatrixAxisX);
        carRot.SetColumn(1, rotMatrixAxisY);
        carRot.SetColumn(2, rotMatrixAxisZ);

        mInitialVehicleSetup.mCarRot.FromRotationMatrix(carRot);

        mImpulseRot *= mInitialVehicleSetup.mAirDensityRot;
    }

    
    mCoreBaseGlobal = mInitialVehicleSetup.mCarGlobalPos + mInitialVehicleSetup.mCarRot * mInitialVehicleSetup.mCoreBase;
    mMeshProcesser->performCollisionDetection(mInitialVehicleSetup.mCarGlobalPos, mCoreBaseGlobalPrev, mMaxCollisionDistance);

    mImpulseLinearInc.y += mInitialVehicleSetup.mChassisMass * (-mInitialVehicleSetup.mGravityVelocity);

    integrate();

    calcWheelRoofImpulses();

    mPhysicsWheels.process(*this);
    bool isTurnOver = mPhysicsRoofs.process(*this);
    isTurnOver |= mPhysicsBody.process(*this);
    turnOverRestore(isTurnOver);
    calcTransmission();
    mPhysicsWheels.calcPhysics(*this, mThrottle, mBreaks);

    //mImpulseLinearInc.y -= mInitialVehicleSetup.mChassisMass * (-mInitialVehicleSetup.mGravityVelocity);
    //mImpulseLinearInc.x += mInitialVehicleSetup.mChassisMass * mInitialVehicleSetup.mGravityVelocity;
    //mImpulseLinearInc.y += mInitialVehicleSetup.mChassisMass * mInitialVehicleSetup.mGravityVelocity;
    //mImpulseLinearInc.z += mInitialVehicleSetup.mChassisMass * mInitialVehicleSetup.mGravityVelocity;

    reposition();
    rerotation();
}

Ogre::Real PhysicsVehicle::adjustSteering() const
{
    Ogre::Real ret = 0.0f;

    Ogre::Real sign = 1.0f;
    if (mSteeringOriginal < 0.0f)
        sign = -1.0f;

    //d.polubotko: TODO adjust steering for AI
    Ogre::Real steeringSpline = mInitialVehicleSetup.mSteering.getPoint(Ogre::Math::Abs(mSteeringOriginal));
    steeringSpline *= sign * 0.95f;

    Ogre::Real impulseMod = mImpulseLinear.length();

    if(impulseMod < 15.0f)
    {
        steeringSpline *= (1.0f - (15.0f - impulseMod) * -0.04f);
    }

    //add field_920 substraction here

    ret = Ogre::Math::Clamp(steeringSpline, -0.95f, 0.95f);

    return ret;
}

void PhysicsVehicle::calcTransmission()
{
    Ogre::Real wheelsAverageVel = mPhysicsWheels.calcVelocity(mVehicleVelocityMod, mThrottle, mBreaks);
    mCarEngine.process(wheelsAverageVel, mThrottle, true);
    //d.polubotko: TODO adjust mThrottle for AI
    Ogre::Real power = mCarEngine.getPower(mThrottle, mImpulseLinear.length());
    mPhysicsWheels.calcVelocityMore(power, mCarEngine.getCurrentGear());
}

void PhysicsVehicle::processEngineIdle()
{
    Ogre::Real wheelsAverageVel = mPhysicsWheels.calcVelocity(mVehicleVelocityMod, mThrottle, mBreaks);
    mCarEngine.process(wheelsAverageVel, mThrottle, false);
}

void PhysicsVehicle::reposition()
{
    mChassis->setPosition(mInitialVehicleSetup.mCarGlobalPos + mInitialVehicleSetup.mCarRot * mInitialVehicleSetup.mCOG);

    mPhysicsWheels.reposition();
}

void PhysicsVehicle::rerotation()
{
    mChassis->setOrientation(mInitialVehicleSetup.mCarRot);

    mPhysicsWheels.rerotation();
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
    mInitialVehicleSetup.mCarRot.ToRotationMatrix(carRot);
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

void PhysicsVehicle::turnOverRestore(bool isTurnOver)
{
    if(isTurnOver)
    {
        if(mTurnOverValue >= 0)
            mTurnOverValue += 2;

        if(mTurnOverValue > 60)
            mTurnOverValue = -1;
    }

    if(mTurnOverValue < 0)
    {
        Ogre::Matrix3 carRot;
        mInitialVehicleSetup.mCarRot.ToRotationMatrix(carRot);
        Ogre::Vector3 rotYAxis = carRot.GetColumn(1);

        Ogre::Real turnOverRestoreVal = 500.0f / (mImpulseRot.length() + 2.0f);
        Ogre::Vector3 upDiff = Ogre::Vector3::UNIT_Y - rotYAxis;
        turnOverRestoreVal *= upDiff.length();
        if(turnOverRestoreVal != 0.0f)
        {
            upDiff *= turnOverRestoreVal;
            mImpulseRot += upDiff.crossProduct(rotYAxis);
        }

        if(mTurnOverValue < -200)
            mTurnOverValue = 0;
    }

    if(mTurnOverValue > 0)
        --mTurnOverValue;
}