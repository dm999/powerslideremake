#include "CheatBomb.h"

#include "../mesh/StaticMeshProcesser.h"

#include "../physics/PhysicsVehicle.h"

Ogre::NameGenerator CheatBomb::nameGenNodes("Cheats/CheatBomb");

CheatBomb::CheatBomb(StaticMeshProcesser * meshProesser, Ogre::SceneManager* sceneManager)
    : mMeshProesser(meshProesser),
    mSceneMgr(sceneManager),
    mIsBombInProgress(false),
    mPlayerVehicle(NULL),
    mSphereNode(NULL)
{
    mNodeName = nameGenNodes.generate();
}

CheatBomb::~CheatBomb(){}

void CheatBomb::createBombByPlayer(PhysicsVehicle * vehicle)
{
    if(!mIsBombInProgress)
    {
        mPlayerVehicle = vehicle;
        mIsBombInProgress = true;
        mIsBombJumpsInProgress = true;
        mIsBombExplosionInProgress = false;
        mBlowCounter = 0;
        mExplosionCounter = 0;

        const InitialVehicleSetup& vehicleSetup = mPlayerVehicle->getVehicleSetup();

        Ogre::Matrix3 carRot;
        vehicleSetup.mCarRot.ToRotationMatrix(carRot);
        Ogre::Vector3 carRotZ;//original data is left hand
        carRotZ = Ogre::Vector3(-carRot[0][2], -carRot[1][2], carRot[2][2]);

        Ogre::Vector3 carPos = vehicleSetup.mCarGlobalPos;
        carPos.z = -carPos.z;//original data is left hand


        mBombVelocity = carRotZ;
        mBombVelocity.y += 1.0f;
        mBombVelocity *= 8.0f;
        mBombVelocity += mPlayerVehicle->getLinearImpulse() * vehicleSetup.mChassisInvMass;
        mBombPosition = carPos + mBombVelocity;

        Ogre::Entity * debugSphere = mSceneMgr->createEntity(mNodeName, Ogre::SceneManager::PT_SPHERE);
        debugSphere->setMaterialName("BaseWhiteNoLighting");
        mSphereNode = mSceneMgr->getRootSceneNode()->createChildSceneNode(mNodeName);
        mSphereNode->attachObject(debugSphere);
        mSphereNode->setPosition(Ogre::Vector3(mBombPosition.x, mBombPosition.y, -mBombPosition.z));//original data is left hand
        mSphereNode->setScale(0.05f, 0.05f, 0.05f);
        debugSphere->setCastShadows(false);
    }
}

void CheatBomb::timeStepForVehicle(PhysicsVehicle * vehicle, const vehicles& vehiclesMap)
{
    if(mPlayerVehicle == vehicle)
    {
        if(mIsBombInProgress)
        {
            if(mIsBombExplosionInProgress)
            {
                --mExplosionCounter;
                mSphereNode->setScale(mSphereNode->getScale() + 0.1f);
                if(mExplosionCounter == 0)
                {
                    mIsBombInProgress = false;
                    stopBomb();
                }
            }
            else
            {
                if(mIsBombJumpsInProgress)
                {
                    Ogre::Vector3 collisionPoint;
                    short partIndex;
                    short triangleIndex;
                    bool isCollided = mMeshProesser->performPointCollisionDetection(mBombPosition, mBombVelocity, collisionPoint, partIndex, triangleIndex);

                    if(isCollided)
                    {
                        Ogre::Vector3 pointOnStaticA, pointOnStaticB, pointOnStaticC;
                        mMeshProesser->getGeoverts(partIndex, triangleIndex, pointOnStaticA, pointOnStaticC, pointOnStaticB);

                        Ogre::Vector3 normal = Ogre::Vector3(pointOnStaticB - pointOnStaticA).crossProduct(Ogre::Vector3(pointOnStaticC - pointOnStaticA));
                        normal.normalise();

                        Ogre::Real reflectCoeff = -mBombVelocity.x * normal.x - mBombVelocity.y * normal.y - mBombVelocity.z * normal.z;

                        mBombVelocity -= reflectCoeff * normal * -2.0f;
                        mBombVelocity *= 0.95f;

                        mBombPosition = collisionPoint - mBombVelocity.x * -0.1f;

                        if(mBombVelocity.length() < 5.0f)
                        {
                            mBombVelocity = Ogre::Vector3::ZERO;
                            mIsBombJumpsInProgress = false;
                        }
                    }
                }//jumps in progress

                for (vehicles::const_iterator i = vehiclesMap.begin(), j = vehiclesMap.end(); i != j; ++i)
                {
                    if((*i).second.get() != mPlayerVehicle)//don`t blow on yorself bomb
                    {
                        Ogre::Vector3 carPos = (*i).second->getVehicleSetup().mCarGlobalPos;
                        carPos.z = -carPos.z;//original data is left hand

                        Ogre::Vector3 posDiff = carPos - mBombPosition;
                        if (posDiff.length() < (*i).second->getVehicleSetup().mCollisionRadius)
                            mBlowCounter = 210;
                    }
                }



                mBombPosition += mBombVelocity;

                if(mIsBombJumpsInProgress)
                {
                    mBombVelocity.y -= 0.8f;
                    mBombVelocity *= 0.97f;
                }

                ++mBlowCounter;

                if(mBlowCounter > 200)
                {
                    for (vehicles::const_iterator i = vehiclesMap.begin(), j = vehiclesMap.end(); i != j; ++i)
                    {
                        Ogre::Vector3 carPos = (*i).second->getVehicleSetup().mCarGlobalPos;
                        carPos.z = -carPos.z;//original data is left hand

                        Ogre::Vector3 posDiff = carPos - mBombPosition;
                        Ogre::Real diffLen = posDiff.length();
                        if(diffLen < 75.0f)
                        {
                            Ogre::Real diffLenDiff = 75.0f - diffLen;
                            Ogre::Real forceAmount = (*i).second->getVehicleSetup().mCollisionRadius * 0.1f / diffLen;
                            Ogre::Vector3 rotImpulse(forceAmount * posDiff);
                            Ogre::Vector3 linearImpulse(0.0f, diffLenDiff * 0.013333334f * 150.0f, 0.0f);
                            (*i).second->adjustImpulseInc(rotImpulse, linearImpulse);
                        }
                    }

                    mIsBombExplosionInProgress = true;
                    mExplosionCounter = 30;
                }

                mSphereNode->setPosition(Ogre::Vector3(mBombPosition.x, mBombPosition.y, -mBombPosition.z));//original data is left hand
            }
        }
    }
}

void CheatBomb::stopBomb()
{
    if(mSphereNode)
    {
        mSceneMgr->getRootSceneNode()->removeAndDestroyChild(mNodeName);
        mSceneMgr->destroyEntity(mNodeName);
        mSphereNode = NULL;
    }
}

CheatBombs::CheatBombs(StaticMeshProcesser * meshProesser, Ogre::SceneManager* sceneManager, size_t bombMaxAmount)
    : mMeshProesser(meshProesser),
    mSceneMgr(sceneManager)
{
    mBombs.reserve(bombMaxAmount);
    for(size_t q = 0; q < bombMaxAmount; ++q)
        mBombs.push_back(CheatBomb(meshProesser, sceneManager));
}

void CheatBombs::createBombByPlayer(PhysicsVehicle * vehicle)
{
    for(size_t q = 0; q < mBombs.size(); ++q)
    {
        if(!mBombs[q].isInProgress())
        {
            mBombs[q].createBombByPlayer(vehicle);
            break;
        }
    }
}

void CheatBombs::timeStepForVehicle(PhysicsVehicle * vehicle, const vehicles& vehiclesMap)
{
    for(size_t q = 0; q < mBombs.size(); ++q)
        mBombs[q].timeStepForVehicle(vehicle, vehiclesMap);
}