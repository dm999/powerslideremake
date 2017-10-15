
#include "CustomRigidBodyWheel.h"


CustomRigidBodyWheel::CustomRigidBodyWheel(const Ogre::String &name,
                    OgreBulletDynamics::DynamicsWorld *world,
                    Ogre::SceneNode* chassisNode,
                    const Ogre::Vector3& connectionPoint,
                    bool isFront,
                    const short collisionGroup,
                    const short collisionMask)
                    : RigidBody(name, world, collisionGroup, collisionMask),
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
    Ogre::Vector3 newWheelPos = OgreBulletCollisions::convert(worldTrans.getOrigin());

    if(mIsExternalUpdate)
    {
        getBulletRigidBody()->getWorldTransform().setOrigin(OgreBulletCollisions::convert(mPrevPositionExternalUpdate));
        newWheelPos = mPrevPositionExternalUpdate;
        mIsExternalUpdate = false;
    }

    //check world collision
    bool isCollided = checkCollisionWithStatic(newWheelPos);

    if(isCollided)
    {
        getBulletRigidBody()->getWorldTransform().setOrigin(OgreBulletCollisions::convert(mPrevPosition));
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

    Ogre::Quaternion wheelRotation = OgreBulletCollisions::convert(worldTrans.getRotation());

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
}

bool CustomRigidBodyWheel::checkCollisionWithStatic(const Ogre::Vector3& newWheelPos)
{
    bool isCollided = false;

    if(mPrevPosition != newWheelPos)
    {
        //d.polubotko: check if center of sphere appeared on other side of static object
        btCollisionWorld::ClosestRayResultCallback rayCallback(OgreBulletCollisions::convert(mPrevPosition),OgreBulletCollisions::convert(newWheelPos));
        static_cast<OgreBulletDynamics::DynamicsWorld *>(mWorld)->getBulletDynamicsWorld()->rayTest(OgreBulletCollisions::convert(mPrevPosition), OgreBulletCollisions::convert(newWheelPos), rayCallback);
        if (rayCallback.hasHit())
        {
            if(rayCallback.m_collisionObject != mObject)
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