
#ifndef COLLISIONDETECTION_H
#define COLLISIONDETECTION_H

#include "../includes/OgreInclude.h"
#include "../includes/CommonIncludes.h"

#include "../loaders/DE2.h"

struct FoundCollision
{
    const DE2::DE2_CollisionInfo* mCollision;
    Ogre::Vector3 mNormal;
    Ogre::Real mDistance;
    short mPartIndex;
    short mTriangleIndex;
    bool mIsFarFromCoreBase;
};

/*all calculations in left hand - like original data*/
class CollisionDetection
{
public:

    void init(const DE2::DE2_File& de2File);

    //for spheres
    void performCollisionDetection(const Ogre::Vector3& pos, const Ogre::Vector3& coreBaseGlobal, Ogre::Real collisionDistance);
    bool collideSphere(const Ogre::Vector3& spherePos, Ogre::Real radius, Ogre::Real tol, 
        const Ogre::Vector3& averagedPos, Ogre::Real averageLen, 
        size_t& foundIndex, Ogre::Real& minDist) const;

    const FoundCollision& getCollision(size_t index) const;
    const std::vector<size_t>& getArrayOfCollisions() const;

    //for point
    bool performPointCollisionDetection(const Ogre::Ray& ray,
        Ogre::Vector3& collisionPoint,
        short& partIndex, short& triangleIndex);

    //getters
    void getGeoverts(short partIndex, short triangleIndex, Ogre::Vector3& pA, Ogre::Vector3& pC, Ogre::Vector3& pB) const;
    void getGeoverts(const FoundCollision& collision, Ogre::Vector3& pA, Ogre::Vector3& pC, Ogre::Vector3& pB) const;
    void getGeovertsTexture(const FoundCollision& collision, Ogre::Vector2& tA, Ogre::Vector2& tC, Ogre::Vector2& tB) const;
    const std::string& getTerrainName(const FoundCollision& collision) const;

private:

    //for spheres
    void broadSearch(const DE2::DE2_CollisionInfo& subTree, const Ogre::Vector3& pos);
    void narrowSearch(const DE2::DE2_CollisionInfo& leaf, const Ogre::Vector3& pos, short partIndex);

    Ogre::Real collide(const Ogre::Vector3& spherePos, Ogre::Real radius, size_t batchNum, Ogre::Real tol = 0.0f) const;
    bool absLess(Ogre::Real found, Ogre::Real tol) const;
    Ogre::Real diffDotP(const Ogre::Vector3& diffProjNorm, const Ogre::Vector3& point) const;
    Ogre::Real diffDotP(const Ogre::Vector3& diffProjNorm, const Ogre::Vector3& point, const Ogre::Vector3& point2) const;
    bool checkAverage(const Ogre::Vector3& averagedPos, Ogre::Real averageLen, size_t batchNum) const;

    Ogre::Vector3 convert(const DE2::DE2_Vertex& val) const;
    Ogre::Vector2 convert(const DE2::DE2_TextureCoord& val) const;
    Ogre::Vector3 getOnlyNegative(const Ogre::Vector3& a, const Ogre::Vector3& b) const;

    std::vector<DE2::DE2_CollisionInfo> mCollisionParts;
    std::vector<DE2::DE2_CollisionInfo> mCollisionGobal;
    std::vector<DE2::DE2_Part> mDataParts;//amount = parts
    std::vector<DE2::DE2_Vertex> mDataVertexes;
    std::vector<Ogre::Vector3> mNormalsPerTri;
    std::vector<DE2::DE2_TextureCoord> mDataTextureCoord;
    std::vector<std::string> mDataTerrans;

    mutable std::vector<FoundCollision> mFoundCollisions;
    mutable std::vector<size_t> mFoundCollisionsSpheres; // index in mFoundCollisions

    Ogre::Real mMaxDistanceSqr;

    //for point
    bool checkPointCollision(const DE2::AABB& aabb, const Ogre::Ray& ray, const Ogre::Vector3& dirRecip) const;
    void pointBroadSearch(const DE2::DE2_CollisionInfo& subTree,
        const Ogre::Ray& ray, const Ogre::Vector3& dirRecip);
    void pointNarrowSearch(const DE2::DE2_CollisionInfo& leaf,
        const Ogre::Ray& ray, const Ogre::Vector3& dirRecip,
        short partIndex);
    void pointCollisionFinal(const Ogre::Ray& ray,
        short partIndex, short triangleIndex);

    bool mPointCollisionFound;
    short mPointFoundPartIndex;
    short mPointFoundTiangleIndex;
    Ogre::Real mPointDiffSqrLen;
    Ogre::Real mPointCollisionWeight;
    Ogre::Vector3 mCollisionPoint;
};

#endif