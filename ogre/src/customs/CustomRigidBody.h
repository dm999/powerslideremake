#ifndef CUSTOMRIGIDBODY_H
#define CUSTOMRIGIDBODY_H

#include "../includes/OgreInclude.h"
#include "../includes/OgreBulletInclude.h"

class CustomRigidBody : public OgreBulletDynamics::RigidBody
{
public: 
    CustomRigidBody(const Ogre::String &name,
                    OgreBulletDynamics::DynamicsWorld *world,
                    const short collisionGroup = 0,
                    const short collisionMask = 0);

    virtual ~CustomRigidBody(){}

    virtual void setTransform(const btTransform& worldTrans);

    Ogre::Vector3 getLinearForce() const {return mLinearForce;}

private:

    Ogre::Vector3 mPrevPos;
    Ogre::Vector3 mPrevVel;

    Ogre::Vector3 mLinearForce;
};

#endif