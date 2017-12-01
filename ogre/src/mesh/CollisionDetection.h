
#ifndef COLLISIONDETECTION_H
#define COLLISIONDETECTION_H

#include "../includes/OgreInclude.h"
#include "../includes/CommonIncludes.h"

#include "../loaders/DE2.h"

class CollisionDetection
{
public:

    void init(const DE2::DE2_File& de2File);

    void performBroadCollisionDetection(const Ogre::Vector3& pos, Ogre::Real collisionDistance);

private:

    void broadSearch(const DE2::DE2_CollisionInfo& subTree, const Ogre::Vector3& pos);
    void narrowSearch(const DE2::DE2_CollisionInfo& leaf, const Ogre::Vector3& pos, short partIndex);

    Ogre::Vector3 convert(const DE2::DE2_Vertex& val) const;
    Ogre::Vector3 getOnlyNegative(const Ogre::Vector3& a, const Ogre::Vector3& b) const;

    std::vector<DE2::DE2_CollisionInfo> mCollisionParts;
    std::vector<DE2::DE2_CollisionInfo> mCollisionGobal;
    std::vector<DE2::DE2_Part> mDataParts;//amount = parts
    std::vector<DE2::DE2_Vertex> mDataVertexes;

    Ogre::Real mMaxDistanceSqr;
};

#endif