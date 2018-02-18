
#include "StaticMeshProcesser.h"

#include "../physics/Physics.h"

#include "../lua/DMLuaManager.h"

#include "../tools/OgreTools.h"

#include "../listeners/TerrainSceneObjectListener.h"
#include "../listeners/LoaderListener.h"

#include "../loaders/DE2Loader.h"
#include "../loaders/TRALoader.h"
#include "../loaders/TEXLoader.h"

#include "../tools/Conversions.h"

#ifdef _MSC_VER
    #pragma warning(disable:4996)
#endif

Ogre::NameGenerator StaticMeshProcesser::nameGenRigidBodies("RigidBody");

StaticMeshProcesser::StaticMeshProcesser()
    : mNameGenMaterials("Scene/Material/Name"),
    mNameGenNodes("Scene/StaticNode/Name"),
    mNameGenTextures("Scene/Texture/Name"),
    mPlainIndices(0),
    mIsVertexArraySupported(false)
{
}

void StaticMeshProcesser::initParts(lua_State * pipeline, 
                                    Ogre::SceneManager* sceneMgr, 
                                    Ogre::SceneNode* mainNode,
                                    bool isGlobalReset,
                                    GameState& gameState,
                                    Physics * world,
                                    LoaderListener* loaderListener)
{
    checkIsVertexArraySupported();

    DMLuaManager luaManager;

    mIsMaskLight = luaManager.ReadScalarBool("Terrain.Material.IsMaskLights", pipeline);

    mTerrainNodes.clear();

    std::vector<MSHData> mergedMSH;

    DE2Loader de2Loader;
    std::vector<MSHData> originalParts;
    std::string pfFolderName = gameState.getSTRPowerslide().getBaseDir(gameState.getTrackName());
    Ogre::DataStreamPtr fileToLoad = gameState.getPFLoaderData().getFile("data/tracks/" + pfFolderName, gameState.getSTRPowerslide().getValue(gameState.getTrackName() + " parameters", "de2 filename", ""));
    bool loadResult = false;
    if(fileToLoad.get() && fileToLoad->isReadable())
    {
        if(loaderListener)
            loaderListener->loadState(0.3f, "DE2 reading");//0.3 from BaseRaceMode::initData

        loadResult = de2Loader.load(originalParts, fileToLoad);

        if(loaderListener)
            loaderListener->loadState(0.31f, "DE2 loaded");//0.3 from BaseRaceMode::initData

        fileToLoad->close();

        if(loadResult)
        {
            mCollisionDetection.init(de2Loader.getDE2());
            createLights(pipeline, sceneMgr, de2Loader.getDE2(), gameState);
        }
    }
    if(loadResult)
    {

        for(size_t q = 0; q < de2Loader.getDE2().Data_TerranName.size(); ++q)
            mTerrainMapsNames.insert(de2Loader.getDE2().Data_TerranName[q]);

        std::map<std::string, mergedInfo> mapTexturesToMSHIndex;

        for(size_t q = 0; q < originalParts.size(); ++q)
        {
            MSHData& mshData = originalParts[q];

            Ogre::Vector3 min, max;
            Ogre::Vector3 centroid = mshData.getCentroid();
            mshData.getMinMax(min, max, centroid);

            mergeMSH(mshData, mapTexturesToMSHIndex, mergedMSH);

            mshData.clear();

        }

        //create textures
        loadTextures(mergedMSH, gameState.getPFLoaderData(), pfFolderName, loaderListener);

        if(loaderListener)
            loaderListener->loadState(0.7f, "Textures loaded");

        for(size_t q = 0; q < mergedMSH.size(); ++q)
        {
            Ogre::Entity* terrain;
            Ogre::SceneNode* terrainNode;

            std::string groupName = gameState.getSTRPowerslide().getBaseDir(gameState.getTrackName());
            std::string nodeName = groupName + Conversions::DMToString(q);

            Ogre::Vector3 min, max;
            Ogre::Vector3 centroid = mergedMSH[q].getCentroid();
            mergedMSH[q].getMinMax(min, max, centroid);

            //create graphics entitys
            if(isGlobalReset)
            {
                mergedMSH[q].preallocatePlainBuffer();
                terrain = createMesh(   pipeline, 
                                        sceneMgr, nodeName, 
                                        centroid, min, max, 
                                        mergedMSH[q], 
                                        gameState.getSTRPowerslide().getTrackSkyColor(gameState.getTrackName()),
                                        gameState.getSTRPowerslide().getFogStartEnd(gameState.getTrackName()),
                                        gameState.getSTRPowerslide().getTrackAmbientColor(gameState.getTrackName()),
                                        gameState.isCastShadows());
            }
            else
            {
                terrain = sceneMgr->createEntity(nodeName, nodeName, TEMP_RESOURCE_GROUP_NAME);
            }
            initPart(pipeline, sceneMgr, mainNode, isGlobalReset, gameState, terrain, terrainNode, centroid);

        }
    }
}

void StaticMeshProcesser::createLights(lua_State * pipeline, Ogre::SceneManager* sceneMgr, const DE2::DE2_File& de2, GameState& gameState)
{
    DMLuaManager luaManager;

    bool isDebugLights = luaManager.ReadScalarBool("Scene.TerrainScene.IsDebugLights", pipeline);
    bool IsDebugLightsSizeAsRange = luaManager.ReadScalarBool("Scene.TerrainScene.IsDebugLightsSizeAsRange", pipeline);

    for(size_t q = 0; q < de2.lights.size(); ++q)
    {
        Ogre::String nameLight = "Lamp_" + Conversions::DMToString(q + 1);
        Ogre::Light* light = sceneMgr->createLight(nameLight);

        Ogre::SceneNode * lightNode = sceneMgr->getRootSceneNode()->createChildSceneNode(nameLight);

        lightNode->attachObject(light);

        light->setCastShadows(false);
        light->setType(Ogre::Light::LT_POINT);
        light->setPosition(de2.lights[q].position.x, de2.lights[q].position.y, -de2.lights[q].position.z);
        light->setDiffuseColour(de2.lights[q].r, de2.lights[q].g, de2.lights[q].b);
        light->setSpecularColour(1.0f, 1.0f, 1.0f);

        //d.polubotko: adjusted to pass to shader light radius In/Out
        light->setAttenuation(de2.lights[q].rangeOut, de2.lights[q].rangeIn, 0.0f, 0.0f);

        if(light->getAttenuationRange() == 20000.0f)
        {
            gameState.setGlobalLight(light);
            light->setLightMask(0x01);
        }
        else
        {
            light->setLightMask(0x02);
        }

        if(isDebugLights)
        {

            Ogre::Real scale = 0.1f;

            if(IsDebugLightsSizeAsRange)
            {
                const Ogre::Real prefabRadius = 50.0f;
                Ogre::Real radius = light->getAttenuationRange();
                scale = radius / prefabRadius;
            }

            Ogre::String debugSphereName = mNameGenNodes.generate();
            Ogre::Entity * debugSphere = sceneMgr->createEntity(debugSphereName, Ogre::SceneManager::PT_SPHERE);
            debugSphere->setMaterialName("BaseWhiteNoLighting");
            Ogre::SceneNode * debugSphereNode = sceneMgr->getRootSceneNode()->createChildSceneNode(debugSphereName);
            debugSphereNode->attachObject(debugSphere);
            debugSphereNode->setPosition(light->getPosition());
            debugSphereNode->setScale(scale, scale, scale);
            debugSphere->setCastShadows(false);

            if(IsDebugLightsSizeAsRange)
                debugSphere->getSubEntity(0)->getMaterial()->getTechnique(0)->getPass(0)->setPolygonMode(Ogre::PM_WIREFRAME);
        }
    }

    if(gameState.isCastShadows())
    {
        //shadow light
        gameState.setShadowLight(sceneMgr->createLight("shadow_light"));
        //Ogre::SceneNode * lightNode = mModelNode->createChildSceneNode("shadow_light");
        Ogre::SceneNode * lightNode = sceneMgr->getRootSceneNode()->createChildSceneNode("shadow_light");
        lightNode->attachObject(gameState.getShadowLight());
        gameState.getShadowLight()->setType(Ogre::Light::LT_SPOTLIGHT);
        gameState.getShadowLight()->setCastShadows(true);
        //mShadowLight->setPosition(0.0f, 40.0f, 0.0f);
        //mShadowLight->setDirection(Ogre::Vector3::NEGATIVE_UNIT_Y);
        gameState.getShadowLight()->setSpotlightOuterAngle(Ogre::Degree(luaManager.ReadScalarFloat("Model.ShadowLightOuterAngle", pipeline)));
        gameState.getShadowLight()->setSpotlightInnerAngle(Ogre::Degree(luaManager.ReadScalarFloat("Model.ShadowLightInnerAngle", pipeline)));


        sceneMgr->setShadowTextureCount(1);
        // Shadow (after lights and camera)
        sceneMgr->setShadowTextureSelfShadow(false);
        sceneMgr->setShadowTextureCasterMaterial("Test/ShadowCaster");
        sceneMgr->setShadowTexturePixelFormat(Ogre::PF_FLOAT16_R);
        sceneMgr->setShadowCasterRenderBackFaces(false);
        //sceneMgr->setShadowTechnique(Ogre::SHADOWTYPE_TEXTURE_ADDITIVE_INTEGRATED);
        sceneMgr->setShadowTechnique(Ogre::SHADOWTYPE_TEXTURE_MODULATIVE);
        sceneMgr->setShadowTextureSize(luaManager.ReadScalarInt("Scene.ShadowMapSize", pipeline));
        sceneMgr->setShadowCameraSetup(Ogre::ShadowCameraSetupPtr(new Ogre::DefaultShadowCameraSetup()));
    }

    if(gameState.getLLTObject())
        gameState.getLLTObject()->setVisible(true);
}

void StaticMeshProcesser::loadTextures(const std::vector<MSHData>& mergedMSH, const PFLoader& pfloader, const std::string& trackName, LoaderListener* loaderListener)
{
    std::set<std::string> texturesNames;

    //get unique names
    for(size_t q = 0; q < mergedMSH.size(); ++q)
    {
        for(size_t w = 0; w < mergedMSH[q].textureNames.size(); ++w)
        {
            texturesNames.insert(mergedMSH[q].textureNames[w]);
        }
    }

#if defined(__ANDROID__)
    mTexturesNames = texturesNames;
#endif

    loadTextures(texturesNames, pfloader, trackName, loaderListener);
}

void StaticMeshProcesser::loadTextures(const std::set<std::string>& texturesNames, const PFLoader& pfloader, const std::string& trackName, LoaderListener* loaderListener)
{
    //ranges from BaseRaceMode::initData
    const float loaderMin = 0.5f;
    const float loaderMax = 0.7f;
    const float loaderDistance = loaderMax - loaderMin;

    size_t loadedAmount = 0;

    for(std::set<std::string>::const_iterator i = texturesNames.begin(), j = texturesNames.end();
        i != j; ++i, ++loadedAmount)
    {
        std::string noExtFileName = (*i).substr(0, (*i).length() - 4);
        Ogre::DataStreamPtr fileToLoad;

        fileToLoad = pfloader.getFile("data/tracks/" + trackName +"/textures", noExtFileName + "_m_1.tex");
        if(!fileToLoad.get() || !fileToLoad->isReadable())
        {
            fileToLoad = pfloader.getFile("data/tracks/" + trackName +"/textures", noExtFileName + "_m_2.tex");
        }
        if(!fileToLoad.get() || !fileToLoad->isReadable())
        {
            fileToLoad = pfloader.getFile("data/tracks/" + trackName +"/textures", noExtFileName + "_m_3.tex");
        }
        if(!fileToLoad.get() || !fileToLoad->isReadable())
        {
            fileToLoad = pfloader.getFile("data/tracks/" + trackName +"/textures", noExtFileName + "_m_4.tex");
        }

        if(fileToLoad.get() && fileToLoad->isReadable())
        {
            TEXLoader().load(fileToLoad, (*i));
            fileToLoad->close();
        }

        if(loaderListener && loadedAmount % 2)
            loaderListener->loadState(loaderMin + loaderDistance * static_cast<float>(loadedAmount) / static_cast<float>(texturesNames.size()), noExtFileName);
    }
}

#if defined(__ANDROID__)
void StaticMeshProcesser::loadTextures(const PFLoader& pfloader, const std::string& trackName, LoaderListener* loaderListener)
{
    loadTextures(mTexturesNames, pfloader, trackName, loaderListener);
}
#endif

void StaticMeshProcesser::mergeMSH(const MSHData& mshData, std::map<std::string, mergedInfo>& mapTexturesToMSHIndex, std::vector<MSHData>& mergedMSH)const
{
    for(size_t q = 0; q < mshData.texturesCount; ++q)
    {
        std::string textureName = mshData.textureNames[q];
        bool isDecal = mshData.isDecalTexture[q];

        std::map<std::string, mergedInfo>::iterator i = mapTexturesToMSHIndex.find(textureName);

        size_t indexForBatch = 0;
        if(i != mapTexturesToMSHIndex.end() && isDecal && (*i).second.isDecalTextureHappened)
        {
            indexForBatch = (*i).second.indexForBatchDecal;
        }
        else if(i != mapTexturesToMSHIndex.end() && !isDecal && (*i).second.isLitTextureHappened)
        {
            indexForBatch = (*i).second.indexForBatchLit;
        }
        else if(i != mapTexturesToMSHIndex.end() && isDecal && (*i).second.isLitTextureHappened)
        {
            indexForBatch = mergedMSH.size();
            MSHData newMsh;
            newMsh.texturesCount = 1;
            newMsh.triCount = 0;
            newMsh.vertCount = 0;
            newMsh.textureNames.push_back(textureName);
            newMsh.isDecalTexture.push_back(isDecal);
            mergedMSH.push_back(newMsh);

            (*i).second.indexForBatchDecal = indexForBatch;
            (*i).second.isDecalTextureHappened = true;
        }
        else if(i != mapTexturesToMSHIndex.end() && !isDecal && (*i).second.isDecalTextureHappened)
        {
            indexForBatch = mergedMSH.size();
            MSHData newMsh;
            newMsh.texturesCount = 1;
            newMsh.triCount = 0;
            newMsh.vertCount = 0;
            newMsh.textureNames.push_back(textureName);
            newMsh.isDecalTexture.push_back(isDecal);
            mergedMSH.push_back(newMsh);

            (*i).second.indexForBatchLit = indexForBatch;
            (*i).second.isLitTextureHappened = true;
        }
        else
        {
            indexForBatch = mergedMSH.size();
            MSHData newMsh;
            newMsh.texturesCount = 1;
            newMsh.triCount = 0;
            newMsh.vertCount = 0;
            newMsh.textureNames.push_back(textureName);
            newMsh.isDecalTexture.push_back(isDecal);
            mergedMSH.push_back(newMsh);

            if(isDecal)
                mapTexturesToMSHIndex.insert(std::make_pair(textureName, mergedInfo(indexForBatch, true, 0, false)));
            if(!isDecal)
                mapTexturesToMSHIndex.insert(std::make_pair(textureName, mergedInfo(0, false, indexForBatch, true)));
        }

        MSHData& mergedData = mergedMSH[indexForBatch];
        
        size_t triCountAdded = 0;
        size_t vertCountAdded = 0;
        for(size_t w = 0; w < mshData.triCount; ++w)
        {
            if(mshData.textureForTriangleIndex[w] == q)
            {
                ++triCountAdded;
                vertCountAdded += 3;

                mergedData.plainVertices.push_back(mshData.plainVertices[w * 3 + 0]);
                mergedData.plainVertices.push_back(mshData.plainVertices[w * 3 + 1]);
                mergedData.plainVertices.push_back(mshData.plainVertices[w * 3 + 2]);

                mergedData.plainNormals.push_back(mshData.plainNormals[w * 3 + 0]);
                mergedData.plainNormals.push_back(mshData.plainNormals[w * 3 + 1]);
                mergedData.plainNormals.push_back(mshData.plainNormals[w * 3 + 2]);

                mergedData.plainTexCoords.push_back(mshData.plainTexCoords[w * 3 + 0]);
                mergedData.plainTexCoords.push_back(mshData.plainTexCoords[w * 3 + 1]);
                mergedData.plainTexCoords.push_back(mshData.plainTexCoords[w * 3 + 2]);

                mergedData.textureForTriangleIndex.push_back(0);

                mergedData.vertexes.push_back(mshData.plainVertices[w * 3 + 0]);
                mergedData.vertexes.push_back(mshData.plainVertices[w * 3 + 1]);
                mergedData.vertexes.push_back(mshData.plainVertices[w * 3 + 2]);
            }
        }

        mergedData.triCount += triCountAdded;
        mergedData.vertCount += vertCountAdded;

    }
}

void StaticMeshProcesser::initPart(lua_State * pipeline, 
                                   Ogre::SceneManager* sceneMgr, 
                                   Ogre::SceneNode* mainNode,
                                   bool isGlobalReset,
                                   GameState& gameState,
                                   Ogre::Entity* terrain,
                                   Ogre::SceneNode*& terrainNode,
                                   const Ogre::Vector3& centroid)
{

    DMLuaManager luaManager;

    mTerrainNodes.push_back(terrain);

    terrain->setCastShadows(false);

    //if mask enabled allow only global lights
    if(mIsMaskLight)
    {
        terrain->setLightMask(0x1);
    }


    terrainNode = mainNode->createChildSceneNode();
    terrainNode->attachObject(terrain);
    terrain->setListener(new TerrainSceneObjectListener(terrain, sceneMgr, gameState.getExclusions()));

    terrainNode->setPosition(centroid);
    
    if(isGlobalReset)
    {
        if(luaManager.ReadScalarBool("Terrain.Scene.IsAdjustNormals", pipeline))
        {
            AddjustNormals(terrain, luaManager.ReadScalarFloat("Terrain.Scene.AdjustNormalsAngleThreshold", pipeline));
        }
    }

    if(luaManager.ReadScalarBool("Terrain.Mesh.IsTangents", pipeline))
    {
        BuildTangents(terrain);
    }

    if(luaManager.ReadScalarBool("Terrain.Mesh.IsBB", pipeline))
    {
        terrainNode->showBoundingBox(true);
    }

    if(luaManager.ReadScalarBool("Terrain.Mesh.IsBS", pipeline))
    {
        const Ogre::Real prefabRadius = 50.0f;
        Ogre::Real radius = terrain->getMesh()->getBoundingSphereRadius();

        Ogre::Real scale = radius / prefabRadius;

        Ogre::String debugSphereName = mNameGenNodes.generate();
        Ogre::Entity * debugSphere = sceneMgr->createEntity(debugSphereName, Ogre::SceneManager::PT_SPHERE);
        debugSphere->setMaterialName("BaseWhiteNoLighting");
        Ogre::SceneNode * debugSphereNode = sceneMgr->getRootSceneNode()->createChildSceneNode(debugSphereName);
        debugSphereNode->attachObject(debugSphere);
        debugSphereNode->setPosition(centroid);
        debugSphereNode->setScale(scale, scale, scale);
        debugSphere->getSubEntity(0)->getMaterial()->getTechnique(0)->getPass(0)->setPolygonMode(Ogre::PM_WIREFRAME);
        debugSphere->setCastShadows(false);
    }

}

void StaticMeshProcesser::checkIsVertexArraySupported()
{
    mIsVertexArraySupported = false;

    //TODO(d.polubotko): implement check for GL_EXT_texture_array extension

#if !defined(__ANDROID__)
    if (    !Ogre::GpuProgramManager::getSingleton().isSyntaxSupported("vs_4_0") &&
            !Ogre::GpuProgramManager::getSingleton().isSyntaxSupported("ps_2_0") && 
            !Ogre::GpuProgramManager::getSingleton().isSyntaxSupported("glsl") &&
            !Ogre::GpuProgramManager::getSingleton().isSyntaxSupported("gp4fp"))
    {
        // not supported?
    }
    else
    {
        //mIsVertexArraySupported = true;
    }
#endif

}

Ogre::Entity* StaticMeshProcesser::createMesh(  lua_State * pipeline, 
                                                Ogre::SceneManager* sceneMgr, 
                                                const Ogre::String& entityName, 
                                                const Ogre::Vector3& centroid,
                                                const Ogre::Vector3& min, 
                                                const Ogre::Vector3& max,
                                                MSHData& mshData,
                                                const Ogre::ColourValue& skyColor,
                                                const Ogre::Vector2& fogStartEnd,
                                                const Ogre::ColourValue& ambient,
                                                bool isShadow)
{
    Ogre::Entity * res = NULL;



    DMLuaManager luaManager;

    Ogre::MaterialPtr overrideMaterial = Ogre::MaterialManager::getSingleton().getByName(luaManager.ReadScalarString("Terrain.Material.SingleSubMaterial", pipeline));
    Ogre::MaterialPtr overrideMaterialFog = Ogre::MaterialManager::getSingleton().getByName(luaManager.ReadScalarString("Terrain.Material.SingleSubMaterialFog", pipeline));
    Ogre::MaterialPtr overrideMaterialArray = Ogre::MaterialManager::getSingleton().getByName(luaManager.ReadScalarString("Terrain.Material.SingleSubMaterialArray", pipeline));

    if(isShadow)
    {
        overrideMaterial = Ogre::MaterialManager::getSingleton().getByName(luaManager.ReadScalarString("Terrain.Material.SingleSubMaterial", pipeline));
        overrideMaterialFog = Ogre::MaterialManager::getSingleton().getByName(luaManager.ReadScalarString("Terrain.Material.SingleSubMaterialFog", pipeline));
        overrideMaterialArray = Ogre::MaterialManager::getSingleton().getByName(luaManager.ReadScalarString("Terrain.Material.SingleSubMaterialArray", pipeline));
    }
    else
    {
        overrideMaterial = Ogre::MaterialManager::getSingleton().getByName(luaManager.ReadScalarString("Terrain.Material.SingleSubMaterialNoShadow", pipeline));
        overrideMaterialFog = Ogre::MaterialManager::getSingleton().getByName(luaManager.ReadScalarString("Terrain.Material.SingleSubMaterialNoShadowFog", pipeline));
        overrideMaterialArray = Ogre::MaterialManager::getSingleton().getByName(luaManager.ReadScalarString("Terrain.Material.SingleSubMaterialArray", pipeline));
    }

    std::string defaultTextureName = luaManager.ReadScalarString("Terrain.Material.DefaultTextureName", pipeline);

    bool isOverrideDefault = luaManager.ReadScalarBool("Terrain.Material.IsOverrideDefaultTextureWithTransparentForSubMaterials", pipeline);

    bool isFogEnabled = fogStartEnd.x >= 1000000.0f ? false : true;
    if(isFogEnabled)
    {
        sceneMgr->setFog(Ogre::FOG_LINEAR, skyColor, 0.0f, fogStartEnd.x, fogStartEnd.y);
    }

    if(!overrideMaterial.isNull())
    {

        std::vector<std::string> materialNames;

        if(mIsVertexArraySupported && !overrideMaterialArray.isNull())
        {
            materialNames = loadWithVertexArray(    isOverrideDefault, 
                                                    defaultTextureName, 
                                                    overrideMaterial->getName(),
                                                    overrideMaterialArray->getName(),
                                                    mshData,
                                                    ambient);
        }
        else
        {
            materialNames = loadWithoutVertexArray( isOverrideDefault, 
                                                    defaultTextureName, 
                                                    isFogEnabled ? overrideMaterialFog->getName() : overrideMaterial->getName(),
                                                    mshData,
                                                    skyColor,
                                                    fogStartEnd,
                                                    ambient,
                                                    isFogEnabled);
        }


        //create interleaved buffer
        mshData.makePlain(centroid);

        //http://www.ogre3d.org/tikiwiki/Generating+A+Mesh
        Ogre::MeshPtr pMesh = Ogre::MeshManager::getSingleton().createManual(entityName, TEMP_RESOURCE_GROUP_NAME);

        pMesh->sharedVertexData = new Ogre::VertexData();
        pMesh->sharedVertexData->vertexCount = mshData.triCount * 3;
        Ogre::VertexDeclaration* decl = pMesh->sharedVertexData->vertexDeclaration;
        size_t offset = 0;
        
        decl->addElement(0, offset, Ogre::VET_FLOAT3, Ogre::VES_POSITION);
        offset += Ogre::VertexElement::getTypeSize(Ogre::VET_FLOAT3);

        decl->addElement(0, offset, Ogre::VET_FLOAT3, Ogre::VES_NORMAL);
        offset += Ogre::VertexElement::getTypeSize(Ogre::VET_FLOAT3);

        decl->addElement(0, offset, Ogre::VET_FLOAT3, Ogre::VES_TEXTURE_COORDINATES);
        offset += Ogre::VertexElement::getTypeSize(Ogre::VET_FLOAT3);

        decl->addElement(0, offset, Ogre::VET_COLOUR, Ogre::VES_DIFFUSE);
        offset += Ogre::VertexElement::getTypeSize(Ogre::VET_COLOUR);

        Ogre::HardwareVertexBufferSharedPtr vbuf = Ogre::HardwareBufferManager::getSingleton().createVertexBuffer(
            offset, pMesh->sharedVertexData->vertexCount, Ogre::HardwareBuffer::HBU_STATIC);

        vbuf->writeData(0, vbuf->getSizeInBytes(), &mshData.plainBuffer[0], true);

        Ogre::VertexBufferBinding* bind = pMesh->sharedVertexData->vertexBufferBinding; 
        bind->setBinding(0, vbuf);

        for(size_t q = 0; q < mshData.submeshesTriangleIndixesDiffuse.size(); ++q)
        {
            Ogre::SubMesh* sub = pMesh->createSubMesh();
            
            sub->setMaterialName(materialNames[q]);

            Ogre::HardwareIndexBufferSharedPtr ibuf = Ogre::HardwareBufferManager::getSingleton().
            createIndexBuffer(
            Ogre::HardwareIndexBuffer::IT_16BIT, 
            mshData.submeshesTriangleIndixesDiffuse[q].size(), 
            Ogre::HardwareBuffer::HBU_STATIC);
     
            /// Upload the index data to the card
            ibuf->writeData(0, ibuf->getSizeInBytes(), &mshData.submeshesTriangleIndixesDiffuse[q][0], true);

            sub->useSharedVertices = true;
            sub->indexData->indexBuffer = ibuf;
            sub->indexData->indexCount = mshData.submeshesTriangleIndixesDiffuse[q].size();
            sub->indexData->indexStart = 0;
        }

        pMesh->_setBounds(Ogre::AxisAlignedBox(min, max),false);
        pMesh->load();

        res = sceneMgr->createEntity(entityName, pMesh);

    }

    return res;
}

std::vector<std::string> StaticMeshProcesser::loadWithVertexArray(bool isOverrideDefault, 
                                                                  std::string defaultTextureName, 
                                                                  const Ogre::String& ovverideMaterialName,
                                                                  const Ogre::String& ovverideMaterialArrayName,
                                                                  MSHData& mshData,
                                                                  const Ogre::ColourValue& ambient)
{
    std::vector<std::string> materialNames;

    //pass for submeshes materials
    if(mshData.texturesCount == 1)
    {
        std::string textureName = mshData.textureNames[0];

        bool isTransparentMaterial = false;

        if(isOverrideDefault)
        {
            if(textureName == defaultTextureName)
            {
                isTransparentMaterial = true;
            }
        }

        std::string materialName = "Basewhite";

        if(isTransparentMaterial)
        {
            materialName = "Test/Transparent";
        }
        else if(mshData.isDecalTexture[0])
        {
            std::vector<Ogre::String> texturesSubMat;

            texturesSubMat.push_back(textureName);

            materialName = mNameGenMaterials.generate();

            Ogre::MaterialPtr newMat = CloneMaterial(  materialName, 
                        "Test/Diffuse", 
                        texturesSubMat, 
                        1.0f,
                        TEMP_RESOURCE_GROUP_NAME);
        }
        else
        {
            std::vector<Ogre::String> texturesSubMat;

            texturesSubMat.push_back(textureName);

            materialName = mNameGenMaterials.generate();

            Ogre::MaterialPtr newMat = CloneMaterial(  materialName, 
                ovverideMaterialName, 
                texturesSubMat, 
                1.0f,
                TEMP_RESOURCE_GROUP_NAME);

            newMat->setAmbient(ambient);
        }

        materialNames.push_back(materialName);
    }

    if(mshData.texturesCount > 1)
    {
        std::string materialName = "Basewhite";

        Ogre::String texName = mNameGenTextures.generate();

        Ogre::TexturePtr tex = Ogre::TextureManager::getSingleton().createManual(texName, 
        TEMP_RESOURCE_GROUP_NAME, 
        Ogre::TEX_TYPE_2D_ARRAY, 
        256, 256, mshData.texturesCount, 
        0, 
        Ogre::PF_R8G8B8 );

        for (size_t i = 0; i < mshData.texturesCount; i++)
        {
            Ogre::Image terrainTex;
            terrainTex.load(mshData.textureNames[i], TEMP_RESOURCE_GROUP_NAME);
            Ogre::HardwarePixelBufferSharedPtr pixelBufferBuf = tex->getBuffer(0);
            const Ogre::PixelBox&  currImage = pixelBufferBuf->lock(Ogre::Box(0,0,i,terrainTex.getHeight(), terrainTex.getHeight(), i+1), Ogre::HardwareBuffer::HBL_DISCARD);
            Ogre::PixelUtil::bulkPixelConversion(terrainTex.getPixelBox(), currImage);
            pixelBufferBuf->unlock();
        }

        std::vector<Ogre::String> texturesSubMat;

        texturesSubMat.push_back(texName);

        materialName = mNameGenMaterials.generate();

        Ogre::MaterialPtr newMat = CloneMaterial(  materialName, 
            ovverideMaterialArrayName, 
            texturesSubMat, 
            1.0f,
            TEMP_RESOURCE_GROUP_NAME);

        newMat->setAmbient(ambient);

        materialNames.push_back(materialName);
    }


    //textures data indices
    for(size_t q = 0; q < mshData.texturesCount; ++q)
    {
        for(size_t w = 0; w < mshData.triCount; ++w)
        {
            if(mshData.textureForTriangleIndex[w] == q)
            {
                //texture index as w coordinate for vertex array
                mshData.plainTexCoords[w * 3 + 0].z = static_cast<float>(q);
                mshData.plainTexCoords[w * 3 + 1].z = static_cast<float>(q);
                mshData.plainTexCoords[w * 3 + 2].z = static_cast<float>(q);
            }
        }
    }

    //all indices for one texture
    {
        std::vector<unsigned short> triPlainIndixes;

        for(size_t w = 0; w < mshData.triCount; ++w)
        {
            {
                triPlainIndixes.push_back(w * 3 + 0);
                triPlainIndixes.push_back(w * 3 + 1);
                triPlainIndixes.push_back(w * 3 + 2);
            }
        }

        mshData.submeshesTriangleIndixesDiffuse.push_back(triPlainIndixes);
    }

    return materialNames;
}

std::vector<std::string> StaticMeshProcesser::loadWithoutVertexArray(bool isOverrideDefault, 
                                                                     std::string defaultTextureName, 
                                                                     const Ogre::String& ovverideMaterialName,
                                                                     MSHData& mshData,
                                                                     const Ogre::ColourValue& skyColor,
                                                                     const Ogre::Vector2& fogStartEnd,
                                                                     const Ogre::ColourValue& ambient,
                                                                     bool isFogEnabled)
{
    std::vector<std::string> materialNames;

    //pass for submeshes materials
    for (size_t i = 0; i < mshData.texturesCount; i++)
    {
        std::string textureName = mshData.textureNames[i];

        bool isTransparentMaterial = false;

        if(isOverrideDefault)
        {
            if(textureName == defaultTextureName)
            {
                isTransparentMaterial = true;
            }
        }

        std::string materialName = "Basewhite";

        if(isTransparentMaterial)
        {
            materialName = "Test/Transparent";
        }
        else if(mshData.isDecalTexture[i])
        {
            std::vector<Ogre::String> texturesSubMat;

            texturesSubMat.push_back(textureName);

            materialName = mNameGenMaterials.generate();

            if(!isFogEnabled)
            {
                Ogre::MaterialPtr newMat = CloneMaterial(  materialName, 
                            "Test/Diffuse", 
                            texturesSubMat, 
                            1.0f,
                            TEMP_RESOURCE_GROUP_NAME);
            }
            else
            {
                Ogre::MaterialPtr newMat = CloneMaterial(  materialName, 
                            "Test/DiffuseFog", 
                            texturesSubMat, 
                            1.0f,
                            TEMP_RESOURCE_GROUP_NAME);

                //newMat->setFog(true, Ogre::FOG_LINEAR, skyColor, 0.0f, fogStartEnd.x, fogStartEnd.y);
            }
        }
        else
        {
            std::vector<Ogre::String> texturesSubMat;

            texturesSubMat.push_back(textureName);

            materialName = mNameGenMaterials.generate();

            Ogre::MaterialPtr newMat = CloneMaterial(  materialName, 
                ovverideMaterialName, 
                texturesSubMat, 
                1.0f,
                TEMP_RESOURCE_GROUP_NAME);

            newMat->setAmbient(ambient);

            //if(isFogEnabled)
            //{
                //newMat->setFog(true, Ogre::FOG_LINEAR, skyColor, 0.0f, fogStartEnd.x, fogStartEnd.y);
            //}
        }

        materialNames.push_back(materialName);
    }

    for(size_t q = 0; q < mshData.texturesCount; ++q)
    {
        std::vector<unsigned short> triPlainIndixes;

        for(size_t w = 0; w < mshData.triCount; ++w)
        {
            if(mshData.textureForTriangleIndex[w] == q)
            {
                triPlainIndixes.push_back(w * 3 + 0);
                triPlainIndixes.push_back(w * 3 + 1);
                triPlainIndixes.push_back(w * 3 + 2);
            }
        }

        mshData.submeshesTriangleIndixesDiffuse.push_back(triPlainIndixes);
    }

    return materialNames;
}

void StaticMeshProcesser::queryLights()
{
    //d.polubotko: make sure light lists created during loading
    for(size_t q = 0; q < mTerrainNodes.size(); ++q)
    {
        mTerrainNodes[q]->queryLights();
    }
}

void StaticMeshProcesser::loadTerrainMaps(GameState& gameState)
{
    typedef std::set<std::string> maps;
    for(maps::iterator i = mTerrainMapsNames.begin(), j = mTerrainMapsNames.end();
        i !=j; ++i)
    {
        std::string mapName = (*i);

        Ogre::DataStreamPtr fileToLoad = gameState.getPFLoaderData().getFile("data/tracks/" + gameState.getSTRPowerslide().getBaseDir(gameState.getTrackName()) +"/terrains", mapName);

        if(fileToLoad.get() && fileToLoad->isReadable())
        {
            CommonIncludes::shared_ptr<Ogre::Image> img = CommonIncludes::shared_ptr<Ogre::Image>(new Ogre::Image());
            bool isLoaded = TRALoader().load(fileToLoad, img);
            if(isLoaded)
            {
                //http://www.ogre3d.org/tikiwiki/Loading+Image+from+Disk
                //Ogre::DataStreamPtr data_stream(new Ogre::FileStreamDataStream(texture_path, &ifs, false));
                //Ogre::DataStreamPtr pStream = Ogre::ResourceGroupManager::getSingleton().openResource( mapName, TEMP_RESOURCE_GROUP_NAME );
                //img->load(mapName, TEMP_RESOURCE_GROUP_NAME);

                mTerrainMaps.insert(std::make_pair(mapName, img));
            }

            fileToLoad->close();
        }
    }
}

void StaticMeshProcesser::setTerrainData(const std::vector<TerrainData>& terrainData)
{
    assert(terrainData.size() == TerrainData::mTerrainsAmount);

    mTerrainData = terrainData;
}

const TerrainData& StaticMeshProcesser:: getTerrainData(size_t index) const
{
    assert(index < mTerrainData.size());

    return mTerrainData[index];
}

void StaticMeshProcesser::performCollisionDetection(const Ogre::Vector3& pos, const Ogre::Vector3& coreBaseGlobal, Ogre::Real collisionDistance)
{
    Ogre::Vector3 posL = pos;
    posL.z = -posL.z;//original data is left hand

    Ogre::Vector3 coreBaseGlobalL = coreBaseGlobal;
    coreBaseGlobalL.z = -coreBaseGlobalL.z;//original data is left hand

    mCollisionDetection.performCollisionDetection(posL, coreBaseGlobalL, collisionDistance);
}

bool StaticMeshProcesser::collideSphere(const Ogre::Vector3& spherePos, Ogre::Real radius, Ogre::Real tol, 
                                        const Ogre::Vector3& averagedPos, Ogre::Real averageLen,
                                        size_t& foundIndex, Ogre::Real& minDist) const
{
    return mCollisionDetection.collideSphere(spherePos, radius, tol, 
        averagedPos, averageLen, 
        foundIndex, minDist);
}

const FoundCollision& StaticMeshProcesser::getCollision(size_t index) const
{
    return mCollisionDetection.getCollision(index);
}

const std::vector<size_t>& StaticMeshProcesser::getArrayOfCollisions() const
{
    return mCollisionDetection.getArrayOfCollisions();
}

bool StaticMeshProcesser::performPointCollisionDetection(const Ogre::Ray& ray,
        Ogre::Vector3& collisionPoint,
        short& partIndex, short& triangleIndex)
{
    return mCollisionDetection.performPointCollisionDetection(ray, collisionPoint, partIndex, triangleIndex);
}

void StaticMeshProcesser::getGeoverts(short partIndex, short triangleIndex, Ogre::Vector3& pA, Ogre::Vector3& pC, Ogre::Vector3& pB) const
{
    mCollisionDetection.getGeoverts(partIndex, triangleIndex, pA, pC, pB);
}

void StaticMeshProcesser::getGeoverts(const FoundCollision& collision, Ogre::Vector3& pA, Ogre::Vector3& pC, Ogre::Vector3& pB) const
{
    mCollisionDetection.getGeoverts(collision, pA, pC, pB);
}

void StaticMeshProcesser::getGeovertsTexture(const FoundCollision& collision, Ogre::Vector2& pA, Ogre::Vector2& pC, Ogre::Vector2& pB) const
{
    mCollisionDetection.getGeovertsTexture(collision, pA, pC, pB);
}

const std::string& StaticMeshProcesser::getTerrainName(const FoundCollision& collision) const
{
    return mCollisionDetection.getTerrainName(collision);
}

const Ogre::Image * StaticMeshProcesser::getTerrainMap(const std::string& terrainMapName) const
{
    const Ogre::Image * ret = NULL;
    if(terrainMapName != "")
    {
        std::map<std::string, CommonIncludes::shared_ptr<Ogre::Image> >::const_iterator im = mTerrainMaps.find(terrainMapName);
        CommonIncludes::shared_ptr<Ogre::Image> imagePtr = (*im).second;
        ret = imagePtr.get();
    }
    return ret;
}

char StaticMeshProcesser::getTerrainType(const Ogre::Image * terrainMap, Ogre::Vector2 texCoord) const
{
    char ret = -1;

    if(terrainMap != NULL)
    {
        float pixel_xf = (terrainMap->getWidth() - 1.0f) * texCoord.x;
        float pixel_yf = (terrainMap->getHeight() - 1.0f) * texCoord.y;

        double integer;
        float frac = static_cast<float>(modf(pixel_xf, &integer));

        size_t pixel_x = static_cast<size_t>(frac > 0.5f ? pixel_xf + 0.5f : pixel_xf);

        frac = static_cast<float>(modf(pixel_yf, &integer));
        size_t pixel_y = static_cast<size_t>(frac > 0.5f ? pixel_yf + 0.5f : pixel_yf);

        Ogre::ColourValue color = terrainMap->getColourAt(pixel_x, pixel_y, 0);

        ret = static_cast<char>(color.r * 255.0f);

        /*
        if(pixel_x & 1)//odd
            ret &= 0xF;
        else
            ret >>= 4;
        */

        ret = Ogre::Math::Clamp<char>(ret, 0, 15);
    }

    return ret;
}