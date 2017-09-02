#include "CustomRigidBody.h"

CustomRigidBody::CustomRigidBody(const Ogre::String &name,
                    OgreBulletDynamics::DynamicsWorld *world,
                    const Ogre::Vector3& initialForcesLinear,
                    const short collisionGroup,
                    const short collisionMask)
                    : RigidBody(name, world, collisionGroup, collisionMask),
                    mPrevPos(Ogre::Vector3::ZERO),
                    mPrevVel(Ogre::Vector3::ZERO),
                    mLinearForce(Ogre::Vector3::ZERO)
{
    mLinearForce = initialForcesLinear;
}

void CustomRigidBody::setTransform(const btTransform& worldTrans)
{
    RigidBody::setTransform(worldTrans);

    Ogre::Vector3 pos = OgreBulletCollisions::convert(worldTrans.getOrigin());

    //Ogre::Vector3 velocity = getLinearVelocity();//already scaled to spf
    Ogre::Vector3 velocity = mPrevPos - pos;
    mPrevPos = pos;

    Ogre::Vector3 acceleration = mPrevVel - velocity;
    mPrevVel = velocity;

    float mass = 1.0f / getBulletRigidBody()->getInvMass();

    mLinearForce = acceleration * mass;
}

Ogre::Vector3 CustomRigidBody::getLinearForce() const
{
    return mLinearForce;
}