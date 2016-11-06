/***************************************************************************

This source file is part of OGREBULLET
(Object-oriented Graphics Rendering Engine Bullet Wrapper)
For the latest info, see http://www.ogre3d.org/phpBB2addons/viewforum.php?f=10

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

#ifndef _OGREBULLETCOLLISIONS_H
#define _OGREBULLETCOLLISIONS_H

#include "OgreBulletCollisionsPreRequisites.h"


#ifdef _PRECOMP


    #include "OgreBulletConverter.h"

	//Debug Helpers
	#include "OgreBulletCollisionsDebugLines.h"
	#include "OgreBulletCollisionsDebugShape.h"
	#include "OgreBulletCollisionsDebugDrawer.h"
	#include "OgreBulletCollisionsDebugContact.h"

    //Shapes
    #include "OgreBulletCollisionsShape.h"

    #include "OgreBulletCollisionsBoxShape.h"
    #include "OgreBulletCollisionsConeShape.h"
    #include "OgreBulletCollisionsSphereShape.h"
    #include "OgreBulletCollisionsCylinderShape.h"
	#include "OgreBulletCollisionsTriangleShape.h"
	#include "OgreBulletCollisionsTerrainShape.h"
	#include "OgreBulletCollisionsCylinderShape.h"
	#include "OgreBulletCollisionsCapsuleShape.h"
    #include "OgreBulletCollisionsStaticPlaneShape.h"

    #include "OgreBulletCollisionsCompoundShape.h"

    #include "OgreBulletCollisionsMultiSphereShape.h"

    #include "OgreBulletCollisionsConvexHullShape.h"
    #include "OgreBulletCollisionsMinkowskiSumShape.h"

	#include "OgreBulletCollisionsTrimeshShape.h"
	#include "OgreBulletCollisionsGImpactShape.h"

    //Utils
    #include "OgreBulletCollisionsMeshToShapeConverter.h"

    // Ray
    //#include "OgreBulletCollisionsConvexCast.h"

    // Core Collisions
    #include "OgreBulletCollisionsObject.h"
    #include "OgreBulletCollisionsObjectState.h"

    #include "OgreBulletCollisionsWorld.h"

    #include "OgreBulletCollisionsRay.h"





    #include "OgreString.h"
    #include "OgreFontManager.h"

    #include "BulletCollision/CollisionShapes/btMinkowskiSumShape.h"
    #include "BulletCollision/CollisionShapes/btTriangleShape.h"


#endif //_PRECOMP

#endif //_OGREBULLETCOLLISIONS_H

