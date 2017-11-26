
#include "CollisionDetection.h"

void CollisionDetection::init(const std::vector<DE2::DE2_CollisionInfo>& collisionTree,
                              const std::vector<DE2::DE2_CollisionInfo>& collisionParts)
{
    mCollisionTree = collisionTree;
    mCollisionParts = collisionParts;
}

void CollisionDetection::performBroadCollisionDetection(const Ogre::Vector3& pos, Ogre::Real collisionDistance)
{
    Ogre::Real distSqr = collisionDistance * collisionDistance;
    size_t maxSearchDepth = 1024;

    assert(mCollisionTree.size() == 1);
    assert(mCollisionTree[0].subparts.size() == 1);

    const DE2::DE2_CollisionInfo& root = mCollisionTree[0].subparts[0];
    if(root.subparts.size() > 0)
    {
        for(size_t q = 0; q < root.subparts.size(); ++q)
        {
            checkSubTree(root.subparts[q]);
        }
    }
    else
    {
        Ogre::Vector3 maxDiff = convert(root.aabb.max) - pos;
        Ogre::Vector3 minDiff = pos - convert(root.aabb.min);
        Ogre::Real sqrLen = getOnlyNegative(minDiff, maxDiff).squaredLength();
        if(sqrLen <= collisionDistance)
        {
            checkLeaf(root);
        }
    }
}

Ogre::Vector3 CollisionDetection::convert(const DE2::DE2_Vertex& val) const
{
    return Ogre::Vector3(val.x, val.y, val.z);
}

Ogre::Vector3 CollisionDetection::getOnlyNegative(const Ogre::Vector3& a, const Ogre::Vector3& b) const
{
    Ogre::Vector3 res = Ogre::Vector3::ZERO;
    if(a.x < 0.0f) res.x = a.x;
    if(b.x < 0.0f) res.x = b.x;
    if(a.y < 0.0f) res.y = a.y;
    if(b.y < 0.0f) res.y = b.y;
    if(a.z < 0.0f) res.y = a.z;
    if(b.z < 0.0f) res.y = b.z;
    return res;
}

void CollisionDetection::checkSubTree(const DE2::DE2_CollisionInfo& subTree)
{
}

void CollisionDetection::checkLeaf(const DE2::DE2_CollisionInfo& leaf)
{
}