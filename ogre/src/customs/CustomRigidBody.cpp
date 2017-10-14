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

    Ogre::Vector3 velocity = getLinearVelocity();//already scaled to spf
    //Ogre::Vector3 velocity = pos - mPrevPos;
    mPrevPos = pos;

    Ogre::Vector3 acceleration = velocity - mPrevVel;
    mPrevVel = velocity;

    const float psCarMass = 45.0f;
    const float psGravity = 0.03f;

    //float mass = 1.0f / getBulletRigidBody()->getInvMass();

    mLinearForce = acceleration * psCarMass;
    //mLinearForce = acceleration * psCarMass * 10.0f;
    //mLinearForce = Ogre::Vector3(psCarMass * acceleration.x, psCarMass * (-psGravity + acceleration.y), psCarMass * acceleration.z);
    //mLinearForce = Ogre::Vector3(psCarMass * acceleration.x, 0.0f, psCarMass * acceleration.z);
    //mLinearForce = Ogre::Vector3(0.0f, psCarMass * -psGravity, 0.0f);
}

Ogre::Vector3 CustomRigidBody::getLinearForce() const
{
    return mLinearForce;
}