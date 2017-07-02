
#include <map>

#include "ModelsPool.h"

#include "../includes/CommonIncludes.h"
#include "../tools/Conversions.h"

#include "../tools/OgreTools.h"

#include "../loaders/DE2Loader.h"


#include "StaticMeshProcesser.h"

Ogre::NameGenerator ModelsPool::nameGenMeshes("Scene/MeshesVehicles/Name");

ModelsPool::ModelsPool()
{
}

void ModelsPool::initModels(Ogre::SceneManager* sceneMgr, const GameState& gameState)
{
    mSceneMgr = sceneMgr;

    mVehicles.clear();
    //vehicles will load on request

    loadArrow(gameState.getPFLoaderData());
}

void ModelsPool::loadArrow(const PFLoader& pfloader)
{
    Ogre::DataStreamPtr fileToLoad = pfloader.getFile("data", "arrow.de2");
    if(fileToLoad.get() && fileToLoad->isReadable())
    {
        std::vector<MSHData> arrowParts;
        bool loadResult = DE2Loader().load(arrowParts, fileToLoad);
        if(loadResult)
        {

            std::vector<Ogre::String> texName;
            texName.push_back("OriginalArrow");
            Ogre::MaterialPtr newMat = CloneMaterial(  "Test/ArrowMaterial", 
                                "Test/Diffuse", 
                                texName, 
                                1.0f,
                                TEMP_RESOURCE_GROUP_NAME);

            assert(arrowParts.size() == 1);

            mArrowModel = createEntityFromMSH(arrowParts[0], "Test/ArrowMaterial", "ArrowBasicModel");

            //newMat->getTechnique(0)->getPass(0)->setDepthCheckEnabled(false);
            //mArrowModel->setRenderQueueGroup( Ogre::RENDER_QUEUE_MAX );
            mArrowModel->setMaterialName("Test/ArrowMaterial");
            mArrowModel->setCastShadows(false);
        }
        fileToLoad->close();
    }
    else {assert(false && "No arrow DE2 file");}
}

void ModelsPool::loadVehicle(const PFLoader& pfloader, vehicleModel& vehicle, const std::string& path, const std::string& fileName)
{
    Ogre::DataStreamPtr fileToLoad = pfloader.getFile(path, fileName);
    if(fileToLoad.get() && fileToLoad->isReadable())
    {
        std::vector<MSHData> vehicleParts;
        bool loadResult = DE2Loader().load(vehicleParts, fileToLoad);
        if(loadResult)
        {
            assert(vehicleParts.size() == 5);
            for(size_t q = 0; q < vehicleParts.size(); ++q)
            {
                //revert order to make cockpit 0
                vehicle.mVehicleModel[4 - q] = createEntityFromMSH(vehicleParts[q], "Test/Diffuse", fileName + Conversions::DMToString(q));
                AddjustNormals(vehicle.mVehicleModel[4 - q], Ogre::Math::Cos(Ogre::Degree(80.0f).valueRadians()));
            }
        }
        fileToLoad->close();
    }
    else {assert(false && "No vehicle DE2 file");}
}

Ogre::Entity * ModelsPool::createEntityFromMSH(MSHData& source, const std::string& subMaterial, const std::string& entityName) const
{
    Ogre::Entity * ret;


    Ogre::MeshPtr pMesh = Ogre::MeshManager::getSingleton().createManual(entityName, TEMP_RESOURCE_GROUP_NAME);

    Ogre::AxisAlignedBox aabb;

    Ogre::Vector3 min, max;
    source.getMinMax(min, max, Ogre::Vector3::ZERO);
    aabb.merge(min);
    aabb.merge(max);

    source.makePlain(Ogre::Vector3::ZERO);

    pMesh->sharedVertexData = new Ogre::VertexData();
    pMesh->sharedVertexData->vertexCount = source.triCount * 3;
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

    vbuf->writeData(0, vbuf->getSizeInBytes(), &source.plainBuffer[0], true);

    Ogre::VertexBufferBinding* bind = pMesh->sharedVertexData->vertexBufferBinding; 
    bind->setBinding(0, vbuf);

    Ogre::SubMesh* sub = pMesh->createSubMesh();

    sub->setMaterialName(subMaterial);

    std::vector<unsigned short> triPlainIndixes;

    for(size_t w = 0; w < source.triCount; ++w)
    {
        triPlainIndixes.push_back(w * 3 + 0);
        triPlainIndixes.push_back(w * 3 + 1);
        triPlainIndixes.push_back(w * 3 + 2);
    }

    Ogre::HardwareIndexBufferSharedPtr ibuf = Ogre::HardwareBufferManager::getSingleton().
    createIndexBuffer(
    Ogre::HardwareIndexBuffer::IT_16BIT, 
    triPlainIndixes.size(), 
    Ogre::HardwareBuffer::HBU_STATIC);

    /// Upload the index data to the card
    ibuf->writeData(0, ibuf->getSizeInBytes(), &triPlainIndixes[0], true);

    sub->useSharedVertices = true;
    sub->indexData->indexBuffer = ibuf;
    sub->indexData->indexCount = triPlainIndixes.size();
    sub->indexData->indexStart = 0;


    pMesh->_setBounds(aabb, false);
    pMesh->_setBoundingSphereRadius(Ogre::Vector3(aabb.getMaximum() - aabb.getMinimum()).length() / 2.0f);
    pMesh->load();

    ret = mSceneMgr->createEntity(entityName, pMesh);

    return ret;
}

void ModelsPool::getCopyOfVehicle(const GameState& gameState, const std::string& vehicleName, Ogre::Entity* vehicleModel[5])
{
    std::map<std::string, ModelsPool::vehicleModel>::iterator found = mVehicles.find(vehicleName);

    ModelsPool::vehicleModel model;

    if(found != mVehicles.end())
    {
        model = (*found).second;
    }
    else
    {
        std::string de2Path = gameState.getSTRPowerslide().getValue(vehicleName + " parameters", "base directory", "feral max");
        std::set<char> delim;
        delim.insert('\\');
        std::vector<std::string> pathComponents = Tools::splitpath(de2Path, delim, false);
        std::string de2Name = gameState.getSTRPowerslide().getValue(vehicleName + " parameters", "de2 filename", "car.de2");
        ModelsPool::vehicleModel loadedModel;
        loadVehicle(gameState.getPFLoaderData(), loadedModel, "data/cars/" + pathComponents[pathComponents.size() - 1], de2Name);
        mVehicles.insert(std::make_pair(vehicleName, loadedModel));
        model = loadedModel;
    }

    for(size_t q = 0; q < 5; ++q)
    {
        Ogre::MeshPtr msh = model.mVehicleModel[q]->getMesh();
        Ogre::String newMeshName = nameGenMeshes.generate();
        Ogre::MeshPtr cloned = msh->clone(newMeshName, TEMP_RESOURCE_GROUP_NAME);
        vehicleModel[q] = mSceneMgr->createEntity(newMeshName, newMeshName, TEMP_RESOURCE_GROUP_NAME);
    }
}