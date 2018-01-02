#include "CheatBurn.h"

#include "../mesh/StaticMeshProcesser.h"

#include "../physics/PhysicsVehicle.h"

#include "../tools/OgreTools.h"

Ogre::NameGenerator CheatBurn::nameGenNodes("Cheats/CheatBurn");
Ogre::NameGenerator CheatBurn::nameGenParticleMaterials("Cheats/CheatBurn/Particle/Material");


CheatBurn::CheatBurn(StaticMeshProcesser * meshProesser, Ogre::SceneManager* sceneManager, bool isFog)
    : mMeshProesser(meshProesser),
    mSceneMgr(sceneManager),
    mIsBurnInProgress(false),
    mPlayerVehicle(NULL),
    mSphereNode(NULL),
    mIsFog(isFog)
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
        Ogre::Vector3 rayDirection = mBurnVelocity * 1000.0f;
        mBurnVelocity *= 10.0f;
        mBurnPosition = carPos + mBurnVelocity;

        Ogre::Vector3 collisionPoint;
        short partIndex;
        short triangleIndex;
        bool isCollided = mMeshProesser->performPointCollisionDetection(Ogre::Ray(mBurnPosition, rayDirection), collisionPoint, partIndex, triangleIndex);
        if(isCollided)
            mFlyPathLength = Ogre::Vector3(collisionPoint - mBurnPosition).length();
        else
            mFlyPathLength = 1000.0f;


        mParticle = mSceneMgr->createParticleSystem(mNodeName, "Particle/Burn");

        Ogre::String materialName = nameGenParticleMaterials.generate();

        Ogre::MaterialPtr newMatParticle;
        if(!mIsFog)
        {
            newMatParticle = CloneMaterial(  materialName, 
                            "Test/ParticleAlpha", 
                            std::vector<Ogre::String>(), 
                            1.0f,
                            TEMP_RESOURCE_GROUP_NAME);
        }
        else
        {
            newMatParticle = CloneMaterial(  materialName, 
                            "Test/ParticleFogAlpha", 
                            std::vector<Ogre::String>(), 
                            1.0f,
                            TEMP_RESOURCE_GROUP_NAME);
        }
        mParticle->setMaterialName(materialName);

        Ogre::TextureUnitState * stateParticle = newMatParticle->getTechnique(0)->getPass(0)->getTextureUnitState(0);
        stateParticle->setTextureScale(8.0f, 8.0f);
        stateParticle->setTextureScroll(5.0f / 16.0f + 0.01f, -5.0f / 16.0f - 0.02f);

        mSphereNode = mSceneMgr->getRootSceneNode()->createChildSceneNode(mNodeName);
        mSphereNode->attachObject(mParticle);
        mSphereNode->setPosition(Ogre::Vector3(mBurnPosition.x, mBurnPosition.y, -mBurnPosition.z));//original data is left hand


        mParticle->getEmitter(0)->setAngle(Ogre::Degree(40.0f));
        mParticle->getEmitter(0)->setColour(Ogre::ColourValue(255.0f/255.0f, 210.0f/255.0f, 76.0f/255.0f, 0.5f));
        mParticle->getEmitter(0)->setEmissionRate(400.0f);
        mParticle->setEmitting(true);
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
                if(mExplosionCounter == 0)
                {
                    mIsBurnInProgress = false;
                    stopBurn();
                }
                if(mExplosionCounter < 15)
                    mParticle->setEmitting(false);
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
                    mParticle->getEmitter(0)->setTimeToLive(2.0f);
                    mParticle->getEmitter(0)->setColour(Ogre::ColourValue(255.0f/255.0f, 210.0f/255.0f, 76.0f/255.0f, 1.0f));
                    mParticle->getEmitter(0)->setEmissionRate(1000.0f);
                    mParticle->getEmitter(0)->setAngle(Ogre::Degree(360.0f));
                    mParticle->getEmitter(0)->setParameter("width", "20");
                    mParticle->getEmitter(0)->setParameter("depth", "20");
                    mParticle->getEmitter(0)->setParticleVelocity(20.0f);
                    mParticle->getAffector(0)->setParameter("alpha", "-4.0");
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
        mSceneMgr->destroyParticleSystem(mNodeName);
        mSphereNode = NULL;
    }
}

CheatBurns::CheatBurns(StaticMeshProcesser * meshProesser, Ogre::SceneManager* sceneManager, bool isFog, size_t burnsMaxAmount)
    : mMeshProesser(meshProesser),
    mSceneMgr(sceneManager)
{
    mBurns.reserve(burnsMaxAmount);
    for(size_t q = 0; q < burnsMaxAmount; ++q)
        mBurns.push_back(CheatBurn(meshProesser, sceneManager, isFog));
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