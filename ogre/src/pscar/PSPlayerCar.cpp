
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
        mModelEntity[q]->setCastShadows(luaManager.ReadScalarBool("Model.IsCastShadows", pipeline));
    }
}

void PSPlayerCar::keyDown(OIS::KeyCode key)
{
    switch(key)
    {
    case OIS::KC_LEFT: 
        setSteerLeft(true);
        mWorld->getVehicle(this)->setSteering(-1.0f);
        break;
    case OIS::KC_RIGHT: 
        setSteerRight(true);
        mWorld->getVehicle(this)->setSteering(1.0f);
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
        setSteerLeft(false);
        mWorld->getVehicle(this)->setSteering(0.0f);
        break;
    case OIS::KC_RIGHT: 
        setSteerRight(false);
        mWorld->getVehicle(this)->setSteering(0.0f);
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