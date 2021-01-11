
#include "PSBaseCar.h"

#include "../tools/OgreTools.h"
#include "../tools/Conversions.h"

#include "../lua/DMLuaManager.h"

#include "../listeners/VehicleSceneObjectListener.h"
#include "../listeners/PlayerVehicleSceneObjectListener.h"

#include "../mesh/ModelsPool.h"

#ifndef NO_OPENAL
    #include "../sound/SoundSource.h"
#endif

#include "../loaders/TEXLoader.h"
#include "../loaders/SUSLoader.h"
#include "../OriginalSettings.h"

#include "../physics/Physics.h"
#include "../physics/PhysicsVehicle.h"

#include "../GameState.h"

PSBaseCar::PSBaseCar() :
    mWheelFrontLColliderString(""),
    mWheelFrontRColliderString(""),
    mWheelBackLColliderString(""),
    mWheelBackRColliderString("")
{
}

void PSBaseCar::timeStepAfter(Physics * physics)
{
    (void)physics;

    if (mPhysicsVehicle->getLife() > 0.0f)
    {
        AdjustSuspension(mModelEntity[0]->getMesh().get(),
            mSuspensionIndices, mSuspensionPointOriginalPos,
            -mPhysicsVehicle->getSuspensionHeight(3),
            -mPhysicsVehicle->getSuspensionHeight(2),
            -mPhysicsVehicle->getSuspensionHeight(1),
            -mPhysicsVehicle->getSuspensionHeight(0));
    }
}

void PSBaseCar::initModel(  lua_State * pipeline, 
                            const GameState& gameState,
                            Ogre::SceneManager* sceneMgr, Ogre::SceneNode* mainNode,
                            ModelsPool* modelsPool,
                            Physics * world,
                            const std::string& characterName,
                            InitialVehicleSetup& initialVehicleSetup,
                            bool isAI)
{

    mIsAI = isAI;

    world->addListener(this);

    STRSettings misSettings;
    misSettings.parse(gameState.getPFLoaderStore(), "data/misc", "car_misc.str");

    std::string carName = gameState.getSTRPowerslide().getValue(characterName + " parameters", "car", "feral max");
    std::string carPath = getCarPath(gameState);

    //init parms
    mCarSettings.parse(gameState.getPFLoaderStore(), "data/cars/" + carPath + "/data/default", "params.str");
    mTrackSettings.parse(gameState.getPFLoaderStore(), "data/cars/global/data/" + gameState.getSTRPowerslide().getDataSubDir(gameState.getTrackName()), "params.str");
    mDefaultSettings.parse(gameState.getPFLoaderStore(), "data/cars/global/data/default", "params.str");

    //init splines
    mCarSplines.parse(gameState.getPFLoaderStore(), "data/cars/" + carPath + "/data/default", "graphs.str");
    mTrackSplines.parse(gameState.getPFLoaderStore(), "data/cars/global/data/" + gameState.getSTRPowerslide().getDataSubDir(gameState.getTrackName()), "graphs.str");
    mDefaultSplines.parse(gameState.getPFLoaderStore(), "data/cars/global/data/default", "graphs.str");

    initialVehicleSetup.mMissileAngle = -0.01f;
    if(carName == "supercar")
        initialVehicleSetup.mMissileAngle = 0.02f;

    initialVehicleSetup.mCollisionRadius = getCarParameter("", "collision radius");

    initialVehicleSetup.mCOG = -getCarArray3Parameter("", "centre of gravity");
    initialVehicleSetup.mCOG.z = -initialVehicleSetup.mCOG.z;

    initialVehicleSetup.mCockpit = getCarArray3Parameter("", "cockpit");
    initialVehicleSetup.mCockpit.z = -initialVehicleSetup.mCockpit.z;

    initialVehicleSetup.mCoreBase = getCarArray3Parameter("", "core base");
    initialVehicleSetup.mCoreBase.z = -initialVehicleSetup.mCoreBase.z;

    //load wheels offsets
    {
        initialVehicleSetup.mConnectionPointWheel[0] = mCarSettings.getArray3Value("", "wheelbase back");
        initialVehicleSetup.mConnectionPointWheel[0].z = -initialVehicleSetup.mConnectionPointWheel[0].z;

        initialVehicleSetup.mConnectionPointWheel[1] = initialVehicleSetup.mConnectionPointWheel[0];
        initialVehicleSetup.mConnectionPointWheel[1].x = -initialVehicleSetup.mConnectionPointWheel[1].x;

        initialVehicleSetup.mConnectionPointWheel[0] += initialVehicleSetup.mCOG;
        initialVehicleSetup.mConnectionPointWheel[1] += initialVehicleSetup.mCOG;

        initialVehicleSetup.mConnectionPointWheel[2] = mCarSettings.getArray3Value("", "wheelbase front");
        initialVehicleSetup.mConnectionPointWheel[2].z = -initialVehicleSetup.mConnectionPointWheel[2].z;

        initialVehicleSetup.mConnectionPointWheel[3] = initialVehicleSetup.mConnectionPointWheel[2];
        initialVehicleSetup.mConnectionPointWheel[3].x = -initialVehicleSetup.mConnectionPointWheel[3].x;

        initialVehicleSetup.mConnectionPointWheel[2] += initialVehicleSetup.mCOG;
        initialVehicleSetup.mConnectionPointWheel[3] += initialVehicleSetup.mCOG;

    }

    Ogre::Quaternion rot = initialVehicleSetup.mTrackPosition.extractQuaternion();
    Ogre::Radian angle;
    Ogre::Vector3 axis;
    rot.ToAngleAxis(angle, axis);
    rot.FromAngleAxis(angle, Ogre::Vector3(-axis.x, -axis.y, axis.z));
    initialVehicleSetup.mCarGlobalPos = initialVehicleSetup.mTrackPosition.getTrans();
    initialVehicleSetup.mCarRot = rot;

    initialVehicleSetup.mRoofRadius[0] = mCarSettings.getFloatValue("", "roof back radius");
    initialVehicleSetup.mRoofRadius[1] = initialVehicleSetup.mRoofRadius[0];
    Ogre::Vector3 roofBack = mCarSettings.getArray3Value("", "roof back");
    initialVehicleSetup.mRoofPos[0] = roofBack;
    initialVehicleSetup.mRoofPos[1] = Ogre::Vector3(-roofBack.x, roofBack.y, roofBack.z);

    initialVehicleSetup.mRoofRadius[2] = mCarSettings.getFloatValue("", "roof front radius");
    initialVehicleSetup.mRoofRadius[3] = initialVehicleSetup.mRoofRadius[2];
    Ogre::Vector3 roofFront = mCarSettings.getArray3Value("", "roof front");
    initialVehicleSetup.mRoofPos[2] = roofFront;
    initialVehicleSetup.mRoofPos[3] = Ogre::Vector3(-roofFront.x, roofFront.y, roofFront.z);

    initialVehicleSetup.mBodyRadius = mCarSettings.getFloatValue("", "body radius");
    Ogre::Vector3 bodyBase = mCarSettings.getArray3Value("", "body base 0");
    initialVehicleSetup.mBodyBasePos = bodyBase;

    Ogre::Vector2 wheelRadius = mCarSettings.getArray2Value("", "wheel radii");
    initialVehicleSetup.mWheelRadius[2] = wheelRadius.x;
    initialVehicleSetup.mWheelRadius[3] = wheelRadius.x;
    initialVehicleSetup.mWheelRadius[0] = wheelRadius.y;
    initialVehicleSetup.mWheelRadius[1] = wheelRadius.y;


    initialVehicleSetup.mAirDensityTranslation = getCarParameter("", "air density translation");
    initialVehicleSetup.mAirDensityRot = getCarParameter("", "air density rotation");

    initialVehicleSetup.mChassisMass = getCarParameter("", "mass");// *1.5f; - simulate truck mode
    initialVehicleSetup.mChassisInvMass = 1.0f / initialVehicleSetup.mChassisMass;
    initialVehicleSetup.mMomentOfInertia = getCarArray3Parameter("", "moment of inertia");// * 1.5f; - simulate truck mode

    initialVehicleSetup.mGravityVelocity = misSettings.getFloatValue("", "gravity force");

    initialVehicleSetup.mEngineIdleRevsStart = getCarParameter("", "idle revs start");
    initialVehicleSetup.mEngineIdleRevsEnd = getCarParameter("", "idle revs end");
    initialVehicleSetup.mGearRatioMain = getCarParameter("", "gear rev ratio");
    {
        Ogre::Vector4 tmpGear = getCarArray4Parameter("", "rev ratio");
        initialVehicleSetup.mGearRatio[0] = tmpGear.x;
        initialVehicleSetup.mGearRatio[1] = tmpGear.y;
        initialVehicleSetup.mGearRatio[2] = tmpGear.z;
        initialVehicleSetup.mGearRatio[3] = tmpGear.w;
    }
    {
        Ogre::Vector4 tmpGear = getCarArray4Parameter("", "change down");
        initialVehicleSetup.mChangeDown[0] = tmpGear.x;
        initialVehicleSetup.mChangeDown[1] = tmpGear.y;
        initialVehicleSetup.mChangeDown[2] = tmpGear.z;
        initialVehicleSetup.mChangeDown[3] = tmpGear.w;
    }
    {
        Ogre::Vector4 tmpGear = getCarArray4Parameter("", "change up");
        initialVehicleSetup.mChangeUp[0] = tmpGear.x;
        initialVehicleSetup.mChangeUp[1] = tmpGear.y;
        initialVehicleSetup.mChangeUp[2] = tmpGear.z;
        initialVehicleSetup.mChangeUp[3] = tmpGear.w;
    }

    initialVehicleSetup.mMaxTravel = getCarParameter("", "max travel");
    initialVehicleSetup.mFrontSuspension = getCarParameter("", "front suspension");
    initialVehicleSetup.mRisingDamp = getCarParameter("", "rising damp");

    initialVehicleSetup.mFrontBreak = getCarParameter("", "front brake");
    initialVehicleSetup.mBackBreak = getCarParameter("", "back brake");

    initialVehicleSetup.mVelocityScale = getCarParameter("", "velocity scale");

    initialVehicleSetup.mTransmissionDrag = getCarParameter("", "transmission drag");

    initialVehicleSetup.mDiffSlip = getCarParameter("", "diff slip");

    initialVehicleSetup.mFrontWheelDrive = getCarParameter("", "front wheel drive");

    initialVehicleSetup.mAccelerationFactor = getCarParameter("", "acceleration factor");

    initialVehicleSetup.mTurnFinish = getCarParameter("", "turn finish");

    initialVehicleSetup.mHighFrontTraction = getCarParameter("", "high front traction");
    initialVehicleSetup.mLowFrontTraction = getCarParameter("", "low front traction");
    initialVehicleSetup.mHighRearTraction = getCarParameter("", "high rear traction");
    initialVehicleSetup.mLowRearTraction = getCarParameter("", "low rear traction");

    initialVehicleSetup.mWheelVelocitySub = getCarParameter("", "wheel velocity sub");

    if(gameState.getAIStrength() == Easy)
        initialVehicleSetup.mOversteer = getCarParameter("", "easy_oversteer");
    else
        initialVehicleSetup.mOversteer = 1.0f;
    initialVehicleSetup.mOversteer *= gameState.getSTRPowerslide().getFloatValue("drive options", "oversteer");



    //splines
    {
        Ogre::Real hscale = getCarParameter("", "wheel under ground d_d hscale", true);
        Ogre::Real vscale = getCarParameter("", "wheel under ground d_d vscale", true);

        std::vector<std::string> splinePointsStr = getCarArrayValueParameter("", "wheel under ground d_d");
        std::vector<Ogre::Real> splinePoints = convertSplinePoints(splinePointsStr);

        initialVehicleSetup.mWheelUnderGroundDD.init(splinePoints, hscale, vscale);
    }
    {
        Ogre::Real hscale = getCarParameter("", "wheel under ground v_v hscale", true);
        Ogre::Real vscale = getCarParameter("", "wheel under ground v_v vscale", true);

        std::vector<std::string> splinePointsStr = getCarArrayValueParameter("", "wheel under ground v_v");
        std::vector<Ogre::Real> splinePoints = convertSplinePoints(splinePointsStr);

        initialVehicleSetup.mWheelUnderGroundVV.init(splinePoints, hscale, vscale);
    }
    {
        Ogre::Real hscale = getCarParameter("", "wheel under ground d_dv hscale", true);
        Ogre::Real vscale = getCarParameter("", "wheel under ground d_dv vscale", true);

        std::vector<std::string> splinePointsStr = getCarArrayValueParameter("", "wheel under ground d_dv");
        std::vector<Ogre::Real> splinePoints = convertSplinePoints(splinePointsStr);

        initialVehicleSetup.mWheelUnderGroundDDV.init(splinePoints, hscale, vscale);
    }
    {
        Ogre::Real hscale = getCarParameter("", "wheel under ground v_dv hscale", true);
        Ogre::Real vscale = getCarParameter("", "wheel under ground v_dv vscale", true);

        std::vector<std::string> splinePointsStr = getCarArrayValueParameter("", "wheel under ground v_dv");
        std::vector<Ogre::Real> splinePoints = convertSplinePoints(splinePointsStr);

        initialVehicleSetup.mWheelUnderGroundVDV.init(splinePoints, hscale, vscale);
    }
    for(int q = 0; q < initialVehicleSetup.mVelocitySplinesAmoint; ++q)
    {
        std::string num = Conversions::DMToString(q);
        Ogre::Real hscale = getCarParameter("", "traction for velocity " + num + " hscale", true);
        Ogre::Real vscale = getCarParameter("", "traction for velocity " + num + " vscale", true);

        std::vector<std::string> splinePointsStr = getCarArrayValueParameter("", "traction for velocity " + num);
        std::vector<Ogre::Real> splinePoints = convertSplinePoints(splinePointsStr);

        initialVehicleSetup.mVelocitySpline[q].init(splinePoints, hscale, vscale);
    }
    {
        Ogre::Real hscale = getCarParameter("", "rev power hscale", true);
        Ogre::Real vscale = getCarParameter("", "rev power vscale", true);

        std::vector<std::string> splinePointsStr = getCarArrayValueParameter("", "rev power");
        std::vector<Ogre::Real> splinePoints = convertSplinePoints(splinePointsStr);
        //simulate truck mode
        //std::transform(splinePoints.begin(), splinePoints.end(), splinePoints.begin(), std::bind1st(std::multiplies<Ogre::Real>(), 0.5f));

        initialVehicleSetup.mPower.init(splinePoints, hscale, vscale);
    }
    {
        Ogre::Real hscale = getCarParameter("", "suspension traction hscale", true);
        Ogre::Real vscale = getCarParameter("", "suspension traction vscale", true);

        std::vector<std::string> splinePointsStr = getCarArrayValueParameter("", "suspension traction");
        std::vector<Ogre::Real> splinePoints = convertSplinePoints(splinePointsStr);

        initialVehicleSetup.mSuspensionTraction.init(splinePoints, hscale, vscale);
    }
    {
        Ogre::Real hscale = getCarParameter("", "Steer hscale", true);
        Ogre::Real vscale = getCarParameter("", "Steer vscale", true);

        std::vector<std::string> splinePointsStr = getCarArrayValueParameter("", "Steer");
        std::vector<Ogre::Real> splinePoints = convertSplinePoints(splinePointsStr);

        initialVehicleSetup.mSteering.init(splinePoints, hscale, vscale);
    }
    {
        Ogre::Real hscale = getCarParameter("", "collision d_d hscale", true);
        Ogre::Real vscale = getCarParameter("", "collision d_d vscale", true);

        std::vector<std::string> splinePointsStr = getCarArrayValueParameter("", "collision d_d");
        std::vector<Ogre::Real> splinePoints = convertSplinePoints(splinePointsStr);

        PSSpline spline;
        spline.init(splinePoints, hscale, vscale);
        world->setCollisionSplinesDD(spline);
    }
    {
        Ogre::Real hscale = getCarParameter("", "collision v_v hscale", true);
        Ogre::Real vscale = getCarParameter("", "collision v_v vscale", true);

        std::vector<std::string> splinePointsStr = getCarArrayValueParameter("", "collision v_v");
        std::vector<Ogre::Real> splinePoints = convertSplinePoints(splinePointsStr);

        PSSpline spline;
        spline.init(splinePoints, hscale, vscale);
        world->setCollisionSplinesVV(spline);
    }
    {
        Ogre::Real hscale = getCarParameter("", "collision d_dv hscale", true);
        Ogre::Real vscale = getCarParameter("", "collision d_dv vscale", true);

        std::vector<std::string> splinePointsStr = getCarArrayValueParameter("", "collision d_dv");
        std::vector<Ogre::Real> splinePoints = convertSplinePoints(splinePointsStr);

        PSSpline spline;
        spline.init(splinePoints, hscale, vscale);
        world->setCollisionSplinesDDV(spline);
    }
    {
        Ogre::Real hscale = getCarParameter("", "collision v_dv hscale", true);
        Ogre::Real vscale = getCarParameter("", "collision v_dv vscale", true);

        std::vector<std::string> splinePointsStr = getCarArrayValueParameter("", "collision v_dv");
        std::vector<Ogre::Real> splinePoints = convertSplinePoints(splinePointsStr);

        PSSpline spline;
        spline.init(splinePoints, hscale, vscale);
        world->setCollisionSplinesVDV(spline);
    }
    //splines END


    if(isAI)
    {
        initGraphicsModel(pipeline, gameState, sceneMgr, mainNode, modelsPool, characterName, initialVehicleSetup, gameState.getAdvancedLightingAI());
    }
    else
    {
        initGraphicsModel(pipeline, gameState, sceneMgr, mainNode, modelsPool, characterName, initialVehicleSetup, gameState.getAdvancedLightingPlayer());
    }
    initPhysicalModel(world, mModelNode, mWheelNodes, initialVehicleSetup, gameState.getInputType());
}

void PSBaseCar::repositionVehicle(const Ogre::Matrix4& transform)
{

    Ogre::Quaternion chassisRot = transform.extractQuaternion();
    Ogre::Radian angle;
    Ogre::Vector3 axis;
    chassisRot.ToAngleAxis(angle, axis);
    chassisRot.FromAngleAxis(angle, Ogre::Vector3(-axis.x, -axis.y, axis.z));
    Ogre::Vector3 chassisPos(transform.getTrans());

    repositionVehicle(chassisPos, chassisRot);

}

void PSBaseCar::repositionVehicle(const Ogre::Vector3& chassisPos, const Ogre::Quaternion& chassisRot)
{

    if(mModelNode)
    {
        PSBaseGraphicsVehicle::repositionVehicle(chassisPos, chassisRot);
        PSBaseVehicle::repositionVehicle(chassisPos, chassisRot, mModelNode, mWheelNodes);
    }

}

void PSBaseCar::clear()
{
    PSBaseVehicle::clear();

    //mModelNode = NULL;
}

Ogre::Vector3 PSBaseCar::getLinearVelocitySpeedometer()const
{
    return mPhysicsVehicle->getLinearVelocitySpeedometer();
}

Ogre::Vector3 PSBaseCar::getAngularVelocitySpeedometer()const
{
    return mPhysicsVehicle->getAngularVelocitySpeedometer();
}

Ogre::Real PSBaseCar::getAlignedVelocity()const
{
    return mPhysicsVehicle->getAlignedVelocity();
}

Ogre::Real PSBaseCar::getAlignedVelocitySpeedometer()const
{
    return mPhysicsVehicle->getAlignedVelocitySpeedometer();
}

Ogre::Real PSBaseCar::getWheelBackLVelocity()const
{
    return mPhysicsVehicle->getWheelBackLVelocity();
}

Ogre::Real PSBaseCar::getWheelBackRVelocity()const
{
    return mPhysicsVehicle->getWheelBackRVelocity();
}

Ogre::Real PSBaseCar::getLateralVelocitySpeedometer()const
{
    Ogre::Real res = 0.0f;

    Ogre::Vector3 carVelocity = getLinearVelocitySpeedometer();

    Ogre::Vector3 carOrientation = mInitialVehicleSetup.mCarRot * Ogre::Vector3::NEGATIVE_UNIT_X;

    res = carOrientation.dotProduct(carVelocity);

    return res;
}

Ogre::Vector3 PSBaseCar::getForwardAxis()const
{
    return mInitialVehicleSetup.mCarRot * Ogre::Vector3::NEGATIVE_UNIT_Z;
}

Ogre::Vector3 PSBaseCar::getUpAxis()const
{
    return mInitialVehicleSetup.mCarRot * Ogre::Vector3::UNIT_Y;
}

Ogre::Vector3 PSBaseCar::getLinearImpulse()const
{
    return mPhysicsVehicle->getLinearImpulse();
}

void PSBaseCar::setLinearImpulse(const Ogre::Vector3& val)
{
    mPhysicsVehicle->setLinearImpulse(val);
}

Ogre::Vector3 PSBaseCar::getAngularImpulse()const
{
    return mPhysicsVehicle->getAngularImpulse();
}

void PSBaseCar::setAngularImpulse(const Ogre::Vector3& val)
{
    mPhysicsVehicle->setAngularImpulse(val);
}

void PSBaseCar::initSounds(lua_State * pipeline, const GameState& gameState)
{
#ifndef NO_OPENAL
    DMLuaManager luaManager;

    Ogre::Real referenceDist = luaManager.ReadScalarFloat("Model.Sfx.ReferenceDist", pipeline);

    stopSounds();

    std::string carName = gameState.getSTRPowerslide().getValue(getCharacterName() + " parameters", "car", "feral max");

    std::string de2Path = gameState.getSTRPowerslide().getValue(carName + " parameters", "base directory", "feral max");
    std::set<char> delim;
    delim.insert('\\');
    std::vector<std::string> pathComponents = Tools::splitpath(de2Path, delim, false);
    std::string carPath = pathComponents[pathComponents.size() - 1];

    mMasterGain = gameState.getSoundsGain();

    if(!mIsAI)
    {
        mEngLow.reset(new SoundSource("data/cars/" + carPath + "/sfx/enggears", "eng_low.its", gameState.getPFLoaderData()));
        mEngMid.reset(new SoundSource("data/cars/" + carPath + "/sfx/enggears", "eng_mid.its", gameState.getPFLoaderData()));
        mEngHigh.reset(new SoundSource("data/cars/" + carPath + "/sfx/enggears", "eng_high.its", gameState.getPFLoaderData()));

        mEngLow->setReferenceDistance(referenceDist);
        mEngMid->setReferenceDistance(referenceDist);
        mEngHigh->setReferenceDistance(referenceDist);

        mEngLow->setLooping(true);
        mEngMid->setLooping(true);
        mEngHigh->setLooping(true);

        mEngLow->setGain(mMasterGain);
        mEngMid->setGain(mMasterGain);
        mEngHigh->setGain(mMasterGain);
    }
    else
    {
        mEngHigh.reset(new SoundSource("data/cars/" + carPath + "/sfx/enggears", "eng_high.its", gameState.getPFLoaderData()));

        mEngHigh->setReferenceDistance(referenceDist);

        mEngHigh->setLooping(true);

        mEngHigh->setGain(mMasterGain);
    }
#endif
}

void PSBaseCar::deinitSounds()
{
#ifndef NO_OPENAL
    stopSounds();

    mEngLow.reset();
    mEngMid.reset();
    mEngHigh.reset();
#endif
}

void PSBaseCar::stopSounds()
{
#ifndef NO_OPENAL
    if(mEngLow.get() && mEngLow->getSourceState() == SoundSource::Playing)
    {
        mEngLow->stopPlaying();
    }

    if(mEngMid.get() && mEngMid->getSourceState() == SoundSource::Playing)
    {
        mEngMid->stopPlaying();
    }

    if(mEngHigh.get() && mEngHigh->getSourceState() == SoundSource::Playing)
    {
        mEngHigh->stopPlaying();
    }
#endif
}

float PSBaseCar::getCarParameter(const std::string& section, const std::string& key, bool isSpline) const
{
    float res = 0.0f;

    bool isFoundVal = false;

    if(!isSpline)
    {
        res = mCarSettings.getFloatValue(section, key, isFoundVal);
        if(!isFoundVal)
        {
            res = mTrackSettings.getFloatValue(section, key, isFoundVal);
            if(!isFoundVal)
            {
                res = mDefaultSettings.getFloatValue(section, key, isFoundVal);
            }
        }
    }
    else
    {
        res = mCarSplines.getFloatValue(section, key, isFoundVal);
        if(!isFoundVal)
        {
            res = mTrackSplines.getFloatValue(section, key, isFoundVal);
            if(!isFoundVal)
            {
                res = mDefaultSplines.getFloatValue(section, key, isFoundVal);
            }
        }
    }

    return res;
}

Ogre::Vector3 PSBaseCar::getCarArray3Parameter(const std::string& section, const std::string& key) const
{
    Ogre::Vector3 res = Ogre::Vector3::ZERO;

    bool isFoundVal = false;
    res = mCarSettings.getArray3Value(section, key, isFoundVal);
    if(!isFoundVal)
    {
        res = mTrackSettings.getArray3Value(section, key, isFoundVal);
        if(!isFoundVal)
        {
            res = mDefaultSettings.getArray3Value(section, key, isFoundVal);
        }
    }

    return res;
}

Ogre::Vector4 PSBaseCar::getCarArray4Parameter(const std::string& section, const std::string& key) const
{
    Ogre::Vector4 res = Ogre::Vector4::ZERO;

    bool isFoundVal = false;
    res = mCarSettings.getArray4Value(section, key, isFoundVal);
    if(!isFoundVal)
    {
        res = mTrackSettings.getArray4Value(section, key, isFoundVal);
        if(!isFoundVal)
        {
            res = mDefaultSettings.getArray4Value(section, key, isFoundVal);
        }
    }

    return res;
}

std::vector<std::string> PSBaseCar::getCarArrayValueParameter(const std::string& section, const std::string& key) const
{
    std::vector<std::string> res;

    bool isFoundVal = false;
    res = mCarSplines.getArrayValue(section, key, isFoundVal);
    if(!isFoundVal)
    {
        res = mTrackSplines.getArrayValue(section, key, isFoundVal);
        if(!isFoundVal)
        {
            res = mDefaultSplines.getArrayValue(section, key, isFoundVal);
        }
    }

    return res;
}

std::vector<Ogre::Real> PSBaseCar::convertSplinePoints(const std::vector<std::string>& points) const
{
    std::vector<Ogre::Real> ret(points.size());

    for(size_t q = 0; q < points.size(); ++q)
    {
        Ogre::Real val;
        Conversions::DMFromString(points[q], val);
        ret[q] = val;
    }

    return ret;
}

