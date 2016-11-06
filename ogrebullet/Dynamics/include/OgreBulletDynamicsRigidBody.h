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

#ifndef _OGREBULLETDYNAMICS_RigidObject_H
#define _OGREBULLETDYNAMICS_RigidObject_H

#include "OgreBulletDynamicsPreRequisites.h"

#include "OgreBulletCollisionsObject.h"
#include "OgreBulletCollisionsWorld.h"
#include "OgreBulletConverter.h"

#include "OgreBulletDynamicsWorld.h"

#include "OgreBulletDynamicsRaycastVehicle.h"
#include "BulletCollision/CollisionShapes/btScaledBvhTriangleMeshShape.h"

namespace OgreBulletDynamics
{
    // -------------------------------------------------------------------------
    // basic rigid body class
    class RigidBody : public OgreBulletCollisions::Object
    {
    public:
        RigidBody(const Ogre::String &name,
                  DynamicsWorld *world,
                  const short collisionGroup = 0,
                  const short collisionMask = 0);

        virtual ~RigidBody();

        void setShape(Ogre::SceneNode *node,
                      OgreBulletCollisions::CollisionShape *shape,
                      const float bodyRestitution,
                      const float bodyFriction,
                      const float bodyRollingFriction,
                      const Ogre::Vector3 &anisotropicFriction,
                      const float bodyMass,
                      const Ogre::Vector3 &pos = Ogre::Vector3::ZERO,
                      const Ogre::Quaternion &quat = Ogre::Quaternion::IDENTITY);

        void setStaticShape(Ogre::SceneNode *node,
                            OgreBulletCollisions::CollisionShape *shape,
                            const float bodyRestitution,
                            const float bodyFriction,
                            const float bodyRollingFriction,
                            const Ogre::Vector3 &pos = Ogre::Vector3::ZERO,
                            const Ogre::Quaternion &quat = Ogre::Quaternion::IDENTITY);


        void setStaticShape(btScaledBvhTriangleMeshShape *shape,
                            const float bodyRestitution,
                            const float bodyFriction,
                            const float bodyRollingFriction,
                            const Ogre::Vector3 &pos = Ogre::Vector3::ZERO,
                            const Ogre::Quaternion &quat = Ogre::Quaternion::IDENTITY);


        void setStaticShape(OgreBulletCollisions::CollisionShape *shape,
                            const float bodyRestitution,
                            const float bodyFriction,
                            const float bodyRollingFriction,
                            const Ogre::Vector3 &pos = Ogre::Vector3::ZERO,
                            const Ogre::Quaternion &quat = Ogre::Quaternion::IDENTITY);

        void setLinearVelocity(const Ogre::Vector3 &vel);
        void setLinearVelocity(const Ogre::Real x, const Ogre::Real y, const Ogre::Real z);
        Ogre::Vector3 getLinearVelocity() const;

        void setAngularVelocity(const Ogre::Vector3 &vel);
        void setAngularVelocity(const Ogre::Real x, const Ogre::Real y, const Ogre::Real z);
        Ogre::Vector3 getAngularVelocity() const;

        void setGravity(const Ogre::Vector3 &acceleration);
        void setGravity(const Ogre::Real x, const Ogre::Real y, const Ogre::Real z);
        Ogre::Vector3 getGravity() const;

        void applyImpulse(const Ogre::Vector3 &impulse, const Ogre::Vector3 &position);
        void applyForce(const Ogre::Vector3 &impulse, const Ogre::Vector3 &position);


        inline btRigidBody *getBulletRigidBody() const;
        inline btDynamicsWorld *getBulletDynamicsWorld() const;
        inline DynamicsWorld *getDynamicsWorld();

        inline Ogre::SceneNode *getSceneNode() const { return mRootNode; }

        inline bool isStaticObject() const;
        inline bool isKinematicObject() const;

        void setKinematicObject(bool isKinematic);

        void setActivationState(int state);
        int getActivationState() const;

        inline void disableDeactivation();
        inline void enableActiveState();
        inline void forceActivationState();
        void setDeactivationTime(const float ftime);

        inline const btTransform &getCenterOfMassTransform() const;
        inline Ogre::Quaternion getCenterOfMassOrientation() const;
        inline Ogre::Vector3 getCenterOfMassPosition() const;

        void setCenterOfMass(const Ogre::Vector3 &center, const Ogre::Quaternion& rot);
        Ogre::Vector3 getCenterOfMassPivot(const Ogre::Vector3 &pivotPosition) const;
        
        void setDamping(const Ogre::Real linearDamping, const Ogre::Real angularDamping);

    protected:
        short mCollisionGroup;
        short mCollisionMask;
    };
    // -------------------------------------------------------------------------
    // basic rigid body class
    class WheeledRigidBody : public RigidBody
    {
    public:
        WheeledRigidBody(const Ogre::String &name, DynamicsWorld *world)
            : RigidBody(name, world),
              mVehicle(NULL)
        {
        }
        
        inline void setVehicle(RaycastVehicle *v) { mVehicle = v; }

        virtual void setPosition(const btVector3 &pos);
        virtual void setOrientation(const btQuaternion &quat);
        virtual void setTransform(const btVector3 &pos, const btQuaternion &quat);
        virtual void setTransform(const btTransform& worldTrans);

        virtual ~WheeledRigidBody() { }

    protected:
        RaycastVehicle *mVehicle;
    };
    // -------------------------------------------------------------------------
    // basic rigid body class inline methods
    // -------------------------------------------------------------------------
    inline btRigidBody *RigidBody::getBulletRigidBody() const
    {
        return static_cast<btRigidBody *>(mObject);
    }
    // -------------------------------------------------------------------------
    inline btDynamicsWorld *RigidBody::getBulletDynamicsWorld() const
    { 
        return static_cast<btDynamicsWorld *>(mWorld->getBulletCollisionWorld());
    }
    // -------------------------------------------------------------------------
    inline DynamicsWorld *RigidBody::getDynamicsWorld()
    { 
        return static_cast<DynamicsWorld *>(mWorld);
    }
    // -------------------------------------------------------------------------
    inline bool RigidBody::isStaticObject() const 
    {
        return getBulletRigidBody()->isStaticObject();
    }
    // -------------------------------------------------------------------------
    inline bool RigidBody::isKinematicObject() const 
    {
        return getBulletRigidBody()->isKinematicObject();
    }
    // -------------------------------------------------------------------------
    inline void RigidBody::setActivationState(int state)
    {
        getBulletRigidBody()->setActivationState(state);
    }
    // -------------------------------------------------------------------------
    inline int RigidBody::getActivationState() const
    {
        return getBulletRigidBody()->getActivationState();
    }
    // -------------------------------------------------------------------------
    inline void RigidBody::disableDeactivation()
    {
        getBulletRigidBody()->setActivationState(DISABLE_DEACTIVATION);
    }
    // -------------------------------------------------------------------------
    inline void RigidBody::enableActiveState()
    {
        getBulletRigidBody()->setActivationState(ACTIVE_TAG);
    }
    // -------------------------------------------------------------------------
    inline void RigidBody::forceActivationState()
    {
        getBulletRigidBody()->forceActivationState(ACTIVE_TAG);
    }
    // -------------------------------------------------------------------------
    // -------------------------------------------------------------------------
    inline const btTransform &RigidBody::getCenterOfMassTransform() const
    {
        return getBulletRigidBody()->getCenterOfMassTransform();
    }
    // -------------------------------------------------------------------------
    inline Ogre::Quaternion RigidBody::getCenterOfMassOrientation() const
    {
        return OgreBulletCollisions::convert(getCenterOfMassTransform().getRotation());
    }
    // -------------------------------------------------------------------------
    inline Ogre::Vector3 RigidBody::getCenterOfMassPosition() const
    {
        return OgreBulletCollisions::convert(getBulletRigidBody()->getCenterOfMassPosition());
    }
    // -------------------------------------------------------------------------   
    
}
#endif //_OGREBULLETDYNAMICS_RigidObject_H

