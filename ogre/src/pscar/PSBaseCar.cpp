
#include "PSBaseCar.h"

#include "../tools/OgreTools.h"
#include "../tools/Conversions.h"

#include "../lua/DMLuaManager.h"

#include "../listeners/VehicleSceneObjectListener.h"
#include "../listeners/PlayerVehicleSceneObjectListener.h"

#include "../mesh/ModelsPool.h"

#ifndef NO_OPENAL
    #include "../OpenAL/OpenALSource.h"
#endif

#include "../loaders/TEXLoader.h"
#include "../loaders/SUSLoader.h"
#include "../OriginalSettings.h"

#include "../GameState.h"

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
    mChassisColliderIndex(0),
    mCharacterName("frantic")
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
                            ModelsPool* modelsPool,
                            Physics * world,
                            const std::string& characterName,
                            InitialVehicleSetup& initialVehicleSetup,
                            bool isAI)
{

    mCharacterName = characterName;

    mIsAI = isAI;

    DMLuaManager luaManager;

    std::string genTextureName = nameGenTextures.generate();
    std::string carPath;
    std::string carName = loadTexture(gameState, genTextureName, carPath);
    modelsPool->getCopyOfVehicle(gameState, carName, mModelEntity);

    bool isAttenuateExcludeBox = luaManager.ReadScalarBool("Model.IsAttenuateExcludeBox", pipeline);

    const Ogre::Vector2& fogStartEnd = gameState.getSTRPowerslide().getFogStartEnd(gameState.getTrackName());
    bool isFogEnabled = fogStartEnd.x >= 1000000.0f ? false : true;

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

                    std::string aiMaterial = "Model.Material.SingleSubMaterialAI";
                    if(isFogEnabled)
                    {
                        aiMaterial = "Model.Material.SingleSubMaterialAIFog";
                    }

                    newMat = CloneMaterial(  nameSub, 
                            luaManager.ReadScalarString(aiMaterial.c_str(), pipeline), 
                            texturesSubMat, 
                            1.0f,
                            TEMP_RESOURCE_GROUP_NAME);

                    //if(isFogEnabled)
                    //{
                        //const Ogre::ColourValue& skyColor = gameState.getSTRPowerslide().getTrackSkyColor(gameState.getTrackName());

                        //newMat->setFog(true, Ogre::FOG_LINEAR, skyColor, 0.0f, fogStartEnd.x, fogStartEnd.y);
                    //}
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

    for(int q = 0; q < InitialVehicleSetup::mWheelsAmount + 1; ++q)
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

        mModelEntity[q]->setCastShadows(luaManager.ReadScalarBool("Model.IsCastShadows", pipeline));

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

    Ogre::Quaternion rot = initialVehicleSetup.mTrackPosition.extractQuaternion();
    Ogre::Radian angle;
    Ogre::Vector3 axis;
    rot.ToAngleAxis(angle, axis);
    rot.FromAngleAxis(angle, Ogre::Vector3(-axis.x, -axis.y, axis.z));
    mModelNode->setOrientation(rot);
    mModelNode->setPosition(initialVehicleSetup.mTrackPosition.getTrans());

    STRSettings misSettings;
    misSettings.parse(gameState.getPFLoaderStore(), "data/misc", "car_misc.str");

    //init parms
    mCarSettings.parse(gameState.getPFLoaderStore(), "data/cars/" + carPath + "/data/default", "params.str");
    mTrackSettings.parse(gameState.getPFLoaderStore(), "data/cars/global/data/" + gameState.getSTRPowerslide().getDataSubDir(gameState.getTrackName()), "params.str");
    mDefaultSettings.parse(gameState.getPFLoaderStore(), "data/cars/global/data/default", "params.str");

    //init splines
    mCarSplines.parse(gameState.getPFLoaderStore(), "data/cars/" + carPath + "/data/default", "graphs.str");
    mTrackSplines.parse(gameState.getPFLoaderStore(), "data/cars/global/data/" + gameState.getSTRPowerslide().getDataSubDir(gameState.getTrackName()), "graphs.str");
    mDefaultSplines.parse(gameState.getPFLoaderStore(), "data/cars/global/data/default", "graphs.str");

    initialVehicleSetup.mCOG = -getCarArray3Parameter("", "centre of gravity");
    initialVehicleSetup.mCOG.z = -initialVehicleSetup.mCOG.z;

    initialVehicleSetup.mCoreBase = getCarArray3Parameter("", "core base");
    initialVehicleSetup.mCoreBase.z = -initialVehicleSetup.mCoreBase.z;

    initialVehicleSetup.mEngineIdleRevsStart = getCarParameter("", "idle revs start");
    initialVehicleSetup.mEngineIdleRevsEnd = getCarParameter("", "idle revs end");
    initialVehicleSetup.mGearRatioMain = getCarParameter("", "gear rev ratio");
    {
        Ogre::Vector4 tmpGear = getCarArray4Parameter("", "rev ratio");
        initialVehicleSetup.mGearRatio[0] = tmpGear.x;
        initialVehicleSetup.mGearRatio[1] = tmpGear.y;
        initialVehicleSetup.mGearRatio[2] = tmpGear.z;
        initialVehicleSetup.mGearRatio[3] = tmpGear.w;
    }
    {
        Ogre::Vector4 tmpGear = getCarArray4Parameter("", "change down");
        initialVehicleSetup.mChangeDown[0] = tmpGear.x;
        initialVehicleSetup.mChangeDown[1] = tmpGear.y;
        initialVehicleSetup.mChangeDown[2] = tmpGear.z;
        initialVehicleSetup.mChangeDown[3] = tmpGear.w;
    }
    {
        Ogre::Vector4 tmpGear = getCarArray4Parameter("", "change up");
        initialVehicleSetup.mChangeUp[0] = tmpGear.x;
        initialVehicleSetup.mChangeUp[1] = tmpGear.y;
        initialVehicleSetup.mChangeUp[2] = tmpGear.z;
        initialVehicleSetup.mChangeUp[3] = tmpGear.w;
    }

    //load wheels offsets
    {
        mBackROriginalPos = mCarSettings.getArray3Value("", "wheelbase back");
        mBackROriginalPos.z = -mBackROriginalPos.z;

        mBackLOriginalPos = mBackROriginalPos;
        mBackLOriginalPos.x = -mBackLOriginalPos.x;

        mBackROriginalPos += initialVehicleSetup.mCOG;
        mBackLOriginalPos += initialVehicleSetup.mCOG;

        mWheelNodes[0]->setPosition(mBackROriginalPos);
        mWheelNodes[1]->setPosition(mBackLOriginalPos);


        mFrontROriginalPos = mCarSettings.getArray3Value("", "wheelbase front");
        mFrontROriginalPos.z = -mFrontROriginalPos.z;

        mFrontLOriginalPos = mFrontROriginalPos;
        mFrontLOriginalPos.x = -mFrontLOriginalPos.x;

        mFrontROriginalPos += initialVehicleSetup.mCOG;
        mFrontLOriginalPos += initialVehicleSetup.mCOG;

        mWheelNodes[2]->setPosition(mFrontROriginalPos);
        mWheelNodes[3]->setPosition(mFrontLOriginalPos);
    }

    if(luaManager.ReadScalarBool("Model.Mesh.IsTangents", pipeline))
    {
        for(size_t q = 0; q < 5; ++q)
            BuildTangents(mModelEntity[q]);
    }


    initialVehicleSetup.mChassisPos = mModelNode->getPosition();
    initialVehicleSetup.mChassisRot = mModelNode->getOrientation();
    initialVehicleSetup.mConnectionPointWheel[0] = mBackROriginalPos;
    initialVehicleSetup.mConnectionPointWheel[1] = mBackLOriginalPos;
    initialVehicleSetup.mConnectionPointWheel[2] = mFrontROriginalPos;
    initialVehicleSetup.mConnectionPointWheel[3] = mFrontLOriginalPos;

    initialVehicleSetup.mRoofRadius[0] = mCarSettings.getFloatValue("", "roof back radius");
    initialVehicleSetup.mRoofRadius[1] = initialVehicleSetup.mRoofRadius[0];
    Ogre::Vector3 roofBack = mCarSettings.getArray3Value("", "roof back");
    roofBack.z = -roofBack.z;
    initialVehicleSetup.mRoofPos[0] = roofBack;
    initialVehicleSetup.mRoofPos[1] = Ogre::Vector3(-roofBack.x, roofBack.y, roofBack.z);

    initialVehicleSetup.mRoofRadius[2] = mCarSettings.getFloatValue("", "roof front radius");
    initialVehicleSetup.mRoofRadius[3] = initialVehicleSetup.mRoofRadius[2];
    Ogre::Vector3 roofFront = mCarSettings.getArray3Value("", "roof front");
    roofFront.z = -roofFront.z;
    initialVehicleSetup.mRoofPos[2] = roofFront;
    initialVehicleSetup.mRoofPos[3] = Ogre::Vector3(-roofFront.x, roofFront.y, roofFront.z);

    initialVehicleSetup.mBodyRadius = mCarSettings.getFloatValue("", "body radius");
    Ogre::Vector3 bodyBase = mCarSettings.getArray3Value("", "body base 0");
    bodyBase.z = -bodyBase.z;
    initialVehicleSetup.mBodyBasePos = bodyBase;

    Ogre::Vector2 wheelRadius = mCarSettings.getArray2Value("", "wheel radii");
    initialVehicleSetup.mWheelRadius[2] = wheelRadius.x;
    initialVehicleSetup.mWheelRadius[3] = wheelRadius.x;
    initialVehicleSetup.mWheelRadius[0] = wheelRadius.y;
    initialVehicleSetup.mWheelRadius[1] = wheelRadius.y;

    initialVehicleSetup.mAnisotropicFriction = Ogre::Vector3(
            luaManager.ReadScalarFloat("Model.Physics.Wheels.AnisotropicFriction.x", pipeline),
            luaManager.ReadScalarFloat("Model.Physics.Wheels.AnisotropicFriction.y", pipeline),
            luaManager.ReadScalarFloat("Model.Physics.Wheels.AnisotropicFriction.z", pipeline)
        );

    initialVehicleSetup.mRollingFriction = luaManager.ReadScalarFloat("Model.Physics.Wheels.RollingFriction", pipeline);

    initialVehicleSetup.mMaxTravel = getCarParameter("", "max travel");

    initialVehicleSetup.mWheelsFSpringStiffness = luaManager.ReadScalarFloat("Model.Physics.Wheels.Front.SpringStiffness", pipeline);
    initialVehicleSetup.mWheelsFSpringDamping = luaManager.ReadScalarFloat("Model.Physics.Wheels.Front.SpringDamping", pipeline);
    initialVehicleSetup.mWheelsRSpringStiffness = luaManager.ReadScalarFloat("Model.Physics.Wheels.Rear.SpringStiffness", pipeline);
    initialVehicleSetup.mWheelsRSpringDamping = luaManager.ReadScalarFloat("Model.Physics.Wheels.Rear.SpringDamping", pipeline);
    initialVehicleSetup.mLimitSpringParamsF = luaManager.ReadScalarBool("Model.Physics.Wheels.Front.LimitSpringParams", pipeline);
    initialVehicleSetup.mLimitSpringParamsR = luaManager.ReadScalarBool("Model.Physics.Wheels.Rear.LimitSpringParams", pipeline);

    initialVehicleSetup.mAirDensityTranslation = getCarParameter("", "air density translation");
    initialVehicleSetup.mAirDensityRot = getCarParameter("", "air density rotation");

    initialVehicleSetup.mChassisMass = getCarParameter("", "mass");
    initialVehicleSetup.mChassisInvMass = 1.0f / initialVehicleSetup.mChassisMass;
    initialVehicleSetup.mMomentOfInertia = getCarArray3Parameter("", "moment of inertia");
    initialVehicleSetup.mFrontSuspension = getCarParameter("", "front suspension");
    initialVehicleSetup.mRisingDamp = getCarParameter("", "rising damp");

    initialVehicleSetup.mGravityForce = misSettings.getFloatValue("", "gravity force");

    initialVehicleSetup.mChassisRestitution = luaManager.ReadScalarFloat("Model.Physics.Chassis.Restitution", pipeline);
    initialVehicleSetup.mChassisFriction = luaManager.ReadScalarFloat("Model.Physics.Chassis.Friction", pipeline);


    initialVehicleSetup.mWheelsFMass = luaManager.ReadScalarFloat("Model.Physics.Wheels.Front.Mass", pipeline);
    initialVehicleSetup.mWheelsRMass = luaManager.ReadScalarFloat("Model.Physics.Wheels.Rear.Mass", pipeline);
    initialVehicleSetup.mWheelsRestitution = luaManager.ReadScalarFloat("Model.Physics.Wheels.Restitution", pipeline);
    initialVehicleSetup.mWheelsFriction = luaManager.ReadScalarFloat("Model.Physics.Wheels.Friction", pipeline);


    //splines
    {
        Ogre::Real hscale = getCarParameter("", "wheel under ground d_d hscale", true);
        Ogre::Real vscale = getCarParameter("", "wheel under ground d_d vscale", true);

        std::vector<std::string> splinePointsStr = getCarArrayValueParameter("", "wheel under ground d_d");
        std::vector<Ogre::Real> splinePoints = convertSplinePoints(splinePointsStr);

        initialVehicleSetup.mWheelUnderGroundDD.init(splinePoints, hscale, vscale);
    }
    {
        Ogre::Real hscale = getCarParameter("", "wheel under ground v_v hscale", true);
        Ogre::Real vscale = getCarParameter("", "wheel under ground v_v vscale", true);

        std::vector<std::string> splinePointsStr = getCarArrayValueParameter("", "wheel under ground v_v");
        std::vector<Ogre::Real> splinePoints = convertSplinePoints(splinePointsStr);

        initialVehicleSetup.mWheelUnderGroundVV.init(splinePoints, hscale, vscale);
    }
    {
        Ogre::Real hscale = getCarParameter("", "wheel under ground d_dv hscale", true);
        Ogre::Real vscale = getCarParameter("", "wheel under ground d_dv vscale", true);

        std::vector<std::string> splinePointsStr = getCarArrayValueParameter("", "wheel under ground d_dv");
        std::vector<Ogre::Real> splinePoints = convertSplinePoints(splinePointsStr);

        initialVehicleSetup.mWheelUnderGroundDDV.init(splinePoints, hscale, vscale);
    }
    {
        Ogre::Real hscale = getCarParameter("", "wheel under ground v_dv hscale", true);
        Ogre::Real vscale = getCarParameter("", "wheel under ground v_dv vscale", true);

        std::vector<std::string> splinePointsStr = getCarArrayValueParameter("", "wheel under ground v_dv");
        std::vector<Ogre::Real> splinePoints = convertSplinePoints(splinePointsStr);

        initialVehicleSetup.mWheelUnderGroundVDV.init(splinePoints, hscale, vscale);
    }
    for(int q = 0; q < initialVehicleSetup.mVelocitySplinesAmoint; ++q)
    {
        std::string num = Conversions::DMToString(q);
        Ogre::Real hscale = getCarParameter("", "traction for velocity " + num + " hscale", true);
        Ogre::Real vscale = getCarParameter("", "traction for velocity " + num + " vscale", true);

        std::vector<std::string> splinePointsStr = getCarArrayValueParameter("", "traction for velocity " + num);
        std::vector<Ogre::Real> splinePoints = convertSplinePoints(splinePointsStr);

        initialVehicleSetup.mVelocitySpline[q].init(splinePoints, hscale, vscale);
    }
    //splines END

    //position wheels
    for(size_t q = 0; q < InitialVehicleSetup::mWheelsAmount; ++q)
    {
        mWheelNodes[q]->setOrientation(initialVehicleSetup.mChassisRot);
        mWheelNodes[q]->setPosition(initialVehicleSetup.mChassisPos + initialVehicleSetup.mChassisRot * initialVehicleSetup.mConnectionPointWheel[q]);
    }

    initPhysicalModel(world, mModelNode, mWheelNodes, initialVehicleSetup);

}

void PSBaseCar::repositionVehicle(const Ogre::Matrix4& transform)
{

    Ogre::Quaternion chassisRot = transform.extractQuaternion();
    Ogre::Radian angle;
    Ogre::Vector3 axis;
    chassisRot.ToAngleAxis(angle, axis);
    chassisRot.FromAngleAxis(angle, Ogre::Vector3(-axis.x, -axis.y, axis.z));
    Ogre::Vector3 chassisPos(transform.getTrans());

    repositionVehicle(chassisPos, chassisRot);

}

void PSBaseCar::repositionVehicle(const Ogre::Vector3& chassisPos, const Ogre::Quaternion& chassisRot)
{

    if(mModelNode)
    {
        mModelNode->setPosition(chassisPos);
        mModelNode->setOrientation(chassisRot);

        mWheelNodes[0]->setPosition(chassisPos + chassisRot * mBackROriginalPos);
        mWheelNodes[1]->setPosition(chassisPos + chassisRot * mBackLOriginalPos);
        mWheelNodes[2]->setPosition(chassisPos + chassisRot * mFrontROriginalPos);
        mWheelNodes[3]->setPosition(chassisPos + chassisRot * mFrontLOriginalPos);

        mWheelNodes[0]->setOrientation(chassisRot);
        mWheelNodes[1]->setOrientation(chassisRot);
        mWheelNodes[2]->setOrientation(chassisRot);
        mWheelNodes[3]->setOrientation(chassisRot);

        PSBaseVehicle::repositionVehicle(chassisPos, chassisRot, mModelNode, mWheelNodes);

    }

}

void PSBaseCar::clear()
{
    PSBaseVehicle::clear();

    //mModelNode = NULL;
}

Ogre::Real PSBaseCar::getAlignedVelocity()const
{
    Ogre::Real res = 0.0f;
    /*
    Ogre::Quaternion rot = mCarChassis->getSceneNode()->_getDerivedOrientation();
    Ogre::Vector3 carVelocity = getLinearVelocity();
    Ogre::Vector3 carOrientation = getForwardAxis();

    res = carOrientation.dotProduct(carVelocity);
*/
    return res;
}

Ogre::Real PSBaseCar::getLateralVelocity()const
{
    Ogre::Real res = 0.0f;
    /*
    Ogre::Quaternion rot = mCarChassis->getSceneNode()->_getDerivedOrientation();
    Ogre::Vector3 carVelocity = getLinearVelocity();

    Ogre::Vector3 carOrientation = rot * Ogre::Vector3::NEGATIVE_UNIT_X;

    res = carOrientation.dotProduct(carVelocity);
*/
    return res;
}

Ogre::Vector3 PSBaseCar::getForwardAxis()const
{
    /*
    Ogre::Quaternion rot = mCarChassis->getSceneNode()->_getDerivedOrientation();
    return rot * Ogre::Vector3::NEGATIVE_UNIT_Z;
    */
    return Ogre::Vector3::ZERO;
}

Ogre::Vector3 PSBaseCar::getLinearImpulse()const
{
    //return mCarChassis->getLinearImpulse();
    return Ogre::Vector3::ZERO;
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
#ifndef NO_OPENAL
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
#endif
}

void PSBaseCar::deinitSounds()
{
#ifndef NO_OPENAL
    stopSounds();

    mEngLow.reset();
    mEngMid.reset();
    mEngHigh.reset();
#endif
}

void PSBaseCar::stopSounds()
{
#ifndef NO_OPENAL
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
#endif
}

float PSBaseCar::getCarParameter(const std::string& section, const std::string& key, bool isSpline) const
{
    float res = 0.0f;

    bool isFoundVal = false;

    if(!isSpline)
    {
        res = mCarSettings.getFloatValue(section, key, isFoundVal);
        if(!isFoundVal)
        {
            res = mTrackSettings.getFloatValue(section, key, isFoundVal);
            if(!isFoundVal)
            {
                res = mDefaultSettings.getFloatValue(section, key, isFoundVal);
            }
        }
    }
    else
    {
        res = mCarSplines.getFloatValue(section, key, isFoundVal);
        if(!isFoundVal)
        {
            res = mTrackSplines.getFloatValue(section, key, isFoundVal);
            if(!isFoundVal)
            {
                res = mDefaultSplines.getFloatValue(section, key, isFoundVal);
            }
        }
    }

    return res;
}

Ogre::Vector3 PSBaseCar::getCarArray3Parameter(const std::string& section, const std::string& key) const
{
    Ogre::Vector3 res = Ogre::Vector3::ZERO;

    bool isFoundVal = false;
    res = mCarSettings.getArray3Value(section, key, isFoundVal);
    if(!isFoundVal)
    {
        res = mTrackSettings.getArray3Value(section, key, isFoundVal);
        if(!isFoundVal)
        {
            res = mDefaultSettings.getArray3Value(section, key, isFoundVal);
        }
    }

    return res;
}

Ogre::Vector4 PSBaseCar::getCarArray4Parameter(const std::string& section, const std::string& key) const
{
    Ogre::Vector4 res = Ogre::Vector4::ZERO;

    bool isFoundVal = false;
    res = mCarSettings.getArray4Value(section, key, isFoundVal);
    if(!isFoundVal)
    {
        res = mTrackSettings.getArray4Value(section, key, isFoundVal);
        if(!isFoundVal)
        {
            res = mDefaultSettings.getArray4Value(section, key, isFoundVal);
        }
    }

    return res;
}

std::vector<std::string> PSBaseCar::getCarArrayValueParameter(const std::string& section, const std::string& key) const
{
    std::vector<std::string> res;

    bool isFoundVal = false;
    res = mCarSplines.getArrayValue(section, key, isFoundVal);
    if(!isFoundVal)
    {
        res = mTrackSplines.getArrayValue(section, key, isFoundVal);
        if(!isFoundVal)
        {
            res = mDefaultSplines.getArrayValue(section, key, isFoundVal);
        }
    }

    return res;
}

std::vector<Ogre::Real> PSBaseCar::convertSplinePoints(const std::vector<std::string>& points) const
{
    std::vector<Ogre::Real> ret(points.size());

    for(size_t q = 0; q < points.size(); ++q)
    {
        Ogre::Real val;
        Conversions::DMFromString(points[q], val);
        ret[q] = val;
    }

    return ret;
}

void PSBaseCar::setVisibility(bool isVisible)
{
    for(size_t q = 0; q < 5; ++q)
    {
        mModelEntity[q]->setVisible(isVisible);
    }
}

std::string PSBaseCar::loadTexture(const GameState& gameState, const std::string& textureName, std::string& carPath)
{

    std::string carName = gameState.getSTRPowerslide().getValue(mCharacterName + " parameters", "car", "feral max");

    std::string de2Path = gameState.getSTRPowerslide().getValue(carName + " parameters", "base directory", "feral max");
    std::set<char> delim;
    delim.insert('\\');
    std::vector<std::string> pathComponents = Tools::splitpath(de2Path, delim, false);
    carPath = pathComponents[pathComponents.size() - 1];

#if defined(__ANDROID__)
    mTextureName = textureName;
#endif

    //load car texture
    {
        std::string carSkinName = gameState.getSTRPowerslide().getValue(carName + " parameters", "texture name", "feral max texture");
        carSkinName += "_m_1.tex";
        TEXLoader().load(gameState.getPFLoaderData(), "data/cars/" + carPath + "/textures/default/" + mCharacterName, carSkinName, textureName);
    }

    return carName;
}

#if defined(__ANDROID__)
void PSBaseCar::reloadTextures(const GameState& gameState)
{
    std::string carPath;
    loadTexture(gameState, mTextureName, carPath);
}
#endif