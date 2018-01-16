
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

void PSPlayerCar::keyDown(OgreBites::Keycode key)
{
    switch(key)
    {
    case SDLK_LEFT:
        setSteerLeft(true);
        mWorld->getVehicle(this)->setSteeringLeft(true);
        break;
    case SDLK_RIGHT:
        setSteerRight(true);
        mWorld->getVehicle(this)->setSteeringRight(true);
        break;
    case SDLK_DOWN:
        mWorld->getVehicle(this)->setBrakes(1.0f);
        setBrake(true);
#if defined(__ANDROID__)
        mWorld->getVehicle(this)->setThrottle(0.0f);
        setAcceleration(false);
#endif
        break;
    case SDLK_UP:
        setAcceleration(true);
        mWorld->getVehicle(this)->setThrottle(1.0f);
        break;
    case SDLK_a:
        mWorld->getVehicle(this)->gearUp();
        break;
    case SDLK_z:
        mWorld->getVehicle(this)->gearDown();
        break;
    default:
        break;
    }
}

void PSPlayerCar::keyUp(OgreBites::Keycode key)
{
    switch(key)
    {
    case SDLK_LEFT:
        setSteerLeft(false);
        mWorld->getVehicle(this)->setSteeringLeft(false);
        break;
    case SDLK_RIGHT:
        setSteerRight(false);
        mWorld->getVehicle(this)->setSteeringRight(false);
        break;
    case SDLK_DOWN:
        mWorld->getVehicle(this)->setBrakes(0.0f);
        setBrake(false);
#if defined(__ANDROID__)
        setAcceleration(true);
        mWorld->getVehicle(this)->setThrottle(1.0f);
#endif
        break;
    case SDLK_UP:
        mWorld->getVehicle(this)->setThrottle(0.0f);
        setAcceleration(false);
        break;
    default:
        break;

    }
}

void PSPlayerCar::mousePressed(Uint8 id)
{
    if(id == SDL_BUTTON_LEFT)
    {
        mWorld->getVehicle(this)->setThrottle(1.0f);
        setAcceleration(true);
    }

    if(id == SDL_BUTTON_RIGHT)
    {
        mWorld->getVehicle(this)->setBrakes(1.0f);
        setBrake(true);
    }
}

void PSPlayerCar::mouseReleased(Uint8 id)
{
    if(id == SDL_BUTTON_LEFT)
    {
        mWorld->getVehicle(this)->setThrottle(0.0f);
        setAcceleration(false);
    }

    if(id == SDL_BUTTON_RIGHT)
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

Ogre::Real PSPlayerCar::getSteering() const
{
    return mPhysicsVehicle->getOriginalSteering();
}