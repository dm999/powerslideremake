
#ifndef COLLISIONDETECTION_H
#define COLLISIONDETECTION_H

#include "../includes/OgreInclude.h"
#include "../includes/CommonIncludes.h"

#include "../loaders/DE2.h"

struct FoundCollision
{
    const DE2::DE2_CollisionInfo* mCollision;
    Ogre::Vector3 mNormal;
    short mPartIndex;
    short mTriangleIndex;
    bool mIsFarFromCoreBase;
};

class CollisionDetection
{
public:

    void init(const DE2::DE2_File& de2File);

    void performCollisionDetection(const Ogre::Vector3& pos, const Ogre::Vector3& coreBaseGlobal, Ogre::Real collisionDistance);
    bool collideSphere(const Ogre::Vector3& spherePos, Ogre::Real radius, Ogre::Real tol, 
        const Ogre::Vector3& averagedPos, Ogre::Real averageLen) const;

private:

    void broadSearch(const DE2::DE2_CollisionInfo& subTree, const Ogre::Vector3& pos);
    void narrowSearch(const DE2::DE2_CollisionInfo& leaf, const Ogre::Vector3& pos, short partIndex);

    Ogre::Real collide(const Ogre::Vector3& spherePos, Ogre::Real radius, size_t batchNum, Ogre::Real tol = 0.0f) const;
    bool absLess(Ogre::Real found, Ogre::Real tol) const;
    Ogre::Real diffDotP(const Ogre::Vector3& diffProjNorm, const Ogre::Vector3& point) const;
    Ogre::Real diffDotP(const Ogre::Vector3& diffProjNorm, const Ogre::Vector3& point, const Ogre::Vector3& point2) const;
    bool checkAverage(const Ogre::Vector3& averagedPos, Ogre::Real averageLen, size_t batchNum) const;

    Ogre::Vector3 convert(const DE2::DE2_Vertex& val) const;
    Ogre::Vector3 getOnlyNegative(const Ogre::Vector3& a, const Ogre::Vector3& b) const;

    std::vector<DE2::DE2_CollisionInfo> mCollisionParts;
    std::vector<DE2::DE2_CollisionInfo> mCollisionGobal;
    std::vector<DE2::DE2_Part> mDataParts;//amount = parts
    std::vector<DE2::DE2_Vertex> mDataVertexes;

    std::vector<FoundCollision> mFoundCollisions;
    mutable std::vector<size_t> mFoundCollisionsSpheres; // index in mFoundCollisions

    Ogre::Real mMaxDistanceSqr;
};

#endif