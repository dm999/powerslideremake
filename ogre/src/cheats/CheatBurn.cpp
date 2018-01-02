#include "CheatBurn.h"

#include "../mesh/StaticMeshProcesser.h"

#include "../physics/PhysicsVehicle.h"

Ogre::NameGenerator CheatBurn::nameGenNodes("Cheats/CheatBurn");


CheatBurn::CheatBurn(StaticMeshProcesser * meshProesser, Ogre::SceneManager* sceneManager)
    : mMeshProesser(meshProesser),
    mSceneMgr(sceneManager),
    mIsBurnInProgress(false),
    mPlayerVehicle(NULL),
    mSphereNode(NULL)
{
    mNodeName = nameGenNodes.generate();
}

CheatBurn::~CheatBurn(){}

void CheatBurn::createBurnByPlayer(PhysicsVehicle * vehicle)
{
    if(!mIsBurnInProgress)
    {
        mPlayerVehicle = vehicle;
        mIsBurnInProgress = true;
        mIsBurnExplosionInProgress = false;
        mFlyPath = 0.0f;
        mExplosionCounter = 0;

        const InitialVehicleSetup& vehicleSetup = mPlayerVehicle->getVehicleSetup();

        Ogre::Matrix3 carRot;
        vehicleSetup.mCarRot.ToRotationMatrix(carRot);
        Ogre::Vector3 carRotY, carRotZ;//original data is left hand
        carRotY = Ogre::Vector3(carRot[0][1], carRot[1][1], -carRot[2][1]);
        carRotZ = Ogre::Vector3(-carRot[0][2], -carRot[1][2], carRot[2][2]);

        Ogre::Vector3 carPos = vehicleSetup.mCarGlobalPos;
        carPos.z = -carPos.z;//original data is left hand


        mBurnVelocity = carRotY * vehicleSetup.mMissileAngle + carRotZ;
        Ogre::Vector3 ray = mBurnVelocity * 1000.0f;
        mBurnVelocity *= 10.0f;
        mBurnPosition = carPos + mBurnVelocity;

        Ogre::Vector3 collisionPoint;
        short partIndex;
        short triangleIndex;
        bool isCollided = mMeshProesser->performPointCollisionDetection(mBurnPosition, ray, collisionPoint, partIndex, triangleIndex);
        if(isCollided)
            mFlyPathLength = Ogre::Vector3(collisionPoint - mBurnPosition).length();
        else
            mFlyPathLength = 1000.0f;

        Ogre::Entity * debugSphere = mSceneMgr->createEntity(mNodeName, Ogre::SceneManager::PT_SPHERE);
        debugSphere->setMaterialName("BaseWhiteNoLighting");
        mSphereNode = mSceneMgr->getRootSceneNode()->createChildSceneNode(mNodeName);
        mSphereNode->attachObject(debugSphere);
        mSphereNode->setPosition(Ogre::Vector3(mBurnPosition.x, mBurnPosition.y, -mBurnPosition.z));//original data is left hand
        mSphereNode->setScale(0.05f, 0.05f, 0.05f);
        debugSphere->setCastShadows(false);
    }
}

void CheatBurn::timeStepForVehicle(PhysicsVehicle * vehicle, const vehicles& vehiclesMap)
{
    if(mPlayerVehicle == vehicle)
    {
        if(mIsBurnInProgress)
        {
            if(mIsBurnExplosionInProgress)
            {
                --mExplosionCounter;
                mSphereNode->setScale(mSphereNode->getScale() + 0.1f);
                if(mExplosionCounter == 0)
                {
                    mIsBurnInProgress = false;
                    stopBurn();
                }
            }
            else
            {
                mFlyPath += 10.0f;
                mBurnPosition += mBurnVelocity;

                //detect car near by
                for (vehicles::const_iterator i = vehiclesMap.begin(), j = vehiclesMap.end(); i != j; ++i)
                {
                    if((*i).second.get() != mPlayerVehicle)//don`t blow up on yourself bomb
                    {
                        Ogre::Vector3 carPos = (*i).second->getVehicleSetup().mCarGlobalPos;
                        carPos.z = -carPos.z;//original data is left hand

                        Ogre::Vector3 posDiff = carPos - mBurnPosition;
                        if (posDiff.length() < ((*i).second->getVehicleSetup().mCollisionRadius + 10.0f))
                        {
                            mFlyPath = mFlyPathLength + 1.0f;
                            (*i).second->adjustImpulseInc(posDiff * 0.2f, mBurnVelocity * 10.0f);
                            break;
                        }
                    }
                }



                if(mFlyPath > mFlyPathLength)
                {
                    mIsBurnExplosionInProgress = true;
                    mExplosionCounter = 25;
                }

                mSphereNode->setPosition(Ogre::Vector3(mBurnPosition.x, mBurnPosition.y, -mBurnPosition.z));//original data is left hand
            }
        }
    }
}

void CheatBurn::stopBurn()
{
    if(mSphereNode)
    {
        mSceneMgr->getRootSceneNode()->removeAndDestroyChild(mNodeName);
        mSceneMgr->destroyEntity(mNodeName);
        mSphereNode = NULL;
    }
}

CheatBurns::CheatBurns(StaticMeshProcesser * meshProesser, Ogre::SceneManager* sceneManager, size_t burnsMaxAmount)
    : mMeshProesser(meshProesser),
    mSceneMgr(sceneManager)
{
    mBurns.reserve(burnsMaxAmount);
    for(size_t q = 0; q < burnsMaxAmount; ++q)
        mBurns.push_back(CheatBurn(meshProesser, sceneManager));
}

void CheatBurns::createBurnByPlayer(PhysicsVehicle * vehicle)
{
    for(size_t q = 0; q < mBurns.size(); ++q)
    {
        if(!mBurns[q].isInProgress())
        {
            mBurns[q].createBurnByPlayer(vehicle);
            break;
        }
    }
}

void CheatBurns::timeStepForVehicle(PhysicsVehicle * vehicle, const vehicles& vehiclesMap)
{
    for(size_t q = 0; q < mBurns.size(); ++q)
        mBurns[q].timeStepForVehicle(vehicle, vehiclesMap);
}