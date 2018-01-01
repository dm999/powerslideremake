#include "CheatBomb.h"

#include "../mesh/StaticMeshProcesser.h"

#include "../physics/PhysicsVehicle.h"

CheatBomb::CheatBomb(StaticMeshProcesser * meshProesser, Ogre::SceneManager* sceneManager)
    : mMeshProesser(meshProesser),
    mSceneMgr(sceneManager),
    mIsBombInProgress(false),
    mPlayerVehicle(NULL),
    mSphereNode(NULL)
{
}

CheatBomb::~CheatBomb(){}

void CheatBomb::createBombByPlayer(PhysicsVehicle * vehicle)
{
    if(!mIsBombInProgress)
    {
        mPlayerVehicle = vehicle;
        mIsBombInProgress = true;
        mIsBombJumpsInProgress = true;
        mBlowCounter = 0;

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

        Ogre::Entity * debugSphere = mSceneMgr->createEntity("CheatBomb", Ogre::SceneManager::PT_SPHERE);
        debugSphere->setMaterialName("BaseWhiteNoLighting");
        mSphereNode = mSceneMgr->getRootSceneNode()->createChildSceneNode("CheatBomb");
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

            mBombPosition += mBombVelocity;

            if(mIsBombJumpsInProgress)
            {
                mBombVelocity.y -= 0.8f;
                mBombVelocity *= 0.97f;
            }

            ++mBlowCounter;

            if(mBlowCounter > 200)
            {
                mIsBombInProgress = false;
            }

            mSphereNode->setPosition(Ogre::Vector3(mBombPosition.x, mBombPosition.y, -mBombPosition.z));//original data is left hand

            if(!mIsBombInProgress)
                stopBomb();
        }
    }
}

void CheatBomb::stopBomb()
{
    if(mSphereNode)
    {
        mSceneMgr->getRootSceneNode()->removeAndDestroyChild("CheatBomb");
        mSceneMgr->destroyEntity("CheatBomb");
        mSphereNode = NULL;
    }
}