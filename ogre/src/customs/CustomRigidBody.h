#ifndef CUSTOMRIGIDBODY_H
#define CUSTOMRIGIDBODY_H

#include "../includes/OgreInclude.h"
#include "../includes/OgreBulletInclude.h"

class CustomRigidBody : public OgreBulletDynamics::RigidBody
{
public: 
    CustomRigidBody(const Ogre::String &name,
                    OgreBulletDynamics::DynamicsWorld *world,
                    const Ogre::Vector3& initialForcesLinear,
                    const short collisionGroup = 0,
                    const short collisionMask = 0);

    virtual ~CustomRigidBody(){}

    virtual void setTransform(const btTransform& worldTrans);

    Ogre::Vector3 getLinearImpulse() const;

private:

    Ogre::Vector3 mLinearImpulse;
};

#endif