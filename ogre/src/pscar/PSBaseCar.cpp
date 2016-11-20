#include "../pcheader.h"

#include "PSBaseCar.h"

#include "../tools/OgreTools.h"

#include "../lua/DMLuaManager.h"

#include "../listeners/VehicleSceneObjectListener.h"
#include "../listeners/PlayerVehicleSceneObjectListener.h"

#include "../customs/CustomRigidBodyWheel.h"

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
Ogre::NameGenerator PSBaseCar::nameGenNodes("Scene/Node/Vehicle/Name");

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


    Ogre::Vector3 chassisPos = mModelNode->getPosition();
    Ogre::Quaternion chassisRot = mModelNode->getOrientation();
    Ogre::Vector3 connectionPoint_rrWheel = mWheelNodes[0]->getPosition();
    Ogre::Vector3 connectionPoint_rlWheel = mWheelNodes[1]->getPosition();
    Ogre::Vector3 connectionPoint_frWheel = mWheelNodes[2]->getPosition();
    Ogre::Vector3 connectionPoint_flWheel = mWheelNodes[3]->getPosition();

    OgreBulletCollisions::CompoundCollisionShape* compound = new OgreBulletCollisions::CompoundCollisionShape();

    //const Ogre::Vector3 chassisShift = strSettings.getArrayValue("", "centre of gravity");
    //OgreBulletCollisions::BoxCollisionShape* chassisShape = new OgreBulletCollisions::BoxCollisionShape(Ogre::Vector3(3.0f,0.75f, 8.0f));
    //compound->addChildShape(chassisShape, chassisShift);

    //if(!isAI)
    {
        float roofBackRadius = carSettings.getFloatValue("", "roof back radius");
        Ogre::Vector3 roofBack = carSettings.getArray3Value("", "roof back");
        roofBack.z = -roofBack.z;

        float roofFrontRadius = carSettings.getFloatValue("", "roof front radius");
        Ogre::Vector3 roofFront = carSettings.getArray3Value("", "roof front");
        roofFront.z = -roofFront.z;

        float bodyRadius = carSettings.getFloatValue("", "body radius");
        Ogre::Vector3 bodyBase = carSettings.getArray3Value("", "body base 0");
        bodyBase.z = -bodyBase.z;

        OgreBulletCollisions::SphereCollisionShape* roofBackR = new OgreBulletCollisions::SphereCollisionShape(roofBackRadius);
        OgreBulletCollisions::SphereCollisionShape* roofBackL = new OgreBulletCollisions::SphereCollisionShape(roofBackRadius);
        compound->addChildShape(roofBackR, roofBack);
        compound->addChildShape(roofBackL, Ogre::Vector3(-roofBack.x, roofBack.y, roofBack.z));
        
        OgreBulletCollisions::SphereCollisionShape* roofFrontR = new OgreBulletCollisions::SphereCollisionShape(roofFrontRadius);
        OgreBulletCollisions::SphereCollisionShape* roofFrontL = new OgreBulletCollisions::SphereCollisionShape(roofFrontRadius);
        compound->addChildShape(roofFrontR, roofFront);
        compound->addChildShape(roofFrontL, Ogre::Vector3(-roofFront.x, roofFront.y, roofFront.z));

        OgreBulletCollisions::SphereCollisionShape* chassisBody = new OgreBulletCollisions::SphereCollisionShape(bodyRadius);
        compound->addChildShape(chassisBody, bodyBase);
    }
    if(isAI)
    {
#if 0
        Ogre::Vector3 cockpit = strSettings.getArrayValue("", "cockpit");
        cockpit.z = -cockpit.z;

        OgreBulletCollisions::SphereCollisionShape* chassisCockpit = new OgreBulletCollisions::SphereCollisionShape(strSettings.getFloatValue("", "collision radius"));
        compound->addChildShape(chassisCockpit, cockpit);
#endif
    }


    mCarChassis = new OgreBulletDynamics::RigidBody(nameGenNodes.generate(), world);
    //mCarChassis = new OgreBulletDynamics::RigidBody("carChassis", world, 0, 1);//no collision with static mesh

    mCarChassis->setShape ( mModelNode, 
                            compound, 
                            luaManager.ReadScalarFloat("Model.Physics.Chassis.Restitution", pipeline), 
                            luaManager.ReadScalarFloat("Model.Physics.Chassis.Friction", pipeline), 
                            1.0f,
                            Ogre::Vector3::UNIT_SCALE,
                            getCarParameter(carSettings, trackSettings, defaultSettings, "", "mass"),
                            chassisPos, 
                            chassisRot);

    mCarChassis->getBulletRigidBody()->setCollisionFlags(mCarChassis->getBulletRigidBody()->getCollisionFlags() | btCollisionObject::CF_CUSTOM_MATERIAL_CALLBACK);

    //mCarChassis->setCenterOfMass(Ogre::Vector3(0.0f, -3.0f, 0.0f));

    Ogre::Real airDensityLinear = getCarParameter(carSettings, trackSettings, defaultSettings, "", "air density translation");
    Ogre::Real airDensityAngular = getCarParameter(carSettings, trackSettings, defaultSettings, "", "air density rotation");

    mCarChassis->setDamping(airDensityLinear, airDensityAngular);

    if(isAI)
    {
        //mCarChassis->setDamping(airDensityLinear, 0.9f);
    }

    //mCarChassis->getBulletRigidBody()->setCollisionFlags(mCarChassis->getBulletRigidBody()->getCollisionFlags() | btCollisionObject::CF_KINEMATIC_OBJECT);

    Ogre::Vector2 wheelRadius = carSettings.getArray2Value("", "wheel radii");
    mWheelRadiusFront = wheelRadius.x;
    mWheelRadiusBack = wheelRadius.y;


    OgreBulletCollisions::SphereCollisionShape* wheelShapeFront = new OgreBulletCollisions::SphereCollisionShape(mWheelRadiusFront);
    OgreBulletCollisions::SphereCollisionShape* wheelShapeBack = new OgreBulletCollisions::SphereCollisionShape(mWheelRadiusBack);

    mCarWheelFrontL = new CustomRigidBodyWheel(nameGenNodes.generate(), world, mModelNode, connectionPoint_flWheel, true, 1, 2);// don`t collide with other car
    mCarWheelFrontR = new CustomRigidBodyWheel(nameGenNodes.generate(), world, mModelNode, connectionPoint_frWheel, true, 1, 2);// don`t collide with other car
    mCarWheelBackL = new CustomRigidBodyWheel(nameGenNodes.generate(), world, mModelNode, connectionPoint_rlWheel, false, 1, 2);// don`t collide with other car
    mCarWheelBackR = new CustomRigidBodyWheel(nameGenNodes.generate(), world, mModelNode, connectionPoint_rrWheel, false, 1, 2);// don`t collide with other car

    Ogre::Vector3 anisotropicFriction(
            luaManager.ReadScalarFloat("Model.Physics.Wheels.AnisotropicFriction.x", pipeline),
            luaManager.ReadScalarFloat("Model.Physics.Wheels.AnisotropicFriction.y", pipeline),
            luaManager.ReadScalarFloat("Model.Physics.Wheels.AnisotropicFriction.z", pipeline)
        );

    const Ogre::Real rollingFriction = luaManager.ReadScalarFloat("Model.Physics.Wheels.RollingFriction", pipeline);

    mCarWheelFrontL->setShape ( mWheelNodes[3], 
                                wheelShapeFront, 
                                luaManager.ReadScalarFloat("Model.Physics.Wheels.Restitution", pipeline), 
                                luaManager.ReadScalarFloat("Model.Physics.Wheels.Friction", pipeline), 
                                rollingFriction,
                                anisotropicFriction,
                                luaManager.ReadScalarFloat("Model.Physics.Wheels.Front.Mass", pipeline), 
                                chassisPos + chassisRot * connectionPoint_flWheel, 
                                chassisRot);

    mCarWheelFrontR->setShape ( mWheelNodes[2], 
                                wheelShapeFront, 
                                luaManager.ReadScalarFloat("Model.Physics.Wheels.Restitution", pipeline), 
                                luaManager.ReadScalarFloat("Model.Physics.Wheels.Friction", pipeline), 
                                rollingFriction,
                                anisotropicFriction,
                                luaManager.ReadScalarFloat("Model.Physics.Wheels.Front.Mass", pipeline), 
                                chassisPos + chassisRot * connectionPoint_frWheel, 
                                chassisRot);

    mCarWheelBackL->setShape (  mWheelNodes[1], 
                                wheelShapeBack, 
                                luaManager.ReadScalarFloat("Model.Physics.Wheels.Restitution", pipeline), 
                                luaManager.ReadScalarFloat("Model.Physics.Wheels.Friction", pipeline), 
                                rollingFriction,
                                anisotropicFriction,
                                luaManager.ReadScalarFloat("Model.Physics.Wheels.Rear.Mass", pipeline), 
                                chassisPos + chassisRot * connectionPoint_rlWheel, 
                                chassisRot);

    mCarWheelBackR->setShape (  mWheelNodes[0], 
                                wheelShapeBack, 
                                luaManager.ReadScalarFloat("Model.Physics.Wheels.Restitution", pipeline), 
                                luaManager.ReadScalarFloat("Model.Physics.Wheels.Friction", pipeline), 
                                rollingFriction,
                                anisotropicFriction,
                                luaManager.ReadScalarFloat("Model.Physics.Wheels.Rear.Mass", pipeline), 
                                chassisPos + chassisRot * connectionPoint_rrWheel, 
                                chassisRot);

    mCarWheelFrontL->setDamping(airDensityLinear, airDensityAngular);
    mCarWheelFrontR->setDamping(airDensityLinear, airDensityAngular);
    mCarWheelBackL->setDamping(airDensityLinear, airDensityAngular);
    mCarWheelBackR->setDamping(airDensityLinear, airDensityAngular);


    mCarChassis->disableDeactivation ();
    mCarWheelFrontL->disableDeactivation ();
    mCarWheelFrontR->disableDeactivation ();
    mCarWheelBackL->disableDeactivation ();
    mCarWheelBackR->disableDeactivation ();

    Ogre::Real maxTravel = -getCarParameter(carSettings, trackSettings, defaultSettings, "", "max travel");

    bool limitSpringParamsF = luaManager.ReadScalarBool("Model.Physics.Wheels.Front.LimitSpringParams", pipeline);
    bool limitSpringParamsR = luaManager.ReadScalarBool("Model.Physics.Wheels.Rear.LimitSpringParams", pipeline);

    {
        //http://bulletphysics.org/mediawiki-1.5.8/index.php/Constraints
        //http://bulletphysics.org/Bullet/phpBB3/viewtopic.php?t=8284
        //http://bulletphysics.org/Bullet/phpBB3/viewtopic.php?t=6662
        //http://bulletphysics.org/Bullet/phpBB3/viewtopic.php?f=9&t=4785&view=previous

        mSixDofSpringFrontL = new OgreBulletDynamics::SixDofSpring2Constraint(mCarChassis, mCarWheelFrontL, connectionPoint_flWheel, Ogre::Quaternion::IDENTITY, Ogre::Vector3::ZERO, Ogre::Quaternion::IDENTITY);
        mSixDofSpringFrontL->setLinearLowerLimit(Ogre::Vector3(0.0f, maxTravel, 0.0f));
        mSixDofSpringFrontL->setLinearUpperLimit(Ogre::Vector3(0.0f, 0.0f, 0.0f));
        mSixDofSpringFrontL->setAngularLowerLimit(Ogre::Vector3(1.0f, 0.0f, 0.0f));
        mSixDofSpringFrontL->setAngularUpperLimit(Ogre::Vector3(0.0f, 0.0f, 0.0f));
        //static_cast<btGeneric6DofSpring2Constraint *>(sixDofSpring->getBulletTypedConstraint())->getTranslationalLimitMotor()->m_enableMotor[1] = true;
        //static_cast<btGeneric6DofSpring2Constraint *>(sixDofSpring->getBulletTypedConstraint())->getTranslationalLimitMotor()->m_targetVelocity[1] = 1.0f;
        //static_cast<btGeneric6DofSpring2Constraint *>(sixDofSpring->getBulletTypedConstraint())->getTranslationalLimitMotor()->m_maxMotorForce[1] = 2.5f;
        mSixDofSpringFrontL->enableSpring(1, true);
        mSixDofSpringFrontL->setStiffness(1, luaManager.ReadScalarFloat("Model.Physics.Wheels.Front.SpringStiffness", pipeline), limitSpringParamsF);
        mSixDofSpringFrontL->setDamping(1, luaManager.ReadScalarFloat("Model.Physics.Wheels.Front.SpringDamping", pipeline), limitSpringParamsF);
        //static_cast<btGeneric6DofSpring2Constraint *>(sixDofSpring->getBulletTypedConstraint())->setBounce(1, 0.0f);
        //sixDofSpring->setParam(BT_CONSTRAINT_STOP_CFM, 1.0e-5f, 5);
        mSixDofSpringFrontL->setEquilibriumPoint(1, maxTravel);
        world->addConstraint(mSixDofSpringFrontL, true);

        //static_cast<btGeneric6DofSpring2Constraint *>(mSixDofSpringFrontL->getBulletTypedConstraint())->enableMotor(3,true);
        //static_cast<btGeneric6DofSpring2Constraint *>(mSixDofSpringFrontL->getBulletTypedConstraint())->setMaxMotorForce(3,1000);
        //static_cast<btGeneric6DofSpring2Constraint *>(sixDofSpring->getBulletTypedConstraint())->setTargetVelocity(3,10);

        //static_cast<btGeneric6DofSpring2Constraint *>(mSixDofSpringFrontL->getBulletTypedConstraint())->enableMotor(4,true);
        //static_cast<btGeneric6DofSpring2Constraint *>(mSixDofSpringFrontL->getBulletTypedConstraint())->setMaxMotorForce(4,1000);
        //static_cast<btGeneric6DofSpring2Constraint *>(sixDofSpring->getBulletTypedConstraint())->setTargetVelocity(4,0.1f);
    }

    {
        mSixDofSpringFrontR = new OgreBulletDynamics::SixDofSpring2Constraint(mCarChassis, mCarWheelFrontR, connectionPoint_frWheel, Ogre::Quaternion::IDENTITY, Ogre::Vector3::ZERO, Ogre::Quaternion::IDENTITY);
        mSixDofSpringFrontR->setLinearLowerLimit(Ogre::Vector3(0.0f, maxTravel, 0.0f));
        mSixDofSpringFrontR->setLinearUpperLimit(Ogre::Vector3(0.0f, 0.0f, 0.0f));
        mSixDofSpringFrontR->setAngularLowerLimit(Ogre::Vector3(1.0f, 0.0f, 0.0f));
        mSixDofSpringFrontR->setAngularUpperLimit(Ogre::Vector3(0.0f, 0.0f, 0.0f));
        //static_cast<btGeneric6DofSpring2Constraint *>(sixDofSpring->getBulletTypedConstraint())->getTranslationalLimitMotor()->m_enableMotor[1] = true;
        //static_cast<btGeneric6DofSpring2Constraint *>(sixDofSpring->getBulletTypedConstraint())->getTranslationalLimitMotor()->m_targetVelocity[1] = 2.0f;
        //static_cast<btGeneric6DofSpring2Constraint *>(sixDofSpring->getBulletTypedConstraint())->getTranslationalLimitMotor()->m_maxMotorForce[1] = 2.5f;
        mSixDofSpringFrontR->enableSpring(1, true);
        mSixDofSpringFrontR->setStiffness(1, luaManager.ReadScalarFloat("Model.Physics.Wheels.Front.SpringStiffness", pipeline), limitSpringParamsF);
        mSixDofSpringFrontR->setDamping(1, luaManager.ReadScalarFloat("Model.Physics.Wheels.Front.SpringDamping", pipeline), limitSpringParamsF);
        //static_cast<btGeneric6DofSpring2Constraint *>(sixDofSpring->getBulletTypedConstraint())->setBounce(1, 0.0f);
        //sixDofSpring->setParam(BT_CONSTRAINT_STOP_CFM, 1.0e-5f, 5);
        mSixDofSpringFrontR->setEquilibriumPoint(1, maxTravel);
        world->addConstraint(mSixDofSpringFrontR, true);

        //static_cast<btGeneric6DofSpring2Constraint *>(mSixDofSpringFrontR->getBulletTypedConstraint())->enableMotor(3,true);
        //static_cast<btGeneric6DofSpring2Constraint *>(mSixDofSpringFrontR->getBulletTypedConstraint())->setMaxMotorForce(3,1000);
        //static_cast<btGeneric6DofSpring2Constraint *>(sixDofSpring->getBulletTypedConstraint())->setTargetVelocity(3,10);

        //static_cast<btGeneric6DofSpring2Constraint *>(mSixDofSpringFrontR->getBulletTypedConstraint())->enableMotor(4,true);
        //static_cast<btGeneric6DofSpring2Constraint *>(mSixDofSpringFrontR->getBulletTypedConstraint())->setMaxMotorForce(4,1000);
        //static_cast<btGeneric6DofSpring2Constraint *>(sixDofSpring->getBulletTypedConstraint())->setTargetVelocity(4,.1f);
    }

    {
        mSixDofSpringBackL = new OgreBulletDynamics::SixDofSpring2Constraint(mCarChassis, mCarWheelBackL, connectionPoint_rlWheel, Ogre::Quaternion::IDENTITY, Ogre::Vector3::ZERO, Ogre::Quaternion::IDENTITY);
        mSixDofSpringBackL->setLinearLowerLimit(Ogre::Vector3(0.0f, maxTravel, 0.0f));
        mSixDofSpringBackL->setLinearUpperLimit(Ogre::Vector3(0.0f, 0.0f, 0.0f));
        mSixDofSpringBackL->setAngularLowerLimit(Ogre::Vector3(1.0f, 0.0f, 0.0f));
        mSixDofSpringBackL->setAngularUpperLimit(Ogre::Vector3(0.0f, 0.0f, 0.0f));
        //static_cast<btGeneric6DofSpring2Constraint *>(sixDofSpring->getBulletTypedConstraint())->getTranslationalLimitMotor()->m_enableMotor[1] = true;
        //static_cast<btGeneric6DofSpring2Constraint *>(sixDofSpring->getBulletTypedConstraint())->getTranslationalLimitMotor()->m_targetVelocity[1] = 2.0f;
        //static_cast<btGeneric6DofSpring2Constraint *>(sixDofSpring->getBulletTypedConstraint())->getTranslationalLimitMotor()->m_maxMotorForce[1] = 2.5f;
        mSixDofSpringBackL->enableSpring(1, true);
        mSixDofSpringBackL->setStiffness(1, luaManager.ReadScalarFloat("Model.Physics.Wheels.Rear.SpringStiffness", pipeline), limitSpringParamsR);
        mSixDofSpringBackL->setDamping(1, luaManager.ReadScalarFloat("Model.Physics.Wheels.Rear.SpringDamping", pipeline), limitSpringParamsR);
        //static_cast<btGeneric6DofSpring2Constraint *>(sixDofSpring->getBulletTypedConstraint())->setBounce(1, 0.0f);
        //sixDofSpring->setParam(BT_CONSTRAINT_STOP_CFM, 1.0e-5f, 5);
        mSixDofSpringBackL->setEquilibriumPoint(1, maxTravel);
        world->addConstraint(mSixDofSpringBackL, true);

        //static_cast<btGeneric6DofSpring2Constraint *>(mSixDofSpringBackL->getBulletTypedConstraint())->enableMotor(3,true);
        //static_cast<btGeneric6DofSpring2Constraint *>(mSixDofSpringBackL->getBulletTypedConstraint())->setMaxMotorForce(3,1000);
        //static_cast<btGeneric6DofSpring2Constraint *>(sixDofSpring->getBulletTypedConstraint())->setTargetVelocity(3,10);
    }

    {
        mSixDofSpringBackR = new OgreBulletDynamics::SixDofSpring2Constraint(mCarChassis, mCarWheelBackR, connectionPoint_rrWheel, Ogre::Quaternion::IDENTITY, Ogre::Vector3::ZERO, Ogre::Quaternion::IDENTITY);
        mSixDofSpringBackR->setLinearLowerLimit(Ogre::Vector3(0.0f, maxTravel, 0.0f));
        mSixDofSpringBackR->setLinearUpperLimit(Ogre::Vector3(0.0f, 0.0f, 0.0f));
        mSixDofSpringBackR->setAngularLowerLimit(Ogre::Vector3(1.0f, 0.0f, 0.0f));
        mSixDofSpringBackR->setAngularUpperLimit(Ogre::Vector3(0.0f, 0.0f, 0.0f));
        //static_cast<btGeneric6DofSpring2Constraint *>(sixDofSpring->getBulletTypedConstraint())->getTranslationalLimitMotor()->m_enableMotor[1] = true;
        //static_cast<btGeneric6DofSpring2Constraint *>(sixDofSpring->getBulletTypedConstraint())->getTranslationalLimitMotor()->m_targetVelocity[1] = 2.0f;
        //static_cast<btGeneric6DofSpring2Constraint *>(sixDofSpring->getBulletTypedConstraint())->getTranslationalLimitMotor()->m_maxMotorForce[1] = 2.5f;
        mSixDofSpringBackR->enableSpring(1, true);
        mSixDofSpringBackR->setStiffness(1, luaManager.ReadScalarFloat("Model.Physics.Wheels.Rear.SpringStiffness", pipeline), limitSpringParamsR);
        mSixDofSpringBackR->setDamping(1, luaManager.ReadScalarFloat("Model.Physics.Wheels.Rear.SpringDamping", pipeline), limitSpringParamsR);
        //static_cast<btGeneric6DofSpring2Constraint *>(sixDofSpring->getBulletTypedConstraint())->setBounce(1, 0.0f);
        //sixDofSpring->setParam(BT_CONSTRAINT_STOP_CFM, 1.0e-5f, 5);
        mSixDofSpringBackR->setEquilibriumPoint(1, maxTravel);
        world->addConstraint(mSixDofSpringBackR, true);

        //static_cast<btGeneric6DofSpring2Constraint *>(mSixDofSpringBackR->getBulletTypedConstraint())->enableMotor(3,true);
        //static_cast<btGeneric6DofSpring2Constraint *>(mSixDofSpringBackR->getBulletTypedConstraint())->setMaxMotorForce(3,1000);
        //static_cast<btGeneric6DofSpring2Constraint *>(sixDofSpring->getBulletTypedConstraint())->setTargetVelocity(3,10);
        
    }

}

void PSBaseCar::setModelPositionOnGrid(const Ogre::Matrix4& transform)
{
    if(mModelNode)
    {
        Ogre::Quaternion rot = transform.extractQuaternion();
        Ogre::Radian angle;
        Ogre::Vector3 axis;
        rot.ToAngleAxis(angle, axis);
        rot.FromAngleAxis(angle, Ogre::Vector3(-axis.x, -axis.y, axis.z));
        Ogre::Vector3 chassisPos(transform.getTrans() + gridShift);
        
        mModelNode->setPosition(chassisPos);
        mModelNode->setOrientation(rot);

        mWheelNodes[0]->setPosition(chassisPos + rot * mBackROriginalPos);
        mWheelNodes[1]->setPosition(chassisPos + rot * mBackLOriginalPos);
        mWheelNodes[2]->setPosition(chassisPos + rot * mFrontROriginalPos);
        mWheelNodes[3]->setPosition(chassisPos + rot * mFrontLOriginalPos);

        mWheelNodes[0]->setOrientation(rot);
        mWheelNodes[1]->setOrientation(rot);
        mWheelNodes[2]->setOrientation(rot);
        mWheelNodes[3]->setOrientation(rot);



        mCarChassis->setCenterOfMass(chassisPos, rot);

        mCarWheelBackR->setCenterOfMass(chassisPos + rot * mBackROriginalPos, rot);
        mCarWheelBackL->setCenterOfMass(chassisPos + rot * mBackLOriginalPos, rot);
        mCarWheelFrontR->setCenterOfMass(chassisPos + rot * mFrontROriginalPos, rot);
        mCarWheelFrontL->setCenterOfMass(chassisPos + rot * mFrontLOriginalPos, rot);
    }
}

void PSBaseCar::getWheelsPositions(Ogre::Vector3& wheel0, Ogre::Vector3& wheel1, Ogre::Vector3& wheel2, Ogre::Vector3& wheel3) const
{
    if(mModelNode)
    {
        wheel0 = mWheelNodes[0]->getPosition();
        wheel1 = mWheelNodes[1]->getPosition();
        wheel2 = mWheelNodes[2]->getPosition();
        wheel3 = mWheelNodes[3]->getPosition();
    }
}

void PSBaseCar::clear()
{
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