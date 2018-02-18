
#include <cmath>

#include "PSPlayerCar.h"

#include "../mesh/StaticMeshProcesser.h"

#include "../lua/DMLuaManager.h"

#include "../physics/Physics.h"
#include "../physics/PhysicsVehicle.h"

PSPlayerCar::PSPlayerCar(){}

void PSPlayerCar::initModel(    lua_State * pipeline, 
                                const GameState& gameState,
                                Ogre::SceneManager* sceneMgr, Ogre::SceneNode* mainNode,
                                ModelsPool* modelsPool,
                                Physics * world,
                                const std::string& characterName,
                                InitialVehicleSetup& initialVehicleSetup,
                                bool isPossesCamera)
{

    PSControllableCar::initModel(pipeline, gameState, sceneMgr, mainNode, modelsPool, world, characterName, 
        initialVehicleSetup,
        isPossesCamera, false);

#if defined(__ANDROID__)
        mWorld->getVehicle(this)->setThrottle(1.0f);
#endif

    DMLuaManager luaManager;

    for(int q = 0; q < 5; ++q)
    {
        mModelEntity[q]->setCastShadows(gameState.isCastShadows());
    }
}

void PSPlayerCar::keyDown(OIS::KeyCode key)
{
    switch(key)
    {
    case OIS::KC_LEFT: 
        mWorld->getVehicle(this)->setSteeringLeft(true);
        break;
    case OIS::KC_RIGHT: 
        mWorld->getVehicle(this)->setSteeringRight(true);
        break;
    case OIS::KC_DOWN: 
        mWorld->getVehicle(this)->setBrakes(1.0f);
        setBrake(true);
#if defined(__ANDROID__)
        mWorld->getVehicle(this)->setThrottle(0.0f);
        setAcceleration(false);
#endif
        break;
    case OIS::KC_UP: 
        setAcceleration(true);
        mWorld->getVehicle(this)->setThrottle(1.0f);
        break;
    case OIS::KC_A: 
        mWorld->getVehicle(this)->gearUp();
        break;
    case OIS::KC_Z: 
        mWorld->getVehicle(this)->gearDown();
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
        mWorld->getVehicle(this)->setSteeringLeft(false);
        break;
    case OIS::KC_RIGHT: 
        mWorld->getVehicle(this)->setSteeringRight(false);
        break;
    case OIS::KC_DOWN: 
        mWorld->getVehicle(this)->setBrakes(0.0f);
        setBrake(false);
#if defined(__ANDROID__)
        setAcceleration(true);
        mWorld->getVehicle(this)->setThrottle(1.0f);
#endif
        break;
    case OIS::KC_UP: 
        mWorld->getVehicle(this)->setThrottle(0.0f);
        setAcceleration(false);
        break;
    default:
        break;

    }
}

void PSPlayerCar::mousePressed(OIS::MouseButtonID id)
{
    if(id == OIS::MB_Left)
    {
        mWorld->getVehicle(this)->setThrottle(1.0f);
        setAcceleration(true);
    }

    if(id == OIS::MB_Right)
    {
        mWorld->getVehicle(this)->setBrakes(1.0f);
        setBrake(true);
    }
}

void PSPlayerCar::mouseReleased(OIS::MouseButtonID id)
{
    if(id == OIS::MB_Left)
    {
        mWorld->getVehicle(this)->setThrottle(0.0f);
        setAcceleration(false);
    }

    if(id == OIS::MB_Right)
    {
        mWorld->getVehicle(this)->setBrakes(0.0f);
        setBrake(false);
    }
}

void PSPlayerCar::mouseMoved(const Ogre::Vector2& pos, Ogre::Real windowWidth)
{
    Ogre::Real steering = windowWidth / 2.0f - pos.x;
    steering /= windowWidth / 2.0f;
    mWorld->getVehicle(this)->setSteering(steering);
}

void PSPlayerCar::raceStarted()
{
    mPhysicsVehicle->setRaceStarted();
    mPhysicsVehicle->gearUp();
}

bool PSPlayerCar::isCollideChassis(const PSBaseCar& otherCar)const
{
    bool res = false;

#if 0
    if(     colObj0Wrap->getCollisionObject() == mCarChassis->getBulletRigidBody()          &&
            colObj1Wrap->getCollisionObject() == otherCar.getChassis().getBulletRigidBody() ||
            colObj1Wrap->getCollisionObject() == mCarChassis->getBulletRigidBody()          &&
            colObj0Wrap->getCollisionObject() == otherCar.getChassis().getBulletRigidBody()
        )
    {
        res = true;
    }
#endif
    return res;
}
