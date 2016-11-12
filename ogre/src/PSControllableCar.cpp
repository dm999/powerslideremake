#include "pcheader.h"

#include "PSControllableCar.h"

#include "StaticMeshProcesser.h"

#include "CustomRigidBodyWheel.h"
#include "CameraMan.h"
#include "Conversions.h"

#include "lua/DMLuaManager.h"

#include "OpenAL/OpenALSource.h"

#include "OgreTools.h"

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
    mDriveImpulse.addPoint(120.0f, 15.0f);
    mDriveImpulse.addPoint(160.0f, 10.0f);
    mDriveImpulse.addPoint(180.0f, 5.0f);
    mDriveImpulse.addPoint(200.0f, 0.0f);

    mWheelsRotationByEngineAddition.addPoint(-50.0f, 20.0f);
    mWheelsRotationByEngineAddition.addPoint(0.0f, 200.0f);
    mWheelsRotationByEngineAddition.addPoint(50.0f, 20.0f);
    mWheelsRotationByEngineAddition.addPoint(100.0f, 0.0f);

    mPitchValue.addPoint(1500.0f, defaultHighPitch);
    mPitchValue.addPoint(10000.0f, 1.0f);
}

void PSControllableCar::initModel(  lua_State * pipeline, 
                                    const GameState& gameState,
                                    Ogre::SceneManager* sceneMgr, Ogre::SceneNode* mainNode,
                                    CameraMan * cameraMan,
                                    ModelsPool* modelsPool,
                                    OgreBulletDynamics::DynamicsWorld * world,
                                    GameCars gameCar,
                                    const Ogre::Matrix4& transform,
                                    bool isPossesCamera,
                                    bool isAI)
{
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

    mCameraMan = cameraMan;

    PSBaseCar::initModel(pipeline, gameState, sceneMgr, mainNode, cameraMan, modelsPool, world, gameCar, transform, isAI);

    {
        //d.polubotko(TODO): refactor (remove absolute path)
        STRSettings carSettings;
        carSettings.parse(gameState.getPFLoaderStore(), "data/cars/feral max/data/default", "params.str");

        STRSettings trackSettings;
        trackSettings.parse(gameState.getPFLoaderStore(), "data/cars/global/data/" + gameState.getTrackName(), "params.str");

        STRSettings defaultSettings;
        defaultSettings.parse(gameState.getPFLoaderStore(), "data/cars/global/data/default", "params.str");

        float gearRevRatio = getCarParameter(carSettings, trackSettings, defaultSettings, "", "gear rev ratio");
        Ogre::Vector4 revRatio = getCarArray4Parameter(carSettings, trackSettings, defaultSettings, "", "rev ratio");
        Ogre::Vector4 changeDown = getCarArray4Parameter(carSettings, trackSettings, defaultSettings, "", "change down");
        Ogre::Vector4 changeUp = getCarArray4Parameter(carSettings, trackSettings, defaultSettings, "", "change up");

        mCarEngine.init(gearRevRatio, revRatio, changeDown, changeUp);
    }

    DMLuaManager luaManager;
    mSteeringMin = luaManager.ReadScalarFloat("Model.Physics.Wheels.Front.MinSteer", pipeline);
    mSteeringMinRad = Ogre::Degree(mSteeringMin).valueRadians();
    mSteeringMax = luaManager.ReadScalarFloat("Model.Physics.Wheels.Front.MaxSteer", pipeline);
    mSteeringMaxRad = Ogre::Degree(mSteeringMax).valueRadians();

    mIsDisableMouse = luaManager.ReadScalarBool("Scene.IsDisableMouse", pipeline);

    mIsPossesCamera = isPossesCamera;
    if(mIsPossesCamera)
    {
        //Ogre::Vector3 cameraOffset(0.0f, 0.0f, 0.0f);
        //mCameraMan->setTarget(mModelNode);//, cameraOffset);
        mCameraMan->setYawPitchDist(mModelNode->getOrientation(), mModelNode->getPosition(), getLateralVelocity(), mModelNode);
    }

    mWheelBackLParticle = sceneMgr->createParticleSystem(nameGenNodes.generate(), "Particle/Wheel");
    mWheelBackRParticle = sceneMgr->createParticleSystem(nameGenNodes.generate(), "Particle/Wheel");

    mParticleMaterialName = nameGenMaterialsParticles.generate();
    CloneMaterial(  mParticleMaterialName, 
                    "Test/Particle", 
                    std::vector<Ogre::String>(), 
                    1.0f,
                    TEMP_RESOURCE_GROUP_NAME);

    mWheelBackLParticle->setMaterialName(mParticleMaterialName);
    mWheelBackRParticle->setMaterialName(mParticleMaterialName);

    mParticleNodeWheelBackL = sceneMgr->getRootSceneNode()->createChildSceneNode(nameGenNodes.generate());
    mParticleNodeWheelBackL->attachObject(mWheelBackLParticle);

    mParticleNodeWheelBackR = sceneMgr->getRootSceneNode()->createChildSceneNode(nameGenNodes.generate());
    mParticleNodeWheelBackR->attachObject(mWheelBackRParticle);

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

                    originTransformed.y += mWheelRadiusFront;
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

                    originTransformed.y += mWheelRadiusFront;
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

                    originTransformed.y += mWheelRadiusBack;
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

                    originTransformed.y += mWheelRadiusBack;
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

void PSControllableCar::processFrameBeforePhysics(const Ogre::FrameEvent &evt, StaticMeshProcesser& processer, bool isRaceStarted)
{
    processSounds(evt);

    adjustWheelsFriction(processer);

    Ogre::Quaternion rot = mCarChassis->getSceneNode()->_getDerivedOrientation();

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


    calculateSteering(evt, isRaceStarted);


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


    Ogre::Real spf = evt.timeSinceLastFrame * 100.0f;
    Ogre::Real wheelsPushImpulse = mDriveImpulse.getVal(projectedVel) * spf;

    Ogre::Vector3 carForwardVector = getForwardAxis();
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

    if (mAccelEnabled && isRaceStarted)
    {
        if(checkRearCollision(true) /*|| checkFrontCollision()*/)
        {
            mCarChassis->applyImpulse(rot * Ogre::Vector3(0.0f, 0.0f, -wheelsPushImpulse * carSlopeCoefficient), rot * Ogre::Vector3(6, -2, 4));
            mCarChassis->applyImpulse(rot * Ogre::Vector3(0.0f, 0.0f, -wheelsPushImpulse * carSlopeCoefficient), rot * Ogre::Vector3(-6, -2, 4));

            mCarChassis->applyImpulse(rot * Ogre::Vector3(lateralVel * mLateralStabilizationCoeff, 0.0f, 0.0f), rot * Ogre::Vector3(0.0f, 0.0f, 0.0f));
        }
    }

    cleanWheelsCollisionsFlags();
}

void PSControllableCar::processFrameAfterPhysics(const Ogre::FrameEvent &evt, bool isRaceStarted)
{
    if(mCameraMan && mIsPossesCamera)
    {
        if(mCameraMan->getCameraPositionType() == CameraPosition_Bumper)
        {
            mModelNode->setVisible(false);
            mWheelEntitys[0]->setVisible(false);
            mWheelEntitys[1]->setVisible(false);
            mWheelEntitys[2]->setVisible(false);
            mWheelEntitys[3]->setVisible(false);
            mParticleNodeWheelBackL->setVisible(false);
            mParticleNodeWheelBackR->setVisible(false);
        }
        else
        {
            mModelNode->setVisible(true);
            mWheelEntitys[0]->setVisible(true);
            mWheelEntitys[1]->setVisible(true);
            mWheelEntitys[2]->setVisible(true);
            mWheelEntitys[3]->setVisible(true);
            mParticleNodeWheelBackL->setVisible(true);
            mParticleNodeWheelBackR->setVisible(true);
        }

        if(mIsDisableMouse){
            mCameraMan->setYawPitchDist(mModelNode->getOrientation(), mModelNode->getPosition(), getLateralVelocity(), mModelNode);
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
    anisotropicFriction.x = latitudeFriction;
    anisotropicFriction.y = longtitudeFriction;
    anisotropicFriction.z = longtitudeFriction;
    mCarWheelBackL->getBulletRigidBody()->setAnisotropicFriction(OgreBulletCollisions::convert(anisotropicFriction));

    latitudeFriction = processer.getLatitudeFriction(getBackRWheelColliderIndex());
    longtitudeFriction = processer.getLongtitudeFriction(getBackRWheelColliderIndex());
    anisotropicFriction.x = latitudeFriction;
    anisotropicFriction.y = longtitudeFriction;
    anisotropicFriction.z = longtitudeFriction;
    mCarWheelBackR->getBulletRigidBody()->setAnisotropicFriction(OgreBulletCollisions::convert(anisotropicFriction));
}

void PSControllableCar::adjustWheelsParticles(const Ogre::Quaternion& rot, Ogre::Real rotAngleAddition)
{
    Ogre::Vector3 wheelBackLPos = mWheelNodes[1]->getPosition();
    Ogre::Vector3 wheelBackRPos = mWheelNodes[0]->getPosition();
    Ogre::Vector3 particlesShift = rot * (Ogre::Vector3::NEGATIVE_UNIT_Z * mWheelRadiusBack);
    Ogre::Vector3 particlesDir = rot * Ogre::Vector3(0.0f, 1.0f, 0.1f);
    wheelBackLPos.y -= mWheelRadiusBack / 1.5f;
    wheelBackRPos.y -= mWheelRadiusBack / 1.5f;
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

    if(!mIsAI)
    {
        mEngLow->setPosition(pos.x, pos.y, pos.z);
        mEngMid->setPosition(pos.x, pos.y, pos.z);
        mEngHigh->setPosition(pos.x, pos.y, pos.z);

        if(engineRPM > 1500.0f)
        {
            mEngHigh->setPitch(mPitchValue.getVal(engineRPM));
            mEngHigh->setGain(1.0f);
            
            mEngLow->setGain(0.0f);
            mEngMid->setGain(0.0f);
        }
        else
        {
            mEngHigh->setGain(0.0f);

            mEngLow->setGain(1.0f);
            mEngMid->setGain(1.0f);
        }
    }
    else
    {
        mEngHigh->setPosition(pos.x, pos.y, pos.z);
        mEngHigh->setPitch(mPitchValue.getVal(engineRPM));
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