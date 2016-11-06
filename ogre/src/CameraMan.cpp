#include "pcheader.h"

#include "CameraMan.h"
#include "Conversions.h"
#include "Tools.h"
#include "BaseApp.h"

#include "BulletCollision/CollisionDispatch/btGhostObject.h"

CameraMan::CameraMan(Ogre::Camera* cam, OgreBulletDynamics::DynamicsWorld * world, Ogre::SceneManager* sceneMgr) 
: mWorld(world),
    mRearCamera(NULL),
    mCamera(cam),
    mCamPositonType(CameraPosition_ChassisC)
{
}

void CameraMan::setYawPitchDist(const Ogre::Quaternion& carRot, const Ogre::Vector3& carPos, Ogre::Real lateralVelocity, Ogre::SceneNode * const target)
{
    Ogre::Vector3 cameraDisplacement(0, 14.0f, 20.0f);

    if(mCamPositonType == CameraPosition_Bumper)
    {
        cameraDisplacement.y = 6.0f;
        cameraDisplacement.z = 0.0f;
    }
    if(mCamPositonType == CameraPosition_ChassisB)
    {
        cameraDisplacement.z = 16.0f;
    }
    if(mCamPositonType == CameraPosition_ChassisC)
    {
        cameraDisplacement.y = 18.0f;
    }

    Ogre::Vector3 forwardAxis = carRot * Ogre::Vector3::NEGATIVE_UNIT_Z;

    //adjust camera according to the normals (pitch), velocity (yaw)
    Ogre::Vector3 carTop = carRot * Ogre::Vector3::UNIT_Y;

    Ogre::Degree chassisIncline = forwardAxis.angleBetween(Ogre::Vector3::UNIT_Y);
    Ogre::Real chassisInclineDegrees = chassisIncline.valueDegrees();
    chassisInclineDegrees -= 70.0f;

    Ogre::Quaternion additionalRotPitch;
    if(carTop.y < 0.0f) //  turn over
        additionalRotPitch.FromAngleAxis(Ogre::Degree(-90.0f * carTop.y), Ogre::Vector3::UNIT_X);
    else
        additionalRotPitch.FromAngleAxis(Ogre::Degree(carTop.y * chassisInclineDegrees), Ogre::Vector3::UNIT_X);

    Ogre::Quaternion additionalRotYaw;
    additionalRotYaw.FromAngleAxis(Ogre::Degree(lateralVelocity / 20.0f), Ogre::Vector3::UNIT_Y);

    Ogre::Vector3 camOffset = carPos + carRot * additionalRotYaw * additionalRotPitch * cameraDisplacement;

    Ogre::Vector3 ray = camOffset - carPos;


    if(mCamPositonType != CameraPosition_Bumper)
    {
        //perform collision correction
        Ogre::Vector3 collisionPoint;
        bool isContactsHappen = checkRayInBetween(carPos, camOffset, collisionPoint);
        if(isContactsHappen)
        {
            ray = (collisionPoint - carPos) * 0.9f;
        }




        Ogre::Vector3 targetOffset(0.0f, 10.0f, 0.0f);

        if(mCamPositonType == CameraPosition_ChassisB)
        {
            targetOffset = Ogre::Vector3::ZERO;
        }

        mCamera->setAutoTracking(true, target, targetOffset);
        mCamera->setOrientation(Ogre::Quaternion::IDENTITY);
        mCamera->setPosition(carPos + ray);

    }
    else
    {
        mCamera->setAutoTracking(false);
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

bool CameraMan::checkRayInBetween(const Ogre::Vector3& From, const Ogre::Vector3& To, Ogre::Vector3& collisionPoint)const
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
                    }
                }
            }
        }
        
    }

    return res;
}