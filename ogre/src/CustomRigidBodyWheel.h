
#ifndef CUSTOMRIGIDBODYWHEEL_H
#define CUSTOMRIGIDBODYWHEEL_H

#include "OgreInclude.h"
#include "OgreBulletInclude.h"

class CustomRigidBodyWheel : public OgreBulletDynamics::RigidBody
{
public:

    CustomRigidBodyWheel(const Ogre::String &name,
                    OgreBulletDynamics::DynamicsWorld *world,
                    Ogre::SceneNode* chassisNode,
                    const Ogre::Vector3& connectionPoint,
                    bool isFront = false,
                    const short collisionGroup = 0,
                    const short collisionMask = 0);

    virtual ~CustomRigidBodyWheel(){}

    virtual void setTransform(const btTransform& worldTrans);

    bool checkCollisionWithStatic(const Ogre::Vector3& newWheelPos);
    void setExternalUpdate(const Ogre::Vector3& wheelPos);

private:

    bool mIsFront;

    Ogre::SceneNode* mChassisNode;

    Ogre::Vector3 mConnectionPoint;

    Ogre::Vector3 mPrevPosition;
    Ogre::Vector3 mPrevPositionExternalUpdate;
    bool mIsExternalUpdate;

};

#endif