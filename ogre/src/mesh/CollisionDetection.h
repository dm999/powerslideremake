
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

    //for cam
    bool performCamCollisionDetection(const Ogre::Vector3& camValue, const Ogre::Vector3& camDiff,
        Ogre::Vector3& collisionPoint,
        short& partIndex, short& triangleIndex);

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
    std::vector<DE2::DE2_TextureCoord> mDataTextureCoord;
    std::vector<std::string> mDataTerrans;

    mutable std::vector<FoundCollision> mFoundCollisions;
    mutable std::vector<size_t> mFoundCollisionsSpheres; // index in mFoundCollisions

    Ogre::Real mMaxDistanceSqr;

    //for cam
    bool checkCamCollision(const DE2::AABB& aabb, const Ogre::Vector3& camValue, const Ogre::Vector3& camDiff, const Ogre::Vector3& camDiffRecip) const;
    void camBroadSearch(const DE2::DE2_CollisionInfo& subTree,
        const Ogre::Vector3& camValue, const Ogre::Vector3& camDiff, const Ogre::Vector3& camDiffRecip);
    void camNarrowSearch(const DE2::DE2_CollisionInfo& leaf,
        const Ogre::Vector3& camValue, const Ogre::Vector3& camDiff, const Ogre::Vector3& camDiffRecip,
        short partIndex);
    void camCollisionFinal(const Ogre::Vector3& camValue, const Ogre::Vector3& camDiff,
        short partIndex, short triangleIndex);

    bool mCamPointFound;
    short mCamFoundPartIndex;
    short mCamFoundTiangleIndex;
    Ogre::Real mCamDiffSqrLen;
    Ogre::Real mCamCollisionWeight;
    Ogre::Vector3 mCamCollisionPoint;
};

#endif