
#include "CollisionDetection.h"

void CollisionDetection::init(const DE2::DE2_File& de2File)
{
    mCollisionGobal = de2File.CollisionInfo_Global;
    mCollisionParts = de2File.CollisionInfo_Parts;
    mDataParts = de2File.Data_Parts;
    mDataVertexes = de2File.Data_Vertexes;
}

void CollisionDetection::performBroadCollisionDetection(const Ogre::Vector3& pos, Ogre::Real collisionDistance)
{
    mMaxDistanceSqr = collisionDistance * collisionDistance;
    size_t maxSearchDepth = 1024;

    assert(mCollisionGobal.size() == 1);
    assert(mCollisionGobal[0].subparts.size() == 1);

    const DE2::DE2_CollisionInfo& root = mCollisionGobal[0].subparts[0];
    if(root.subparts.size() > 0)
    {
        for(size_t q = 0; q < root.subparts.size(); ++q)
        {
            broadSearch(root.subparts[q], pos);
        }
    }
    else
    {
        Ogre::Vector3 maxDiff = convert(root.aabb.max) - pos;
        Ogre::Vector3 minDiff = pos - convert(root.aabb.min);
        Ogre::Real sqrLen = getOnlyNegative(minDiff, maxDiff).squaredLength();
        if(sqrLen <= collisionDistance)
        {
            assert(false);//shouldn`t be here
            narrowSearch(root, pos, 0);
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

void CollisionDetection::broadSearch(const DE2::DE2_CollisionInfo& subTree, const Ogre::Vector3& pos)
{
    Ogre::Vector3 maxDiff = convert(subTree.aabb.max) - pos;
    Ogre::Vector3 minDiff = pos - convert(subTree.aabb.min);
    Ogre::Real sqrLen = getOnlyNegative(minDiff, maxDiff).squaredLength();
    if(mMaxDistanceSqr >= sqrLen)
    {
        if(!subTree.subparts.empty())
        {
            for(size_t q = 0; q < subTree.subparts.size(); ++q)
            {
                broadSearch(subTree.subparts[q], pos);
            }
        }
        else
        {
            short partIndex = subTree.triIndex;
            if(partIndex != -1)
            {
                const DE2::DE2_CollisionInfo& leaf = mCollisionParts[partIndex];
                
                //some function here

                for(size_t q = 0; q < leaf.subparts.size(); ++q)
                {
                    narrowSearch(leaf.subparts[q], pos, partIndex);
                }
            }
        }
    }
}

void CollisionDetection::narrowSearch(const DE2::DE2_CollisionInfo& leaf, const Ogre::Vector3& pos, short partIndex)
{
    Ogre::Vector3 maxDiff = convert(leaf.aabb.max) - pos;
    Ogre::Vector3 minDiff = pos - convert(leaf.aabb.min);
    Ogre::Real sqrLen = getOnlyNegative(minDiff, maxDiff).squaredLength();
    if(mMaxDistanceSqr >= sqrLen)
    {
        if(!leaf.subparts.empty())
        {
            for(size_t q = 0; q < leaf.subparts.size(); ++q)
            {
                narrowSearch(leaf.subparts[q], pos, partIndex);
            }
        }
        else
        {
            short triangleIndex = leaf.triIndex;
            Ogre::Vector3 pointA = convert(mDataVertexes[mDataParts[partIndex].Data_Triangles[triangleIndex].v0]);
            Ogre::Vector3 pointB = convert(mDataVertexes[mDataParts[partIndex].Data_Triangles[triangleIndex].v1]);
            Ogre::Vector3 pointC = convert(mDataVertexes[mDataParts[partIndex].Data_Triangles[triangleIndex].v2]);
            pointA.z = -pointA.z;//original data is left hand
            pointB.z = -pointB.z;//original data is left hand
            pointC.z = -pointC.z;//original data is left hand
            Ogre::Vector3 norm = Ogre::Vector3(pointC - pointA).crossProduct(Ogre::Vector3(pointB - pointA));
            norm.normalise();
        }
    }
}