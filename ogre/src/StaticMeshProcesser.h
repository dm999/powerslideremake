
#ifndef STATICMESHPROCESSER_H
#define STATICMESHPROCESSER_H

#include "OgreInclude.h"
#include "OgreBulletInclude.h"

#include "CommonIncludes.h"

#include "MshData.h"

#include "BulletCollision/NarrowPhaseCollision/btVoronoiSimplexSolver.h"

#include "GameState.h"
#include "loaders/DE2.h"
#include "loaders/PFLoader.h"

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

class StaticMeshProcesser
{
public :

    StaticMeshProcesser();

    void initParts( lua_State * pipeline, 
                    Ogre::SceneManager* sceneMgr, 
                    Ogre::SceneNode* mainNode,
                    bool isGlobalReset,
                    GameState& gameState, 
                    const PFLoader& pfloader,
                    OgreBulletDynamics::DynamicsWorld * world,
                    const Ogre::ColourValue& ambient);

    void queryLights();

    bool isRigidBodyStatic(const btCollisionObject * object, std::pair<int, int>& address) const;

    void deinit() {mParts.clear(); mBodies.clear(); mTerrainMaps.clear(); mTerrainMapsNames.clear(); mPlainIndices = 0;}

    void checkIsVertexArraySupported();

    void loadTerrainMaps(PFLoader& pfloader, GameState& gameState);

    const DE2SingleBatch& getBatchByAddress(std::pair<int, int> address)const {return mParts[address.first].mBatches[address.second];}
    const DE2Part& getPartAddress(std::pair<int, int> address)const {return mParts[address.first];}

    unsigned char getTerrainType(std::pair<int, int> address, int triIndex, const btVector3& ptB);

    Ogre::Real getLatitudeFriction(unsigned char terrainType);
    Ogre::Real getLongtitudeFriction(unsigned char terrainType);

    void setFrictionRemapArray(const std::vector<size_t>& remap);
    void setLatutuideFrictionArray(const std::vector<float>& frictions);
    void setLongtitudeFrictionArray(const std::vector<float>& frictions);

private:

    void loadTextures(const std::vector<MSHData>& mergedMSH, const PFLoader& pfloader, const std::string& trackName);

    void createLights(lua_State * pipeline, Ogre::SceneManager* sceneMgr, const DE2::DE2_File& de2, GameState& gameState);

    void addStaticTrimesh(   OgreBulletDynamics::DynamicsWorld * world,
                                                        const Ogre::Vector3& offset,
                                                        const Ogre::Real bodyRestitution, 
                                                        const Ogre::Real bodyFriction,
                                                        const Ogre::Real bodyRollingFriction,
                                                        const MSHData& mshData);

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

    /**
     * return - return in range [0, 1]
    */
    Ogre::Vector2 getTextureCoordinateInTriangle(std::pair<int, int> address, int triIndex, const btVector3& ptB);

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
                                const Ogre::ColourValue& ambient);

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
                                                        const Ogre::ColourValue& ambient);

    typedef std::map<const btCollisionObject *, std::pair<int, int> > bodies;
    bodies mBodies;

    static Ogre::NameGenerator nameGenRigidBodies;

    size_t mPlainIndices;

    std::vector<DE2Part> mParts;
    std::set<std::string> mTerrainMapsNames;
    std::map<std::string, CommonIncludes::shared_ptr<Ogre::Image> > mTerrainMaps;

    void prepareBuffers(const MSHData& mshData);

    OgreBulletCollisions::TriangleMeshCollisionShape * createTrimesh(DE2Part& part, DE2SingleBatch& batch);

    btVoronoiSimplexSolver mSimplexSolver; // to find barycentric

    Ogre::Vector3 getBarycentric(std::pair<int, int> address, int triIndex, const btVector3& ptB);

    std::vector<size_t> mFrictionRemap;
    std::vector<float> mLatitudeFrictions;
    std::vector<float> mLongtitudeFrictions;

    bool mIsVertexArraySupported;

};

#endif