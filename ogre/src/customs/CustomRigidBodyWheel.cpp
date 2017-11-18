
#include "CustomRigidBodyWheel.h"

#include "BulletCollision/CollisionDispatch/btCollisionWorld.h"

#include "../physics/Physics.h"
#include "../tools/PhysicsTools.h"

CustomRigidBodyWheel::CustomRigidBodyWheel(const Ogre::String &name,
                    Physics *world,
                    Ogre::SceneNode* chassisNode,
                    const Ogre::Vector3& connectionPoint,
                    bool isFront,
                    const short collisionGroup,
                    const short collisionMask)
                    //: RigidBody(name, world, collisionGroup, collisionMask),
                    : mWorld(world),
                    mIsFront(isFront),
                    mIsExternalUpdate(false)
{
    mChassisNode = chassisNode;
    mConnectionPoint = connectionPoint;

    Ogre::Quaternion rot = mChassisNode->getOrientation();
    Ogre::Vector3 chassisPos = mChassisNode->getPosition();

    mPrevPosition = (rot * mConnectionPoint) + chassisPos;
}

void CustomRigidBodyWheel::setTransform(const btTransform& worldTrans)
{
#if 0
    Ogre::Vector3 newWheelPos = PhysicsTools::convert(worldTrans.getOrigin());

    if(mIsExternalUpdate)
    {
        getBulletRigidBody()->getWorldTransform().setOrigin(PhysicsTools::convert(mPrevPositionExternalUpdate));
        newWheelPos = mPrevPositionExternalUpdate;
        mIsExternalUpdate = false;
    }

    //check world collision
    bool isCollided = checkCollisionWithStatic(newWheelPos);

    if(isCollided)
    {
        getBulletRigidBody()->getWorldTransform().setOrigin(PhysicsTools::convert(mPrevPosition));
    }
    else
    {
        mPrevPosition = newWheelPos;
    }


    //limit Ogre object movements
    Ogre::Quaternion rot = mChassisNode->getOrientation();
    Ogre::Quaternion invRot = rot.Inverse();
    Ogre::Vector3 chassisPos = mChassisNode->getPosition();

    Ogre::Vector3 localSpaceNewPos = invRot * (newWheelPos - chassisPos);

    if(localSpaceNewPos.y > 0.0f)
    {
        localSpaceNewPos.y = 0;
    }

    if(localSpaceNewPos.y < -4.0f)
    {
        localSpaceNewPos.y = -4.0f;
    }

    localSpaceNewPos.x = mConnectionPoint.x;
    localSpaceNewPos.z = mConnectionPoint.z;

    localSpaceNewPos = (rot * localSpaceNewPos) + chassisPos;

    Ogre::Quaternion wheelRotation = PhysicsTools::convert(worldTrans.getRotation());

    if(!mIsFront)
    {
        Ogre::Quaternion wheelRotationLocal = invRot * wheelRotation;

        Ogre::Degree rotDegree;
        Ogre::Vector3 rotAxis;
        wheelRotationLocal.ToAngleAxis(rotDegree, rotAxis);

        rotAxis.y = 0.0f;
        rotAxis.z = 0.0f;

        wheelRotationLocal.FromAngleAxis(rotDegree, rotAxis);
        wheelRotation = rot * wheelRotationLocal;
    }

    mRootNode->setPosition(localSpaceNewPos);
    mRootNode->setOrientation(wheelRotation);
#endif
}

bool CustomRigidBodyWheel::checkCollisionWithStatic(const Ogre::Vector3& newWheelPos)
{
    bool isCollided = false;

    if(mPrevPosition != newWheelPos)
    {
        //d.polubotko: check if center of sphere appeared on other side of static object
        btCollisionWorld::AllHitsRayResultCallback rayCallback(PhysicsTools::convert(mPrevPosition),PhysicsTools::convert(newWheelPos));
        mWorld->launchRay(rayCallback);
        if (rayCallback.hasHit())
        {
            //if(rayCallback.m_collisionObject != mObject)
            {
                isCollided = true;
            }
        }
    }
    return isCollided;
}


void CustomRigidBodyWheel::setExternalUpdate(const Ogre::Vector3& wheelPos)
{
    mPrevPositionExternalUpdate = wheelPos;
    mIsExternalUpdate = true;
}