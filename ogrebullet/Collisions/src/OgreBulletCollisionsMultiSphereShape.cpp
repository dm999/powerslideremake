/***************************************************************************

This source file is part of OGREBULLET
(Object-oriented Graphics Rendering Engine Bullet Wrapper)

Copyright (c) 2007 tuan.kuranes@gmail.com (Use it Freely, even Statically, but have to contribute any changes)



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

#include "OgreBulletCollisionsMultiSphereShape.h"
#include "OgreBulletCollisionsDebugLines.h"
#include "OgreBulletConverter.h"

using namespace OgreBulletCollisions;

namespace OgreBulletCollisions
{
    // -------------------------------------------------------------------------
    MultiSphereCollisionShape::MultiSphereCollisionShape(const Ogre::Vector3 &inertiaHalfExtents,
                                                         const Ogre::Vector3 *positions,
                                                         const Ogre::Real *radi,
                                                         int numSpheres)
        : CollisionShape()
    {
        btVector3 *btPositions = new btVector3[numSpheres];
		{
			unsigned int currNum = numSpheres;
			while (currNum-- > 0)
			{
				btPositions[currNum] = convert(positions[currNum]);
			}
		}

        mShape = new btMultiSphereShape(btPositions,
                                        (const btScalar *)radi,
                                        numSpheres);

		delete [] btPositions;
    }
    MultiSphereCollisionShape::MultiSphereCollisionShape(Ogre::Real radius, Ogre::Real height, int numSegments, Ogre::Real smallSpheresHeightFactor)
    {
        //from Hinge Vehicle demo

        const int numOfSmallSpheresPerCircle = numSegments;	// 12 // Twickable (less = faster, higher = more precise)
        //const btScalar smallSpheresHeightFactor = smallSpheresHeightFactor;//0.95f;	// (0,1) I'd stick to 0.95f (lower values should increase the tyre curvature at the center)

        const btScalar scaling = height/(radius*2.0f);//height/(radius*2.0);	// correct value: height/(radius*2.0); we don't use it to have a better round tyre surface towards the ground (exceeding a bit the shape height (i.e.width))	
        const btScalar newHeight = radius*2.0f;	// When we'll scale it, it will became = height

        const int numPositions = 2*numOfSmallSpheresPerCircle+1;	// This is the number of spheres in the btMultiSphereShape
        btAlignedObjectArray<btVector3> positions;positions.resize(numPositions);
        btAlignedObjectArray<btScalar> radii;radii.resize(numPositions);

        int i = 0;
        positions[i] = btVector3(0.f,0.f,0.f);
        radii[i++] = radius;

        // Small spheres
        const btScalar delta = smallSpheresHeightFactor * radius;	// + smallRadius
        const btScalar smallRadius = newHeight*0.25f < delta*0.5f ? newHeight*0.25f : delta*0.5f;
        const btScalar R= delta-smallRadius >= smallRadius ? delta-smallRadius : smallRadius;
        const btScalar h1 = newHeight*0.25f*0.75f;	// The "correct" value should be newHeight*0.25, but the 0.75 factor increases the tyre curvature at the borders a bit, making it more realistic (TODO: further test this).
        const btScalar h2 = -h1;

        for (int t=0;t<numOfSmallSpheresPerCircle;t++)	{
            const btScalar s = R*btSin(2.f*SIMD_PI*(btScalar)t/numOfSmallSpheresPerCircle);
            const btScalar c = R*btCos(2.f*SIMD_PI*(btScalar)t/numOfSmallSpheresPerCircle);
            positions[i] = btVector3(h1,s,c);
            radii[i++] = smallRadius;
            positions[i] = btVector3(h2,c,s);
            radii[i++] = smallRadius;
        }
        	
        const btVector3	localScaling = btVector3(scaling,1.0,1.0);						
        const int numSpheres = numPositions;//sizeof(positions)/sizeof(positions[0]);							
        if (numSpheres != radii.size()) {
            assert(false && "Error: numPositions!=numRadii in btMultiSphereShape [btWheelShape::Create(...)]\n");
        }
        mShape = new btMultiSphereShape(&positions[0],&radii[0],numSpheres);
        mShape->setLocalScaling(localScaling);
    }
    // -------------------------------------------------------------------------
    MultiSphereCollisionShape::~MultiSphereCollisionShape()
    {
    }
}

