
#include <cmath>

#include "PSPlayerCar.h"

#include "../mesh/StaticMeshProcesser.h"

#include "../customs/CustomRigidBody.h"
#include "../customs/CustomRigidBodyWheel.h"

#include "../lua/DMLuaManager.h"

PSPlayerCar::PSPlayerCar() :
    mSteeringAngleVelocity(0.0f)
{
    mRotationIntensity.addPoint(-50.0f, 1.5f);
    mRotationIntensity.addPoint(0.0f, 0.0f);
    mRotationIntensity.addPoint(20.0f, 1.0f);
}

void PSPlayerCar::initModel(    lua_State * pipeline, 
                                const GameState& gameState,
                                Ogre::SceneManager* sceneMgr, Ogre::SceneNode* mainNode,
                                ModelsPool* modelsPool,
                                OgreBulletDynamics::DynamicsWorld * world,
                                const std::string& characterName,
                                const Ogre::Matrix4& transform,
                                bool isPossesCamera)
{

    mSteeringAngleVelocity = 0.0f;

    PSControllableCar::initModel(pipeline, gameState, sceneMgr, mainNode, modelsPool, world, characterName, 
        transform, Ogre::Vector3::ZERO,
        isPossesCamera, false);


    DMLuaManager luaManager;

    for(int q = 0; q < 5; ++q)
    {
        mModelEntity[q]->setCastShadows(luaManager.ReadScalarBool("Model.IsCastShadows", pipeline));
    }
}

void PSPlayerCar::processInternalTick(float timeStep, bool isRaceStarted)
{
    PSControllableCar::processInternalTick(timeStep, isRaceStarted);

    Ogre::Real spfFake = 1.5f;

    Ogre::Vector3 pos = mCarChassis->getSceneNode()->_getDerivedPosition();
    Ogre::Quaternion rot = mCarChassis->getSceneNode()->_getDerivedOrientation();

    Ogre::Real projectedVel = getAlignedVelocity();

    Ogre::Real rotationIntensity = 1.0f;

    if(projectedVel < 0.0f/* && mBrakeEnabled*/)
    {
        if (mSteeringLeft && checkFrontCollision())
        {
            mSteeringAngleVelocity = -mRotationIntensity.getVal(projectedVel);
        }

        if (mSteeringRight && checkFrontCollision())
        {
            mSteeringAngleVelocity = mRotationIntensity.getVal(projectedVel);
        }

        if(!mSteeringLeft && !mSteeringRight) mSteeringAngleVelocity = 0.0f;
    }
    else
    {
        processSteering(isRaceStarted);
        rotationIntensity = mRotationIntensity.getVal(projectedVel);
    }

    if(checkFrontCollision() && isRaceStarted)
    {
        //mCarChassis->setAngularVelocity(rot * Ogre::Vector3::UNIT_Y * mSteeringAngleVelocity * rotationIntensity * spfFake);
        mCarChassis->applyImpulse(rot * Ogre::Vector3(-mSteeringAngleVelocity * rotationIntensity * spfFake * 10.0f, 0.0f, 0.0f), rot * Ogre::Vector3(0.0f, 0.0f, -15.0f));
    }
}

bool PSPlayerCar::checkOverSteer()
{
    bool ret = false;

    if((!mSteeringLeft || !mSteeringRight) && !mAccelEnabled)
    {
        mTimerOversteer.reset();
    }
    else
    {
        const unsigned long timeThreshold = 1000; //ms
        unsigned long millisecondsPassed = mTimerOversteer.getMilliseconds();

        if(mAccelEnabled && (mSteeringLeft || mSteeringRight) && millisecondsPassed > timeThreshold)
            ret = true;
    }

    return ret;
}

void PSPlayerCar::processSteering(bool isRaceStarted)
{
    if(isRaceStarted)
    {
        const float steeringAccelMax = 1.4f;
        const float steeringAccelOversteerMax = 2.4f;
        const float steeringAccelStep = 0.03f;
        const float steeringAccelOversteerStep = steeringAccelStep / 2.0f;
        const float steeringAccelStepFade = steeringAccelStep / 2.0f;
        const float steeringAccelMinThreshols = steeringAccelStepFade * 2.0f;

        bool isOversteer = checkOverSteer();

        if (mSteeringLeft && checkFrontCollision())
        {
            if(mSteeringAngleVelocity < steeringAccelMax)
            {
                mSteeringAngleVelocity += steeringAccelStep;
                if(isOversteer) mSteeringAngleVelocity += steeringAccelOversteerStep;
            }
            else
            {
                if(isOversteer)
                {
                    if(mSteeringAngleVelocity < steeringAccelOversteerMax)
                        mSteeringAngleVelocity += steeringAccelOversteerStep;
                    if(mSteeringAngleVelocity >= steeringAccelOversteerMax)
                        mSteeringAngleVelocity = steeringAccelOversteerMax;
                }
                else
                    mSteeringAngleVelocity = steeringAccelMax;
            }
        }

        if (mSteeringRight && checkFrontCollision())
        {
            if(mSteeringAngleVelocity > -steeringAccelMax)
            {
                mSteeringAngleVelocity -= steeringAccelStep;
                if(isOversteer) mSteeringAngleVelocity -= steeringAccelOversteerStep;
            }
            else
            {
                if(isOversteer)
                {
                    if(mSteeringAngleVelocity > -steeringAccelOversteerMax)
                        mSteeringAngleVelocity -= steeringAccelOversteerStep;
                    if(mSteeringAngleVelocity <= -steeringAccelOversteerMax)
                        mSteeringAngleVelocity = -steeringAccelOversteerMax;
                }
                else
                    mSteeringAngleVelocity = -steeringAccelMax;
            }
        }

        if (!mSteeringRight && !mSteeringLeft)
        {
            if(mSteeringAngleVelocity < -steeringAccelMinThreshols)
            {
                mSteeringAngleVelocity += steeringAccelStepFade;
            }
            else if(mSteeringAngleVelocity > steeringAccelMinThreshols)
            {
                mSteeringAngleVelocity -= steeringAccelStepFade;
            }
            else mSteeringAngleVelocity = 0.0f;
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
    case OIS::KC_A: 
        mCarEngine.gearUp();
        break;
    case OIS::KC_Z: 
        mCarEngine.gearDown();
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