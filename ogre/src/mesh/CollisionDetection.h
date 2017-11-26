
#ifndef COLLISIONDETECTION_H
#define COLLISIONDETECTION_H

#include "../includes/OgreInclude.h"
#include "../includes/CommonIncludes.h"

#include "../loaders/DE2.h"

class CollisionDetection
{
public:

    void init(const std::vector<DE2::DE2_CollisionInfo>& collisionTree,
        const std::vector<DE2::DE2_CollisionInfo>& collisionParts);

    void performBroadCollisionDetection(const Ogre::Vector3& pos, Ogre::Real collisionDistance);

private:

    void checkSubTree(const DE2::DE2_CollisionInfo& subTree);
    void checkLeaf(const DE2::DE2_CollisionInfo& leaf);

    Ogre::Vector3 convert(const DE2::DE2_Vertex& val) const;
    Ogre::Vector3 getOnlyNegative(const Ogre::Vector3& a, const Ogre::Vector3& b) const;

    std::vector<DE2::DE2_CollisionInfo> mCollisionParts;
    std::vector<DE2::DE2_CollisionInfo> mCollisionTree;
};

#endif