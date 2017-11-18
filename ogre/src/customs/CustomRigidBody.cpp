#include "CustomRigidBody.h"

#include "../physics/Physics.h"
#include "../tools/PhysicsTools.h"

CustomRigidBody::CustomRigidBody(const Ogre::String &name,
                    Physics *world,
                    const Ogre::Vector3& initialImpulseLinear,
                    const Ogre::Vector3& pos,
                    const short collisionGroup,
                    const short collisionMask)
                    //: RigidBody(name, world, collisionGroup, collisionMask),
                    : mWorld(world),
                    mLinearImpulse(Ogre::Vector3::ZERO)
{
    mLinearImpulse = initialImpulseLinear;

    mPrevPosition = pos;
}

void CustomRigidBody::setTransform(const btTransform& worldTrans)
{
#if 0
    Ogre::Vector3 newPos = PhysicsTools::convert(worldTrans.getOrigin());
    Ogre::Quaternion rotation = PhysicsTools::convert(worldTrans.getRotation());


    bool isCollided = checkCollisionWithStatic(newPos, rotation);
    if(isCollided)
    {
        getBulletRigidBody()->getWorldTransform().setOrigin(PhysicsTools::convert(mPrevPosition));
    }
    else
    {
        mPrevPosition = newPos;
    }

    mRootNode->setPosition(newPos);
    mRootNode->setOrientation(rotation);

    Ogre::Vector3 velocity = getLinearVelocity();//already scaled to spf

    const float psCarMass = 45.0f;

    mLinearImpulse = velocity * psCarMass * 0.005f;//d.polubotko: should be reduced otherwise NN saturates
#endif
}

Ogre::Vector3 CustomRigidBody::getLinearImpulse() const
{
    return mLinearImpulse;
}

bool CustomRigidBody::checkCollisionWithStatic(const Ogre::Vector3& newPos, const Ogre::Quaternion& rotation)
{
    bool isCollided = false;
#if 0
    if(mPrevPosition != newPos)
    {
        btCompoundShape * shape = static_cast<btCompoundShape *>(mShape->getBulletShape());

        for(int q = 0; q < shape->getNumChildShapes(); ++q)
        {
            Ogre::Vector3 connectionPoint = PhysicsTools::convert(shape->getChildTransform(q).getOrigin());

            //d.polubotko: check if center of sphere appeared on other side of static object
            btCollisionWorld::AllHitsRayResultCallback rayCallback(PhysicsTools::convert(mPrevPosition + rotation * connectionPoint),PhysicsTools::convert(newPos + rotation * connectionPoint));
            mWorld->launchRay(rayCallback);
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
#endif
    return isCollided;
}