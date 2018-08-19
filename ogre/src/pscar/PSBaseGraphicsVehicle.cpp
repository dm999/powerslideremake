
#include "PSBaseGraphicsVehicle.h"

#include "../tools/OgreTools.h"

#include "../lua/DMLuaManager.h"

#include "../listeners/VehicleSceneObjectListener.h"
#include "../listeners/PlayerVehicleSceneObjectListener.h"

#include "../mesh/ModelsPool.h"

#include "../loaders/TEXLoader.h"
#include "../loaders/SUSLoader.h"

#include "../GameState.h"

Ogre::NameGenerator PSBaseGraphicsVehicle::nameGenMaterials("Scene/Material/Vehicle/Name");
Ogre::NameGenerator PSBaseGraphicsVehicle::nameGenTextures("Scene/Texture/Vehicle/Name");

PSBaseGraphicsVehicle::PSBaseGraphicsVehicle() :
    mModelNode(NULL),
    mCharacterName("frantic")
{
}

void PSBaseGraphicsVehicle::initGraphicsModel(  lua_State * pipeline, 
                            const GameState& gameState,
                            Ogre::SceneManager* sceneMgr, Ogre::SceneNode* mainNode,
                            ModelsPool* modelsPool,
                            const std::string& characterName,
                            const InitialVehicleSetup& initialVehicleSetup,
                            bool isAdvancedLighting)
{

    mCharacterName = characterName;

    DMLuaManager luaManager;

    std::string genTextureName = nameGenTextures.generate();
    std::string carName = loadTexture(gameState, genTextureName);
    modelsPool->getCopyOfVehicle(gameState, carName, mModelEntity);

    bool isAttenuateExcludeBox = gameState.getAttenuationPlayer();

    const Ogre::Vector2& fogStartEnd = gameState.getSTRPowerslide().getFogStartEnd(gameState.getTrackName());
    bool isFogEnabled = fogStartEnd.x >= 1000000.0f ? false : true;

    bool isSandBlaster = gameState.isSandblasterTrack();

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
                if(isAdvancedLighting)
                {
                    std::string playerMaterial = luaManager.ReadScalarString("Model.Material.SingleSubMaterial", pipeline);

                    if(isSandBlaster)
                    {
                        playerMaterial = luaManager.ReadScalarString("Model.Material.SingleSubMaterialNoAtt", pipeline);
                    }

                    if(isAttenuateExcludeBox && !isSandBlaster)
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

        if(!isAdvancedLighting)
            mModelEntity[q]->setListener(new VehicleSceneObjectListener(mModelEntity[q], sceneMgr, false));
        else
        {
            if(isAttenuateExcludeBox && !isSandBlaster)
                mModelEntity[q]->setListener(new PlayerVehicleSceneObjectListener(mModelEntity[q], sceneMgr, gameState.getExclusions()));
            else
                mModelEntity[q]->setListener(new VehicleSceneObjectListener(mModelEntity[q], sceneMgr, true));
        }

        mModelEntity[q]->setCastShadows(gameState.isCastShadows());

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


    mModelNode->setOrientation(initialVehicleSetup.mCarRot);
    mModelNode->setPosition(initialVehicleSetup.mCarGlobalPos);

    mModelNode->setPosition(initialVehicleSetup.mCarGlobalPos + initialVehicleSetup.mCarRot * initialVehicleSetup.mCOG);

    //position wheels
    for(size_t q = 0; q < InitialVehicleSetup::mWheelsAmount; ++q)
    {
        mWheelNodes[q]->setOrientation(initialVehicleSetup.mCarRot);
        mWheelNodes[q]->setPosition(initialVehicleSetup.mCarGlobalPos + initialVehicleSetup.mCarRot * initialVehicleSetup.mConnectionPointWheel[q]);
    }

    if(luaManager.ReadScalarBool("Model.Mesh.IsTangents", pipeline))
    {
        for(size_t q = 0; q < 5; ++q)
            BuildTangents(mModelEntity[q]);
    }

    mBackROriginalPos = initialVehicleSetup.mConnectionPointWheel[0];
    mBackLOriginalPos = initialVehicleSetup.mConnectionPointWheel[1];
    mFrontROriginalPos = initialVehicleSetup.mConnectionPointWheel[2];
    mFrontLOriginalPos = initialVehicleSetup.mConnectionPointWheel[3];
    mCOG = initialVehicleSetup.mCOG;

}

void PSBaseGraphicsVehicle::repositionVehicle(const Ogre::Vector3& chassisPos, const Ogre::Quaternion& chassisRot,
    const Ogre::Vector3 wheelPos[InitialVehicleSetup::mWheelsAmount], const Ogre::Quaternion wheelRot[InitialVehicleSetup::mWheelsAmount])
{
    mModelNode->setPosition(chassisPos);
    mModelNode->setOrientation(chassisRot);

    for(size_t q = 0; q < InitialVehicleSetup::mWheelsAmount; ++q)
    {
        mWheelNodes[q]->setPosition(wheelPos[q]);
        mWheelNodes[q]->setOrientation(wheelRot[q]);
    }

    AdjustSuspension(   mModelEntity[0]->getMesh().get(),
                        mSuspensionIndices, mSuspensionPointOriginalPos,
                        -(chassisPos + chassisRot * (mFrontLOriginalPos - mCOG)).distance(wheelPos[3]),
                        -(chassisPos + chassisRot * (mFrontROriginalPos - mCOG)).distance(wheelPos[2]),
                        -(chassisPos + chassisRot * (mBackLOriginalPos - mCOG)).distance(wheelPos[1]),
                        -(chassisPos + chassisRot * (mBackROriginalPos - mCOG)).distance(wheelPos[0]));
}

void PSBaseGraphicsVehicle::repositionVehicle(const Ogre::Vector3& chassisPos, const Ogre::Quaternion& chassisRot)
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
}

std::string PSBaseGraphicsVehicle::getCarPath(const GameState& gameState) const
{
    std::string carName = gameState.getSTRPowerslide().getValue(mCharacterName + " parameters", "car", "feral max");

    std::string de2Path = gameState.getSTRPowerslide().getValue(carName + " parameters", "base directory", "feral max");
    std::set<char> delim;
    delim.insert('\\');
    std::vector<std::string> pathComponents = Tools::splitpath(de2Path, delim, false);
    std::string carPath = pathComponents[pathComponents.size() - 1];
    return carPath;
}

void PSBaseGraphicsVehicle::initSuspension(const GameState& gameState)
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

std::string PSBaseGraphicsVehicle::loadTexture(const GameState& gameState, const std::string& textureName)
{
    std::string carName = gameState.getSTRPowerslide().getValue(mCharacterName + " parameters", "car", "feral max");
    std::string carPath = getCarPath(gameState);

#if defined(__ANDROID__)
    mTextureName = textureName;
#endif

    //load car texture
    {
        std::string carSkinName = gameState.getSTRPowerslide().getValue(carName + " parameters", "texture name", "feral max texture");
        carSkinName += "_m_1.tex";
        TEXLoader().load(gameState.getPFLoaderData(), "data/cars/" + carPath + "/textures/default/" + mCharacterName, carSkinName, textureName, TEMP_RESOURCE_GROUP_NAME, gameState.getGamma());
    }

    return carName;
}

#if defined(__ANDROID__)
void PSBaseGraphicsVehicle::reloadTextures(const GameState& gameState)
{
    loadTexture(gameState, mTextureName);
}
#endif

void PSBaseGraphicsVehicle::setVisibility(bool isVisible)
{
    for(size_t q = 0; q < 5; ++q)
    {
        mModelEntity[q]->setVisible(isVisible);
    }
}

