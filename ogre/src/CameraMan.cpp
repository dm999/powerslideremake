
#include "CameraMan.h"

#include "physics/InitialVehicleSetup.h"

CameraMan::CameraMan(Ogre::Camera* cam) 
: mRearCamera(NULL),
    mCamera(cam),
    mCamPositonType(CameraPosition_ChassisC),
    mCamTypeSwitched(true)
{
    mSpeedToFOV.addPoint(120.0f, 85.0f);
    mSpeedToFOV.addPoint(250.0f, 100.0f);
}

void CameraMan::setYawPitchDist(const InitialVehicleSetup& initialVehicleSetup, const Ogre::Quaternion& carRot, const Ogre::Vector3& carPos, Ogre::Real lateralVelocity, Ogre::Real alignedVelocity)
{

    Ogre::Matrix3 carRotMatrix;

    carRot.ToRotationMatrix(carRotMatrix);
    Ogre::Vector3 carRotV[3];//original data is left hand
    carRotV[0] = Ogre::Vector3(carRotMatrix[0][0], carRotMatrix[1][0], -carRotMatrix[2][0]);
    carRotV[1] = Ogre::Vector3(carRotMatrix[0][1], carRotMatrix[1][1], -carRotMatrix[2][1]);
    carRotV[2] = Ogre::Vector3(-carRotMatrix[0][2], -carRotMatrix[1][2], carRotMatrix[2][2]);

    Ogre::Matrix3 carRotPS;
    carRotPS.FromAxes(carRotV[0], carRotV[1], carRotV[2]);

    Ogre::Vector3 camPos (initialVehicleSetup.mCarGlobalPos);
    camPos.z = -camPos.z;//original data is left hand

    if(mCamPositonType != CameraPosition_Bumper)
    {
        if(mCamTypeSwitched)
        {
            recalcCamParams(initialVehicleSetup);

        }
        else
        {
        }

        if(false)//d.polubotko: TODO - check collision with static
        {
        }

        if(mCamTypeSwitched)
        {
            mCamTypeSwitched = false;
        }
        else
        {
        }
    }
    else
    {
        Ogre::Vector3 cog(initialVehicleSetup.mCOG);
        cog.z = -cog.z;//original data is left hand

        Ogre::Vector3 cockpit(initialVehicleSetup.mCockpit);
        cockpit.z = -cockpit.z;//original data is left hand

        Ogre::Vector3 cogGlobalRot(carRotPS * cog);

        Ogre::Vector3 cogGlobal(camPos + cogGlobalRot);
        camPos = cogGlobal + carRotPS * cockpit;
    }

    carRotV[1].z = -carRotV[1].z;//original data is left hand
    carRotV[2].x = -carRotV[2].x;//original data is left hand
    carRotV[2].y = -carRotV[2].y;//original data is left hand
    Ogre::Vector3 rotMatrixAxisX = carRotV[1].crossProduct(carRotV[2]);
    carRotMatrix.SetColumn(0, rotMatrixAxisX);
    carRotMatrix.SetColumn(1, carRotV[1]);
    carRotMatrix.SetColumn(2, carRotV[2]);

    camPos.z = -camPos.z;//original data is left hand


    Ogre::Quaternion camRot;
    camRot.FromRotationMatrix(carRotMatrix);
    mCamera->setOrientation(camRot);
    mCamera->setPosition(camPos);

    float fov = mSpeedToFOV.getVal(alignedVelocity);
    if(mCamera)
        mCamera->setFOVy(Ogre::Degree(fov));



    if(mRearCamera)
    {
        Ogre::Quaternion rotationRear(Ogre::Quaternion::IDENTITY);
        rotationRear.FromAngleAxis(Ogre::Degree(180.0f), Ogre::Vector3::UNIT_Y);
        mRearCamera->setOrientation(carRot * rotationRear);
        mRearCamera->setPosition(carPos + carRot * Ogre::Vector3(0.0f, 10.0f, 0.0f));
    }
}

void CameraMan::setCameraPositionType(const CameraPositions& type)
{
    mCamTypeSwitched = true;
    mCamPositonType = type;
}

void CameraMan::recalcCamParams(const InitialVehicleSetup& initialVehicleSetup)
{

    Ogre::Real someVal;
    Ogre::Real someVal2;

    if(mCamPositonType == CameraPosition_ChassisA)
    {
        someVal = 18.5f;
        someVal2 = 10.0f;
        mCamParam2D = Ogre::Vector2(6.0f, 26.5f);
        mCamParam3D = Ogre::Vector3(0.6f, 0.8f, 0.714286f);
    }
    
    if(mCamPositonType == CameraPosition_ChassisB)
    {
        someVal = 15.5f;
        someVal2 = 10.0f;
        mCamParam2D = Ogre::Vector2(10.0f, 26.5f);
        mCamParam3D = Ogre::Vector3(0.6f, 0.8f, 0.714286f);
    }

    if(mCamPositonType == CameraPosition_ChassisC)
    {
        someVal = 10.6f;
        someVal2 = 0.0f;
        mCamParam2D = Ogre::Vector2(10.5f, 19.0f);
        mCamParam3D = Ogre::Vector3(0.6f, 0.9f, 0.909091f);
    }

    someVal -= 5.0f;
    mCamParam2D.x -= 3.0f;

    if(mCamPositonType == CameraPosition_ChassisA)
    {
        someVal -= 2.0f;
        mCamParam2D.x += 1.0f;
    }

    Ogre::Vector3 coreBase(initialVehicleSetup.mCoreBase);
    coreBase.z = -coreBase.z;//original data is left hand

    Ogre::Vector3 COG(initialVehicleSetup.mCOG);
    COG.z = -COG.z;//original data is left hand

    mCameraOffset = coreBase - COG;
    mCamParam = Ogre::Vector3(mCameraOffset.x, mCamParam2D.x - mCameraOffset.y, -mCamParam2D.y - mCameraOffset.z).length();
    mCamAngle = atan2(someVal - mCamParam2D.x, someVal2 + mCamParam2D.y) +
                atan2(mCamParam2D.x, mCamParam2D.y);
}