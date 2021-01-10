
#include "PSControllableCar.h"

#include "../mesh/StaticMeshProcesser.h"
#include "../physics/Physics.h"
#include "../physics/PhysicsVehicle.h"

#include "../GameState.h"

#include "../CameraMan.h"
#include "../tools/Conversions.h"

#ifndef NO_OPENAL
    #include "../sound/SoundSource.h"
#endif

#include "../tools/OgreTools.h"

namespace
{
    const float defaultLowPitch = 0.25f;
    const float defaultMidPitch = 0.15f;
    const float defaultHighPitch = 0.5f;

    const float carMaxSpeed = 300.0f;

    const float psCarMass = 45.0f;
    const float psInvCarMass = 1.0f / psCarMass;
}

Ogre::NameGenerator PSControllableCar::nameGenMaterialsParticles("Scene/Material/Particles/Name");

PSControllableCar::PSControllableCar() :
    mIsDisableMouse(true),
    mCameraMan(NULL),
    mBrakeEnabled(false),
    mAccelEnabled(false)
{
}

void PSControllableCar::initModel(  lua_State * pipeline, 
                                    const GameState& gameState,
                                    Ogre::SceneManager* sceneMgr, Ogre::SceneNode* mainNode,
                                    ModelsPool* modelsPool,
                                    Physics * world,
                                    const std::string& characterName,
                                    InitialVehicleSetup& initialVehicleSetup,
                                    bool isPossesCamera,
                                    bool isAI)
{
    mIsDisableMouse = true;
    mBrakeEnabled = false;
    mAccelEnabled = false;
#if defined(__ANDROID__)
    if(!isAI)
        mAccelEnabled = true;
#endif

    mParticles = gameState.getParticles();

    PSBaseCar::initModel(pipeline, gameState, sceneMgr, mainNode, modelsPool, world, characterName, 
        initialVehicleSetup,
        isAI);

#if 0
    //debug impulse
    {
        mManual =  sceneMgr->createManualObject(); 

        mManual->begin("BaseWhiteNoLighting", Ogre::RenderOperation::OT_LINE_LIST); 

        mManual->position(Ogre::Vector3::ZERO);
        mManual->position(Ogre::Vector3::ZERO);

        mManual->end(); 
        mManual->setCastShadows(false);
        mModelNode->attachObject(mManual);
    }
#endif

    {
        //steering params
        //todo

        //sound params
        std::vector<std::string> freqLow = getCarArrayValueParameter("", "frequency 0");
        std::vector<std::string> freqMid = getCarArrayValueParameter("", "frequency 1");
        std::vector<std::string> freqHigh = getCarArrayValueParameter("", "frequency 2");
        std::vector<std::string> volLow = getCarArrayValueParameter("", "volume 0");
        std::vector<std::string> volMid = getCarArrayValueParameter("", "volume 1");
        std::vector<std::string> volHigh = getCarArrayValueParameter("", "volume 2");
        assert(freqLow.size() == 11);
        assert(freqMid.size() == 11);
        assert(freqHigh.size() == 11);
        assert(volLow.size() == 11);
        assert(volMid.size() == 11);
        assert(volHigh.size() == 11);

        float hscale0 = getCarParameter("", "frequency 0 hscale", true);
        float hscale1 = getCarParameter("", "frequency 1 hscale", true);
        float hscale2 = getCarParameter("", "frequency 2 hscale", true);

        mPitchValueLow.clear();
        mPitchValueMid.clear();
        mPitchValueHigh.clear();
        mGainValueLow.clear();
        mGainValueMid.clear();
        mGainValueHigh.clear();

        for(size_t q = 0; q < 11; ++q)
        {
            float val;
            Conversions::DMFromString(freqLow[q], val);
            mPitchValueLow.addPoint(static_cast<float>(q) * 1000.0f, val * hscale0);

            Conversions::DMFromString(freqMid[q], val);
            mPitchValueMid.addPoint(static_cast<float>(q) * 1000.0f, val * hscale1);

            Conversions::DMFromString(freqHigh[q], val);
            mPitchValueHigh.addPoint(static_cast<float>(q) * 1000.0f, val * hscale2);

            Conversions::DMFromString(volLow[q], val);
            mGainValueLow.addPoint(static_cast<float>(q) * 1000.0f, val);

            Conversions::DMFromString(volMid[q], val);
            mGainValueMid.addPoint(static_cast<float>(q) * 1000.0f, val);

            Conversions::DMFromString(volHigh[q], val);
            mGainValueHigh.addPoint(static_cast<float>(q) * 1000.0f, val);
        }
    }

    mIsPossesCamera = isPossesCamera;

    mWheelBackLParticle = sceneMgr->createParticleSystem(nameGenNodes.generate(), "Particle/Wheel");
    mWheelBackRParticle = sceneMgr->createParticleSystem(nameGenNodes.generate(), "Particle/Wheel");
    mDeadParticle = sceneMgr->createParticleSystem(nameGenNodes.generate(), "Particle/Wheel");
    mDeadParticle2 = sceneMgr->createParticleSystem(nameGenNodes.generate(), "Particle/Wheel");

    const Ogre::Vector2& fogStartEnd = gameState.getSTRPowerslide().getFogStartEnd(gameState.getTrackName());
    bool isFogEnabled = fogStartEnd.x >= 1000000.0f ? false : true;

    mParticleMaterialName = nameGenMaterialsParticles.generate();
    mParticleMaterialNameDead = nameGenMaterialsParticles.generate();
    mParticleMaterialNameDead2 = nameGenMaterialsParticles.generate();

    if(!isFogEnabled)
    {
        CloneMaterial(  mParticleMaterialName, 
                        "Test/Particle", 
                        std::vector<Ogre::String>(), 
                        1.0f,
                        TEMP_RESOURCE_GROUP_NAME);

        CloneMaterial(mParticleMaterialNameDead,
            "Test/Particle",
            std::vector<Ogre::String>(),
            1.0f,
            TEMP_RESOURCE_GROUP_NAME);

        CloneMaterial(mParticleMaterialNameDead2,
            "Test/ParticleAlpha",
            std::vector<Ogre::String>(),
            1.0f,
            TEMP_RESOURCE_GROUP_NAME);
    }
    else
    {
        Ogre::MaterialPtr newMatParticle = CloneMaterial(  mParticleMaterialName, 
                        "Test/ParticleFog", 
                        std::vector<Ogre::String>(), 
                        1.0f,
                        TEMP_RESOURCE_GROUP_NAME);

        CloneMaterial(mParticleMaterialNameDead,
            "Test/ParticleFog",
            std::vector<Ogre::String>(),
            1.0f,
            TEMP_RESOURCE_GROUP_NAME);

        CloneMaterial(mParticleMaterialNameDead2,
            "Test/ParticleFogAlpha",
            std::vector<Ogre::String>(),
            1.0f,
            TEMP_RESOURCE_GROUP_NAME);

        const Ogre::ColourValue& skyColor = gameState.getSTRPowerslide().getTrackSkyColor(gameState.getTrackName());

        //newMatParticle->setFog(true, Ogre::FOG_LINEAR, skyColor, 0.0f, fogStartEnd.x, fogStartEnd.y);
    }

    mWheelBackLParticle->setMaterialName(mParticleMaterialName);
    mWheelBackRParticle->setMaterialName(mParticleMaterialName);
    mDeadParticle->setMaterialName(mParticleMaterialNameDead);
    mDeadParticle2->setMaterialName(mParticleMaterialNameDead2);

    {
        mDeadParticle->getEmitter(0)->setColour(Ogre::ColourValue(0.0f, 0.0f, 0.0f));
        mDeadParticle->getEmitter(0)->setDirection(Ogre::Vector3::UNIT_Y);

        Ogre::MaterialPtr particleMaterial = Ogre::MaterialManager::getSingleton().getByName(mParticleMaterialNameDead);
        Ogre::TextureUnitState * stateParticle = particleMaterial->getTechnique(0)->getPass(0)->getTextureUnitState(0);
        stateParticle->setTextureScale(8.0f, 8.0f);

        mDeadParticle->getEmitter(0)->setMaxParticleVelocity(40.0f);
        mDeadParticle->getEmitter(0)->setAngle(Ogre::Degree(20.0f));
    }

    {
        mDeadParticle2->getEmitter(0)->setColour(Ogre::ColourValue(0.5f, 0.3f, 0.0f));
        mDeadParticle2->getEmitter(0)->setDirection(Ogre::Vector3::UNIT_Y);

        Ogre::MaterialPtr particleMaterial = Ogre::MaterialManager::getSingleton().getByName(mParticleMaterialNameDead2);
        Ogre::TextureUnitState * stateParticle = particleMaterial->getTechnique(0)->getPass(0)->getTextureUnitState(0);
        stateParticle->setTextureScale(8.0f, 8.0f);

        mDeadParticle2->getEmitter(0)->setMaxParticleVelocity(40.0f);
        mDeadParticle2->getEmitter(0)->setAngle(Ogre::Degree(20.0f));
    }

    mParticleNodeWheelBackL = sceneMgr->getRootSceneNode()->createChildSceneNode(nameGenNodes.generate());
    mParticleNodeWheelBackL->attachObject(mWheelBackLParticle);

    mParticleNodeWheelBackR = sceneMgr->getRootSceneNode()->createChildSceneNode(nameGenNodes.generate());
    mParticleNodeWheelBackR->attachObject(mWheelBackRParticle);

    mParticleDead = sceneMgr->getRootSceneNode()->createChildSceneNode(nameGenNodes.generate());
    mParticleDead->attachObject(mDeadParticle);

    mParticleDead2 = sceneMgr->getRootSceneNode()->createChildSceneNode(nameGenNodes.generate());
    mParticleDead2->attachObject(mDeadParticle2);
}


void PSControllableCar::processFrameBeforePhysics(const StaticMeshProcesser& processer, bool isRaceStarted)
{
    processSounds();

    Ogre::Vector3 carUpVector = mInitialVehicleSetup.mCarRot * Ogre::Vector3::UNIT_Y;
    Ogre::Real upProjY = Ogre::Vector3::UNIT_Y.dotProduct(carUpVector);

    Ogre::Real speedometerVel = getAlignedVelocitySpeedometer();
    Ogre::Real wheelVel = getWheelBackLVelocity();

    //wheels rotation by engine addition
    if( (mAccelEnabled || mBrakeEnabled) &&
        isRaceStarted && 
        mPhysicsVehicle->getCarEngine().getCurrentGear() != 0 &&
        ( speedometerVel > 10.0f || wheelVel > 10.0f))
    {
        if(!mParticles.empty())
        {
            adjustWheelsParticles(mInitialVehicleSetup.mCarRot);
        }
    }
    else
    {
        mWheelBackLParticle->setEmitting(false);
        mWheelBackRParticle->setEmitting(false);
    }

    if (mPhysicsVehicle->getLife() < 1.0f)
    {
        mParticleDead->setPosition(mModelNode->getPosition());
        mParticleDead2->setPosition(mModelNode->getPosition());
        mDeadParticle->setEmitting(true);
        const Ogre::Real emissionRateMax = 200.0f;
        Ogre::Real life = mPhysicsVehicle->getLife();
        Ogre::Real emissionRate = emissionRateMax * (1.0f - std::max(life, 0.0f));
        if (life < 0.5f)
        {
            mDeadParticle2->setEmitting(true);
            mDeadParticle2->getEmitter(0)->setEmissionRate(emissionRate);

            if (life <= 0.0f)
            {
                mDeadParticle2->getEmitter(0)->setColour(Ogre::ColourValue(0.8f, 0.1f, 0.0f));
            }
        }
        mDeadParticle->getEmitter(0)->setEmissionRate(emissionRate);
    }
    else
    {
        mDeadParticle->setEmitting(false);
        mDeadParticle2->setEmitting(false);
    }

}

void PSControllableCar::processCamera(GameState& gameState)
{
    if(mCameraMan && mIsPossesCamera)
    {
        if(mCameraMan->getCameraPositionType() == CameraPosition_Bumper)
        {
            setVisibility(false);
        }
        else
        {
            setVisibility(true);
        }

        if(mIsDisableMouse)
        {
            Ogre::Camera* cam = mCameraMan->getCamera();
            if(cam)
            {
                cam->setAutoTracking(false);
                mCameraMan->setYawPitchDist(mInitialVehicleSetup,
                    gameState.getGlobalLight(), gameState.getShadowLight());
            }
        }
        else
        {
            Ogre::Camera* cam = mCameraMan->getCamera();
            if(cam)
            {
                cam->setAutoTracking(true, mModelNode);
            }
        }
        
    }
}

void PSControllableCar::adjustWheelsParticles(const Ogre::Quaternion& rot)
{
    Ogre::Vector3 wheelBackLPos = mWheelNodes[1]->getPosition();
    Ogre::Vector3 wheelBackRPos = mWheelNodes[0]->getPosition();
    Ogre::Vector3 particlesShift = rot * (Ogre::Vector3::NEGATIVE_UNIT_Z * mInitialVehicleSetup.mWheelRadius[0]);
    Ogre::Vector3 particlesDirL = rot * Ogre::Vector3(0.0f, 1.0f, 0.1f);
    Ogre::Vector3 particlesDirR = rot * Ogre::Vector3(0.0f, 1.0f, 0.1f);
    wheelBackLPos.y -= mInitialVehicleSetup.mWheelRadius[0] / 1.5f;
    wheelBackRPos.y -= mInitialVehicleSetup.mWheelRadius[0] / 1.5f;
    wheelBackLPos -= particlesShift;
    wheelBackRPos -= particlesShift;
    mParticleNodeWheelBackL->setPosition(wheelBackLPos);
    mParticleNodeWheelBackR->setPosition(wheelBackRPos);

    char terrainIndexL = mPhysicsVehicle->getWheelBackLTerrainIndex();
    char terrainIndexR = mPhysicsVehicle->getWheelBackRTerrainIndex();

    if(terrainIndexL != - 1 && terrainIndexR != - 1)
    {
        Ogre::Real wheelBackLVelocitySign = Ogre::Math::Sign(mPhysicsVehicle->getWheelBackLVelocity());
        Ogre::Real wheelBackRVelocitySign = Ogre::Math::Sign(mPhysicsVehicle->getWheelBackRVelocity());
        particlesDirL.z *= wheelBackLVelocitySign;
        particlesDirR.z *= wheelBackRVelocitySign;

        const Particle& particleL = mParticles[terrainIndexL];
        const Particle& particleR = mParticles[terrainIndexR];
        
        mWheelBackLParticle->setEmitting(   mPhysicsVehicle->getWheelBackLCollision()   && 
                                            (mAccelEnabled || mBrakeEnabled)    &&
                                            particleL.isVisible
                                            );

        mWheelBackRParticle->setEmitting(   mPhysicsVehicle->getWheelBackRCollision()   && 
                                            (mAccelEnabled || mBrakeEnabled)    &&
                                            particleR.isVisible
                                            );

        mWheelBackLParticle->getEmitter(0)->setColour(particleL.rgb1);
        mWheelBackRParticle->getEmitter(0)->setColour(particleR.rgb1);

        mWheelBackLParticle->getEmitter(0)->setDirection(particlesDirL);
        mWheelBackRParticle->getEmitter(0)->setDirection(particlesDirR);
        //mWheelBackLParticle->getEmitter(0)->setParticleVelocity(linearVel);
        //mWheelBackRParticle->getEmitter(0)->setParticleVelocity(linearVel);
        //if(linearVel > 10.0f || linearVel < -10.0f)
        if(!mPhysicsVehicle->isNitro())
        {
            //Ogre::Real emmisionRate = mParticlesEmmisionRate.getVal(rotAngleAddition);
            Ogre::Real emmisionRate = 1.0f;//Ogre::Math::Abs(rotAngleAddition);
            mWheelBackLParticle->getEmitter(0)->setEmissionRate(emmisionRate + 40.0f);
            mWheelBackRParticle->getEmitter(0)->setEmissionRate(emmisionRate + 40.0f);
        }
        else
        {
            mWheelBackLParticle->getEmitter(0)->setEmissionRate(200.0f);
            mWheelBackRParticle->getEmitter(0)->setEmissionRate(200.0f);
        }

        Ogre::MaterialPtr particleMaterial = Ogre::MaterialManager::getSingleton().getByName(mParticleMaterialName);
        Ogre::TextureUnitState * stateParticle = particleMaterial->getTechnique(0)->getPass(0)->getTextureUnitState(0);
        stateParticle->setTextureScale(8.0f, 8.0f);

        //snow? (1st row left to right)[left 4 samples]
        if(particleL.index == 0)
        {
            stateParticle->setTextureScroll(-7.0f / 16.0f, -7.0f / 16.0f);
            //stateParticle->setTextureScroll(-5.0f / 16.0f, -7.0f / 16.0f);
            //stateParticle->setTextureScroll(-3.0f / 16.0f, -7.0f / 16.0f);
            //stateParticle->setTextureScroll(-1.0f / 16.0f, -7.0f / 16.0f);
        }

        //(2nd row left to right)[left 4 samples]
        if(particleL.index == 1)
        {
            stateParticle->setTextureScroll(-7.0f / 16.0f, -5.0f / 16.0f);
            //stateParticle->setTextureScroll(-5.0f / 16.0f, -5.0f / 16.0f);
            //stateParticle->setTextureScroll(-3.0f / 16.0f, -5.0f / 16.0f);
            //stateParticle->setTextureScroll(-1.0f / 16.0f, -5.0f / 16.0f);
        }

        //(3rd row left to right)[left 4 samples]
        if(particleL.index == 2)
        {
            stateParticle->setTextureScroll(-7.0f / 16.0f, -3.0f / 16.0f);
            //stateParticle->setTextureScroll(-5.0f / 16.0f, -3.0f / 16.0f);
            //stateParticle->setTextureScroll(-3.0f / 16.0f, -3.0f / 16.0f);
            //stateParticle->setTextureScroll(-1.0f / 16.0f, -3.0f / 16.0f);
        }

        //mud (4th row left to right)[left 4 samples]
        if(particleL.index == 3)
        {
            stateParticle->setTextureScroll(-7.0f / 16.0f, -1.0f / 16.0f);
            //stateParticle->setTextureScroll(-5.0f / 16.0f, -1.0f / 16.0f);
            //stateParticle->setTextureScroll(-5.0f / 16.0f, -1.0f / 16.0f);
            //stateParticle->setTextureScroll(-3.0f / 16.0f, -1.0f / 16.0f);
        }

        //(2nd row left to right)[right 4 samples]
        //stateParticle->setTextureScroll(1.0f / 16.0f, -5.0f / 16.0f);
        //stateParticle->setTextureScroll(3.0f / 16.0f, -5.0f / 16.0f);
        //stateParticle->setTextureScroll(5.0f / 16.0f, -5.0f / 16.0f);
        //stateParticle->setTextureScroll(7.0f / 16.0f, -5.0f / 16.0f);
    }
    else
    {
        mWheelBackLParticle->setEmitting(false);
        mWheelBackRParticle->setEmitting(false);
    }

}

void PSControllableCar::setParticlesVisibility(bool isVisible)
{
    if (mWheelBackLParticle) mWheelBackLParticle->setVisible(isVisible);
    if (mWheelBackRParticle) mWheelBackRParticle->setVisible(isVisible);
}

void PSControllableCar::processSounds()
{

    Ogre::Vector3 pos = mModelNode->getPosition();

    Ogre::Real engineRPM = mWorld->getVehicle(this)->getCarEngine().getEngineRPM();

#ifndef NO_OPENAL
    if(!mIsAI)
    {
        mEngLow->setPosition(pos.x, pos.y, pos.z);
        mEngMid->setPosition(pos.x, pos.y, pos.z);
        mEngHigh->setPosition(pos.x, pos.y, pos.z);

        mEngLow->setPitch(mPitchValueLow.getVal(engineRPM) * 0.5f);
        mEngLow->setGain(mGainValueLow.getVal(engineRPM) * mMasterGain);

        mEngMid->setPitch(mPitchValueMid.getVal(engineRPM) * 0.5f);
        mEngMid->setGain(mGainValueMid.getVal(engineRPM) * mMasterGain);

        mEngHigh->setPitch(mPitchValueHigh.getVal(engineRPM));
        mEngHigh->setGain(mGainValueHigh.getVal(engineRPM) * mMasterGain);
    }
    else
    {
        mEngHigh->setPosition(pos.x, pos.y, pos.z);
        mEngHigh->setPitch(mPitchValueHigh.getVal(engineRPM));
        mEngHigh->setGain(0.5f * mMasterGain);
    }



    if(!mIsAI && mEngLow->getSourceState() == SoundSource::Stopped)
    {
        mEngLow->setPitch(defaultLowPitch);
        mEngLow->startPlaying();
    }

    if(!mIsAI && mEngMid->getSourceState() == SoundSource::Stopped)
    {
        mEngMid->setPitch(defaultMidPitch);
        mEngMid->startPlaying();
    }

    if(mEngHigh->getSourceState() == SoundSource::Stopped)
    {
        mEngHigh->setPitch(defaultHighPitch);
        mEngHigh->startPlaying();
    }
#endif
}
