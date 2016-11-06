/***************************************************************************

This source file is part of OGREBULLET
(Object-oriented Graphics Rendering Engine Bullet Wrapper)

Copyright (c) 2013 alexey.knyshev@gmail.com (Use it Freely, even Statically, but have to contribute any changes)



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

#include "OgreBulletCollisionsTerrainShape.h"
#include "OgreBulletConverter.h"

#include "BulletCollision/CollisionShapes/btHeightfieldTerrainShape.h"

namespace OgreBulletCollisions
{
    HeightmapCollisionShape::HeightmapCollisionShape(int width, int length,
                                                     const Ogre::Vector3 &scale,
                                                     Ogre::Real *pHeightData,
                                                     bool bFlip)
	{
		int upIndex = 1;
        bool useFloatDatam = true;
        bool flipQuadEdges = bFlip;

		btHeightfieldTerrainShape* pHeightShape = 
            new btHeightfieldTerrainShape(width, length, pHeightData,
                                          scale.y, upIndex, useFloatDatam, flipQuadEdges);

		pHeightShape->setUseDiamondSubdivision(true);

		mShape = pHeightShape;

        mShape->setLocalScaling(convert(scale));
	}
	
    bool HeightmapCollisionShape::drawWireFrame(DebugLines *wire,
                                                const Ogre::Vector3 &pos,
                                                const Ogre::Quaternion &quat) const
	{
        btHeightfieldTerrainShape *pHeightShape =
                static_cast<btHeightfieldTerrainShape *>(mShape);

		btTransform bt;
		bt.setIdentity();

        const btVector3 colour(255.0, 255.0, 255.0);

		DebugHelper ddraw(wire);
		DebugTriangleDrawCallback cb(&ddraw, bt, colour);

        const btScalar max(BT_LARGE_FLOAT);
        const btScalar min(-BT_LARGE_FLOAT);
        const btVector3 aabbMax(max, max, max);
        const btVector3 aabbMin(min, min, min);
		pHeightShape->processAllTriangles(&cb, aabbMin, aabbMax);
		return true;
	}
}
