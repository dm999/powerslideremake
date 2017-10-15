#include "CustomRigidBody.h"

CustomRigidBody::CustomRigidBody(const Ogre::String &name,
                    OgreBulletDynamics::DynamicsWorld *world,
                    const Ogre::Vector3& initialImpulseLinear,
                    const short collisionGroup,
                    const short collisionMask)
                    : RigidBody(name, world, collisionGroup, collisionMask),
                    mLinearImpulse(Ogre::Vector3::ZERO)
{
    mLinearImpulse = initialImpulseLinear;
}

void CustomRigidBody::setTransform(const btTransform& worldTrans)
{
    RigidBody::setTransform(worldTrans);

    Ogre::Vector3 velocity = getLinearVelocity();//already scaled to spf

    const float psCarMass = 45.0f;

    mLinearImpulse = velocity * psCarMass * 0.001f;//d.polubotko: should be reduced otherwise NN saturates
}

Ogre::Vector3 CustomRigidBody::getLinearImpulse() const
{
    return mLinearImpulse;
}