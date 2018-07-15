#include "CheatBomb.h"

#include "../mesh/StaticMeshProcesser.h"

#include "../physics/PhysicsVehicle.h"

#include "../tools/OgreTools.h"

#ifndef NO_OPENAL
    #include "../sound/SoundsProcesser.h"
#endif

Ogre::NameGenerator CheatBomb::nameGenNodes("Cheats/CheatBomb");
Ogre::NameGenerator CheatBomb::nameGenParticleMaterials("Cheats/CheatBomb/Particle/Material");

CheatBomb::CheatBomb(StaticMeshProcesser * meshProesser, Ogre::SceneManager* sceneManager
#ifndef NO_OPENAL
        , SoundsProcesser * soundProcesser
#endif
    )
    : mMeshProesser(meshProesser),
    mSceneMgr(sceneManager),
#ifndef NO_OPENAL
    mSoundProcesser(soundProcesser),
#endif
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


        mParticle = mSceneMgr->createParticleSystem(mNodeName, "Particle/Bomb");

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
        mSphereNode->setPosition(Ogre::Vector3(mBombPosition.x, mBombPosition.y, -mBombPosition.z));//original data is left hand
        mSphereNode->setScale(0.05f, 0.05f, 0.05f);

        mParticle->getEmitter(0)->setAngle(Ogre::Degree(40.0f));
        mParticle->getEmitter(0)->setColour(Ogre::ColourValue::White);
        mParticle->getEmitter(0)->setEmissionRate(100.0f);
        mParticle->setEmitting(true);
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

                if(mExplosionCounter < 15)
                    mParticle->setEmitting(false);
            }
            else
            {
                if(mIsBombJumpsInProgress)
                {
                    Ogre::Vector3 collisionPoint;
                    short partIndex;
                    short triangleIndex;
                    bool isCollided = mMeshProesser->performPointCollisionDetection(Ogre::Ray(mBombPosition, mBombVelocity), collisionPoint, partIndex, triangleIndex);

                    if(isCollided)
                    {
                        Ogre::Vector3 pointOnStaticA, pointOnStaticB, pointOnStaticC;
                        mMeshProesser->getGeoverts(partIndex, triangleIndex, pointOnStaticA, pointOnStaticC, pointOnStaticB);

                        Ogre::Vector3 normal = Ogre::Vector3(pointOnStaticB - pointOnStaticA).crossProduct(Ogre::Vector3(pointOnStaticC - pointOnStaticA));
                        normal.normalise();

                        Ogre::Real reflectCoeff = -mBombVelocity.dotProduct(normal);

                        mBombVelocity -= reflectCoeff * normal * -2.0f;
                        mBombVelocity *= 0.95f;

                        mBombPosition = collisionPoint - mBombVelocity.x * -0.1f;

                        if(mBombVelocity.length() < 5.0f)
                        {
                            mBombVelocity = Ogre::Vector3::ZERO;
                            mIsBombJumpsInProgress = false;
                            mParticle->getEmitter(0)->setEmissionRate(40.0f);
                            mParticle->getAffector(0)->setParameter("alpha", "-1.0");
                        }
                    }
                }//jumps in progress

                //detect car near by
                for (vehicles::const_iterator i = vehiclesMap.begin(), j = vehiclesMap.end(); i != j; ++i)
                {
                    if((*i).second.get() != mPlayerVehicle)//don`t blow up on yourself bomb
                    {
                        Ogre::Vector3 carPos = (*i).second->getVehicleSetup().mCarGlobalPos;
                        carPos.z = -carPos.z;//original data is left hand

                        Ogre::Vector3 posDiff = carPos - mBombPosition;
                        if (posDiff.length() < (*i).second->getVehicleSetup().mCollisionRadius)
                        {
                            mBlowCounter = 210;
                            break;
                        }
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

                    mParticle->getEmitter(0)->setTimeToLive(2.0f);
                    mParticle->getEmitter(0)->setColour(Ogre::ColourValue(255.0f/255.0f, 210.0f/255.0f, 76.0f/255.0f, 1.0f));
                    mParticle->getEmitter(0)->setEmissionRate(1000.0f);
                    mParticle->getEmitter(0)->setAngle(Ogre::Degree(180.0f));
                    mParticle->getEmitter(0)->setParticleVelocity(60.0f);
                    mParticle->getAffector(0)->setParameter("alpha", "-4.0");

                    mIsBombExplosionInProgress = true;
                    mExplosionCounter = 30;

#ifndef NO_OPENAL
                    mSoundProcesser->playExplosion(Ogre::Vector3(mBombPosition.x, mBombPosition.y, -mBombPosition.z));//original data is left hand
#endif
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
        mSceneMgr->destroyParticleSystem(mNodeName);
        mSphereNode = NULL;
    }
}

CheatBombs::CheatBombs(StaticMeshProcesser * meshProesser, Ogre::SceneManager* sceneManager
#ifndef NO_OPENAL
        , SoundsProcesser * soundProcesser
#endif
    , size_t bombsMaxAmount)
    : mMeshProesser(meshProesser),
    mSceneMgr(sceneManager)
{
    mBombs.reserve(bombsMaxAmount);
    for(size_t q = 0; q < bombsMaxAmount; ++q)
        mBombs.push_back(CheatBomb(meshProesser, sceneManager
#ifndef NO_OPENAL
        , soundProcesser
#endif
        ));
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