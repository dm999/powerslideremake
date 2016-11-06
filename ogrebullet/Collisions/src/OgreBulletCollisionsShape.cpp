/***************************************************************************

This source file is part of OGREBULLET
(Object-oriented Graphics Rendering Engine Bullet Wrapper)

Copyright (c) 2007 tuan.kuranes@gmail.com (Use it Freely, even Statically, but have to contribute any changes)
Copyright (c) 2013 alexey.knyshev@gmail.com


Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
-----------------------------------------------------------------------------
*/

#include "OgreBulletCollisions.h"

#include "OgreBulletCollisionsShape.h"
#include "OgreBulletConverter.h"
#include "OgreBulletCollisionsDebugLines.h"

using namespace Ogre;
using namespace OgreBulletCollisions;

inline static Vector3 getVertex(const btVector3 &vec, const btTransform &transform, const btConvexShape *convex)
{
    return convert(transform * convex->localGetSupportingVertex(vec));
}

namespace OgreBulletCollisions
{
    // -------------------------------------------------------------------------
    CollisionShape::CollisionShape()
        : mShape(NULL)
    { }
    // -------------------------------------------------------------------------
    CollisionShape::CollisionShape(btCollisionShape *shape)
        : mShape(shape)
    { }
    // -------------------------------------------------------------------------
    CollisionShape::~CollisionShape()
    {
    }
    // -------------------------------------------------------------------------
    bool CollisionShape::drawWireFrame(DebugLines *wire, const Vector3 &pos, const Quaternion &quat)
    {   
        if (mShape->isConvex())
        {
			return drawConvexWireFrame(wire, pos, quat);
        } else if (mShape->isConcave()) {
            return drawConcaveWireFrame(wire, pos, quat);
        } else {
            assert(0 && "can only debug draw convex/concave shapes!");
        }

        return false;
    }
    // -------------------------------------------------------------------------
    void CollisionShape::processTriangle(btVector3 *triangle, int partId, int triangleIndex)
    {
        btVector3 a = mTrans * triangle[0];
        btVector3 b = mTrans * triangle[1];
        btVector3 c = mTrans * triangle[2];
        mWire->addLine(convert(a), convert(b));
        mWire->addLine(convert(b), convert(c));
        mWire->addLine(convert(c), convert(a));
    }
    // -------------------------------------------------------------------------
    bool CollisionShape::drawConcaveWireFrame(DebugLines *wire, const Vector3 &pos, const Quaternion &quat)
    {
        btVector3 min;
        btVector3 max;
        btTransform ident;
        ident.setIdentity();
        mShape->getAabb(ident, min, max);
        mWire = wire;
        mTrans = btTransform(convert(quat), convert(pos));
        static_cast<btConcaveShape*>(mShape)->processAllTriangles(this, min, max);
        mWire = 0;
        return true;
    }
    // -------------------------------------------------------------------------
    bool CollisionShape::drawConvexWireFrame(DebugLines *wire, const Vector3 &pos, const Quaternion &quat) const
    {   
        assert(mShape->isConvex());

        if (0 && mShape->getShapeType() <= CUSTOM_POLYHEDRAL_SHAPE_TYPE)
		{
            const btPolyhedralConvexShape * const polyshape = static_cast<btPolyhedralConvexShape *>(mShape);

			const bool hasVecTransform = (pos != Vector3::ZERO);
			const bool hasQuatTransform = (quat != Quaternion::IDENTITY);
			const bool hasTransform = (hasVecTransform) || (hasQuatTransform);

            const btTransform trans(convert(quat), convert(pos));

            btVector3 a, b;
            for (int i = 0; i < polyshape->getNumEdges(); ++i)
			{
				polyshape->getEdge(i, a, b);

				if (hasTransform)
				{
					a = trans * a;
					b = trans * b;
                }

                wire->addLine(convert(a), convert(b));

			}
		}
		else
		{
			Vector3 lastVec;
			bool sideBeginning;
            const btConvexShape * const s = static_cast<btConvexShape *>(mShape);

            const btTransform trans(convert(quat), convert(pos));

//#define getVertex(X,Y,Z) BtOgreConverter::to(trans * s->localGetSupportingVertex (btVector3(X,Y,Z)))

			Vector3 curVec;
			size_t i = 0;
			const int subDivisionCount = 1;
			const float subDivide = 1.0f / subDivisionCount;
            for (int x = -subDivisionCount; x <= subDivisionCount; ++x)
			{
                for (int y = -subDivisionCount; y <= subDivisionCount; ++y)
				{
					sideBeginning = true;
                    for (int z = -subDivisionCount; z <= subDivisionCount; ++z)
					{
//                        curVec = getVertex(x * subDivide, y * subDivide, z * subDivide);
                        curVec = getVertex(btVector3(x * subDivide, y * subDivide, z * subDivide),
                                           trans, s);

						if (sideBeginning)
                        {
							sideBeginning = false;
                        }
						else
                        {
							wire->addLine (lastVec, curVec);
                        }

						lastVec = curVec;

                        ++i;
					}
				}
			}


            for (int x = -subDivisionCount; x <= subDivisionCount; ++x)
			{
                for (int z = -subDivisionCount; z <= subDivisionCount; ++z)
				{
					sideBeginning = true;
                    for (int y = -subDivisionCount; y <= subDivisionCount; ++y)
					{
                        curVec = getVertex(btVector3(x * subDivide, y * subDivide, z * subDivide),
                                           trans, s);

						if (sideBeginning)
                        {
							sideBeginning = false;
                        }
						else
                        {
							wire->addLine (lastVec, curVec);
                        }

						lastVec = curVec;

                        ++i;
					}
				}
			}


            for (int z = -subDivisionCount; z <= subDivisionCount; ++z)
			{
                for (int y = -subDivisionCount; y <= subDivisionCount; ++y)
				{
					sideBeginning = true;
                    for (int x = -subDivisionCount; x <= subDivisionCount; ++x)
					{
                        curVec = getVertex(btVector3(x * subDivide, y * subDivide, z * subDivide),
                                           trans, s);
						

						if (sideBeginning)
                        {
							sideBeginning = false;
                        }
						else
                        {
							wire->addLine (lastVec, curVec);
                        }

						lastVec = curVec;

                        ++i;
					}
				}
			}
//#undef getVertex
		}
        return true;
    }
}

