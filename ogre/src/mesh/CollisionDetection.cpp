
#include "CollisionDetection.h"

void CollisionDetection::init(const DE2::DE2_File& de2File)
{
    mCollisionGobal = de2File.CollisionInfo_Global;
    mCollisionParts = de2File.CollisionInfo_Parts;
    mDataParts = de2File.Data_Parts;
    mDataVertexes = de2File.Data_Vertexes;
}

void CollisionDetection::performCollisionDetection(const Ogre::Vector3& pos, const Ogre::Vector3& coreBaseGlobal, Ogre::Real collisionDistance)
{
    mFoundCollisions.clear();
    mFoundCollisions.reserve(20);

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

    for(size_t q = 0; q < mFoundCollisions.size(); ++q)
    {
        Ogre::Vector3 pointA = convert(mDataVertexes[mDataParts[mFoundCollisions[q].mPartIndex].Data_Triangles[mFoundCollisions[q].mTriangleIndex].v0]);
        Ogre::Vector3 someVector = (coreBaseGlobal - pointA);
        mFoundCollisions[q].mIsFarFromCoreBase = (someVector.dotProduct(mFoundCollisions[q].mNormal)) <= 0.0f;
    }
}

bool CollisionDetection::collideSphere(const Ogre::Vector3& spherePos, Ogre::Real radius, Ogre::Real tol,
                                       const Ogre::Vector3& averagedPos, Ogre::Real averageLen,
                                       size_t& foundIndex,
                                       Ogre::Real& minDist) const
{
    bool isFound = false;

    mFoundCollisionsSpheres.clear();
    mFoundCollisionsSpheres.reserve(10);

    minDist = tol;

    for(size_t q = 0; q < mFoundCollisions.size(); ++q)
    {
        if(!mFoundCollisions[q].mIsFarFromCoreBase && !checkAverage(averagedPos, averageLen, q))
        {
            Ogre::Real res;
            if(minDist > 0.0f)
                res = collide(spherePos, radius, q, tol);
            else
                res = collide(spherePos, radius, q);

            if(res < 0.0f)
            {
                mFoundCollisions[q].mDistance = res;
                mFoundCollisionsSpheres.push_back(q);
            }

            if(absLess(res, minDist))
            {
                minDist = res;
                isFound = true;
                foundIndex = q;
            }

        }
    }

    return isFound;
}

const FoundCollision& CollisionDetection::getCollision(size_t index) const
{
    assert(index < mFoundCollisions.size());

    return mFoundCollisions[index];
}

const std::vector<size_t>& CollisionDetection::getArrayOfCollisions() const
{
    return mFoundCollisionsSpheres;
}

void CollisionDetection::getGeoverts(const FoundCollision& collision, Ogre::Vector3& pA, Ogre::Vector3& pC, Ogre::Vector3& pB) const
{
    pA = convert(mDataVertexes[mDataParts[collision.mPartIndex].Data_Triangles[collision.mTriangleIndex].v0]);
    pC = convert(mDataVertexes[mDataParts[collision.mPartIndex].Data_Triangles[collision.mTriangleIndex].v1]);
    pB = convert(mDataVertexes[mDataParts[collision.mPartIndex].Data_Triangles[collision.mTriangleIndex].v2]);
}

bool CollisionDetection::checkAverage(const Ogre::Vector3& averagedPos, Ogre::Real averageLen, size_t batchNum) const
{
    bool ret = false;

    Ogre::Vector3 maxDiff = convert(mFoundCollisions[batchNum].mCollision->aabb.max) - averagedPos;
    Ogre::Vector3 minDiff = averagedPos - convert(mFoundCollisions[batchNum].mCollision->aabb.min);

    maxDiff += averageLen;
    minDiff += averageLen;

    ret =   minDiff.x < 0.0f ||
            minDiff.y < 0.0f ||
            minDiff.z < 0.0f ||
            maxDiff.x < 0.0f ||
            maxDiff.y < 0.0f ||
            maxDiff.z < 0.0f;

    return ret;
}

Ogre::Real CollisionDetection::collide(const Ogre::Vector3& spherePos, Ogre::Real radius, size_t batchNum, Ogre::Real tol) const
{
    //similar?
    //http://realtimecollisiondetection.net/blog/?p=103
    Ogre::Real res = 1.0e20f;

    Ogre::Vector3 pointA = convert(mDataVertexes[mDataParts[mFoundCollisions[batchNum].mPartIndex].Data_Triangles[mFoundCollisions[batchNum].mTriangleIndex].v0]);
    Ogre::Vector3 pointC = convert(mDataVertexes[mDataParts[mFoundCollisions[batchNum].mPartIndex].Data_Triangles[mFoundCollisions[batchNum].mTriangleIndex].v1]);
    Ogre::Vector3 pointB = convert(mDataVertexes[mDataParts[mFoundCollisions[batchNum].mPartIndex].Data_Triangles[mFoundCollisions[batchNum].mTriangleIndex].v2]);

    Ogre::Vector3 diffA = spherePos - pointA;
    Ogre::Real dotDiffA_N = diffA.dotProduct(mFoundCollisions[batchNum].mNormal);
    if(dotDiffA_N <= radius)
    {
        Ogre::Vector3 absNorm = mFoundCollisions[batchNum].mNormal;
        absNorm.x = Ogre::Math::Abs(absNorm.x);
        absNorm.y = Ogre::Math::Abs(absNorm.y);
        absNorm.z = Ogre::Math::Abs(absNorm.z);

        Ogre::Vector3 projNorm = dotDiffA_N * mFoundCollisions[batchNum].mNormal;
        Ogre::Vector3 diffProjNorm = spherePos - projNorm;

        Ogre::Real param_1, param_11, param_111, param_1111;
        Ogre::Real param_2, param_22, param_222, param_2222;
        Ogre::Real param_3, param_33, param_33_1, param_333, param_3333;

        if(absNorm.x > absNorm.y && absNorm.x > absNorm.z)//absNorm.x is maximum
        {
            if(mFoundCollisions[batchNum].mNormal.x >= 0.0f)
            {
                param_1 = diffProjNorm.y - pointB.y;
                param_11 = pointC.z - pointB.z;
                param_111 = diffProjNorm.z - pointB.z;
                param_1111 = pointC.y - pointB.y;

                param_2 = diffProjNorm.z - pointC.z;
                param_222 = diffProjNorm.y - pointC.y;
                param_2222 = pointA.z - pointC.z;

                param_3 = diffProjNorm.y - pointA.y;
                param_33 = diffProjNorm.z - pointA.z;
                param_333 = pointA.y - pointC.y;
                param_33_1 = pointB.z - pointA.z;
                param_3333 = pointB.y - pointA.y;
            }
            else
            {
                param_1 = diffProjNorm.z - pointB.z;
                param_11 = pointC.y - pointB.y;
                param_111 = diffProjNorm.y - pointB.y;
                param_1111 = pointC.z - pointB.z;

                param_2 = diffProjNorm.y - pointC.y;
                param_222 = diffProjNorm.z - pointC.z;
                param_2222 = pointA.y - pointC.y;

                param_3 = diffProjNorm.z - pointA.z;
                param_33 = diffProjNorm.y - pointA.y;
                param_333 = pointA.z - pointC.z;
                param_33_1 = pointB.y - pointA.y;
                param_3333 = pointB.z - pointA.z;
            }
        }
        else
        {
            if(absNorm.y <= absNorm.z)//absNorm.z is maximum
            {
                if(mFoundCollisions[batchNum].mNormal.z <= 0.0f)
                {
                    param_1 = diffProjNorm.y - pointB.y;
                    param_11 = pointC.x - pointB.x;
                    param_111 = diffProjNorm.x - pointB.x;
                    param_1111 = pointC.y - pointB.y;

                    param_2 = diffProjNorm.x - pointC.x;
                    param_222 = diffProjNorm.y - pointC.y;
                    param_2222 = pointA.x - pointC.x;

                    param_3 = diffProjNorm.y - pointA.y;
                    param_33 = diffProjNorm.x - pointA.x;
                    param_333 = pointA.y - pointC.y;
                    param_33_1 = pointB.x - pointA.x;
                    param_3333 = pointB.y - pointA.y;
                }
                else
                {
                    param_1 = diffProjNorm.x - pointB.x;
                    param_11 = pointC.y - pointB.y;
                    param_111 = diffProjNorm.y - pointB.y;
                    param_1111 = pointC.x - pointB.x;

                    param_2 = diffProjNorm.y - pointC.y;
                    param_222 = diffProjNorm.x - pointC.x;
                    param_2222 = pointA.y - pointC.y;

                    param_3 = diffProjNorm.x - pointA.x;
                    param_33 = diffProjNorm.y - pointA.y;
                    param_333 = pointA.x - pointC.x;
                    param_33_1 = pointB.y - pointA.y;
                    param_3333 = pointB.x - pointA.x;
                }
            }
            else//absNorm.y is maximum
            {
                if(mFoundCollisions[batchNum].mNormal.y <= 0.0f)
                {
                    param_1 = diffProjNorm.x - pointB.x;
                    param_11 = pointC.z - pointB.z;
                    param_111 = diffProjNorm.z - pointB.z;
                    param_1111 = pointC.x - pointB.x;

                    param_2 = diffProjNorm.z - pointC.z;
                    param_222 = diffProjNorm.x - pointC.x;
                    param_2222 = pointA.z - pointC.z;

                    param_3 = diffProjNorm.x - pointA.x;
                    param_33 = diffProjNorm.z - pointA.z;
                    param_333 = pointA.x - pointC.x;
                    param_33_1 = pointB.z - pointA.z;
                    param_3333 = pointB.x - pointA.x;
                }
                else
                {
                    param_1 = diffProjNorm.z - pointB.z;
                    param_11 = pointC.x - pointB.x;
                    param_111 = diffProjNorm.x - pointB.x;
                    param_1111 = pointC.z - pointB.z;

                    param_2 = diffProjNorm.x - pointC.x;
                    param_222 = diffProjNorm.z - pointC.z;
                    param_2222 = pointA.x - pointC.x;

                    param_3 = diffProjNorm.z - pointA.z;
                    param_33 = diffProjNorm.x - pointA.x;
                    param_333 = pointA.z - pointC.z;
                    param_33_1 = pointB.x - pointA.x;
                    param_3333 = pointB.z - pointA.z;
                }
            }
        }

        param_22 = -param_333;

        size_t param_A = 0, param_B = 0, param_C = 0;

        if((param_3 * param_33_1 + param_33 * -param_3333) <= 0.0f) param_A = 1;
        if((param_1 * param_11 + param_111 * -param_1111) <= 0.0f) param_B = 2;
        if((param_2 * param_22 + param_222 * param_2222) <= 0.0f) param_C = 4;

        size_t param = param_A | param_B | param_C;
        Ogre::Real dotP = 0.0f;
        Ogre::Real radiusSqr = radius * radius;
        bool calced = false;
        switch(param)
        {
        case 1 :
            dotP = diffDotP(diffProjNorm, pointC);
            calced = true;
            break;
        case 2 :
            dotP = diffDotP(diffProjNorm, pointA);
            calced = true;
            break;
        case 3 :
            dotP = diffDotP(diffProjNorm, pointC, pointA);
            calced = true;
            break;
        case 4 :
            dotP = diffDotP(diffProjNorm, pointB);
            calced = true;
            break;
        case 5 :
            dotP = diffDotP(diffProjNorm, pointB, pointC);
            calced = true;
            break;
        case 6 :
            dotP = diffDotP(diffProjNorm, pointA, pointB);
            calced = true;
            break;
        case 7 :
            res = dotDiffA_N - radius;
            break;
        }

        if(calced)
        {
            if(dotP < radiusSqr) res = dotDiffA_N - Ogre::Math::Sqrt(radiusSqr - dotP);
        }

    }

    return res;
}

bool CollisionDetection::absLess(Ogre::Real found, Ogre::Real tol) const
{
    bool res = false;

    if(tol >= 0.0f)
    {
        if(found < tol)
            res = true;
    }
    else if(found < 0.0f && found > tol)
    {
        res = true;
    }

    return res;
}

Ogre::Real CollisionDetection::diffDotP(const Ogre::Vector3& diffProjNorm, const Ogre::Vector3& point) const
{
    Ogre::Vector3 diff = diffProjNorm - point;

    return diff.dotProduct(diff);
}

Ogre::Real CollisionDetection::diffDotP(const Ogre::Vector3& diffProjNorm, const Ogre::Vector3& point, const Ogre::Vector3& point2) const
{
    Ogre::Vector3 diff = diffProjNorm - point;
    Ogre::Vector3 diffP = point2 - point;

    Ogre::Real dotPoints = diff.dotProduct(diffP);

    return diff.dotProduct(diff) - dotPoints * dotPoints / diffP.dotProduct(diffP);
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
            FoundCollision foundCollision;
            foundCollision.mCollision = &leaf;
            foundCollision.mPartIndex = partIndex;
            foundCollision.mTriangleIndex = leaf.triIndex;

            short triangleIndex = leaf.triIndex;
            Ogre::Vector3 pointA = convert(mDataVertexes[mDataParts[partIndex].Data_Triangles[triangleIndex].v0]);
            Ogre::Vector3 pointB = convert(mDataVertexes[mDataParts[partIndex].Data_Triangles[triangleIndex].v1]);
            Ogre::Vector3 pointC = convert(mDataVertexes[mDataParts[partIndex].Data_Triangles[triangleIndex].v2]);
            foundCollision.mNormal = Ogre::Vector3(pointC - pointA).crossProduct(Ogre::Vector3(pointB - pointA));
            foundCollision.mNormal.normalise();

            mFoundCollisions.push_back(foundCollision);
        }
    }
}