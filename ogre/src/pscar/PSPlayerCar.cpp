
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

void PSPlayerCar::keyDown(OIS::KeyCode key, const GameState& gameState)
{
    if (gameState.checkKeyCode(key, InputKeyMapping::kmLeft))
    {
        mWorld->getVehicle(this)->setSteeringLeft(true);
    }

    if (gameState.checkKeyCode(key, InputKeyMapping::kmRight))
    {
        mWorld->getVehicle(this)->setSteeringRight(true);
    }

    if (gameState.checkKeyCode(key, InputKeyMapping::kmUp))
    {
        setAcceleration(true);
        mWorld->getVehicle(this)->setThrottle(1.0f);
    }

    if (gameState.checkKeyCode(key, InputKeyMapping::kmDown))
    {
        mWorld->getVehicle(this)->setBrakes(1.0f);
        setBrake(true);
#if defined(__ANDROID__)
        mWorld->getVehicle(this)->setThrottle(0.0f);
        setAcceleration(false);
#endif
    }

    if (gameState.checkKeyCode(key, InputKeyMapping::kmHandBreak))
    {
        mWorld->getVehicle(this)->setHandBrakes(1.0f);
        setBrake(true);
    }

    if (gameState.checkKeyCode(key, InputKeyMapping::kmGearUp))
    {
        mWorld->getVehicle(this)->gearUp();
    }

    if (gameState.checkKeyCode(key, InputKeyMapping::kmGearDown))
    {
        mWorld->getVehicle(this)->gearDown();
    }
}

void PSPlayerCar::keyUp(OIS::KeyCode key, const GameState& gameState)
{
    if (gameState.checkKeyCode(key, InputKeyMapping::kmLeft))
    {
        mWorld->getVehicle(this)->setSteeringLeft(false);
    }

    if (gameState.checkKeyCode(key, InputKeyMapping::kmRight))
    {
        mWorld->getVehicle(this)->setSteeringRight(false);
    }

    if (gameState.checkKeyCode(key, InputKeyMapping::kmUp))
    {
        mWorld->getVehicle(this)->setThrottle(0.0f);
        setAcceleration(false);
    }

    if (gameState.checkKeyCode(key, InputKeyMapping::kmDown))
    {
        mWorld->getVehicle(this)->setBrakes(0.0f);
        setBrake(false);
#if defined(__ANDROID__)
        setAcceleration(true);
        mWorld->getVehicle(this)->setThrottle(1.0f);
#endif
    }

    if (gameState.checkKeyCode(key, InputKeyMapping::kmHandBreak))
    {
        mWorld->getVehicle(this)->setHandBrakes(0.0f);
        setBrake(false);
    }

}

void PSPlayerCar::mousePressed(OIS::MouseButtonID id, const GameState& gameState)
{
    /*
    if(id == OIS::MB_Left)
    {
        mWorld->getVehicle(this)->setThrottle(1.0f);
        setAcceleration(true);
    }

    if(id == OIS::MB_Right)
    {
        mWorld->getVehicle(this)->setBrakes(1.0f);
        setBrake(true);
    }*/

#if !defined(__ANDROID__)
    if (gameState.checkKeyCode(id, InputKeyMapping::kmLeft))
    {
        mWorld->getVehicle(this)->setSteeringLeft(true);
    }

    if (gameState.checkKeyCode(id, InputKeyMapping::kmRight))
    {
        mWorld->getVehicle(this)->setSteeringRight(true);
    }

    if (gameState.checkKeyCode(id, InputKeyMapping::kmUp))
    {
        setAcceleration(true);
        mWorld->getVehicle(this)->setThrottle(1.0f);
    }

    if (gameState.checkKeyCode(id, InputKeyMapping::kmDown))
    {
        mWorld->getVehicle(this)->setBrakes(1.0f);
        setBrake(true);
    }

    if (gameState.checkKeyCode(id, InputKeyMapping::kmHandBreak))
    {
        mWorld->getVehicle(this)->setHandBrakes(1.0f);
        setBrake(true);
    }

    if (gameState.checkKeyCode(id, InputKeyMapping::kmGearUp))
    {
        mWorld->getVehicle(this)->gearUp();
    }

    if (gameState.checkKeyCode(id, InputKeyMapping::kmGearDown))
    {
        mWorld->getVehicle(this)->gearDown();
    }
#endif
}

void PSPlayerCar::mouseReleased(OIS::MouseButtonID id, const GameState& gameState)
{
    /*
    if(id == OIS::MB_Left)
    {
        mWorld->getVehicle(this)->setThrottle(0.0f);
        setAcceleration(false);
    }

    if(id == OIS::MB_Right)
    {
        mWorld->getVehicle(this)->setBrakes(0.0f);
        setBrake(false);
    }*/

#if !defined(__ANDROID__)
    if (gameState.checkKeyCode(id, InputKeyMapping::kmLeft))
    {
        mWorld->getVehicle(this)->setSteeringLeft(false);
    }

    if (gameState.checkKeyCode(id, InputKeyMapping::kmRight))
    {
        mWorld->getVehicle(this)->setSteeringRight(false);
    }

    if (gameState.checkKeyCode(id, InputKeyMapping::kmUp))
    {
        mWorld->getVehicle(this)->setThrottle(0.0f);
        setAcceleration(false);
    }

    if (gameState.checkKeyCode(id, InputKeyMapping::kmDown))
    {
        mWorld->getVehicle(this)->setBrakes(0.0f);
        setBrake(false);
    }

    if (gameState.checkKeyCode(id, InputKeyMapping::kmHandBreak))
    {
        mWorld->getVehicle(this)->setHandBrakes(0.0f);
        setBrake(false);
    }
#endif
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
