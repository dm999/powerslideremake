
#include "CollisionDetection.h"

void CollisionDetection::init(const DE2::DE2_File& de2File)
{
    mCollisionGobal = de2File.CollisionInfo_Global;
    mCollisionParts = de2File.CollisionInfo_Parts;
    mDataParts = de2File.Data_Parts;
    mDataVertexes = de2File.Data_Vertexes;
    mDataTextureCoord = de2File.Data_Texture_Coord;
    mDataTerrans = de2File.Data_TerranName;
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

void CollisionDetection::getGeoverts(short partIndex, short triangleIndex, Ogre::Vector3& pA, Ogre::Vector3& pC, Ogre::Vector3& pB) const
{
    pA = convert(mDataVertexes[mDataParts[partIndex].Data_Triangles[triangleIndex].v0]);
    pC = convert(mDataVertexes[mDataParts[partIndex].Data_Triangles[triangleIndex].v1]);
    pB = convert(mDataVertexes[mDataParts[partIndex].Data_Triangles[triangleIndex].v2]);
}

void CollisionDetection::getGeoverts(const FoundCollision& collision, Ogre::Vector3& pA, Ogre::Vector3& pC, Ogre::Vector3& pB) const
{
    getGeoverts(collision.mPartIndex, collision.mTriangleIndex, pA, pC, pB);
}

void CollisionDetection::getGeovertsTexture(const FoundCollision& collision, Ogre::Vector2& tA, Ogre::Vector2& tC, Ogre::Vector2& tB) const
{
    tA = convert(mDataTextureCoord[mDataParts[collision.mPartIndex].Data_Triangles[collision.mTriangleIndex].t0]);
    tC = convert(mDataTextureCoord[mDataParts[collision.mPartIndex].Data_Triangles[collision.mTriangleIndex].t1]);
    tB = convert(mDataTextureCoord[mDataParts[collision.mPartIndex].Data_Triangles[collision.mTriangleIndex].t2]);
}

const std::string& CollisionDetection::getTerrainName(const FoundCollision& collision) const
{
    return mDataTerrans[mDataParts[collision.mPartIndex].Data_Triangles[collision.mTriangleIndex].hz1];
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

Ogre::Vector2 CollisionDetection::convert(const DE2::DE2_TextureCoord& val) const
{
    return Ogre::Vector2(val.uv, val.uw);
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

bool CollisionDetection::performCamCollisionDetection(const Ogre::Vector3& camValue, const Ogre::Vector3& camDiff,
    Ogre::Vector3& collisionPoint,
    short& partIndex, short& triangleIndex)
{
    mCamPointFound = false;
    mCamCollisionWeight = 1.0f;
    mCamDiffSqrLen = camDiff.squaredLength();

    Ogre::Vector3 camDiffRecip = Ogre::Vector3::ZERO;
    if(camDiff.x != 0.0f)
        camDiffRecip.x = 1.0f / camDiff.x;
    if(camDiff.y != 0.0f)
        camDiffRecip.y = 1.0f / camDiff.y;
    if(camDiff.z != 0.0f)
        camDiffRecip.z = 1.0f / camDiff.z;

    if(mCamDiffSqrLen > 0.0f)
    {
        assert(mCollisionGobal.size() == 1);
        assert(mCollisionGobal[0].subparts.size() == 1);

        const DE2::DE2_CollisionInfo& root = mCollisionGobal[0].subparts[0];
        camBroadSearch(root, camValue, camDiff, camDiffRecip);
    }

    partIndex = mCamFoundPartIndex;
    triangleIndex = mCamFoundTiangleIndex;
    collisionPoint = mCamCollisionPoint;

    return mCamPointFound;
}

void CollisionDetection::camBroadSearch(const DE2::DE2_CollisionInfo& subTree,
    const Ogre::Vector3& camValue, const Ogre::Vector3& camDiff, const Ogre::Vector3& camDiffRecip)
{
    if(checkCamCollision(subTree.aabb, camValue, camDiff, camDiffRecip))
    {
        if(!subTree.subparts.empty())
        {
            for(size_t q = 0; q < subTree.subparts.size(); ++q)
            {
                camBroadSearch(subTree.subparts[q], camValue, camDiff, camDiffRecip);
            }
        }
        else
        {
            short partIndex = subTree.triIndex;
            if(partIndex != -1)
            {
                const DE2::DE2_CollisionInfo& leaf = mCollisionParts[partIndex];

                if(checkCamCollision(leaf.aabb, camValue, camDiff, camDiffRecip))
                {
                    if(!leaf.subparts.empty())
                    {
                        for(size_t q = 0; q < leaf.subparts.size(); ++q)
                        {
                            camNarrowSearch(leaf.subparts[q], camValue, camDiff, camDiffRecip, partIndex);
                        }
                    }
                    else
                    {
                        camCollisionFinal(camValue, camDiff, partIndex, leaf.triIndex);
                    }
                }
            }
        }
    }
}

void CollisionDetection::camNarrowSearch(const DE2::DE2_CollisionInfo& leaf,
    const Ogre::Vector3& camValue, const Ogre::Vector3& camDiff, const Ogre::Vector3& camDiffRecip,
    short partIndex)
{
    if(checkCamCollision(leaf.aabb, camValue, camDiff, camDiffRecip))
    {
        if(!leaf.subparts.empty())
        {
            for(size_t q = 0; q < leaf.subparts.size(); ++q)
            {
                camNarrowSearch(leaf.subparts[q], camValue, camDiff, camDiffRecip, partIndex);
            }
        }
        else
        {
            camCollisionFinal(camValue, camDiff, partIndex, leaf.triIndex);
        }
    }
}

void CollisionDetection::camCollisionFinal(const Ogre::Vector3& camValue, const Ogre::Vector3& camDiff,
    short partIndex, short triangleIndex)
{
    Ogre::Vector3 pointA = convert(mDataVertexes[mDataParts[partIndex].Data_Triangles[triangleIndex].v0]);
    Ogre::Vector3 pointC = convert(mDataVertexes[mDataParts[partIndex].Data_Triangles[triangleIndex].v1]);
    Ogre::Vector3 pointB = convert(mDataVertexes[mDataParts[partIndex].Data_Triangles[triangleIndex].v2]);
    Ogre::Vector3 normal = Ogre::Vector3(pointB - pointA).crossProduct(Ogre::Vector3(pointC - pointA));
    normal.normalise();

    Ogre::Real dotP = -(camValue.dotProduct(normal) - pointA.dotProduct(normal)) / camDiff.dotProduct(normal);

    if(dotP >= 0.0f && dotP < mCamCollisionWeight)
    {
        Ogre::Vector3 proj = camDiff * dotP + camValue;

        Ogre::Vector3 normalAbs = Ogre::Vector3(Ogre::Math::Abs(normal.x), Ogre::Math::Abs(normal.y), Ogre::Math::Abs(normal.z));

        Ogre::Real paramA, paramB, paramC, paramD, paramE, paramF, paramG, paramH;

        if(normalAbs.x <= normalAbs.y || normalAbs.x <= normalAbs.z)//y,z max
        {
            if(normalAbs.y <= normalAbs.z)//z max
            {
                if(normal.z > 0.0f)
                {
                    paramA = pointA.x;
                    paramB = pointA.y;
                    paramC = pointB.x;
                    paramD = pointB.y;
                    paramE = pointC.x;
                    paramF = pointC.y;
                    paramG = proj.x;
                    paramH = proj.y;
                }
                else
                {
                    paramA = pointA.y;
                    paramB = pointA.x;
                    paramC = pointB.y;
                    paramD = pointB.x;
                    paramE = pointC.y;
                    paramF = pointC.x;
                    paramG = proj.y;
                    paramH = proj.x;
                }
            }
            else//y max
            {
                if(normal.y > 0.0f)
                {
                    paramA = pointA.z;
                    paramB = pointA.x;
                    paramC = pointB.z;
                    paramD = pointB.x;
                    paramE = pointC.z;
                    paramF = pointC.x;
                    paramG = proj.z;
                    paramH = proj.x;
                }
                else
                {
                    paramA = pointA.x;
                    paramB = pointA.z;
                    paramC = pointB.x;
                    paramD = pointB.z;
                    paramE = pointC.x;
                    paramF = pointC.z;
                    paramG = proj.x;
                    paramH = proj.z;
                }
            }
        }
        else//x max
        {
            if(normal.x <= 0.0f)
            {
                paramA = pointA.z;
                paramB = pointA.y;
                paramC = pointB.z;
                paramD = pointB.y;
                paramE = pointC.z;
                paramF = pointC.y;
                paramG = proj.z;
                paramH = proj.y;
            }
            else
            {
                paramA = pointA.y;
                paramB = pointA.z;
                paramC = pointB.y;
                paramD = pointB.z;
                paramE = pointC.y;
                paramF = pointC.z;
                paramG = proj.y;
                paramH = proj.z;
            }
        }

        Ogre::Real diffA = paramG - paramC;
        Ogre::Real diffB = paramH - paramD;

        if ( 
            (paramC - paramA) * diffB + -(paramD - paramB) * diffA >= 0.0   &&
            (paramE - paramC) * diffB + -(paramF - paramD) * diffA >= 0.0   &&
            (paramG - paramE) * -(paramB - paramF) + (paramH - paramF) * (paramA - paramE) >= 0.0 
            )
        {
            mCamPointFound = true;
            mCamFoundPartIndex = partIndex;
            mCamFoundTiangleIndex = triangleIndex;
            mCamCollisionWeight = dotP;
            mCamCollisionPoint = proj;
        }
    }
}

bool CollisionDetection::checkCamCollision(const DE2::AABB& aabb, const Ogre::Vector3& camValue, const Ogre::Vector3& camDiff, const Ogre::Vector3& camDiffRecip) const
{
    bool ret = false;

    short flagA = 1, flagB = 1, flagC, flagD;

    Ogre::Vector3 vals;

    if(camValue.x >= aabb.min.x)
    {
        if(camValue.x <= aabb.max.x)
        {
            flagD = 2;
        }
        else
        {
            flagA = 0;
            flagD = 0;
            vals.x = aabb.max.x;
        }
    }
    else
    {
        flagA = 0;
        flagD = 1;
        vals.x = aabb.min.x;
    }

    if(camValue.y >= aabb.min.y)
    {
        if(camValue.y <= aabb.max.y)
        {
            flagC = 2;
        }
        else
        {
            flagA = 0;
            flagC = 0;
            vals.y = aabb.max.y;
        }
    }
    else
    {
        flagA = 0;
        flagC = 1;
        vals.y = aabb.min.y;
    }

    if(camValue.z >= aabb.min.z)
    {
        if(camValue.z <= aabb.max.z)
        {
            flagB = 2;
        }
        else
        {
            flagA = 0;
            flagB = 0;
            vals.z = aabb.max.z;
        }
    }
    else
    {
        flagA = 0;
        vals.z = aabb.min.z;
    }

    if(flagA)
    {
        ret = true;
    }
    else
    {
        Ogre::Real valA, valB, valC;

        if(flagD == 2 || camDiff.x == 0.0f)
            valA = -1.0f;
        else
            valA = (vals.x - camValue.x) * camDiffRecip.x;

        if(flagC == 2 || camDiff.y == 0.0f)
            valB = -1.0f;
        else
            valB = (vals.y - camValue.y) * camDiffRecip.y;

        if(flagB == 2 || camDiff.z == 0.0f)
            valC = -1.0f;
        else
            valC = (vals.z - camValue.z) * camDiffRecip.z;

        if(valA <= valB || valA <= valC)
        {
            if(valB <= valC)
            {
                if ( valC >= 0.0f )
                {
                    Ogre::Real valCC = camDiff.x * valC + camValue.x;

                    if(valCC >= aabb.min.x && valCC <= aabb.max.x)
                    {
                        Ogre::Real valCCC = camDiff.y * valC + camValue.y;

                        if(valCCC >= aabb.min.y && valCCC <= aabb.max.y)
                        {
                            Ogre::Vector3 diff(valCC - camValue.x, valCCC - camValue.y, vals.z - camValue.z);
                            if(diff.squaredLength() <= mCamDiffSqrLen)
                                ret = true;
                        }
                    }
                }
            }
            else if(valB >= 0.0f)
            {
                Ogre::Real valBB = camDiff.x * valB + camValue.x;

                if(valBB >= aabb.min.x && valBB <= aabb.max.x)
                {
                    Ogre::Real valBBB = camDiff.z * valB + camValue.z;

                    if(valBBB >= aabb.min.z && valBBB <= aabb.max.z)
                    {
                        Ogre::Vector3 diff(valBB - camValue.x, vals.y - camValue.y, valBBB - camValue.z);
                        if(diff.squaredLength() <= mCamDiffSqrLen)
                            ret = true;
                    }
                }
            }
        }
        else if(valA >= 0.0f)
        {
            Ogre::Real valAA = camDiff.y * valA + camValue.y;

            if(valAA >= aabb.min.y && valAA <= aabb.max.y)
            {
                Ogre::Real valAAA = camDiff.z * valA + camValue.z;

                if(valAAA >= aabb.min.z && valAAA <= aabb.max.z)
                {
                    Ogre::Vector3 diff(vals.x - camValue.x, valAA - camValue.y, valAAA - camValue.z);
                    if(diff.squaredLength() <= mCamDiffSqrLen)
                        ret = true;
                }
            }
        }
    }

    return ret;
}