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

#include "OgreBulletCollisionsWorld.h"
#include "OgreBulletCollisionsRay.h"
#include "OgreBulletConverter.h"

#include "OgreBulletCollisionsObject.h"
#include "OgreBulletCollisionsDebugShape.h"

using namespace Ogre;
using namespace OgreBulletCollisions;

namespace OgreBulletCollisions
{
    // -------------------------------------------------------------------------
    CollisionRayResultCallback::CollisionRayResultCallback(const Ogre::Ray &ray,
                                                           CollisionsWorld *world,
                                                           Ogre::Real max_distance,
                                                           bool init)
        : mRayResultCallback(NULL),
          mWorld(world),
          mRay(ray),
          mMaxDistance(max_distance)
    {
        if (init)
        {
            //mRay = new btCollisionWorld::RayResultCallback (
            //    convert(ray.getOrigin ()),
            //   convert(ray.getDirection ()));
        }
	}
    // -------------------------------------------------------------------------
    CollisionRayResultCallback::~CollisionRayResultCallback()
    {
        if (mRayResultCallback)
        {
            delete mRayResultCallback;
            mRayResultCallback = NULL;
        }
    }
    Ogre::Vector3 CollisionRayResultCallback::getRayStartPoint() const
    {
        return mRay.getOrigin();
    }
    // -------------------------------------------------------------------------
    Ogre::Vector3 CollisionRayResultCallback::getRayEndPoint() const
    {
        return mRay.getPoint(mMaxDistance);
    }
    // -------------------------------------------------------------------------
    bool CollisionRayResultCallback::doesCollide() const
    {
        return mRayResultCallback->hasHit();
    }
    // -------------------------------------------------------------------------
    const Object *CollisionClosestRayResultCallback::getCollidedObject() const
    {        
        return mWorld->findObject(static_cast<btCollisionWorld::ClosestRayResultCallback *>(mRayResultCallback)->m_collisionObject);
	}
    // -------------------------------------------------------------------------
    CollisionClosestRayResultCallback::CollisionClosestRayResultCallback(const Ogre::Ray &ray, CollisionsWorld *world, Ogre::Real max_distance)
        : CollisionRayResultCallback(ray, world, max_distance, false)
    {
        mRayResultCallback = new btCollisionWorld::ClosestRayResultCallback(convert(getRayStartPoint()),
                                                                            convert(getRayEndPoint()));
    } 
    // -------------------------------------------------------------------------
    Vector3 CollisionClosestRayResultCallback::getCollisionPoint() const
    {
        return convert(getBulletClosestRayResultCallback()->m_hitPointWorld);
	}
	// -------------------------------------------------------------------------
	Vector3 CollisionClosestRayResultCallback::getCollisionNormal() const
	{
        return convert(getBulletClosestRayResultCallback()->m_hitNormalWorld);
	}



    // -------------------------------------------------------------------------
    CollisionAllRayResultCallback::CollisionAllRayResultCallback(const Ogre::Ray &ray, CollisionsWorld *world, Ogre::Real max_distance)
        : CollisionRayResultCallback(ray, world, max_distance, false)
    {
        mRayResultCallback = new btCollisionWorld::AllHitsRayResultCallback(convert(getRayStartPoint()),
                                                                            convert(getRayEndPoint()));
    } 
    // -------------------------------------------------------------------------
}

