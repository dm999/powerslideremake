
#ifndef STATICMESHPROCESSER_H
#define STATICMESHPROCESSER_H

#include "../includes/OgreInclude.h"
#include "../includes/CommonIncludes.h"

#include "MshData.h"
#include "CollisionDetection.h"

#include "../GameState.h"
#include "../loaders/DE2.h"
#include "../loaders/PFLoader.h"

#include "../physics/TerrainData.h"

struct DE2SingleBatch
{
    std::vector<unsigned int> mIndexBuffer;

    std::string mTerrainMap;
};

struct DE2Part
{
    std::vector<DE2SingleBatch> mBatches;

    std::vector<Ogre::Vector3> mVertexBuffer;   //shared
    std::vector<Ogre::Vector3> mTexCoordsBuffer;//shared

    Ogre::Vector3 offset;
};

struct lua_State;
class LoaderListener;
class Physics;

class StaticMeshProcesser
{
public :

    StaticMeshProcesser();

    void initParts( lua_State * pipeline, 
                    Ogre::SceneManager* sceneMgr, 
                    Ogre::SceneNode* mainNode,
                    bool isGlobalReset,
                    GameState& gameState, 
                    Physics * world,
                    LoaderListener* loaderListener);

    void queryLights();

    void deinit() {mTerrainMaps.clear(); mTerrainMapsNames.clear(); mPlainIndices = 0;}

    void checkIsVertexArraySupported();

    void loadTerrainMaps(GameState& gameState);

    void setTerrainData(const std::vector<TerrainData>& terrainData);
    const TerrainData& getTerrainData(size_t index) const;

    //for spheres
    void performCollisionDetection(const Ogre::Vector3& pos, const Ogre::Vector3& coreBaseGlobal, Ogre::Real collisionDistance);
    bool collideSphere(const Ogre::Vector3& spherePos, Ogre::Real radius, Ogre::Real tol,
        const Ogre::Vector3& averagedPos, Ogre::Real averageLen,
        size_t& foundIndex, Ogre::Real& minDist) const;
    const FoundCollision& getCollision(size_t index) const;
    const std::vector<size_t>& getArrayOfCollisions() const;

    //for cam
    bool performPointCollisionDetection(const Ogre::Ray& ray,
        Ogre::Vector3& collisionPoint,
        short& partIndex, short& triangleIndex);

    void getGeoverts(short partIndex, short triangleIndex, Ogre::Vector3& pA, Ogre::Vector3& pC, Ogre::Vector3& pB) const;
    void getGeoverts(const FoundCollision& collision, Ogre::Vector3& pA, Ogre::Vector3& pC, Ogre::Vector3& pB) const;
    void getGeovertsTexture(const FoundCollision& collision, Ogre::Vector2& pA, Ogre::Vector2& pC, Ogre::Vector2& pB) const;
    const std::string& getTerrainName(const FoundCollision& collision) const;
    const Ogre::Image * getTerrainMap(const std::string& terrainMapName) const;
    char getTerrainType(const Ogre::Image * terrainMap, Ogre::Vector2 texCoord) const;

#if defined(__ANDROID__)
    void loadTextures(const PFLoader& pfloader, const std::string& trackName, LoaderListener* loaderListener);
#endif

private:

    void loadTextures(const std::vector<MSHData>& mergedMSH, const PFLoader& pfloader, const std::string& trackName, LoaderListener* loaderListener);
    void loadTextures(const std::set<std::string>& texturesNames, const PFLoader& pfloader, const std::string& trackName, LoaderListener* loaderListener);

#if defined(__ANDROID__)
    std::set<std::string> mTexturesNames;
#endif

    void createLights(lua_State * pipeline, Ogre::SceneManager* sceneMgr, const DE2::DE2_File& de2, GameState& gameState);

    void initPart(  lua_State * pipeline, 
                    Ogre::SceneManager* sceneMgr, 
                    Ogre::SceneNode* mainNode,
                    bool isGlobalReset,
                    GameState& gameState,
                    Ogre::Entity* terrain,
                    Ogre::SceneNode*& terrainNode,
                    const Ogre::Vector3& centroid);

    struct mergedInfo
    {
        size_t indexForBatchDecal;
        bool isDecalTextureHappened;
        size_t indexForBatchLit;
        bool isLitTextureHappened;

        mergedInfo( size_t IndexForBatchDecal, bool IsDecalTextureHappened, 
                    size_t IndexForBatchLit, bool IsLitTextureHappened)
        {
            indexForBatchDecal = IndexForBatchDecal;
            isDecalTextureHappened = IsDecalTextureHappened;
            indexForBatchLit = IndexForBatchLit;
            isLitTextureHappened = IsLitTextureHappened;
        }
    };
    /**
     * merge different original DE2 parts with same texture into single part
     * used to reduce batch count
    */
    void mergeMSH(const MSHData& mshData, std::map<std::string, mergedInfo>& mapTexturesToMSHIndex, std::vector<MSHData>& mergedMSH)const;

    std::vector<Ogre::Entity*> mTerrainNodes;
    Ogre::NameGenerator mNameGenMaterials;
    Ogre::NameGenerator mNameGenNodes;
    Ogre::NameGenerator mNameGenTextures;
    bool mIsMaskLight;

    Ogre::Entity* createMesh(   lua_State * pipeline, 
                                Ogre::SceneManager* sceneMgr, 
                                const Ogre::String& entityName, 
                                const Ogre::Vector3& centroid,
                                const Ogre::Vector3& min, 
                                const Ogre::Vector3& max,
                                MSHData& mshData,
                                const Ogre::ColourValue& skyColor,//as fog color
                                const Ogre::Vector2& fogStartEnd,
                                const Ogre::ColourValue& ambient,
                                bool isShadow);

    /**
     * return - material names
     */
    std::vector<std::string> loadWithVertexArray(   bool isOverrideDefault, 
                                                    std::string defaultTextureName, 
                                                    const Ogre::String& ovverideMaterialName,
                                                    const Ogre::String& ovverideMaterialArrayName,
                                                    MSHData& mshData,
                                                    const Ogre::ColourValue& ambient);

    /**
     * return - material names
     */
    std::vector<std::string> loadWithoutVertexArray(    bool isOverrideDefault, 
                                                        std::string defaultTextureName, 
                                                        const Ogre::String& ovverideMaterialName,
                                                        MSHData& mshData,
                                                        const Ogre::ColourValue& skyColor,//as fog color
                                                        const Ogre::Vector2& fogStartEnd,
                                                        const Ogre::ColourValue& ambient,
                                                        bool isFogEnabled);

    static Ogre::NameGenerator nameGenRigidBodies;

    size_t mPlainIndices;

    std::set<std::string> mTerrainMapsNames;
    std::map<std::string, CommonIncludes::shared_ptr<Ogre::Image> > mTerrainMaps;

    std::vector<TerrainData> mTerrainData;

    bool mIsVertexArraySupported;

    CollisionDetection mCollisionDetection;

};

#endif