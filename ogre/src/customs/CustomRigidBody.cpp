#include "CustomRigidBody.h"

CustomRigidBody::CustomRigidBody(const Ogre::String &name,
                    OgreBulletDynamics::DynamicsWorld *world,
                    const Ogre::Vector3& initialImpulseLinear,
                    const Ogre::Vector3& pos,
                    const short collisionGroup,
                    const short collisionMask)
                    : RigidBody(name, world, collisionGroup, collisionMask),
                    mLinearImpulse(Ogre::Vector3::ZERO)
{
    mLinearImpulse = initialImpulseLinear;

    mPrevPosition = pos;
}

void CustomRigidBody::setTransform(const btTransform& worldTrans)
{
    Ogre::Vector3 newPos = OgreBulletCollisions::convert(worldTrans.getOrigin());
    Ogre::Quaternion rotation = OgreBulletCollisions::convert(worldTrans.getRotation());


    bool isCollided = checkCollisionWithStatic(newPos, rotation);
    if(isCollided)
    {
        getBulletRigidBody()->getWorldTransform().setOrigin(OgreBulletCollisions::convert(mPrevPosition));
    }
    else
    {
        mPrevPosition = newPos;
    }

    mRootNode->setPosition(newPos);
    mRootNode->setOrientation(rotation);

    Ogre::Vector3 velocity = getLinearVelocity();//already scaled to spf

    const float psCarMass = 45.0f;

    mLinearImpulse = velocity * psCarMass * 0.001f;//d.polubotko: should be reduced otherwise NN saturates
}

Ogre::Vector3 CustomRigidBody::getLinearImpulse() const
{
    return mLinearImpulse;
}

bool CustomRigidBody::checkCollisionWithStatic(const Ogre::Vector3& newPos, const Ogre::Quaternion& rotation)
{
    bool isCollided = false;

    if(mPrevPosition != newPos)
    {
        btCompoundShape * shape = static_cast<btCompoundShape *>(mShape->getBulletShape());

        for(int q = 0; q < shape->getNumChildShapes(); ++q)
        {
            Ogre::Vector3 connectionPoint = OgreBulletCollisions::convert(shape->getChildTransform(q).getOrigin());

            //d.polubotko: check if center of sphere appeared on other side of static object
            btCollisionWorld::ClosestRayResultCallback rayCallback(OgreBulletCollisions::convert(mPrevPosition + rotation * connectionPoint),OgreBulletCollisions::convert(newPos + rotation * connectionPoint));
            static_cast<OgreBulletDynamics::DynamicsWorld *>(mWorld)->getBulletDynamicsWorld()->rayTest(OgreBulletCollisions::convert(mPrevPosition + rotation * connectionPoint), OgreBulletCollisions::convert(newPos + rotation * connectionPoint), rayCallback);
            if (rayCallback.hasHit())
            {
                if(rayCallback.m_collisionObject->isStaticObject())
                {
                    isCollided = true;
                    break;
                }
            }
        }

    }
    return isCollided;
}