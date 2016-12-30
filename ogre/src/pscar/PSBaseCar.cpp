#include "../pcheader.h"

#include "PSBaseCar.h"

#include "../tools/OgreTools.h"

#include "../lua/DMLuaManager.h"

#include "../listeners/VehicleSceneObjectListener.h"
#include "../listeners/PlayerVehicleSceneObjectListener.h"

#include "../mesh/ModelsPool.h"

#include "../OpenAL/OpenALSource.h"

#include "../loaders/TEXLoader.h"
#include "../loaders/SUSLoader.h"
#include "../OriginalSettings.h"

#include "../GameState.h"

namespace{
    Ogre::Vector3 gridShift(0.0f, 1.0f, 0.0f);
}

Ogre::NameGenerator PSBaseCar::nameGenMaterials("Scene/Material/Vehicle/Name");
Ogre::NameGenerator PSBaseCar::nameGenTextures("Scene/Texture/Vehicle/Name");

PSBaseCar::PSBaseCar() :
    mModelNode(NULL),
    mWheelFrontLColliderString(""),
    mWheelFrontRColliderString(""),
    mWheelBackLColliderString(""),
    mWheelBackRColliderString(""),
    mWheelFrontLColliderIndex(0),
    mWheelFrontRColliderIndex(0),
    mWheelBackLColliderIndex(0),
    mWheelBackRColliderIndex(0),
    mChassisColliderIndex(0)
{
}

void PSBaseCar::processFrameAfterPhysics(const Ogre::FrameEvent &evt)
{
    AdjustSuspension(   mModelEntity[0], mWheelNodes, 
                        mModelNode->getPosition(), mModelNode->getOrientation(),
                        mSuspensionIndices, mSuspensionPointOriginalPos,
                        mFrontLOriginalPos, mFrontROriginalPos, mBackLOriginalPos, mBackROriginalPos);
}

void PSBaseCar::initModel(  lua_State * pipeline, 
                            const GameState& gameState,
                            Ogre::SceneManager* sceneMgr, Ogre::SceneNode* mainNode,
                            CameraMan * cameraMan,
                            ModelsPool* modelsPool,
                            OgreBulletDynamics::DynamicsWorld * world,
                            const std::string& characterName,
                            const Ogre::Matrix4& transform,
                            bool isAI)
{

    mCharacterName = characterName;

    mIsAI = isAI;

    mWorld = world;

    DMLuaManager luaManager;

    std::string carName = gameState.getSTRPowerslide().getValue(mCharacterName + " parameters", "car", "feral max");

    std::string de2Path = gameState.getSTRPowerslide().getValue(carName + " parameters", "base directory", "feral max");
    std::set<char> delim;
    delim.insert('\\');
    std::vector<std::string> pathComponents = Tools::splitpath(de2Path, delim, false);
    std::string carPath = pathComponents[pathComponents.size() - 1];

    modelsPool->getCopyOfVehicle(gameState, carName, mModelEntity);

    std::string genTextureName = nameGenTextures.generate();

    //load car texture
    {
        std::string carSkinName = gameState.getSTRPowerslide().getValue(carName + " parameters", "texture name", "feral max texture");
        carSkinName += "_m_1.tex";
        TEXLoader().load(gameState.getPFLoaderData(), "data/cars/" + carPath + "/textures/default/" + mCharacterName, carSkinName, genTextureName);
    }

    bool isAttenuateExcludeBox = luaManager.ReadScalarBool("Model.IsAttenuateExcludeBox", pipeline);

    if(luaManager.ReadScalarBool("Model.Material.IsOverrideSubMaterials", pipeline))
    {
        for(size_t q = 0; q < 5; ++q)
        {
            Ogre::TextureUnitState * state = mModelEntity[q]->getSubEntity(0)->getMaterial()->getTechnique(0)->getPass(0)->getTextureUnitState(0);

            if(state)
            {
                //std::string textureName = state->getTextureName();

                Ogre::String nameSub = nameGenMaterials.generate();

                std::vector<Ogre::String> texturesSubMat(1);
                texturesSubMat[0] = genTextureName;

                Ogre::MaterialPtr newMat;
                if(!isAI)
                {
                    std::string playerMaterial = luaManager.ReadScalarString("Model.Material.SingleSubMaterial", pipeline);

                    if(isAttenuateExcludeBox)
                    {
                        playerMaterial = luaManager.ReadScalarString("Model.Material.SingleSubMaterialExclude", pipeline);
                    }

                    newMat = CloneMaterial(  nameSub, 
                            playerMaterial, 
                            texturesSubMat, 
                            1.0f,
                            TEMP_RESOURCE_GROUP_NAME);
                }
                else
                {
                    newMat = CloneMaterial(  nameSub, 
                            luaManager.ReadScalarString("Model.Material.SingleSubMaterialAI", pipeline), 
                            texturesSubMat, 
                            1.0f,
                            TEMP_RESOURCE_GROUP_NAME);
                }

                newMat->setAmbient( luaManager.ReadScalarFloat("Model.Material.SingleAmbient.r", pipeline),
                                    luaManager.ReadScalarFloat("Model.Material.SingleAmbient.g", pipeline),
                                    luaManager.ReadScalarFloat("Model.Material.SingleAmbient.b", pipeline));
                newMat->setDiffuse( luaManager.ReadScalarFloat("Model.Material.SingleDiffuse.r", pipeline),
                                    luaManager.ReadScalarFloat("Model.Material.SingleDiffuse.g", pipeline),
                                    luaManager.ReadScalarFloat("Model.Material.SingleDiffuse.b", pipeline), 1.0f);
                newMat->setSpecular(luaManager.ReadScalarFloat("Model.Material.SingleSpecular.r", pipeline),
                                    luaManager.ReadScalarFloat("Model.Material.SingleSpecular.g", pipeline),
                                    luaManager.ReadScalarFloat("Model.Material.SingleSpecular.b", pipeline), 1.0f);
                newMat->setShininess(luaManager.ReadScalarFloat("Model.Material.SingleSpecular.power", pipeline));

                newMat->setShininess(100.0f);
                //wheel smoother
                if(q > 0) newMat->setShininess(10.0f);

                Ogre::TextureUnitState * stateNewMat = newMat->getTechnique(0)->getPass(0)->getTextureUnitState(0);
                stateNewMat->setTextureAddressingMode(Ogre::TextureUnitState::TAM_CLAMP);

                mModelEntity[q]->getSubEntity(0)->setMaterialName(nameSub);
            }
        }
    }

    for(size_t q = 0; q < 5; ++q)
    {
        Ogre::SceneNode* modelNode = mainNode->createChildSceneNode();

        modelNode->attachObject(mModelEntity[q]);

        if(luaManager.ReadScalarBool("Model.Mesh.IsBB", pipeline))
        {
            modelNode->showBoundingBox(true);
        }

        if(mIsAI)
            mModelEntity[q]->setListener(new VehicleSceneObjectListener(mModelEntity[q], sceneMgr));
        else
        {
            if(isAttenuateExcludeBox)
                mModelEntity[q]->setListener(new PlayerVehicleSceneObjectListener(mModelEntity[q], sceneMgr, gameState.getExclusions()));
            else
                mModelEntity[q]->setListener(new VehicleSceneObjectListener(mModelEntity[q], sceneMgr));
        }

        mModelEntity[q]->setCastShadows(false);

        if(q > 0)
        {
            mWheelNodes[q - 1] = modelNode;
            mWheelEntitys[q - 1] = mModelEntity[q];
        }
        else
        {
            mModelNode = modelNode;
        }
    }

    initSuspension(gameState);

    //use shadow buffer & HBU_WRITE_ONLY for cockpit & get suspension points
    AdjustBufferToUseShadow(mModelEntity[0], mSuspensionData, mSuspensionIndices, mSuspensionPointOriginalPos);

    Ogre::Quaternion rot = transform.extractQuaternion();
    Ogre::Radian angle;
    Ogre::Vector3 axis;
    rot.ToAngleAxis(angle, axis);
    rot.FromAngleAxis(angle, Ogre::Vector3(-axis.x, -axis.y, axis.z));
    mModelNode->setOrientation(rot);
    mModelNode->setPosition(transform.getTrans() + gridShift);


    STRSettings carSettings;
    carSettings.parse(gameState.getPFLoaderStore(), "data/cars/" + carPath + "/data/default", "params.str");

    STRSettings trackSettings;
    trackSettings.parse(gameState.getPFLoaderStore(), "data/cars/global/data/" + gameState.getSTRPowerslide().getDataSubDir(gameState.getTrackName()), "params.str");

    STRSettings defaultSettings;
    defaultSettings.parse(gameState.getPFLoaderStore(), "data/cars/global/data/default", "params.str");

    //load wheels offsets
    {
        mBackROriginalPos = carSettings.getArray3Value("", "wheelbase back");
        mBackROriginalPos.z = -mBackROriginalPos.z;

        mBackLOriginalPos = mBackROriginalPos;
        mBackLOriginalPos.x = -mBackLOriginalPos.x;

        mWheelNodes[0]->setPosition(mBackROriginalPos);
        mWheelNodes[1]->setPosition(mBackLOriginalPos);


        mFrontROriginalPos = carSettings.getArray3Value("", "wheelbase front");
        mFrontROriginalPos.z = -mFrontROriginalPos.z;

        mFrontLOriginalPos = mFrontROriginalPos;
        mFrontLOriginalPos.x = -mFrontLOriginalPos.x;

        mWheelNodes[2]->setPosition(mFrontROriginalPos);
        mWheelNodes[3]->setPosition(mFrontLOriginalPos);
    }

    if(luaManager.ReadScalarBool("Model.Mesh.IsTangents", pipeline))
    {
        for(size_t q = 0; q < 5; ++q)
            BuildTangents(mModelEntity[q]);
    }


    InitialVehicleSetup initialVehicleSetup;
    initialVehicleSetup.mChassisPos = mModelNode->getPosition();
    initialVehicleSetup.mChassisRot = mModelNode->getOrientation();
    initialVehicleSetup.mConnectionPointRRWheel = mBackROriginalPos;
    initialVehicleSetup.mConnectionPointRLWheel = mBackLOriginalPos;
    initialVehicleSetup.mConnectionPointFRWheel = mFrontROriginalPos;
    initialVehicleSetup.mConnectionPointFLWheel = mFrontLOriginalPos;

    initialVehicleSetup.mRoofBackRadius = carSettings.getFloatValue("", "roof back radius");
    Ogre::Vector3 roofBack = carSettings.getArray3Value("", "roof back");
    roofBack.z = -roofBack.z;
    initialVehicleSetup.mRoofBackPos = roofBack;

    initialVehicleSetup.mRoofFrontRadius = carSettings.getFloatValue("", "roof front radius");
    Ogre::Vector3 roofFront = carSettings.getArray3Value("", "roof front");
    roofFront.z = -roofFront.z;
    initialVehicleSetup.mRoofFrontPos = roofFront;

    initialVehicleSetup.mBodyRadius = carSettings.getFloatValue("", "body radius");
    Ogre::Vector3 bodyBase = carSettings.getArray3Value("", "body base 0");
    bodyBase.z = -bodyBase.z;
    initialVehicleSetup.mBodyBasePos = bodyBase;

    initialVehicleSetup.mWheelRadius = carSettings.getArray2Value("", "wheel radii");

    initialVehicleSetup.mAnisotropicFriction = Ogre::Vector3(
            luaManager.ReadScalarFloat("Model.Physics.Wheels.AnisotropicFriction.x", pipeline),
            luaManager.ReadScalarFloat("Model.Physics.Wheels.AnisotropicFriction.y", pipeline),
            luaManager.ReadScalarFloat("Model.Physics.Wheels.AnisotropicFriction.z", pipeline)
        );

    initialVehicleSetup.mRollingFriction = luaManager.ReadScalarFloat("Model.Physics.Wheels.RollingFriction", pipeline);

    initialVehicleSetup.mMaxTravel = -getCarParameter(carSettings, trackSettings, defaultSettings, "", "max travel");

    initialVehicleSetup.mWheelsFSpringStiffness = luaManager.ReadScalarFloat("Model.Physics.Wheels.Front.SpringStiffness", pipeline);
    initialVehicleSetup.mWheelsFSpringDamping = luaManager.ReadScalarFloat("Model.Physics.Wheels.Front.SpringDamping", pipeline);
    initialVehicleSetup.mWheelsRSpringStiffness = luaManager.ReadScalarFloat("Model.Physics.Wheels.Rear.SpringStiffness", pipeline);
    initialVehicleSetup.mWheelsRSpringDamping = luaManager.ReadScalarFloat("Model.Physics.Wheels.Rear.SpringDamping", pipeline);
    initialVehicleSetup.mLimitSpringParamsF = luaManager.ReadScalarBool("Model.Physics.Wheels.Front.LimitSpringParams", pipeline);
    initialVehicleSetup.mLimitSpringParamsR = luaManager.ReadScalarBool("Model.Physics.Wheels.Rear.LimitSpringParams", pipeline);

    initialVehicleSetup.mAirDensityLinear = getCarParameter(carSettings, trackSettings, defaultSettings, "", "air density translation");
    initialVehicleSetup.mAirDensityAngular = getCarParameter(carSettings, trackSettings, defaultSettings, "", "air density rotation");

    initialVehicleSetup.mChassisMass = getCarParameter(carSettings, trackSettings, defaultSettings, "", "mass");
    initialVehicleSetup.mChassisRestitution = luaManager.ReadScalarFloat("Model.Physics.Chassis.Restitution", pipeline);
    initialVehicleSetup.mChassisFriction = luaManager.ReadScalarFloat("Model.Physics.Chassis.Friction", pipeline);

    initialVehicleSetup.mWheelsFMass = luaManager.ReadScalarFloat("Model.Physics.Wheels.Front.Mass", pipeline);
    initialVehicleSetup.mWheelsRMass = luaManager.ReadScalarFloat("Model.Physics.Wheels.Rear.Mass", pipeline);
    initialVehicleSetup.mWheelsRestitution = luaManager.ReadScalarFloat("Model.Physics.Wheels.Restitution", pipeline);
    initialVehicleSetup.mWheelsFriction = luaManager.ReadScalarFloat("Model.Physics.Wheels.Friction", pipeline);

    initPhysicalModel(world, mModelNode, mWheelNodes, initialVehicleSetup);

}

void PSBaseCar::setModelPositionOnGrid(const Ogre::Matrix4& transform)
{
#if 0
    if(mModelNode)
    {
        
        
        removeFromWorld();

        //mWorld->getBulletCollisionWorld()->removeCollisionObject(mCarChassis->getBulletRigidBody());
        //mWorld->getBulletCollisionWorld()->removeCollisionObject(mCarWheelBackR->getBulletRigidBody());
        //mWorld->getBulletCollisionWorld()->removeCollisionObject(mCarWheelBackL->getBulletRigidBody());
        //mWorld->getBulletCollisionWorld()->removeCollisionObject(mCarWheelFrontR->getBulletRigidBody());
        //mWorld->getBulletCollisionWorld()->removeCollisionObject(mCarWheelFrontL->getBulletRigidBody());

        int originalFlagsChassis = mCarChassis->getBulletRigidBody()->getCollisionFlags();
        int originalFlagsBackR = mCarWheelBackR->getBulletRigidBody()->getCollisionFlags();
        int originalFlagsBackL = mCarWheelBackL->getBulletRigidBody()->getCollisionFlags();
        int originalFlagsFrontR = mCarWheelFrontR->getBulletRigidBody()->getCollisionFlags();
        int originalFlagsFrontL = mCarWheelFrontL->getBulletRigidBody()->getCollisionFlags();
        mCarChassis->getBulletRigidBody()->setCollisionFlags(originalFlagsChassis | btCollisionObject::CF_KINEMATIC_OBJECT | btCollisionObject::CF_NO_CONTACT_RESPONSE);
        mCarWheelBackR->getBulletRigidBody()->setCollisionFlags(originalFlagsBackR | btCollisionObject::CF_KINEMATIC_OBJECT | btCollisionObject::CF_NO_CONTACT_RESPONSE);
        mCarWheelBackL->getBulletRigidBody()->setCollisionFlags(originalFlagsBackL | btCollisionObject::CF_KINEMATIC_OBJECT | btCollisionObject::CF_NO_CONTACT_RESPONSE);
        mCarWheelFrontR->getBulletRigidBody()->setCollisionFlags(originalFlagsFrontR | btCollisionObject::CF_KINEMATIC_OBJECT | btCollisionObject::CF_NO_CONTACT_RESPONSE);
        mCarWheelFrontL->getBulletRigidBody()->setCollisionFlags(originalFlagsFrontL | btCollisionObject::CF_KINEMATIC_OBJECT | btCollisionObject::CF_NO_CONTACT_RESPONSE);

        Ogre::Quaternion rot = transform.extractQuaternion();
        Ogre::Radian angle;
        Ogre::Vector3 axis;
        rot.ToAngleAxis(angle, axis);
        rot.FromAngleAxis(angle, Ogre::Vector3(-axis.x, -axis.y, axis.z));
        Ogre::Vector3 chassisPos(transform.getTrans() + gridShift);
        
        //mModelNode->setPosition(chassisPos);
        //mModelNode->setOrientation(rot);

        //mWheelNodes[0]->setPosition(chassisPos + rot * mBackROriginalPos);
        //mWheelNodes[1]->setPosition(chassisPos + rot * mBackLOriginalPos);
        //mWheelNodes[2]->setPosition(chassisPos + rot * mFrontROriginalPos);
        //mWheelNodes[3]->setPosition(chassisPos + rot * mFrontLOriginalPos);

        //mWheelNodes[0]->setOrientation(rot);
        //mWheelNodes[1]->setOrientation(rot);
        //mWheelNodes[2]->setOrientation(rot);
        //mWheelNodes[3]->setOrientation(rot);

        mCarChassis->getBulletRigidBody()->setMassProps(0.0f, btVector3(0.0f,0.0f,0.0f));
        mCarWheelBackR->getBulletRigidBody()->setMassProps(0.0f, btVector3(0.0f,0.0f,0.0f));
        mCarWheelBackL->getBulletRigidBody()->setMassProps(0.0f, btVector3(0.0f,0.0f,0.0f));
        mCarWheelFrontR->getBulletRigidBody()->setMassProps(0.0f, btVector3(0.0f,0.0f,0.0f));
        mCarWheelFrontL->getBulletRigidBody()->setMassProps(0.0f, btVector3(0.0f,0.0f,0.0f));

        mCarChassis->getBulletRigidBody()->updateInertiaTensor();
        mCarWheelBackR->getBulletRigidBody()->updateInertiaTensor();
        mCarWheelBackL->getBulletRigidBody()->updateInertiaTensor();
        mCarWheelFrontR->getBulletRigidBody()->updateInertiaTensor();
        mCarWheelFrontL->getBulletRigidBody()->updateInertiaTensor();


        mCarChassis->setCenterOfMass(chassisPos, rot);
        mCarWheelBackR->setCenterOfMass(chassisPos + rot * mBackROriginalPos, rot);
        mCarWheelBackL->setCenterOfMass(chassisPos + rot * mBackLOriginalPos, rot);
        mCarWheelFrontR->setCenterOfMass(chassisPos + rot * mFrontROriginalPos, rot);
        mCarWheelFrontL->setCenterOfMass(chassisPos + rot * mFrontLOriginalPos, rot);


        btTransform initialTransformChassis;
        initialTransformChassis.setOrigin(OgreBulletCollisions::convert(chassisPos));
        initialTransformChassis.setRotation(OgreBulletCollisions::convert(rot));

        mCarChassis->getBulletRigidBody()->setWorldTransform(initialTransformChassis);
        mCarChassis->getBulletRigidBody()->getMotionState()->setWorldTransform(initialTransformChassis);

        btTransform initialTransformBackR;
        btTransform initialTransformBackL;
        btTransform initialTransformFrontR;
        btTransform initialTransformFrontL;
        initialTransformBackR.setOrigin(OgreBulletCollisions::convert(chassisPos + rot * mBackROriginalPos));
        initialTransformBackR.setRotation(OgreBulletCollisions::convert(rot));
        initialTransformBackL.setOrigin(OgreBulletCollisions::convert(chassisPos + rot * mBackLOriginalPos));
        initialTransformBackL.setRotation(OgreBulletCollisions::convert(rot));
        initialTransformFrontR.setOrigin(OgreBulletCollisions::convert(chassisPos + rot * mFrontROriginalPos));
        initialTransformFrontR.setRotation(OgreBulletCollisions::convert(rot));
        initialTransformFrontL.setOrigin(OgreBulletCollisions::convert(chassisPos + rot * mFrontLOriginalPos));
        initialTransformFrontL.setRotation(OgreBulletCollisions::convert(rot));
        
        mCarWheelBackR->getBulletRigidBody()->setWorldTransform(initialTransformBackR);
        mCarWheelBackR->getBulletRigidBody()->getMotionState()->setWorldTransform(initialTransformBackR);

        mCarWheelBackL->getBulletRigidBody()->setWorldTransform(initialTransformBackL);
        mCarWheelBackL->getBulletRigidBody()->getMotionState()->setWorldTransform(initialTransformBackL);

        mCarWheelFrontR->getBulletRigidBody()->setWorldTransform(initialTransformFrontR);
        mCarWheelFrontR->getBulletRigidBody()->getMotionState()->setWorldTransform(initialTransformFrontR);

        mCarWheelFrontL->getBulletRigidBody()->setWorldTransform(initialTransformFrontL);
        mCarWheelFrontL->getBulletRigidBody()->getMotionState()->setWorldTransform(initialTransformFrontL);


        mCarChassis->getBulletRigidBody()->setCollisionFlags(originalFlagsChassis);
        mCarWheelBackR->getBulletRigidBody()->setCollisionFlags(originalFlagsBackR);
        mCarWheelBackL->getBulletRigidBody()->setCollisionFlags(originalFlagsBackL);
        mCarWheelFrontR->getBulletRigidBody()->setCollisionFlags(originalFlagsFrontR);
        mCarWheelFrontL->getBulletRigidBody()->setCollisionFlags(originalFlagsFrontL);

        btVector3 inertia(0,0,0);
        mCarChassis->getShape()->getBulletShape()->calculateLocalInertia(15.0f, inertia);
        mCarChassis->getBulletRigidBody()->setMassProps(15.0f, inertia);

        mCarWheelBackR->getShape()->getBulletShape()->calculateLocalInertia(2.0f, inertia);
        mCarWheelBackR->getBulletRigidBody()->setMassProps(2.0f, inertia);

        mCarWheelBackL->getShape()->getBulletShape()->calculateLocalInertia(2.0f, inertia);
        mCarWheelBackL->getBulletRigidBody()->setMassProps(2.0f, inertia);

        mCarWheelFrontR->getShape()->getBulletShape()->calculateLocalInertia(1.0f, inertia);
        mCarWheelFrontR->getBulletRigidBody()->setMassProps(1.0f, inertia);

        mCarWheelFrontL->getShape()->getBulletShape()->calculateLocalInertia(1.0f, inertia);
        mCarWheelFrontL->getBulletRigidBody()->setMassProps(1.0f, inertia);

        mCarChassis->getBulletRigidBody()->updateInertiaTensor();
        mCarWheelBackR->getBulletRigidBody()->updateInertiaTensor();
        mCarWheelBackL->getBulletRigidBody()->updateInertiaTensor();
        mCarWheelFrontR->getBulletRigidBody()->updateInertiaTensor();
        mCarWheelFrontL->getBulletRigidBody()->updateInertiaTensor();

        addToWorld();

    }
#endif
}

#if 0

void PSBaseCar::removeFromWorld()
{

    mWorld->removeConstraint(mSixDofSpringFrontL);
    mWorld->removeConstraint(mSixDofSpringFrontR);
    mWorld->removeConstraint(mSixDofSpringBackL);
    mWorld->removeConstraint(mSixDofSpringBackR);
    mWorld->removeRigidBody(mCarChassis);
    mWorld->removeRigidBody(mCarWheelBackR);
    mWorld->removeRigidBody(mCarWheelBackL);
    mWorld->removeRigidBody(mCarWheelFrontR);
    mWorld->removeRigidBody(mCarWheelFrontL);

    mCarChassis->setAngularVelocity(Ogre::Vector3::ZERO);
    mCarChassis->setLinearVelocity(Ogre::Vector3::ZERO);
    mCarWheelBackR->setAngularVelocity(Ogre::Vector3::ZERO);
    mCarWheelBackR->setLinearVelocity(Ogre::Vector3::ZERO);
    mCarWheelBackL->setAngularVelocity(Ogre::Vector3::ZERO);
    mCarWheelBackL->setLinearVelocity(Ogre::Vector3::ZERO);
    mCarWheelFrontR->setAngularVelocity(Ogre::Vector3::ZERO);
    mCarWheelFrontR->setLinearVelocity(Ogre::Vector3::ZERO);
    mCarWheelFrontL->setAngularVelocity(Ogre::Vector3::ZERO);
    mCarWheelFrontL->setLinearVelocity(Ogre::Vector3::ZERO);

    mCarChassis->getBulletRigidBody()->clearForces();
    mCarWheelBackR->getBulletRigidBody()->clearForces();
    mCarWheelBackL->getBulletRigidBody()->clearForces();
    mCarWheelFrontR->getBulletRigidBody()->clearForces();
    mCarWheelFrontL->getBulletRigidBody()->clearForces();

    mCarChassis->getBulletRigidBody()->setLinearFactor(btVector3(0.0f, 0.0f, 0.0f));
    mCarChassis->getBulletRigidBody()->setAngularFactor(btVector3(0.0f, 0.0f, 0.0f));
    mCarWheelBackR->getBulletRigidBody()->setLinearFactor(btVector3(0.0f, 0.0f, 0.0f));
    mCarWheelBackR->getBulletRigidBody()->setAngularFactor(btVector3(0.0f, 0.0f, 0.0f));
    mCarWheelBackL->getBulletRigidBody()->setLinearFactor(btVector3(0.0f, 0.0f, 0.0f));
    mCarWheelBackL->getBulletRigidBody()->setAngularFactor(btVector3(0.0f, 0.0f, 0.0f));
    mCarWheelFrontR->getBulletRigidBody()->setLinearFactor(btVector3(0.0f, 0.0f, 0.0f));
    mCarWheelFrontR->getBulletRigidBody()->setAngularFactor(btVector3(0.0f, 0.0f, 0.0f));
    mCarWheelFrontL->getBulletRigidBody()->setLinearFactor(btVector3(0.0f, 0.0f, 0.0f));
    mCarWheelFrontL->getBulletRigidBody()->setAngularFactor(btVector3(0.0f, 0.0f, 0.0f));

    mCarChassis->getBulletRigidBody()->updateInertiaTensor();
    mCarWheelBackR->getBulletRigidBody()->updateInertiaTensor();
    mCarWheelBackL->getBulletRigidBody()->updateInertiaTensor();
    mCarWheelFrontR->getBulletRigidBody()->updateInertiaTensor();
    mCarWheelFrontL->getBulletRigidBody()->updateInertiaTensor();

    //mWorld->stepSimulation(0.01f, 1, 1.0f / 600.0f);

}

void PSBaseCar::addToWorld()
{

    mCarChassis->setAngularVelocity(Ogre::Vector3::ZERO);
    mCarChassis->setLinearVelocity(Ogre::Vector3::ZERO);
    mCarWheelBackR->setAngularVelocity(Ogre::Vector3::ZERO);
    mCarWheelBackR->setLinearVelocity(Ogre::Vector3::ZERO);
    mCarWheelBackL->setAngularVelocity(Ogre::Vector3::ZERO);
    mCarWheelBackL->setLinearVelocity(Ogre::Vector3::ZERO);
    mCarWheelFrontR->setAngularVelocity(Ogre::Vector3::ZERO);
    mCarWheelFrontR->setLinearVelocity(Ogre::Vector3::ZERO);
    mCarWheelFrontL->setAngularVelocity(Ogre::Vector3::ZERO);
    mCarWheelFrontL->setLinearVelocity(Ogre::Vector3::ZERO);

    mCarChassis->getBulletRigidBody()->clearForces();
    mCarWheelBackR->getBulletRigidBody()->clearForces();
    mCarWheelBackL->getBulletRigidBody()->clearForces();
    mCarWheelFrontR->getBulletRigidBody()->clearForces();
    mCarWheelFrontL->getBulletRigidBody()->clearForces();

    mCarChassis->getBulletRigidBody()->setLinearFactor(btVector3(1.0f, 1.0f, 1.0f));
    mCarChassis->getBulletRigidBody()->setAngularFactor(btVector3(1.0f, 1.0f, 1.0f));
    mCarWheelBackR->getBulletRigidBody()->setLinearFactor(btVector3(1.0f, 1.0f, 1.0f));
    mCarWheelBackR->getBulletRigidBody()->setAngularFactor(btVector3(1.0f, 1.0f, 1.0f));
    mCarWheelBackL->getBulletRigidBody()->setLinearFactor(btVector3(1.0f, 1.0f, 1.0f));
    mCarWheelBackL->getBulletRigidBody()->setAngularFactor(btVector3(1.0f, 1.0f, 1.0f));
    mCarWheelFrontR->getBulletRigidBody()->setLinearFactor(btVector3(1.0f, 1.0f, 1.0f));
    mCarWheelFrontR->getBulletRigidBody()->setAngularFactor(btVector3(1.0f, 1.0f, 1.0f));
    mCarWheelFrontL->getBulletRigidBody()->setLinearFactor(btVector3(1.0f, 1.0f, 1.0f));
    mCarWheelFrontL->getBulletRigidBody()->setAngularFactor(btVector3(1.0f, 1.0f, 1.0f));

    mCarChassis->getBulletRigidBody()->updateInertiaTensor();
    mCarWheelBackR->getBulletRigidBody()->updateInertiaTensor();
    mCarWheelBackL->getBulletRigidBody()->updateInertiaTensor();
    mCarWheelFrontR->getBulletRigidBody()->updateInertiaTensor();
    mCarWheelFrontL->getBulletRigidBody()->updateInertiaTensor();

    mWorld->addRigidBody(mCarChassis, 0, 0);
    mWorld->addRigidBody(mCarWheelBackR, 1, 2);// don`t collide with other car
    mWorld->addRigidBody(mCarWheelBackL, 1, 2);// don`t collide with other car
    mWorld->addRigidBody(mCarWheelFrontR, 1, 2);// don`t collide with other car
    mWorld->addRigidBody(mCarWheelFrontL, 1, 2);// don`t collide with other car
    mWorld->addConstraint(mSixDofSpringFrontL, true);
    mWorld->addConstraint(mSixDofSpringFrontR, true);
    mWorld->addConstraint(mSixDofSpringBackL, true);
    mWorld->addConstraint(mSixDofSpringBackR, true);

    mWorld->stepSimulation(0.01f);

}

#endif

void PSBaseCar::clear()
{
    PSBaseVehicle::clear();

    mModelNode = NULL;
}

Ogre::Real PSBaseCar::getAlignedVelocity()const
{
    Ogre::Real res = 0.0f;
    Ogre::Quaternion rot = mCarChassis->getSceneNode()->_getDerivedOrientation();
    Ogre::Vector3 carVelocity = getLinearVelocity();
    Ogre::Vector3 carOrientation = getForwardAxis();

    res = carOrientation.dotProduct(carVelocity);

    return res;
}

Ogre::Real PSBaseCar::getLateralVelocity()const
{
    Ogre::Real res = 0.0f;
    Ogre::Quaternion rot = mCarChassis->getSceneNode()->_getDerivedOrientation();
    Ogre::Vector3 carVelocity = getLinearVelocity();

    Ogre::Vector3 carOrientation = rot * Ogre::Vector3::NEGATIVE_UNIT_X;

    res = carOrientation.dotProduct(carVelocity);

    return res;
}

Ogre::Vector3 PSBaseCar::getForwardAxis()const
{
    Ogre::Quaternion rot = mCarChassis->getSceneNode()->_getDerivedOrientation();
    return rot * Ogre::Vector3::NEGATIVE_UNIT_Z;
}

void PSBaseCar::initSuspension(const GameState& gameState)
{

    std::string carName = gameState.getSTRPowerslide().getValue(mCharacterName + " parameters", "car", "feral max");

    std::string de2Path = gameState.getSTRPowerslide().getValue(carName + " parameters", "base directory", "feral max");
    std::set<char> delim;
    delim.insert('\\');
    std::vector<std::string> pathComponents = Tools::splitpath(de2Path, delim, false);

    std::string de2Name = gameState.getSTRPowerslide().getValue(carName + " parameters", "de2 filename", "car.de2");
    std::string susName = de2Name.substr(0, de2Name.length() - 3) + "sus";

    SUSLoader().load(gameState.getPFLoaderData(), pathComponents[pathComponents.size() - 1], susName, mSuspensionData);

    assert(mSuspensionData.size() == 4 && "BaseApp:initSuspension incorrect number of wheels");
}

void PSBaseCar::initSounds(lua_State * pipeline, const GameState& gameState)
{
    DMLuaManager luaManager;

    Ogre::Real referenceDist = luaManager.ReadScalarFloat("Model.Sfx.ReferenceDist", pipeline);
    Ogre::Real maxDist = luaManager.ReadScalarFloat("Model.Sfx.MaxDist", pipeline);

    stopSounds();

    std::string carName = gameState.getSTRPowerslide().getValue(mCharacterName + " parameters", "car", "feral max");

    std::string de2Path = gameState.getSTRPowerslide().getValue(carName + " parameters", "base directory", "feral max");
    std::set<char> delim;
    delim.insert('\\');
    std::vector<std::string> pathComponents = Tools::splitpath(de2Path, delim, false);
    std::string carPath = pathComponents[pathComponents.size() - 1];

    if(!mIsAI)
    {
        mEngLow.reset(new OpenALSource("data/cars/" + carPath + "/sfx/enggears", "eng_low.its", gameState.getPFLoaderData()));
        mEngMid.reset(new OpenALSource("data/cars/" + carPath + "/sfx/enggears", "eng_mid.its", gameState.getPFLoaderData()));
        mEngHigh.reset(new OpenALSource("data/cars/" + carPath + "/sfx/enggears", "eng_high.its", gameState.getPFLoaderData()));

        mEngLow->setReferenceDistance(referenceDist);
        mEngMid->setReferenceDistance(referenceDist);
        mEngHigh->setReferenceDistance(referenceDist);

        mEngLow->setMaxDistance(maxDist);
        mEngMid->setMaxDistance(maxDist);
        mEngHigh->setMaxDistance(maxDist);

        mEngLow->setLooping(AL_TRUE);
        mEngMid->setLooping(AL_TRUE);
        mEngHigh->setLooping(AL_TRUE);
    }
    else
    {
        mEngHigh.reset(new OpenALSource("data/cars/" + carPath + "/sfx/enggears", "eng_high.its", gameState.getPFLoaderData(), 1.0f));

        mEngHigh->setReferenceDistance(referenceDist);

        mEngHigh->setMaxDistance(maxDist);

        mEngHigh->setLooping(AL_TRUE);
    }

}

void PSBaseCar::deinitSounds()
{
    stopSounds();

    mEngLow.reset();
    mEngMid.reset();
    mEngHigh.reset();
}

void PSBaseCar::stopSounds()
{
    if(mEngLow.get() && mEngLow->getSourceState() == AL_PLAYING)
    {
        mEngLow->stopPlaying();
    }

    if(mEngMid.get() && mEngMid->getSourceState() == AL_PLAYING)
    {
        mEngMid->stopPlaying();
    }

    if(mEngHigh.get() && mEngHigh->getSourceState() == AL_PLAYING)
    {
        mEngHigh->stopPlaying();
    }
}

float PSBaseCar::getCarParameter(const STRSettings& carSettings, const STRSettings& trackSettings, const STRSettings& defaultSettings, const std::string& section, const std::string& key) const
{
    float res = 0.0f;

    bool isFoundVal = false;
    res = carSettings.getFloatValue(section, key, isFoundVal);
    if(!isFoundVal)
    {
        res = trackSettings.getFloatValue(section, key, isFoundVal);
        if(!isFoundVal)
        {
            res = defaultSettings.getFloatValue(section, key, isFoundVal);
        }
    }

    return res;
}

Ogre::Vector4 PSBaseCar::getCarArray4Parameter(const STRSettings& carSettings, const STRSettings& trackSettings, const STRSettings& defaultSettings, const std::string& section, const std::string& key) const
{
    Ogre::Vector4 res = Ogre::Vector4::ZERO;

    bool isFoundVal = false;
    res = carSettings.getArray4Value(section, key, isFoundVal);
    if(!isFoundVal)
    {
        res = trackSettings.getArray4Value(section, key, isFoundVal);
        if(!isFoundVal)
        {
            res = defaultSettings.getArray4Value(section, key, isFoundVal);
        }
    }

    return res;
}

void PSBaseCar::setVisibility(bool isVisible)
{
    for(size_t q = 0; q < 5; ++q)
    {
        mModelEntity[q]->setVisible(isVisible);
    }
}