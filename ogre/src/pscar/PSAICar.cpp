
#include "PSAICar.h"

#include "../customs/CustomRigidBodyWheel.h"
#include "../tools/Conversions.h"
#include "../mesh/StaticMeshProcesser.h"

PSAICar::PSAICar() :
    mSteeringImpulse(0.0f)
{
}

void PSAICar::initModel(    lua_State * pipeline, 
                            const GameState& gameState,
                            Ogre::SceneManager* sceneMgr, Ogre::SceneNode* mainNode,
                            ModelsPool* modelsPool,
                            OgreBulletDynamics::DynamicsWorld * world,
                            const std::string& characterName,
                            const Ogre::Matrix4& transform,
                            const Ogre::Vector3& initialForcesLinear,
                            bool isPossesCamera)
{
    mSteeringImpulse = 0.0f;

    PSControllableCar::initModel(pipeline, gameState, sceneMgr, mainNode, modelsPool, world, characterName, 
        transform, initialForcesLinear,
        isPossesCamera);
}

void PSAICar::processInternalTick(float timeStep, bool isRaceStarted)
{
    PSControllableCar::processInternalTick(timeStep, isRaceStarted);

    Ogre::Real spfFake = 1.5f;

    Ogre::Quaternion rot = mCarChassis->getSceneNode()->_getDerivedOrientation();

    if (mSteeringLeft)
    {
        if(checkFrontCollision())
        {
            mCarWheelFrontL->applyImpulse(rot * Ogre::Vector3(-mSteeringImpulse * spfFake, 0.0f, 0.0f), rot * Ogre::Vector3(0.0f, 0.0f, -1.0f));
            mCarWheelFrontR->applyImpulse(rot * Ogre::Vector3(-mSteeringImpulse * spfFake, 0.0f, 0.0f), rot * Ogre::Vector3(0.0f, 0.0f, -1.0f));
        }
    }
    else if (mSteeringRight)
    {
        if(checkFrontCollision())
        {
            mCarWheelFrontL->applyImpulse(rot * Ogre::Vector3(mSteeringImpulse * spfFake, 0.0f, 0.0f), rot * Ogre::Vector3(0.0f, 0.0f, -1.0f));
            mCarWheelFrontR->applyImpulse(rot * Ogre::Vector3(mSteeringImpulse * spfFake, 0.0f, 0.0f), rot * Ogre::Vector3(0.0f, 0.0f, -1.0f));
        }
    }
}

void PSAICar::processWheelsCollision(   btManifoldPoint& cp, 
                                            const btCollisionObjectWrapper* colObj0Wrap, 
                                            const btCollisionObjectWrapper* colObj1Wrap,
                                            StaticMeshProcesser& processer,
                                            int triIndex)
{

    Ogre::Vector3 linearVel = getLinearVelocity();
    Ogre::Real speed = linearVel.length();
    linearVel.normalise();

    std::pair<int, int> address;

    if(processer.isRigidBodyStatic(colObj1Wrap->getCollisionObject(), address))
    {

        if(colObj0Wrap->getCollisionObject() == mCarWheelFrontL->getBulletRigidBody())
        {
            setWheelCollisionHappenFrontL();

            std::string terrainMap = processer.getBatchByAddress(address).mTerrainMap;
            terrainMap = terrainMap.substr(0, terrainMap.length() - 4);
            unsigned char terrainType = processer.getTerrainType(address, triIndex, cp.getPositionWorldOnB());
            mWheelFrontLColliderString = terrainMap + " " + Conversions::DMToString((size_t)terrainType);
            mWheelFrontLColliderIndex = terrainType;
        }

        if(colObj0Wrap->getCollisionObject() == mCarWheelFrontR->getBulletRigidBody())
        {
            setWheelCollisionHappenFrontR();

            std::string terrainMap = processer.getBatchByAddress(address).mTerrainMap;
            terrainMap = terrainMap.substr(0, terrainMap.length() - 4);
            unsigned char terrainType = processer.getTerrainType(address, triIndex, cp.getPositionWorldOnB());
            mWheelFrontRColliderString = terrainMap + " " + Conversions::DMToString((size_t)terrainType);
            mWheelFrontRColliderIndex = terrainType;
        }

        if(colObj0Wrap->getCollisionObject() == mCarWheelBackL->getBulletRigidBody())
        {
            setWheelCollisionHappenBackL();

            std::string terrainMap = processer.getBatchByAddress(address).mTerrainMap;
            terrainMap = terrainMap.substr(0, terrainMap.length() - 4);
            unsigned char terrainType = processer.getTerrainType(address, triIndex, cp.getPositionWorldOnB());
            mWheelBackLColliderString = terrainMap + " " + Conversions::DMToString((size_t)terrainType);
            mWheelBackLColliderIndex = terrainType;
        }

        if(colObj0Wrap->getCollisionObject() == mCarWheelBackR->getBulletRigidBody())
        {
            setWheelCollisionHappenBackR();

            std::string terrainMap = processer.getBatchByAddress(address).mTerrainMap;
            terrainMap = terrainMap.substr(0, terrainMap.length() - 4);
            unsigned char terrainType = processer.getTerrainType(address, triIndex, cp.getPositionWorldOnB());
            mWheelBackRColliderString = terrainMap + " " + Conversions::DMToString((size_t)terrainType);
            mWheelBackRColliderIndex = terrainType;
        }
    }
}

#if 0
void PSAICar::adjustWheelsFriction(StaticMeshProcesser& processer)
{
    Ogre::Vector3 anisotropicFriction;
    Ogre::Real latitudeFriction = 1.0f;
    Ogre::Real longtitudeFriction = 0.0f;
    anisotropicFriction.x = latitudeFriction;
    anisotropicFriction.y = longtitudeFriction;
    anisotropicFriction.z = longtitudeFriction;

    mCarWheelFrontL->getBulletRigidBody()->setAnisotropicFriction(OgreBulletCollisions::convert(anisotropicFriction));
    mCarWheelFrontR->getBulletRigidBody()->setAnisotropicFriction(OgreBulletCollisions::convert(anisotropicFriction));
    mCarWheelBackL->getBulletRigidBody()->setAnisotropicFriction(OgreBulletCollisions::convert(anisotropicFriction));
    mCarWheelBackR->getBulletRigidBody()->setAnisotropicFriction(OgreBulletCollisions::convert(anisotropicFriction));
}
#endif

void PSAICar::performAICorrection(bool isRaceStarted, bool isGamePaused)
{
    mAIUtils.performAICorrection(this, isRaceStarted, isGamePaused);
}

void PSAICar::clearAIData()
{
    mAIUtils.clear();
}

void PSAICar::setAIData(const AIWhole& aiWhole, Ogre::SceneManager* sceneMgr, bool isDebugAI)
{
    mAIUtils.setAIData(aiWhole, sceneMgr, isDebugAI);
}

void PSAICar::raceStarted()
{
    mAIUtils.raceStarted();
}