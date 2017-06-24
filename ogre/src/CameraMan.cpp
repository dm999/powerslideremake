#include "pcheader.h"

#include "CameraMan.h"
#include "tools/Conversions.h"
#include "tools/Tools.h"
#include "BaseApp.h"
#include "tools/OgreTools.h"

#include "BulletCollision/CollisionDispatch/btGhostObject.h"

CameraMan::CameraMan(Ogre::Camera* cam, OgreBulletDynamics::DynamicsWorld * world, Ogre::SceneManager* sceneMgr) 
: mWorld(world),
    mRearCamera(NULL),
    mCamera(cam),
    mCamPositonType(CameraPosition_ChassisC)
{
}

void CameraMan::setYawPitchDist(const Ogre::Quaternion& carRot, const Ogre::Vector3& carPos, Ogre::Real lateralVelocity)
{
    Ogre::Vector3 cameraDisplacement(0.0f, 0.0f, 0.0f);

    if(mCamPositonType == CameraPosition_Bumper)
    {
        cameraDisplacement.y = 6.0f;
        cameraDisplacement.z = 0.0f;
    }
    if(mCamPositonType == CameraPosition_ChassisA)
    {
        cameraDisplacement.y = 5.0f;
        cameraDisplacement.z = 24.0f;
    }
    if(mCamPositonType == CameraPosition_ChassisB)
    {
        cameraDisplacement.y = 10.0f;
        cameraDisplacement.z = 18.0f;
    }
    if(mCamPositonType == CameraPosition_ChassisC)
    {
        cameraDisplacement.y = 9.0f;
        cameraDisplacement.z = 24.0f;
    }



    if(mCamPositonType != CameraPosition_Bumper)
    {
        Ogre::Vector3 targetOffset(0.0f, 0.0f, 0.0f);

        if(mCamPositonType == CameraPosition_ChassisA)
        {
            targetOffset = Ogre::Vector3(0.0f, 6.0f, 0.0f);
        }
        if(mCamPositonType == CameraPosition_ChassisB)
        {
            targetOffset = Ogre::Vector3(0.0f, 6.0f, 0.0f);
        }
        if(mCamPositonType == CameraPosition_ChassisC)
        {
            targetOffset = Ogre::Vector3(0.0f, 6.0f, 0.0f);
        }

        Ogre::Vector3 forwardAxis = carRot * Ogre::Vector3::NEGATIVE_UNIT_Z;

        Ogre::Quaternion additionalRotYaw;
        additionalRotYaw.FromAngleAxis(Ogre::Degree(lateralVelocity / 20.0f), Ogre::Vector3::UNIT_Y);

        Ogre::Quaternion camRot;
        Ogre::Degree rot;

        Ogre::Vector3 projectedOnXZPlane(forwardAxis.x, 0.0f, forwardAxis.z);

        rot = Ogre::Degree(GetSignedAngle(projectedOnXZPlane, Ogre::Vector3::NEGATIVE_UNIT_Z));
        camRot.FromAngleAxis(rot, Ogre::Vector3::UNIT_Y);

        Ogre::Vector3 camOffset = carPos + camRot * additionalRotYaw * cameraDisplacement;

        Ogre::Vector3 ray = camOffset - carPos;

#if 1
        //perform collision correction
        {
            const float prolongFactor = 1.2f;
            Ogre::Vector3 camOffsetProlong = carPos + camRot * additionalRotYaw * (cameraDisplacement * prolongFactor);
            Ogre::Vector3 collisionPoint;
            Ogre::Vector3 collisionNormal;
            bool isContactsHappen = checkRayInBetween(carPos, camOffsetProlong, collisionPoint, collisionNormal);
            if(isContactsHappen)
            {

                Ogre::Vector3 collisionPointStub;
                Ogre::Vector3 collisionNormalStub;
                Ogre::Vector3 positionAlongNormal = collisionPoint + collisionNormal;
                bool isContactsHappenNormal = checkRayInBetween(carPos, positionAlongNormal, collisionPointStub, collisionNormalStub);
                if(!isContactsHappenNormal)
                {
                    collisionPoint = positionAlongNormal;
                }
                
                Ogre::Vector3 distanceFromCarToCollision = collisionPoint - carPos;

                if(isContactsHappenNormal)
                {
                    Ogre::Vector3 distanceFromCarToCollision = collisionPoint - carPos;
                    
                    //not too close
                    const float minDistanceThreshold = 15.0f;
                    float distanceFromCarToCollisionLength = distanceFromCarToCollision.length();
                    if(distanceFromCarToCollisionLength < minDistanceThreshold)
                    {
                        distanceFromCarToCollision *= (minDistanceThreshold / distanceFromCarToCollisionLength);
                    }
                }


                ray = distanceFromCarToCollision * 0.9f;
            }
        }
#endif


        mCamera->setOrientation(Ogre::Quaternion::IDENTITY);
        mCamera->setPosition(carPos + ray);
        mCamera->lookAt(carPos + targetOffset);

    }
    else
    {
        mCamera->setPosition(carPos);
        mCamera->lookAt(carPos + carRot * Ogre::Vector3(0.0f, 0.0f, -20.0f));
    }



    if(mRearCamera)
    {
        Ogre::Quaternion rotationRear(Ogre::Quaternion::IDENTITY);
        rotationRear.FromAngleAxis(Ogre::Degree(180.0f), Ogre::Vector3::UNIT_Y);
        mRearCamera->setOrientation(carRot * rotationRear);
        mRearCamera->setPosition(carPos + carRot * Ogre::Vector3(0.0f, 10.0f, 0.0f));
    }
}

bool CameraMan::checkRayInBetween(const Ogre::Vector3& From, const Ogre::Vector3& To, Ogre::Vector3& collisionPoint, Ogre::Vector3& collisionNormal)const
{
    Ogre::Vector3 shift = To - From;

    Ogre::Real shiftAmount = shift.length();

    bool res = false;

    Ogre::Real minimalFraction = 1.0f;

    OgreBulletCollisions::CollisionAllRayResultCallback callback(Ogre::Ray(From, shift.normalisedCopy()), mWorld, shiftAmount);
    mWorld->launchRay(callback);
    if(callback.doesCollide())
    {
        btCollisionWorld::AllHitsRayResultCallback * bulletCallback = callback.getBulletAllRayResultCallback();
        if(bulletCallback)
        {
            for(int q = 0; q < bulletCallback->m_collisionObjects.size(); ++q)
            {
                const btRigidBody* object = static_cast<const btRigidBody*>(bulletCallback->m_collisionObjects[q]);
                if(object && object->getInvMass() == 0.0f)//collision only with static
                {
                    res = true;
                    if(bulletCallback->m_hitFractions[q] < minimalFraction)
                    {
                        minimalFraction = bulletCallback->m_hitFractions[q];
                        collisionPoint = OgreBulletCollisions::convert(bulletCallback->m_hitPointWorld[q]);
                        collisionNormal = OgreBulletCollisions::convert(bulletCallback->m_hitNormalWorld[q]);
                    }
                }
            }
        }
        
    }

    return res;
}