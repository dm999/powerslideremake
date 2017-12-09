
#include "PSControllableCar.h"

#include "../mesh/StaticMeshProcesser.h"
#include "../physics/Physics.h"
#include "../physics/PhysicsVehicle.h"

#include "../GameState.h"

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

    const float carMaxSpeed = 300.0f;

    const float psCarMass = 45.0f;
    const float psInvCarMass = 1.0f / psCarMass;
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
    mAIImpulseHelper(Ogre::Vector2::ZERO),
    mWheelRotationalAngleF(0.0f),
    mWheelRotationalAngleB(0.0f)
{
    mDriveImpulse.addPoint(-100.0f, 0.0f);
    mDriveImpulse.addPoint(0.0f, 25.0f);
    mDriveImpulse.addPoint(60.0f, 20.0f);
    mDriveImpulse.addPoint(120.0f, 18.0f);
    mDriveImpulse.addPoint(200.0f, 6.0f);
    mDriveImpulse.addPoint(240.0f, 3.0f);
    mDriveImpulse.addPoint(carMaxSpeed, 0.0f);

    mResistanceImpulse.addPoint(160.0f, 0.0f);
    mResistanceImpulse.addPoint(carMaxSpeed, 20.0f);

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
                                    Physics * world,
                                    const std::string& characterName,
                                    InitialVehicleSetup& initialVehicleSetup,
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
    mWheelRotationalAngleF = 0.0f;
    mWheelRotationalAngleB = 0.0f;

    mParticles = gameState.getParticles();

    PSBaseCar::initModel(pipeline, gameState, sceneMgr, mainNode, modelsPool, world, characterName, 
        initialVehicleSetup,
        isAI);

#if 0
    //debug impulse
    {
        mManual =  sceneMgr->createManualObject(); 

        mManual->begin("BaseWhiteNoLighting", Ogre::RenderOperation::OT_LINE_LIST); 

        mManual->position(Ogre::Vector3::ZERO);
        mManual->position(Ogre::Vector3::ZERO);

        mManual->end(); 
        mManual->setCastShadows(false);
        mModelNode->attachObject(mManual);
    }
#endif

    {
        //steering params
        //todo

        //sound params
        std::vector<std::string> freqLow = getCarArrayValueParameter("", "frequency 0");
        std::vector<std::string> freqMid = getCarArrayValueParameter("", "frequency 1");
        std::vector<std::string> freqHigh = getCarArrayValueParameter("", "frequency 2");
        std::vector<std::string> volLow = getCarArrayValueParameter("", "volume 0");
        std::vector<std::string> volMid = getCarArrayValueParameter("", "volume 1");
        std::vector<std::string> volHigh = getCarArrayValueParameter("", "volume 2");
        assert(freqLow.size() == 11);
        assert(freqMid.size() == 11);
        assert(freqHigh.size() == 11);
        assert(volLow.size() == 11);
        assert(volMid.size() == 11);
        assert(volHigh.size() == 11);

        float hscale0 = getCarParameter("", "frequency 0 hscale", true);
        float hscale1 = getCarParameter("", "frequency 1 hscale", true);
        float hscale2 = getCarParameter("", "frequency 2 hscale", true);

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

void PSControllableCar::processInternalTick(float timeStep, bool isRaceStarted)
{
#if 0
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
//#if defined(__ANDROID__)
#if 1
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

            if(mCarEngine.getCurrentGear() > 0)//forward
            {
                mCarChassis->applyImpulse(rot * Ogre::Vector3(0.0f, 0.0f, -wheelsPushImpulse * carSlopeCoefficient * backLRollImpulse), rot * Ogre::Vector3(-6, -2, 4));
                mCarChassis->applyImpulse(rot * Ogre::Vector3(0.0f, 0.0f, -wheelsPushImpulse * carSlopeCoefficient * backRRollImpulse), rot * Ogre::Vector3(6, -2, 4));
            }
            if(mCarEngine.getCurrentGear() == -1)//R
            {
                mCarChassis->applyImpulse(rot * Ogre::Vector3(0.0f, 0.0f, wheelsPushImpulse * carAntiSlopeCoefficient), rot * Ogre::Vector3(6, -2, 4));
                mCarChassis->applyImpulse(rot * Ogre::Vector3(0.0f, 0.0f, wheelsPushImpulse * carAntiSlopeCoefficient), rot * Ogre::Vector3(-6, -2, 4));
            }


            if(!mIsAI)
                mCarChassis->applyImpulse(rot * Ogre::Vector3(lateralVel * mLateralStabilizationCoeff, 0.0f, 0.0f), Ogre::Vector3::ZERO);
            else
            {
                mCarChassis->applyImpulse(rot * Ogre::Vector3(lateralVel * mLateralStabilizationCoeff, 0.0f, 0.0f), Ogre::Vector3::ZERO);

                //if(mAIImpulseHelper == Ogre::Vector2::ZERO)
                    //mCarChassis->applyImpulse(rot * Ogre::Vector3(lateralVel * mLateralStabilizationCoeff, 0.0f, 0.0f), Ogre::Vector3::ZERO);
                //else
                {
                    //mCarChassis->applyImpulse(rot * Ogre::Vector3(mAIImpulseHelper.x, 0.0f, 0.0f), Ogre::Vector3::ZERO);
                    //mCarChassis->applyImpulse(Ogre::Vector3(mAIImpulseHelper.x, 0.0f, mAIImpulseHelper.y) * 20.0f, Ogre::Vector3::ZERO);
                }
#if 0
                //debug impulse
                {
                    mManual->beginUpdate(0);

                    mManual->position(Ogre::Vector3::ZERO);
                    mManual->position(Ogre::Vector3(mAIImpulseHelper.x, 0.0f, mAIImpulseHelper.y) * 200.0f);
                    //mManual->position(rot.Inverse() * Ogre::Vector3(mAIImpulseHelper.x, 0.0f, mAIImpulseHelper.y) * 15.0f);

                    mManual->end(); 
                }
#endif
            }
        }
    }

    //terrain back friction
    if(checkRearCollision() && isRaceStarted)
    {
            mCarChassis->applyImpulse(rot * Ogre::Vector3(0.0f, 0.0f, wheelsResistanceImpulse * mBackLRollResistance / reduceRollResistance), rot * Ogre::Vector3(-6, -2, 4));
            mCarChassis->applyImpulse(rot * Ogre::Vector3(0.0f, 0.0f, wheelsResistanceImpulse * mBackRRollResistance / reduceRollResistance), rot * Ogre::Vector3(6, -2, 4));
    }

    //airSpoiler impulse
    if(!checkRearCollision() && !checkFrontCollision())
        mCarChassis->applyImpulse(Ogre::Vector3(0.0f, -cockpitAirSpoilerImpulse, 0.0f), rot * Ogre::Vector3(0.0f, -1.0f, -0.5f));
    else
        mCarChassis->applyImpulse(Ogre::Vector3(0.0f, -cockpitGroundSpoilerImpulse, 0.0f), Ogre::Vector3::ZERO);

#endif
}

void PSControllableCar::processFrameBeforePhysics(const Ogre::FrameEvent &evt, const StaticMeshProcesser& processer, bool isRaceStarted)
{
#if 0
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


    //wheels rotation by engine addition
    if( (mAccelEnabled || mBrakeEnabled) && isRaceStarted && mCarEngine.getCurrentGear() != 0)
    {
        Ogre::Real rotAngleAddition = mWheelsRotationByEngineAddition.getVal(projectedVel);
        if(mBrakeEnabled && rotAngleAddition > 0.0f) rotAngleAddition *= -1.0f;

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

    mWheelRotationalAngleF += projectedVel * psInvCarMass * 1.27f / mInitialVehicleSetup.mWheelRadius.x;
    mWheelRotationalAngleB += projectedVel * psInvCarMass * 1.27f / mInitialVehicleSetup.mWheelRadius.y;

    if(mWheelRotationalAngleF <= -Ogre::Math::TWO_PI)
    {
        mWheelRotationalAngleF += Ogre::Math::TWO_PI;
    }
    else
    {
        mWheelRotationalAngleF -= Ogre::Math::TWO_PI;
    }

    if(mWheelRotationalAngleB <= -Ogre::Math::TWO_PI)
    {
        mWheelRotationalAngleB += Ogre::Math::TWO_PI;
    }
    else
    {
        mWheelRotationalAngleB -= Ogre::Math::TWO_PI;
    }

    Ogre::Quaternion rotDriveF;
    rotDriveF.FromAngleAxis(Ogre::Radian(-mWheelRotationalAngleF), Ogre::Vector3(1.0f, 0.0f, 0.0f));

    Ogre::Quaternion rotDriveB;
    rotDriveB.FromAngleAxis(Ogre::Radian(-mWheelRotationalAngleB), Ogre::Vector3(1.0f, 0.0f, 0.0f));

    Ogre::Quaternion turnWheel;
    turnWheel.FromAngleAxis(Ogre::Degree(mSteering), Ogre::Vector3(0.0f, 1.0f, 0.0f));

    if(isRaceStarted)
    {
        mCarWheelFrontL->getBulletObject()->getWorldTransform().setRotation(OgreBulletCollisions::convert(rot * turnWheel * rotDriveF));
        mCarWheelFrontR->getBulletObject()->getWorldTransform().setRotation(OgreBulletCollisions::convert(rot * turnWheel * rotDriveF));
    }
    else
    {
        mWheelNodes[2]->setOrientation(rot * turnWheel);
        mWheelNodes[3]->setOrientation(rot * turnWheel);
    }

    mCarWheelBackL->getBulletObject()->getWorldTransform().setRotation(OgreBulletCollisions::convert(rot * rotDriveB));
    mCarWheelBackR->getBulletObject()->getWorldTransform().setRotation(OgreBulletCollisions::convert(rot * rotDriveB));


    cleanWheelsCollisionsFlags();

#endif
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


void PSControllableCar::adjustWheelsFriction(const StaticMeshProcesser& processer)
{
#if 0
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

#endif
}

void PSControllableCar::adjustWheelsParticles(const Ogre::Quaternion& rot, Ogre::Real rotAngleAddition)
{
    Ogre::Vector3 wheelBackLPos = mWheelNodes[1]->getPosition();
    Ogre::Vector3 wheelBackRPos = mWheelNodes[0]->getPosition();
    Ogre::Vector3 particlesShift = rot * (Ogre::Vector3::NEGATIVE_UNIT_Z * mInitialVehicleSetup.mWheelRadius[0]);
    Ogre::Vector3 particlesDir = rot * Ogre::Vector3(0.0f, 1.0f, 0.1f);
    wheelBackLPos.y -= mInitialVehicleSetup.mWheelRadius[0] / 1.5f;
    wheelBackRPos.y -= mInitialVehicleSetup.mWheelRadius[0] / 1.5f;
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

void PSControllableCar::processSounds()
{

    Ogre::Vector3 pos = mModelNode->getPosition();

    Ogre::Real engineRPM = mWorld->getVehicle(this)->getCarEngine().getEngineRPM();

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
#if 0
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
#endif
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

#if 0
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
#endif

    return res;
}