#include "pcheader.h"

#include "PSPlayerCar.h"

#include "StaticMeshProcesser.h"

#include "CustomRigidBodyWheel.h"

#include "lua/DMLuaManager.h"

PSPlayerCar::PSPlayerCar() :
    mSideImpulse(0.0f),
    mIsTimerFirstTime(true),
    mIsLastTurnLeft(false),
    mSmoothSideImpulse(0.0f)
{
    mSteeringImpulse.addPoint(0.0f, 0.0f);
    mSteeringImpulse.addPoint(10.0f, 40.0f);
    mSteeringImpulse.addPoint(60.0f, 80.0f);
    mSteeringImpulse.addPoint(120.0f, 100.0f);

    mOverSteeringImpulseLength.addPoint(0.0f, 10.0f);
    mOverSteeringImpulseLength.addPoint(20.0f, 1.0f);
    mOverSteeringImpulseLength.addPoint(100.0f, 0.25f);

    //mParticlesEmmisionRate.addPoint(-50.0f, 20.0f);
    //mParticlesEmmisionRate.addPoint(-10.0f, 100.0f);
    //mParticlesEmmisionRate.addPoint(-1.0f, 0.0f);
    //mParticlesEmmisionRate.addPoint(1.0f, 0.0f);
    //mParticlesEmmisionRate.addPoint(10.0f, 100.0f);
    //mParticlesEmmisionRate.addPoint(100.0f, 10.0f);
    //mParticlesEmmisionRate.addPoint(200.0f, 5.0f);

    //mJumpImpulse.addPoint(10.0f, 0.0f);
    //mJumpImpulse.addPoint(15.0f, 300.0f);
    //mJumpImpulse.addPoint(100.0f, 400.0f);
}

void PSPlayerCar::initModel(    lua_State * pipeline, 
                                const GameState& gameState,
                                Ogre::SceneManager* sceneMgr, Ogre::SceneNode* mainNode,
                                CameraMan * cameraMan,
                                ModelsPool* modelsPool,
                                OgreBulletDynamics::DynamicsWorld * world,
                                const std::string& characterName,
                                const Ogre::Matrix4& transform,
                                bool isPossesCamera)
{
    mSideImpulse = 0.0f;
    mIsTimerFirstTime = true;
    mIsLastTurnLeft = false;
    mSmoothSideImpulse = 0.0f;

    PSControllableCar::initModel(pipeline, gameState, sceneMgr, mainNode, cameraMan, modelsPool, world, characterName, transform, isPossesCamera, false);


    DMLuaManager luaManager;

    for(int q = 0; q < 5; ++q)
    {
        mModelEntity[q]->setCastShadows(luaManager.ReadScalarBool("Model.IsCastShadows", pipeline));
    }
}

void PSPlayerCar::calculateSteering(const Ogre::FrameEvent &evt, bool isRaceStarted)
{

    Ogre::Real spf = evt.timeSinceLastFrame * 100.0f;

    Ogre::Vector3 pos = mCarChassis->getSceneNode()->_getDerivedPosition();
    Ogre::Quaternion rot = mCarChassis->getSceneNode()->_getDerivedOrientation();

    Ogre::Real projectedVel = getAlignedVelocity();



    Ogre::Real wheelsTurnImpulse = 0.0f;
    if(projectedVel < 0.0f)
    {
        wheelsTurnImpulse = -50.0f;
    }
    else
    {
        wheelsTurnImpulse = mSteeringImpulse.getVal(projectedVel);
    }
    //wheelsTurnImpulse = Ogre::Math::Clamp(wheelsTurnImpulse * 0.1f * spf, -50.0f, 60.0f);
    wheelsTurnImpulse *= (0.1f * spf);

    Ogre::Real wheelsSkidImpulse = 300.0f * spf;


    if (mSteeringLeft)
    {
        if(checkFrontCollision() && isRaceStarted)
        {
            if(!mIsLastTurnLeft)
            {
                mSmoothSideImpulse = wheelsTurnImpulse;
            }
            wheelsTurnImpulse -= mSmoothSideImpulse;
            mSmoothSideImpulse -= mSmoothSideImpulse * evt.timeSinceLastFrame * 4.0f;
            if(mSmoothSideImpulse < 0.01f)mSmoothSideImpulse = 0.0f;

            mCarWheelFrontL->applyImpulse(rot * Ogre::Vector3(-wheelsTurnImpulse, 0.0f, 0.0f), rot * Ogre::Vector3(0.0f, 0.0f, -1.0f));
            mCarWheelFrontR->applyImpulse(rot * Ogre::Vector3(-wheelsTurnImpulse, 0.0f, 0.0f), rot * Ogre::Vector3(0.0f, 0.0f, -1.0f));
            mSideImpulse = wheelsTurnImpulse;
            mIsLastTurnLeft = true;

            if(mIsTimerFirstTime)
            {
                mSteerTimer.reset();
                mIsTimerFirstTime = false;
            }
            else
            {
                mLastSteerLength = mSteerTimer.getMilliseconds();
            }

            //skid
            if(mAccelEnabled && projectedVel > 20.0f && projectedVel < 70.0f)
            {
                mCarWheelBackL->applyImpulse(rot * Ogre::Vector3(wheelsSkidImpulse / projectedVel, 0.0f, 0.0f), rot * Ogre::Vector3(0.0f, 0.0f, -1.0f));
                mCarWheelBackR->applyImpulse(rot * Ogre::Vector3(wheelsSkidImpulse / projectedVel, 0.0f, 0.0f), rot * Ogre::Vector3(0.0f, 0.0f, -1.0f));
            }
        }

        mSteering += mSteeringIncrement * spf;
        if (mSteering > mSteeringMax)
            mSteering = mSteeringMax;
    }
    else if (mSteeringRight)
    {
        if(checkFrontCollision() && isRaceStarted)
        {
            if(mIsLastTurnLeft)
            {
                mSmoothSideImpulse = wheelsTurnImpulse;
            }
            wheelsTurnImpulse -= mSmoothSideImpulse;
            mSmoothSideImpulse -= mSmoothSideImpulse * evt.timeSinceLastFrame * 4.0f;
            if(mSmoothSideImpulse < 0.01f) mSmoothSideImpulse = 0.0f;

            mCarWheelFrontL->applyImpulse(rot * Ogre::Vector3(wheelsTurnImpulse, 0.0f, 0.0f), rot * Ogre::Vector3(0.0f, 0.0f, -1.0f));
            mCarWheelFrontR->applyImpulse(rot * Ogre::Vector3(wheelsTurnImpulse, 0.0f, 0.0f), rot * Ogre::Vector3(0.0f, 0.0f, -1.0f));
            mSideImpulse = wheelsTurnImpulse;
            mIsLastTurnLeft = false;

            if(mIsTimerFirstTime)
            {
                mSteerTimer.reset();
                mIsTimerFirstTime = false;
            }
            else
            {
                mLastSteerLength = mSteerTimer.getMilliseconds();
            }

            //skid
            if(mAccelEnabled && projectedVel > 20.0f && projectedVel < 70.0f)
            {
                mCarWheelBackL->applyImpulse(rot * Ogre::Vector3(-wheelsSkidImpulse / projectedVel, 0.0f, 0.0f), rot * Ogre::Vector3(0.0f, 0.0f, -1.0f));
                mCarWheelBackR->applyImpulse(rot * Ogre::Vector3(-wheelsSkidImpulse / projectedVel, 0.0f, 0.0f), rot * Ogre::Vector3(0.0f, 0.0f, -1.0f));
            }
        }

        mSteering -= mSteeringIncrement * spf;
        if (mSteering < mSteeringMin)
            mSteering = mSteeringMin;
    }
    else
    {
        mIsTimerFirstTime = true;

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

        //oversteer
        mLastSteerLength = Ogre::Math::Clamp(mLastSteerLength, (unsigned long)1, (unsigned long)4000);
        mSideImpulse -= mSideImpulse * evt.timeSinceLastFrame * mOverSteeringImpulseLength.getVal(projectedVel) / (mLastSteerLength / 1000.0f);
        if(mSideImpulse > 0.01f)
        {
            if(checkFrontCollision() && isRaceStarted)
            {
                if(mIsLastTurnLeft)
                {
                    mCarWheelFrontL->applyImpulse(rot * Ogre::Vector3(-mSideImpulse, 0.0f, 0.0f), rot * Ogre::Vector3(0.0f, 0.0f, -1.0f));
                    mCarWheelFrontR->applyImpulse(rot * Ogre::Vector3(-mSideImpulse, 0.0f, 0.0f), rot * Ogre::Vector3(0.0f, 0.0f, -1.0f));
                }
                else
                {
                    mCarWheelFrontL->applyImpulse(rot * Ogre::Vector3(mSideImpulse, 0.0f, 0.0f), rot * Ogre::Vector3(0.0f, 0.0f, -1.0f));
                    mCarWheelFrontR->applyImpulse(rot * Ogre::Vector3(mSideImpulse, 0.0f, 0.0f), rot * Ogre::Vector3(0.0f, 0.0f, -1.0f));
                }
            }
        }
        else
        {
            mSideImpulse = 0.0f;
        }
    }
}

void PSPlayerCar::keyDown(OIS::KeyCode key)
{
    switch(key)
    {
    case OIS::KC_LEFT: 
        setSteerLeft(true);
        break;
    case OIS::KC_RIGHT: 
        setSteerRight(true);
        break;
    case OIS::KC_DOWN: 
        setBrake(true);
#if defined(__ANDROID__)
        setAcceleration(false);
#endif
        break;
    case OIS::KC_UP: 
        setAcceleration(true);
        break;
    default:
        break;
    }
}

void PSPlayerCar::keyUp(OIS::KeyCode key)
{
    switch(key)
    {
    case OIS::KC_LEFT: 
        setSteerLeft(false);
        break;
    case OIS::KC_RIGHT: 
        setSteerRight(false);
        break;
    case OIS::KC_DOWN: 
        setBrake(false);
#if defined(__ANDROID__)
        setAcceleration(true);
#endif
        break;
    case OIS::KC_UP: 
        setAcceleration(false);
        break;
    default:
        break;

    }
}

bool PSPlayerCar::isCollideChassis(const PSBaseCar& otherCar, const btCollisionObjectWrapper* colObj0Wrap, const btCollisionObjectWrapper* colObj1Wrap)const
{
    bool res = false;

    if(     colObj0Wrap->getCollisionObject() == mCarChassis->getBulletRigidBody()          &&
            colObj1Wrap->getCollisionObject() == otherCar.getChassis().getBulletRigidBody() ||
            colObj1Wrap->getCollisionObject() == mCarChassis->getBulletRigidBody()          &&
            colObj0Wrap->getCollisionObject() == otherCar.getChassis().getBulletRigidBody()
        )
    {
        res = true;
    }

    return res;
}