
#include "PhysicsVehicle.h"

#include "Physics.h"
#include "../mesh/StaticMeshProcesser.h"

PhysicsVehicle::PhysicsVehicle(Physics* physics, 
                               StaticMeshProcesser * meshProesser,
                               InitialVehicleSetup& initialVehicleSetup, 
                               Ogre::SceneNode *wheelNodes[InitialVehicleSetup::mWheelsAmount],
                               Ogre::SceneNode *chassis,
                               InputType type)
: mPhysics(physics),
    mMeshProcesser(meshProesser),
    mChassis(chassis),
    mAICar(NULL),
    mVehicleVelocityMod(0.0f),
    mInitialVehicleSetup(initialVehicleSetup),
    mPhysicsWheels(initialVehicleSetup, physics, meshProesser),
    mPhysicsRoofs(initialVehicleSetup, physics, meshProesser),
    mPhysicsBody(initialVehicleSetup, physics, meshProesser),
    mCarEngine(initialVehicleSetup),
    mVehicleType(HumanVehicle),
    mThrottle(0.0f),
    mBreaks(0.0f),
    mSteeringOriginal(0.0f),
    mThrottleAdjusterCounter(0),
    mSlipStreamFactor(0.0f),
    mSteeringAdditionalParam(0.0f),
    mCollisionSteeringAdditionalParam(0.0f),
    mIsSteeringLeft(false),
    mIsSteeringRight(false),
    mSteeringIncrement(0.0f),
    mTurnOverValue(0),
    mIsRaceStarted(false),
    mInputType(type)
{
    mCarEngine.setTransmissionType(trAuto);

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

void PhysicsVehicle::timeStep(const GameState& gameState)
{
    mPhysicsWheels.initStep();
    mPhysicsRoofs.initStep();
    mPhysicsBody.initStep();
    mCoreBaseGlobalPrev = mCoreBaseGlobal;

    doAIStep(gameState);
    mPhysicsWheels.setSteering(adjustSteering());

    integrate();

    Ogre::Real velScale = mInitialVehicleSetup.mVelocityScale * doGetVelocityScale();

    Ogre::Vector3 impulse(mImpulseLinear);
    impulse.z = -impulse.z;//original data is left hand
    Ogre::Real linearImpulseMod = impulse.length();
    mVehicleVelocityMod = linearImpulseMod * mInitialVehicleSetup.mChassisInvMass;
    if(mVehicleVelocityMod < 0.0001f) mVehicleVelocityMod = 0.0001f;

    mInitialVehicleSetup.mCarGlobalPos += impulse * velScale * mInitialVehicleSetup.mChassisInvMass;
    Ogre::Real airDensTransCoeff = (mSlipStreamFactor - 1.0f) * mInitialVehicleSetup.mAirDensityTranslation * linearImpulseMod + 1.0f;
    mImpulseLinear *= airDensTransCoeff;

    //do falloff check

    Ogre::Real rotImpulseMod = mImpulseRot.length();
    if(rotImpulseMod > 0.00001f)
    {
        Ogre::Matrix3 carRot;
        Ogre::Vector3 normalisedImpulseRot = mImpulseRot.normalisedCopy();
        Ogre::Real momentProj = momentOfInertiaProj(normalisedImpulseRot);

        Ogre::Real rotAngle = 1.0f / momentProj * rotImpulseMod;

        rotAngle *= doGetVelocityScale();

        mInitialVehicleSetup.mCarRot.ToRotationMatrix(carRot);
        Ogre::Vector3 carRotV[3];//original data is left hand
        carRotV[0] = Ogre::Vector3(carRot[0][0], carRot[1][0], -carRot[2][0]);
        carRotV[1] = Ogre::Vector3(carRot[0][1], carRot[1][1], -carRot[2][1]);
        carRotV[2] = Ogre::Vector3(-carRot[0][2], -carRot[1][2], carRot[2][2]);

        createRotMatrix(carRotV[1], normalisedImpulseRot, rotAngle);
        createRotMatrix(carRotV[2], normalisedImpulseRot, rotAngle);
        carRotV[1].z = -carRotV[1].z;//original data is left hand
        carRotV[2].x = -carRotV[2].x;//original data is left hand
        carRotV[2].y = -carRotV[2].y;//original data is left hand
        Ogre::Vector3 rotMatrixAxisX = carRotV[1].crossProduct(carRotV[2]);
        carRot.SetColumn(0, rotMatrixAxisX);
        carRot.SetColumn(1, carRotV[1]);
        carRot.SetColumn(2, carRotV[2]);

        mInitialVehicleSetup.mCarRot.FromRotationMatrix(carRot);

        mImpulseRot *= mInitialVehicleSetup.mAirDensityRot;
    }

    
    mCoreBaseGlobal = mInitialVehicleSetup.mCarGlobalPos + mInitialVehicleSetup.mCarRot * mInitialVehicleSetup.mCoreBase;
    mMeshProcesser->performCollisionDetection(mInitialVehicleSetup.mCarGlobalPos, mCoreBaseGlobalPrev, mMaxCollisionDistance);

    mImpulseLinearInc.y += mInitialVehicleSetup.mChassisMass * (-mInitialVehicleSetup.mGravityVelocity);

    integrate();

    calcWheelRoofImpulses();

    mPhysicsWheels.process(*this);
    bool isTurnOver;
    isTurnOver = mPhysicsRoofs.process(*this);
    isTurnOver |= mPhysicsBody.process(*this);
    turnOverRestore(isTurnOver);
    calcTransmission();
    mPhysicsWheels.calcPhysics(*this, mThrottle, mBreaks, doGetTractionScale(), mThrottleAdjusterCounter, mThrottleAdjuster);

    //mImpulseLinearInc.y -= mInitialVehicleSetup.mChassisMass * (-mInitialVehicleSetup.mGravityVelocity);
    //mImpulseLinearInc.x += mInitialVehicleSetup.mChassisMass * mInitialVehicleSetup.mGravityVelocity;
    //mImpulseLinearInc.y += mInitialVehicleSetup.mChassisMass * mInitialVehicleSetup.mGravityVelocity;
    //mImpulseLinearInc.z += mInitialVehicleSetup.mChassisMass * mInitialVehicleSetup.mGravityVelocity;

    mInitialVehicleSetup.mCOGGlobal = mInitialVehicleSetup.mCarGlobalPos + mInitialVehicleSetup.mCarRot * mInitialVehicleSetup.mCOG;

    reposition();
    rerotation();
}

Ogre::Real PhysicsVehicle::adjustSteering()
{
    if(mVehicleType == HumanVehicle && mInputType == itKeyboard)
    {
        Ogre::Real steerDirection = 0.0f;
        if (mIsSteeringLeft) steerDirection = 1.0f;
        if (mIsSteeringRight) steerDirection = -1.0f;

        if(mSteeringOriginal == steerDirection)
            mSteeringIncrement = 0.0f;
        else
            mSteeringIncrement += 1.0f;

        Ogre::Real steerWeight = 0.07f - mSteeringIncrement * -0.005f;
        if(steerWeight > 0.35f) steerWeight = 0.35f;

        if(mSteeringOriginal < steerDirection)
        {
            if(mSteeringOriginal >= 0.0f)
                mSteeringOriginal += steerWeight;
            else
                mSteeringOriginal += steerWeight * 2.0f;

            if(mSteeringOriginal > steerDirection)
                mSteeringOriginal = steerDirection;
        }
        else
        {
            if(mSteeringOriginal <= 0.0f)
                mSteeringOriginal -= steerWeight;
            else
                mSteeringOriginal -= steerWeight * 2.0f;

            if(mSteeringOriginal < steerDirection)
                mSteeringOriginal = steerDirection;
        }
    }

    mSteeringOriginal = Ogre::Math::Clamp(mSteeringOriginal, -1.0f, 1.0f);

    Ogre::Real ret = 0.0f;

    Ogre::Real sign = 1.0f;
    if (mSteeringOriginal < 0.0f)
        sign = -1.0f;

    
    Ogre::Real steeringSpline;
    if(mVehicleType == HumanVehicle)
        steeringSpline = mInitialVehicleSetup.mSteering.getPoint(Ogre::Math::Abs(mSteeringOriginal));
    if(mVehicleType == AIVehicle)
        steeringSpline = doAdjustAISteering(Ogre::Math::Abs(mSteeringOriginal));

    steeringSpline *= sign * 0.95f;

    Ogre::Real impulseMod = mImpulseLinear.length();

    if(impulseMod < 15.0f)
    {
        steeringSpline *= (1.0f - (15.0f - impulseMod) * -0.04f);
    }

    steeringSpline -= mSteeringAdditionalParam;
    mSteeringAdditionalParam = mCollisionSteeringAdditionalParam;
    mCollisionSteeringAdditionalParam = 0.0f;

    ret = Ogre::Math::Clamp(steeringSpline, -0.95f, 0.95f);

    return ret;
}

void PhysicsVehicle::calcTransmission()
{
    Ogre::Real wheelsAverageVel = mPhysicsWheels.calcVelocity(mVehicleVelocityMod, mThrottle, mBreaks);
    mCarEngine.process(wheelsAverageVel, mThrottle);
    Ogre::Real power = mCarEngine.getPower(mThrottle * doGetThrottleScale(), mImpulseLinear.length());
    mPhysicsWheels.calcVelocityMore(power, mCarEngine.getCurrentGear());
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
    Ogre::Matrix3 carRotPS;
    Ogre::Vector3 carRotV[3];//original data is left hand
    carRotV[0] = Ogre::Vector3(carRot[0][0], carRot[1][0], -carRot[2][0]);
    carRotV[1] = Ogre::Vector3(carRot[0][1], carRot[1][1], -carRot[2][1]);
    carRotV[2] = Ogre::Vector3(-carRot[0][2], -carRot[1][2], carRot[2][2]);
    carRotPS.FromAxes(carRotV[0], carRotV[1], carRotV[2]);

    Ogre::Vector3 tmpRes = carRotPS * axis;

    ret = mInitialVehicleSetup.mMomentOfInertia.dotProduct(Ogre::Vector3(tmpRes * tmpRes));

    return ret;
}

Ogre::Vector3 PhysicsVehicle::findTangent(const Ogre::Vector3& normal, const Ogre::Vector3& input)
{
    //40E8C0
    Ogre::Real dot = input.dotProduct(normal);
    return input - dot * normal;
}

Ogre::Vector2 PhysicsVehicle::findTexCoordinates(const Ogre::Vector3& normal, 
    const Ogre::Vector3& point,
    const Ogre::Vector3& pA, const Ogre::Vector3& pB, const Ogre::Vector3& pC,
    const Ogre::Vector2& tA, const Ogre::Vector2& tB, const Ogre::Vector2& tC)
{
    //4617C0
    Ogre::Vector2 res = Ogre::Vector2::ZERO;

    Ogre::Real dotP = point.dotProduct(normal) - pA.dotProduct(normal);
    dotP = -dotP;

    Ogre::Vector2 proj = dotP * Ogre::Vector2(normal.x, normal.z) + Ogre::Vector2(point.x, point.z);

    Ogre::Vector3 absNorm = normal;
    absNorm.x = Ogre::Math::Abs(absNorm.x);
    absNorm.y = Ogre::Math::Abs(absNorm.y);
    absNorm.z = Ogre::Math::Abs(absNorm.z);

    size_t switcher = 0;
    if(absNorm.x <= absNorm.y)
    {
        switcher = 1;//absNorm.y - max
        if ( absNorm.y <= absNorm.z )//absNorm.z - max
            switcher = 2;
    }
    else
        switcher = absNorm.x <= absNorm.z;//absNorm.z - max

    Ogre::Real valA, valB, valC, valD, valE, valF;
    if(switcher)
    {
        --switcher;
        if(switcher)
        {
            if(switcher == 1)
            {
                valA = pA.x;
                valB = pC.y;
                valC = pA.y;
                valD = pB.y;
                valE = pC.x;
                valF = pB.x;
            }
            else
            {
                assert(false);
            }
        }
        else
        {
            valA = pA.x;
            valB = pC.z;
            valC = pA.z;
            valD = pB.z;
            valE = pC.x;
            valF = pB.x;
        }
    }
    else
    {
        valA = pA.y;
        valB = pC.z;
        valC = pA.z;
        valD = pB.z;
        valE = pC.y;
        valF = pB.y;
    }

    Ogre::Real diffDC, diffEA, diffBC, diffFA;
    diffDC = valD - valC;
    diffEA = valE - valA;
    diffBC = valB - valC;
    diffFA = valF - valA;

    Ogre::Real weight = 1.0f / (diffFA * diffBC - diffDC * diffEA);

    Ogre::Vector2 diffProj = proj - Ogre::Vector2(valA, valC);
    Ogre::Vector2 diffProjWeight = diffProj * Ogre::Vector2(diffBC, diffFA);
    Ogre::Vector2 coord; 
    coord.x = (diffProjWeight.x - diffProj.y * diffEA) * weight;
    coord.y = (diffProjWeight.y - diffProj.x * diffDC) * weight;

    res = (tC - tA) * coord.y + (tB - tA) * coord.x + tA;

    if(res.x < 0.0f)
    {
        double integer;
        float frac = static_cast<float>(modf(res.x, &integer));
        res.x = frac + 1.0f;
    }
    if(res.x > 1.0f)
    {
        double integer;
        res.x = static_cast<float>(modf(res.x, &integer));
    }

    if(res.y < 0.0f)
    {
        double integer;
        float frac = static_cast<float>(modf(res.y, &integer));
        res.y = frac + 1.0f;
    }
    if(res.y > 1.0f)
    {
        double integer;
        res.y = static_cast<float>(modf(res.y, &integer));
    }

    return res;
}

void PhysicsVehicle::calcWheelRoofImpulses()
{
    mImpulseRotPrev = mImpulseRot;
    Ogre::Vector3 normalisedImpulseRot = mImpulseRot.normalisedCopy();
    Ogre::Real momentProj = momentOfInertiaProj(normalisedImpulseRot);

    Ogre::Real recipMomentProj = 1.0f / momentProj;

    mPhysicsWheels.calcImpulses(mImpulseRot, mImpulseRotPrev, normalisedImpulseRot, 
                                mImpulseLinear, 
                                recipMomentProj);

    mPhysicsRoofs.calcImpulses(mImpulseRot, mImpulseRotPrev, normalisedImpulseRot, 
                                mImpulseLinear, 
                                recipMomentProj);

}

void PhysicsVehicle::adjustImpulseInc(const Ogre::Vector3& rotAxis, const Ogre::Vector3& impulse)
{
    mImpulseRotInc += impulse.crossProduct(rotAxis);
    mImpulseLinearInc += impulse;
}

void PhysicsVehicle::createRotMatrix(Ogre::Vector3& matAxis, const Ogre::Vector3& normalisedImpulse, Ogre::Real angle)
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
        Ogre::Vector3 carRotV[3];//original data is left hand
        carRotV[0] = Ogre::Vector3(carRot[0][0], carRot[1][0], -carRot[2][0]);
        carRotV[1] = Ogre::Vector3(carRot[0][1], carRot[1][1], -carRot[2][1]);
        carRotV[2] = Ogre::Vector3(-carRot[0][2], -carRot[1][2], carRot[2][2]);

        Ogre::Real turnOverRestoreVal = 500.0f / (mImpulseRot.length() + 2.0f);
        Ogre::Vector3 upDiff = Ogre::Vector3::UNIT_Y - carRotV[1];
        turnOverRestoreVal *= upDiff.length();
        if(turnOverRestoreVal != 0.0f)
        {
            upDiff *= turnOverRestoreVal;
            mImpulseRotInc += upDiff.crossProduct(carRotV[1]);
        }

        --mTurnOverValue;

        if(mTurnOverValue < -200)
            mTurnOverValue = 0;
    }

    if(mTurnOverValue > 0)
        --mTurnOverValue;
}

void PhysicsVehicle::gearUp()
{
    if(mIsRaceStarted)
    {
        mCarEngine.gearUp();

        if(mVehicleType == HumanVehicle)
        {
            if(mCarEngine.getCurrentGear() == 1)
            {
                mThrottleAdjusterCounter = 180;
                mThrottleAdjuster = (mCarEngine.getEngineRPM() - 6200.0f) * 0.00025f;
                mPhysicsWheels.setBackVelocity(mCarEngine.getEngineRPM() * mInitialVehicleSetup.mGearRatioMain * mInitialVehicleSetup.mGearRatio[0]);
            }
        }
    }
}

void PhysicsVehicle::gearDown()
{
    if(mIsRaceStarted)
    {
        mCarEngine.gearDown();

        if(mVehicleType == HumanVehicle)
        {
            if(mCarEngine.getCurrentGear() == -1)
            {
                mThrottleAdjusterCounter = 180;
                mThrottleAdjuster = (mCarEngine.getEngineRPM() - 6200.0f) * 0.00025f;
                mPhysicsWheels.setBackVelocity(-mCarEngine.getEngineRPM() * mInitialVehicleSetup.mGearRatioMain * mInitialVehicleSetup.mGearRatio[0]);
            }
        }
    }
}

Ogre::Vector3 PhysicsVehicle::getLinearVelocity() const
{
    Ogre::Vector3 impulse(mImpulseLinear);
    impulse.z = -impulse.z;//original data is left hand
    return impulse * mInitialVehicleSetup.mChassisInvMass * 33.0f;
}

Ogre::Vector3 PhysicsVehicle::getAngularVelocity() const
{
    return mImpulseRot * mInitialVehicleSetup.mChassisInvMass * 33.0f;
}

Ogre::Vector3 PhysicsVehicle::getLinearImpulse() const
{
    return mImpulseLinear;
}

void PhysicsVehicle::zeroImpulses()
{
    mImpulseLinear = Ogre::Vector3::ZERO;
    mImpulseLinearInc = Ogre::Vector3::ZERO;
    mImpulseRot = Ogre::Vector3::ZERO;
    mImpulseRotInc = Ogre::Vector3::ZERO;
}