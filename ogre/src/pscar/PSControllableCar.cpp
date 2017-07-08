
#include "PSControllableCar.h"

#include "../mesh/StaticMeshProcesser.h"

#include "../customs/CustomRigidBodyWheel.h"
#include "../CameraMan.h"
#include "../tools/Conversions.h"

#include "../lua/DMLuaManager.h"

#ifndef NO_OPENAL
    #include "../OpenAL/OpenALSource.h"
#endif

#include "../tools/OgreTools.h"

namespace
{
    const float defaultLowPitch = 0.25f;
    const float defaultMidPitch = 0.15f;
    const float defaultHighPitch = 0.5f;
}

Ogre::NameGenerator PSControllableCar::nameGenMaterialsParticles("Scene/Material/Particles/Name");

PSControllableCar::PSControllableCar() :
    mIsDisableMouse(true),
    mCameraMan(NULL),
    mWheelCollisionFrontL(false),
    mWheelCollisionFrontR(false),
    mWheelCollisionBackL(false),
    mWheelCollisionBackR(false),
    mChassisCollision(false),
    mSteering(0.0f),
    mSteeringLeft(false),
    mSteeringRight(false),
    mBrakeEnabled(false),
    mAccelEnabled(false),
    mSteeringIncrement(2.0f),
    mLateralStabilizationCoeff(0.2f),
    mWheelRotationalAngle(0.0f)
{
    mDriveImpulse.addPoint(-100.0f, 0.0f);
    mDriveImpulse.addPoint(0.0f, 25.0f);
    mDriveImpulse.addPoint(60.0f, 20.0f);
    mDriveImpulse.addPoint(120.0f, 18.0f);
    mDriveImpulse.addPoint(200.0f, 15.0f);
    mDriveImpulse.addPoint(240.0f, 6.0f);
    mDriveImpulse.addPoint(300.0f, 0.0f);

    mResistanceImpulse.addPoint(160.0f, 0.0f);
    mResistanceImpulse.addPoint(300.0f, 20.0f);

    mGroundSpoilerImpulse.addPoint(0.0f, 0.0f);
    mGroundSpoilerImpulse.addPoint(200.0f, 15.0f);

    mWheelsRotationByEngineAddition.addPoint(-50.0f, 20.0f);
    mWheelsRotationByEngineAddition.addPoint(0.0f, 200.0f);
    mWheelsRotationByEngineAddition.addPoint(50.0f, 20.0f);
    mWheelsRotationByEngineAddition.addPoint(100.0f, 0.0f);
}

void PSControllableCar::initModel(  lua_State * pipeline, 
                                    const GameState& gameState,
                                    Ogre::SceneManager* sceneMgr, Ogre::SceneNode* mainNode,
                                    ModelsPool* modelsPool,
                                    OgreBulletDynamics::DynamicsWorld * world,
                                    const std::string& characterName,
                                    const Ogre::Matrix4& transform,
                                    bool isPossesCamera,
                                    bool isAI)
{
    mIsDisableMouse = true;
    mWheelCollisionFrontL = false;
    mWheelCollisionFrontR = false;
    mWheelCollisionBackL = false;
    mWheelCollisionBackR = false;
    mChassisCollision = false;
    mSteering = 0.0f;
    mSteeringLeft = false;
    mSteeringRight = false;
    mBrakeEnabled = false;
    mAccelEnabled = false;
#if defined(__ANDROID__)
    if(!isAI)
        mAccelEnabled = true;
#endif
    mWheelRotationalAngle = 0.0f;

    mParticles = gameState.getParticles();

    PSBaseCar::initModel(pipeline, gameState, sceneMgr, mainNode, modelsPool, world, characterName, transform, isAI);

    {
        std::string carName = gameState.getSTRPowerslide().getValue(characterName + " parameters", "car", "feral max");
        std::string de2Path = gameState.getSTRPowerslide().getValue(carName + " parameters", "base directory", "feral max");
        std::set<char> delim;
        delim.insert('\\');
        std::vector<std::string> pathComponents = Tools::splitpath(de2Path, delim, false);
        std::string carPath = pathComponents[pathComponents.size() - 1];

        STRSettings carSettings;
        carSettings.parse(gameState.getPFLoaderStore(), "data/cars/" + carPath + "/data/default", "params.str");

        STRSettings trackSettings;
        trackSettings.parse(gameState.getPFLoaderStore(), "data/cars/global/data/" + gameState.getSTRPowerslide().getDataSubDir(gameState.getTrackName()), "params.str");

        STRSettings defaultSettings;
        defaultSettings.parse(gameState.getPFLoaderStore(), "data/cars/global/data/default", "params.str");

        float gearRevRatio = getCarParameter(carSettings, trackSettings, defaultSettings, "", "gear rev ratio");
        Ogre::Vector4 revRatio = getCarArray4Parameter(carSettings, trackSettings, defaultSettings, "", "rev ratio");
        Ogre::Vector4 changeDown = getCarArray4Parameter(carSettings, trackSettings, defaultSettings, "", "change down");
        Ogre::Vector4 changeUp = getCarArray4Parameter(carSettings, trackSettings, defaultSettings, "", "change up");

        mCarEngine.init(gearRevRatio, revRatio, changeDown, changeUp);

        //sound params
        STRSettings soundSettings;
        soundSettings.parse(gameState.getPFLoaderStore(), "data/cars/" + carPath + "/data/default", "graphs.str");
        std::vector<std::string> freqLow = soundSettings.getArrayValue("", "frequency 0");
        std::vector<std::string> freqMid = soundSettings.getArrayValue("", "frequency 1");
        std::vector<std::string> freqHigh = soundSettings.getArrayValue("", "frequency 2");
        std::vector<std::string> volLow = soundSettings.getArrayValue("", "volume 0");
        std::vector<std::string> volMid = soundSettings.getArrayValue("", "volume 1");
        std::vector<std::string> volHigh = soundSettings.getArrayValue("", "volume 2");
        assert(freqLow.size() == 11);
        assert(freqMid.size() == 11);
        assert(freqHigh.size() == 11);
        assert(volLow.size() == 11);
        assert(volMid.size() == 11);
        assert(volHigh.size() == 11);

        float hscale0 = soundSettings.getFloatValue("", "frequency 0 hscale");
        float hscale1 = soundSettings.getFloatValue("", "frequency 1 hscale");
        float hscale2 = soundSettings.getFloatValue("", "frequency 2 hscale");

        mPitchValueLow.clear();
        mPitchValueMid.clear();
        mPitchValueHigh.clear();
        mGainValueLow.clear();
        mGainValueMid.clear();
        mGainValueHigh.clear();

        for(size_t q = 0; q < 11; ++q)
        {
            float val;
            Conversions::DMFromString(freqLow[q], val);
            mPitchValueLow.addPoint(static_cast<float>(q) * 1000.0f, val * hscale0);

            Conversions::DMFromString(freqMid[q], val);
            mPitchValueMid.addPoint(static_cast<float>(q) * 1000.0f, val * hscale1);

            Conversions::DMFromString(freqHigh[q], val);
            mPitchValueHigh.addPoint(static_cast<float>(q) * 1000.0f, val * hscale2);

            Conversions::DMFromString(volLow[q], val);
            mGainValueLow.addPoint(static_cast<float>(q) * 1000.0f, val);

            Conversions::DMFromString(volMid[q], val);
            mGainValueMid.addPoint(static_cast<float>(q) * 1000.0f, val);

            Conversions::DMFromString(volHigh[q], val);
            mGainValueHigh.addPoint(static_cast<float>(q) * 1000.0f, val);
        }
    }

    DMLuaManager luaManager;
    mSteeringMin = luaManager.ReadScalarFloat("Model.Physics.Wheels.Front.MinSteer", pipeline);
    mSteeringMinRad = Ogre::Degree(mSteeringMin).valueRadians();
    mSteeringMax = luaManager.ReadScalarFloat("Model.Physics.Wheels.Front.MaxSteer", pipeline);
    mSteeringMaxRad = Ogre::Degree(mSteeringMax).valueRadians();

    mIsPossesCamera = isPossesCamera;

    mWheelBackLParticle = sceneMgr->createParticleSystem(nameGenNodes.generate(), "Particle/Wheel");
    mWheelBackRParticle = sceneMgr->createParticleSystem(nameGenNodes.generate(), "Particle/Wheel");

    const Ogre::Vector2& fogStartEnd = gameState.getSTRPowerslide().getFogStartEnd(gameState.getTrackName());
    bool isFogEnabled = fogStartEnd.x >= 1000000.0f ? false : true;

    mParticleMaterialName = nameGenMaterialsParticles.generate();

    if(!isFogEnabled)
    {
        CloneMaterial(  mParticleMaterialName, 
                        "Test/Particle", 
                        std::vector<Ogre::String>(), 
                        1.0f,
                        TEMP_RESOURCE_GROUP_NAME);
    }
    else
    {
        Ogre::MaterialPtr newMatParticle = CloneMaterial(  mParticleMaterialName, 
                        "Test/ParticleFog", 
                        std::vector<Ogre::String>(), 
                        1.0f,
                        TEMP_RESOURCE_GROUP_NAME);

        const Ogre::ColourValue& skyColor = gameState.getSTRPowerslide().getTrackSkyColor(gameState.getTrackName());

        //newMatParticle->setFog(true, Ogre::FOG_LINEAR, skyColor, 0.0f, fogStartEnd.x, fogStartEnd.y);
    }

    mWheelBackLParticle->setMaterialName(mParticleMaterialName);
    mWheelBackRParticle->setMaterialName(mParticleMaterialName);

    mParticleNodeWheelBackL = sceneMgr->getRootSceneNode()->createChildSceneNode(nameGenNodes.generate());
    mParticleNodeWheelBackL->attachObject(mWheelBackLParticle);

    mParticleNodeWheelBackR = sceneMgr->getRootSceneNode()->createChildSceneNode(nameGenNodes.generate());
    mParticleNodeWheelBackR->attachObject(mWheelBackRParticle);

    mAirSpoilerImpulse.addPoint(0.0f, 0.0f);
    if(isAI)
        mAirSpoilerImpulse.addPoint(200.0f, 30.0f);
    else
        mAirSpoilerImpulse.addPoint(150.0f, 50.0f);
}

void PSControllableCar::processWheelsCollision(   btManifoldPoint& cp, 
                                            const btCollisionObjectWrapper* colObj0Wrap, 
                                            const btCollisionObjectWrapper* colObj1Wrap,
                                            StaticMeshProcesser& processer,
                                            int triIndex)
{

    Ogre::Vector3 normalOnStatic = OgreBulletCollisions::convert(cp.m_normalWorldOnB);
    Ogre::Vector3 linearVel = getLinearVelocity();
    Ogre::Real speed = linearVel.length();
    linearVel.normalise();
    bool isHorizontalNormal = normalOnStatic.y < 0.2f ? true : false;
    Ogre::Real linearDotNormal = Ogre::Math::Abs(linearVel.dotProduct(normalOnStatic));
    //const Ogre::Real jumpImpulse = mJumpImpulse.getVal(speed);
    const Ogre::Real normalThreshold = 0.3f;
    const unsigned long timerThreshold = 10; // ms

    bool isSpeedEnoughToJump = speed > 10.0f;


    std::pair<int, int> address;

    if(processer.isRigidBodyStatic(colObj1Wrap->getCollisionObject(), address))
    {

        if(colObj0Wrap->getCollisionObject() == mCarWheelFrontL->getBulletRigidBody())
        {
            mWheelCollisionFrontL = true;

            std::string terrainMap = processer.getBatchByAddress(address).mTerrainMap;
            terrainMap = terrainMap.substr(0, terrainMap.length() - 4);
            //mWheelFrontLColliderIndex = terrainMap;
            //mWheelFrontLColliderIndex = Conversions::DMToString(triIndex);
            //Ogre::Vector2 texCoord = processer.getTextureCoordinateInTriangle(address, triIndex, cp.getPositionWorldOnB());
            //mWheelFrontLColliderIndex = Conversions::DMToString(texCoord.x);
            unsigned char terrainType = processer.getTerrainType(address, triIndex, cp.getPositionWorldOnB());
            mWheelFrontLColliderString = terrainMap + " " + Conversions::DMToString((size_t)terrainType);
            mWheelFrontLColliderIndex = terrainType;

            if(isHorizontalNormal && isSpeedEnoughToJump)
            {
                if(linearDotNormal > normalThreshold && mTimerJumpHappenFrontL.getMilliseconds() > timerThreshold)
                {
                    mTimerJumpHappenFrontL.reset();
                    //mCarWheelFrontL->applyImpulse(Ogre::Vector3(0.0f, jumpImpulse, 0.0f), Ogre::Vector3::ZERO);

                    Ogre::Vector3 origin = OgreBulletCollisions::convert(mCarWheelFrontL->getBulletRigidBody()->getWorldTransform().getOrigin());
                    
                    Ogre::Quaternion rot = mModelNode->getOrientation();
                    Ogre::Quaternion rotInv = rot.Inverse();
                    Ogre::Vector3 originTransformed = rotInv * origin;

                    originTransformed.y += mInitialVehicleSetup.mWheelRadius.x;
                    originTransformed = rot * originTransformed;

                    mCarWheelFrontL->setExternalUpdate(originTransformed);
                }
            }
        }

        if(colObj0Wrap->getCollisionObject() == mCarWheelFrontR->getBulletRigidBody())
        {
            mWheelCollisionFrontR = true;

            std::string terrainMap = processer.getBatchByAddress(address).mTerrainMap;
            terrainMap = terrainMap.substr(0, terrainMap.length() - 4);
            //mWheelFrontRColliderIndex = processer.getBatchByAddress(address).mTerrainMap;
            unsigned char terrainType = processer.getTerrainType(address, triIndex, cp.getPositionWorldOnB());
            mWheelFrontRColliderString = terrainMap + " " + Conversions::DMToString((size_t)terrainType);
            mWheelFrontRColliderIndex = terrainType;

            if(isHorizontalNormal && isSpeedEnoughToJump)
            {
                if(linearDotNormal > normalThreshold && mTimerJumpHappenFrontR.getMilliseconds() > timerThreshold)
                {
                    mTimerJumpHappenFrontR.reset();
                    //mCarWheelFrontR->applyImpulse(Ogre::Vector3(0.0f, jumpImpulse, 0.0f), Ogre::Vector3::ZERO);

                    Ogre::Vector3 origin = OgreBulletCollisions::convert(mCarWheelFrontR->getBulletRigidBody()->getWorldTransform().getOrigin());
                    
                    Ogre::Quaternion rot = mModelNode->getOrientation();
                    Ogre::Quaternion rotInv = rot.Inverse();
                    Ogre::Vector3 originTransformed = rotInv * origin;

                    originTransformed.y += mInitialVehicleSetup.mWheelRadius.x;
                    originTransformed = rot * originTransformed;

                    mCarWheelFrontR->setExternalUpdate(originTransformed);
                }
            }
        }

        if(colObj0Wrap->getCollisionObject() == mCarWheelBackL->getBulletRigidBody())
        {
            mWheelCollisionBackL = true;

            std::string terrainMap = processer.getBatchByAddress(address).mTerrainMap;
            terrainMap = terrainMap.substr(0, terrainMap.length() - 4);
            //mWheelBackLColliderIndex = processer.getBatchByAddress(address).mTerrainMap;
            unsigned char terrainType = processer.getTerrainType(address, triIndex, cp.getPositionWorldOnB());
            mWheelBackLColliderString = terrainMap + " " + Conversions::DMToString((size_t)terrainType);
            mWheelBackLColliderIndex = terrainType;

            if(isHorizontalNormal && isSpeedEnoughToJump)
            {
                if(linearDotNormal > normalThreshold && mTimerJumpHappenRearL.getMilliseconds() > timerThreshold)
                {
                    mTimerJumpHappenRearL.reset();
                    //mCarWheelBackL->applyImpulse(Ogre::Vector3(0.0f, jumpImpulse, 0.0f), Ogre::Vector3::ZERO);

                    Ogre::Vector3 origin = OgreBulletCollisions::convert(mCarWheelBackL->getBulletRigidBody()->getWorldTransform().getOrigin());
                    
                    Ogre::Quaternion rot = mModelNode->getOrientation();
                    Ogre::Quaternion rotInv = rot.Inverse();
                    Ogre::Vector3 originTransformed = rotInv * origin;

                    originTransformed.y += mInitialVehicleSetup.mWheelRadius.y;
                    originTransformed = rot * originTransformed;

                    mCarWheelBackL->setExternalUpdate(originTransformed);
                }
            }
        }

        if(colObj0Wrap->getCollisionObject() == mCarWheelBackR->getBulletRigidBody())
        {
            mWheelCollisionBackR = true;

            std::string terrainMap = processer.getBatchByAddress(address).mTerrainMap;
            terrainMap = terrainMap.substr(0, terrainMap.length() - 4);
            //mWheelBackRColliderIndex = processer.getBatchByAddress(address).mTerrainMap;
            unsigned char terrainType = processer.getTerrainType(address, triIndex, cp.getPositionWorldOnB());
            mWheelBackRColliderString = terrainMap + " " + Conversions::DMToString((size_t)terrainType);
            mWheelBackRColliderIndex = terrainType;

            if(isHorizontalNormal && isSpeedEnoughToJump)
            {
                if(linearDotNormal > normalThreshold && mTimerJumpHappenRearR.getMilliseconds() > timerThreshold)
                {
                    mTimerJumpHappenRearR.reset();
                    //mCarWheelBackR->applyImpulse(Ogre::Vector3(0.0f, jumpImpulse, 0.0f), Ogre::Vector3::ZERO);

                    Ogre::Vector3 origin = OgreBulletCollisions::convert(mCarWheelBackR->getBulletRigidBody()->getWorldTransform().getOrigin());
                    
                    Ogre::Quaternion rot = mModelNode->getOrientation();
                    Ogre::Quaternion rotInv = rot.Inverse();
                    Ogre::Vector3 originTransformed = rotInv * origin;

                    originTransformed.y += mInitialVehicleSetup.mWheelRadius.y;
                    originTransformed = rot * originTransformed;

                    mCarWheelBackR->setExternalUpdate(originTransformed);
                }
            }
        }
    }
}

void PSControllableCar::processChassisCollision(   btManifoldPoint& cp, 
                                            const btCollisionObjectWrapper* colObj0Wrap, 
                                            const btCollisionObjectWrapper* colObj1Wrap,
                                            StaticMeshProcesser& processer,
                                            int triIndex)
{
    if(colObj0Wrap->getCollisionObject() == mCarChassis->getBulletRigidBody())
    {
        std::pair<int, int> address;

        if(processer.isRigidBodyStatic(colObj1Wrap->getCollisionObject(), address))
        {
            mChassisCollision = true;
            unsigned char terrainType = processer.getTerrainType(address, triIndex, cp.getPositionWorldOnB());
            mChassisColliderIndex = terrainType;
        }
    }
}

void PSControllableCar::processInternalTick(float timeStep, bool isRaceStarted)
{
    Ogre::Quaternion rot = mCarChassis->getSceneNode()->_getDerivedOrientation();

    Ogre::Real projectedVel = getAlignedVelocity();
    Ogre::Vector3 carForwardVector = getForwardAxis();

    Ogre::Real spfFake = 1.5f;
    Ogre::Real wheelsPushImpulse = mDriveImpulse.getVal(projectedVel) * spfFake;
    if(projectedVel < 0.0f && mAccelEnabled)wheelsPushImpulse = mDriveImpulse.getVal(-projectedVel) * spfFake;

    Ogre::Real wheelsResistanceImpulse = mResistanceImpulse.getVal(projectedVel) * spfFake;
    Ogre::Real cockpitGroundSpoilerImpulse = mGroundSpoilerImpulse.getVal(projectedVel) * spfFake;
    Ogre::Real cockpitAirSpoilerImpulse = mAirSpoilerImpulse.getVal(projectedVel) * spfFake;

    Ogre::Real antiForwardProjY = Ogre::Vector3::NEGATIVE_UNIT_Y.dotProduct(carForwardVector);
    Ogre::Real carAntiSlopeCoefficient = 1.0f - antiForwardProjY;

    if (mBrakeEnabled && isRaceStarted)
    {
        if(checkRearCollision() || checkFrontCollision())
        {
#if defined(__ANDROID__)
            if(!mIsAI)
            {
                if(projectedVel > 10.0f)
                {
                    mCarChassis->applyImpulse(rot * Ogre::Vector3(0.0f, 0.0f, wheelsPushImpulse * carAntiSlopeCoefficient), rot * Ogre::Vector3(6, -2, 4));
                    mCarChassis->applyImpulse(rot * Ogre::Vector3(0.0f, 0.0f, wheelsPushImpulse * carAntiSlopeCoefficient), rot * Ogre::Vector3(-6, -2, 4));
                }
                else
                {
                    setBrake(false);
                }
            }
#else
            mCarChassis->applyImpulse(rot * Ogre::Vector3(0.0f, 0.0f, wheelsPushImpulse * carAntiSlopeCoefficient), rot * Ogre::Vector3(6, -2, 4));
            mCarChassis->applyImpulse(rot * Ogre::Vector3(0.0f, 0.0f, wheelsPushImpulse * carAntiSlopeCoefficient), rot * Ogre::Vector3(-6, -2, 4));
#endif
        }
    }

    Ogre::Real forwardProjY = Ogre::Vector3::UNIT_Y.dotProduct(carForwardVector);
    Ogre::Real carSlopeCoefficient = 1.0f - forwardProjY;
    //carSlopeCoefficient = Ogre::Math::Clamp(carSlopeCoefficient, 0.0f, 1.0f);

    Ogre::Real lateralVel = getLateralVelocity();

    const float reduceRollResistance = 10.0f;

    if (mAccelEnabled && isRaceStarted)
    {
        if(checkRearCollision(true) /*|| checkFrontCollision()*/)
        {
            const Ogre::Real backLRollImpulse = 1.0f - mBackLRollResistance / reduceRollResistance;
            const Ogre::Real backRRollImpulse = 1.0f - mBackRRollResistance / reduceRollResistance;

            mCarChassis->applyImpulse(rot * Ogre::Vector3(0.0f, 0.0f, -wheelsPushImpulse * carSlopeCoefficient * backLRollImpulse), rot * Ogre::Vector3(-6, -2, 4));
            mCarChassis->applyImpulse(rot * Ogre::Vector3(0.0f, 0.0f, -wheelsPushImpulse * carSlopeCoefficient * backRRollImpulse), rot * Ogre::Vector3(6, -2, 4));

            mCarChassis->applyImpulse(rot * Ogre::Vector3(lateralVel * mLateralStabilizationCoeff, 0.0f, 0.0f), Ogre::Vector3::ZERO);
        }
    }

    //terrain back friction
    if(checkRearCollision() && isRaceStarted)
    {
            mCarChassis->applyImpulse(rot * Ogre::Vector3(0.0f, 0.0f, wheelsResistanceImpulse * mBackLRollResistance / reduceRollResistance), rot * Ogre::Vector3(-6, -2, 4));
            mCarChassis->applyImpulse(rot * Ogre::Vector3(0.0f, 0.0f, wheelsResistanceImpulse * mBackRRollResistance / reduceRollResistance), rot * Ogre::Vector3(6, -2, 4));
    }

    if(!checkRearCollision() && !checkFrontCollision())
        mCarChassis->applyImpulse(Ogre::Vector3(0.0f, -cockpitAirSpoilerImpulse, 0.0f), rot * Ogre::Vector3(0.0f, -1.0f, -0.5f));
    else
        mCarChassis->applyImpulse(Ogre::Vector3(0.0f, -cockpitGroundSpoilerImpulse, 0.0f), Ogre::Vector3::ZERO);
}

void PSControllableCar::processFrameBeforePhysics(const Ogre::FrameEvent &evt, StaticMeshProcesser& processer, bool isRaceStarted)
{
    processSounds(evt);

    adjustWheelsFriction(processer);

    Ogre::Quaternion rot = mCarChassis->getSceneNode()->_getDerivedOrientation();

    Ogre::Vector3 linearVelocity = getLinearVelocity();

    Ogre::Vector3 carUpVector = rot * Ogre::Vector3::UNIT_Y;
    Ogre::Real upProjY = Ogre::Vector3::UNIT_Y.dotProduct(carUpVector);

    //freeze on low speed
    if( Ogre::Math::Abs(linearVelocity.length()) < 10.0f    &&
        upProjY > 0.96f                         &&
        !mSteeringLeft                          &&
        !mSteeringRight                         &&
        !mBrakeEnabled                          &&
        !mAccelEnabled
        )
    {
        mCarChassis->getBulletRigidBody()->setLinearFactor(btVector3(0.0f, 1.0f, 0.0f));
        mCarChassis->getBulletRigidBody()->setLinearVelocity(btVector3(0.0f, 0.0f, 0.0f));
        //mCarChassis->setActivationState(DISABLE_SIMULATION);
    }
    else
    {
        mCarChassis->getBulletRigidBody()->setLinearFactor(btVector3(1.0f, 1.0f, 1.0f));
        //mCarChassis->forceActivationState();
    }

    Ogre::Real projectedVel = getAlignedVelocity();


    const Ogre::Real wheelRadius = 2.0f;
    Ogre::Real projectedDist = projectedVel * evt.timeSinceLastFrame;
    Ogre::Real wheelRotationalIncrement = projectedDist / wheelRadius;
    mWheelRotationalAngle += wheelRotationalIncrement * 80.0f;

    //wheels rotation by engine addition
    if( (mAccelEnabled || mBrakeEnabled) && isRaceStarted)
    {
        Ogre::Real rotAngleAddition = mWheelsRotationByEngineAddition.getVal(projectedVel);
        if(mBrakeEnabled && rotAngleAddition > 0.0f) rotAngleAddition *= -1.0f;
        mWheelRotationalAngle += rotAngleAddition;

        if(!mParticles.empty())
        {
            adjustWheelsParticles(rot, rotAngleAddition);
        }
    }
    else
    {
        mWheelBackLParticle->setEmitting(false);
        mWheelBackRParticle->setEmitting(false);
    }


    adjustFrontWheelsAngle(evt);


    Ogre::Quaternion rotDrive;
    rotDrive.FromAngleAxis(Ogre::Degree(-mWheelRotationalAngle), Ogre::Vector3(1.0f, 0.0f, 0.0f));

    Ogre::Quaternion turnWheel;
    turnWheel.FromAngleAxis(Ogre::Degree(mSteering), Ogre::Vector3(0.0f, 1.0f, 0.0f));

    if(isRaceStarted)
    {
        mCarWheelFrontL->getBulletObject()->getWorldTransform().setRotation(OgreBulletCollisions::convert(rot * turnWheel * rotDrive));
        mCarWheelFrontR->getBulletObject()->getWorldTransform().setRotation(OgreBulletCollisions::convert(rot * turnWheel * rotDrive));
    }
    else
    {
        mWheelNodes[2]->setOrientation(rot * turnWheel);
        mWheelNodes[3]->setOrientation(rot * turnWheel);
    }

    mCarWheelBackL->getBulletObject()->getWorldTransform().setRotation(OgreBulletCollisions::convert(rot * rotDrive));
    mCarWheelBackR->getBulletObject()->getWorldTransform().setRotation(OgreBulletCollisions::convert(rot * rotDrive));


    cleanWheelsCollisionsFlags();
}

void PSControllableCar::adjustFrontWheelsAngle(const Ogre::FrameEvent &evt)
{
    Ogre::Real spf = evt.timeSinceLastFrame * 100.0f;

    if (mSteeringLeft)
    {
        mSteering += mSteeringIncrement * spf;
        if (mSteering > mSteeringMax)
            mSteering = mSteeringMax;
    }
    else if (mSteeringRight)
    {
        mSteering -= mSteeringIncrement * spf;
        if (mSteering < mSteeringMin)
            mSteering = mSteeringMin;
    }
    else
    {
        if(mSteering < -mSteeringIncrement)
        {
            mSteering += mSteeringIncrement * spf;
        }
        else if(mSteering > mSteeringIncrement)
        {
            mSteering -= mSteeringIncrement * spf;
        }
        else
        {
            mSteering = 0.0f;
        }
    }
}

void PSControllableCar::processFrameAfterPhysics(const Ogre::FrameEvent &evt, bool isRaceStarted)
{
    if(mCameraMan && mIsPossesCamera)
    {
        if(mCameraMan->getCameraPositionType() == CameraPosition_Bumper)
        {
            setVisibility(false);
            //mParticleNodeWheelBackL->setVisible(false);
            //mParticleNodeWheelBackR->setVisible(false);
        }
        else
        {
            setVisibility(true);
            //mParticleNodeWheelBackL->setVisible(true);
            //mParticleNodeWheelBackR->setVisible(true);
        }

        if(mIsDisableMouse)
        {
            Ogre::Camera* cam = mCameraMan->getCamera();
            cam->setAutoTracking(false);
            mCameraMan->setYawPitchDist(mModelNode->getOrientation(), mModelNode->getPosition(), getLateralVelocity(), getAlignedVelocity());
        }
        else
        {
            Ogre::Camera* cam = mCameraMan->getCamera();
            cam->setAutoTracking(true, mModelNode);
        }
        
    }

    if(isRaceStarted && isRollOver())
    {
        restoreRollOver();
    }

    PSBaseCar::processFrameAfterPhysics(evt);
}


void PSControllableCar::adjustWheelsFriction(StaticMeshProcesser& processer)
{
    Ogre::Real latitudeFriction;
    Ogre::Real longtitudeFriction;
    Ogre::Vector3 anisotropicFriction;

    latitudeFriction = processer.getLatitudeFriction(getFrontLWheelColliderIndex());
    longtitudeFriction = processer.getLongtitudeFriction(getFrontLWheelColliderIndex());
    anisotropicFriction.x = latitudeFriction;
    anisotropicFriction.y = longtitudeFriction;
    anisotropicFriction.z = longtitudeFriction;
    mCarWheelFrontL->getBulletRigidBody()->setAnisotropicFriction(OgreBulletCollisions::convert(anisotropicFriction));

    latitudeFriction = processer.getLatitudeFriction(getFrontRWheelColliderIndex());
    longtitudeFriction = processer.getLongtitudeFriction(getFrontRWheelColliderIndex());
    anisotropicFriction.x = latitudeFriction;
    anisotropicFriction.y = longtitudeFriction;
    anisotropicFriction.z = longtitudeFriction;
    mCarWheelFrontR->getBulletRigidBody()->setAnisotropicFriction(OgreBulletCollisions::convert(anisotropicFriction));

    latitudeFriction = processer.getLatitudeFriction(getBackLWheelColliderIndex());
    longtitudeFriction = processer.getLongtitudeFriction(getBackLWheelColliderIndex());
    mBackLRollResistance = longtitudeFriction;
    anisotropicFriction.x = latitudeFriction;
    anisotropicFriction.y = longtitudeFriction;
    anisotropicFriction.z = longtitudeFriction;
    mCarWheelBackL->getBulletRigidBody()->setAnisotropicFriction(OgreBulletCollisions::convert(anisotropicFriction));

    latitudeFriction = processer.getLatitudeFriction(getBackRWheelColliderIndex());
    longtitudeFriction = processer.getLongtitudeFriction(getBackRWheelColliderIndex());
    mBackRRollResistance = longtitudeFriction;
    anisotropicFriction.x = latitudeFriction;
    anisotropicFriction.y = longtitudeFriction;
    anisotropicFriction.z = longtitudeFriction;
    mCarWheelBackR->getBulletRigidBody()->setAnisotropicFriction(OgreBulletCollisions::convert(anisotropicFriction));
}

void PSControllableCar::adjustWheelsParticles(const Ogre::Quaternion& rot, Ogre::Real rotAngleAddition)
{
    Ogre::Vector3 wheelBackLPos = mWheelNodes[1]->getPosition();
    Ogre::Vector3 wheelBackRPos = mWheelNodes[0]->getPosition();
    Ogre::Vector3 particlesShift = rot * (Ogre::Vector3::NEGATIVE_UNIT_Z * mInitialVehicleSetup.mWheelRadius.y);
    Ogre::Vector3 particlesDir = rot * Ogre::Vector3(0.0f, 1.0f, 0.1f);
    wheelBackLPos.y -= mInitialVehicleSetup.mWheelRadius.y / 1.5f;
    wheelBackRPos.y -= mInitialVehicleSetup.mWheelRadius.y / 1.5f;
    wheelBackLPos -= particlesShift;
    wheelBackRPos -= particlesShift;
    mParticleNodeWheelBackL->setPosition(wheelBackLPos);
    mParticleNodeWheelBackR->setPosition(wheelBackRPos);

    unsigned char terrainIndexL = getBackLWheelColliderIndex();
    unsigned char terrainIndexR = getBackRWheelColliderIndex();

    const Particle& particleL = mParticles[terrainIndexL];
    const Particle& particleR = mParticles[terrainIndexR];
    
    mWheelBackLParticle->setEmitting(   mWheelCollisionBackL                && 
                                        (mAccelEnabled || mBrakeEnabled)    &&
                                        particleL.isVisible
                                        );

    mWheelBackRParticle->setEmitting(   mWheelCollisionBackR                && 
                                        (mAccelEnabled || mBrakeEnabled)    &&
                                        particleR.isVisible
                                        );

    mWheelBackLParticle->getEmitter(0)->setColour(particleL.rgb1);
    mWheelBackRParticle->getEmitter(0)->setColour(particleR.rgb1);

    mWheelBackLParticle->getEmitter(0)->setDirection(particlesDir);
    mWheelBackRParticle->getEmitter(0)->setDirection(particlesDir);
    //mWheelBackLParticle->getEmitter(0)->setParticleVelocity(linearVel);
    //mWheelBackRParticle->getEmitter(0)->setParticleVelocity(linearVel);
    //if(linearVel > 10.0f || linearVel < -10.0f)
    {
        //Ogre::Real emmisionRate = mParticlesEmmisionRate.getVal(rotAngleAddition);
        Ogre::Real emmisionRate = Ogre::Math::Abs(rotAngleAddition);
        mWheelBackLParticle->getEmitter(0)->setEmissionRate(emmisionRate + 40.0f);
        mWheelBackRParticle->getEmitter(0)->setEmissionRate(emmisionRate + 40.0f);
    }

    Ogre::MaterialPtr particleMaterial = Ogre::MaterialManager::getSingleton().getByName(mParticleMaterialName);
    Ogre::TextureUnitState * stateParticle = particleMaterial->getTechnique(0)->getPass(0)->getTextureUnitState(0);
    stateParticle->setTextureScale(8.0f, 8.0f);

    //snow? (1st row left to right)[left 4 samples]
    if(particleL.index == 0)
    {
        stateParticle->setTextureScroll(-7.0f / 16.0f, -7.0f / 16.0f);
        //stateParticle->setTextureScroll(-5.0f / 16.0f, -7.0f / 16.0f);
        //stateParticle->setTextureScroll(-3.0f / 16.0f, -7.0f / 16.0f);
        //stateParticle->setTextureScroll(-1.0f / 16.0f, -7.0f / 16.0f);
    }

    //(2nd row left to right)[left 4 samples]
    if(particleL.index == 1)
    {
        stateParticle->setTextureScroll(-7.0f / 16.0f, -5.0f / 16.0f);
        //stateParticle->setTextureScroll(-5.0f / 16.0f, -5.0f / 16.0f);
        //stateParticle->setTextureScroll(-3.0f / 16.0f, -5.0f / 16.0f);
        //stateParticle->setTextureScroll(-1.0f / 16.0f, -5.0f / 16.0f);
    }

    //(3rd row left to right)[left 4 samples]
    if(particleL.index == 2)
    {
        stateParticle->setTextureScroll(-7.0f / 16.0f, -3.0f / 16.0f);
        //stateParticle->setTextureScroll(-5.0f / 16.0f, -3.0f / 16.0f);
        //stateParticle->setTextureScroll(-3.0f / 16.0f, -3.0f / 16.0f);
        //stateParticle->setTextureScroll(-1.0f / 16.0f, -3.0f / 16.0f);
    }

    //mud (4th row left to right)[left 4 samples]
    if(particleL.index == 3)
    {
        stateParticle->setTextureScroll(-7.0f / 16.0f, -1.0f / 16.0f);
        //stateParticle->setTextureScroll(-5.0f / 16.0f, -1.0f / 16.0f);
        //stateParticle->setTextureScroll(-5.0f / 16.0f, -1.0f / 16.0f);
        //stateParticle->setTextureScroll(-3.0f / 16.0f, -1.0f / 16.0f);
    }

    //(2nd row left to right)[right 4 samples]
    //stateParticle->setTextureScroll(1.0f / 16.0f, -5.0f / 16.0f);
    //stateParticle->setTextureScroll(3.0f / 16.0f, -5.0f / 16.0f);
    //stateParticle->setTextureScroll(5.0f / 16.0f, -5.0f / 16.0f);
    //stateParticle->setTextureScroll(7.0f / 16.0f, -5.0f / 16.0f);

}

bool PSControllableCar::checkRearCollision(bool isBoth)
{ 
    bool res = false;
    if(isBoth)
    {
        res = mWheelCollisionBackL && mWheelCollisionBackR;
    }
    else
    {
        res = mWheelCollisionBackL || mWheelCollisionBackR;
    }

    const unsigned long timePeriodToRestoreRoll = 100; // ms

    if(res)
    {
        mTimerCollisionHappenRear.reset();
    }

    if(!res && mTimerCollisionHappenRear.getMilliseconds() < timePeriodToRestoreRoll)
    {
        res = true;
    }

    return res;
}

bool PSControllableCar::checkFrontCollision()
{
    bool res = mWheelCollisionFrontL || mWheelCollisionFrontR;

    const unsigned long timePeriodToRestoreRoll = 100; // ms

    if(res)
    {
        mTimerCollisionHappenFront.reset();
    }

    if(!res && mTimerCollisionHappenFront.getMilliseconds() < timePeriodToRestoreRoll)
    {
        res = true;
    }


    return res;
}

void PSControllableCar::processSounds(const Ogre::FrameEvent &evt)
{
    Ogre::Real projectedVel = getAlignedVelocity();

    Ogre::Vector3 pos = mModelNode->getPosition();

    mCarEngine.process(projectedVel, mAccelEnabled, mBrakeEnabled, checkRearCollision() || checkFrontCollision(), evt);
    Ogre::Real engineRPM = mCarEngine.getEngineRPM();

#ifndef NO_OPENAL
    if(!mIsAI)
    {
        mEngLow->setPosition(pos.x, pos.y, pos.z);
        mEngMid->setPosition(pos.x, pos.y, pos.z);
        mEngHigh->setPosition(pos.x, pos.y, pos.z);

        mEngLow->setPitch(mPitchValueLow.getVal(engineRPM) * 0.5f);
        mEngLow->setGain(mGainValueLow.getVal(engineRPM));

        mEngMid->setPitch(mPitchValueMid.getVal(engineRPM) * 0.5f);
        mEngMid->setGain(mGainValueMid.getVal(engineRPM));

        mEngHigh->setPitch(mPitchValueHigh.getVal(engineRPM));
        mEngHigh->setGain(mGainValueHigh.getVal(engineRPM));
    }
    else
    {
        mEngHigh->setPosition(pos.x, pos.y, pos.z);
        mEngHigh->setPitch(mPitchValueHigh.getVal(engineRPM));
    }



    if(!mIsAI && mEngLow->getSourceState() == AL_INITIAL)
    {
        mEngLow->setPitch(defaultLowPitch);
        mEngLow->startPlaying();
    }

    if(!mIsAI && mEngMid->getSourceState() == AL_INITIAL)
    {
        mEngMid->setPitch(defaultMidPitch);
        mEngMid->startPlaying();
    }

    if(mEngHigh->getSourceState() == AL_INITIAL)
    {
        mEngHigh->setPitch(defaultHighPitch);
        mEngHigh->startPlaying();
    }
#endif
}

void PSControllableCar::restoreRollOver()
{
    if(mCarChassis)
    {
        Ogre::Quaternion rot = mCarChassis->getSceneNode()->_getDerivedOrientation();

        Ogre::Vector3 base = rot * Ogre::Vector3::UNIT_X;

        Ogre::Real impulse = 50.0f;
        if(base.y < 0.0f)
            impulse = -impulse;

        if(checkCollisionReadyToRestoreRollOverSide())
        {
            mCarChassis->applyImpulse(rot * Ogre::Vector3(impulse, 0.0f, 0.0f), rot * Ogre::Vector3(0.0f, 10.0f, 0.0f));
        }
        else if(checkCollisionReadyToRestoreRollOverFront())
        {
            mCarChassis->applyImpulse(rot * Ogre::Vector3(0.0f, 0.0f, 50.0f), rot * Ogre::Vector3(10.0f, 0.0f, 0.0f));
        }
    }
}

bool PSControllableCar::checkCollisionReadyToRestoreRollOverSide()
{
    bool res =  mWheelCollisionBackL && !mWheelCollisionBackR   ||
                !mWheelCollisionBackL && mWheelCollisionBackR;

    return res;
}

bool PSControllableCar::checkCollisionReadyToRestoreRollOverFront()
{
    return mWheelCollisionFrontL && mWheelCollisionFrontR || mChassisCollision;
}

bool PSControllableCar::isRollOver()
{
    bool res = false;

    const unsigned long timePeriodToRestoreRoll = 1000; // ms

    Ogre::Vector3 linearVelocity = mCarChassis->getLinearVelocity();
    Ogre::Real linearVelLength = linearVelocity.length();

    Ogre::Quaternion rot = mModelNode->getOrientation();
    Ogre::Real angleNormDot = (rot * Ogre::Vector3::UNIT_Y).y;

    //adjust car if roll over
    if(angleNormDot < 0.4f)
    {
        const Ogre::Real minVelForCorrection = 120.0f;
        if(linearVelLength < minVelForCorrection)
        {
            if(mTimerRestoreRollOver.getMilliseconds() > timePeriodToRestoreRoll)
            {
                res = true;
            }
        }
        else
        {
            mTimerRestoreRollOver.reset();
        }
    }
    else
    {
        mTimerRestoreRollOver.reset();
    }


    return res;
}