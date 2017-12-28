
#include "CameraMan.h"

#include "physics/InitialVehicleSetup.h"
#include "physics/PhysicsVehicle.h"

CameraMan::CameraMan(Ogre::Camera* cam) 
: mRearCamera(NULL),
    mCamera(cam),
    mCamPositonType(CameraPosition_ChassisB),
    mCamTypeSwitched(true)
{}

void CameraMan::setYawPitchDist(const InitialVehicleSetup& initialVehicleSetup, const Ogre::Quaternion& carRot)
{

    Ogre::Matrix3 carRotMatrix;

    carRot.ToRotationMatrix(carRotMatrix);
    Ogre::Vector3 carRotV[3];//original data is left hand
    carRotV[0] = Ogre::Vector3(carRotMatrix[0][0], carRotMatrix[1][0], -carRotMatrix[2][0]);
    carRotV[1] = Ogre::Vector3(carRotMatrix[0][1], carRotMatrix[1][1], -carRotMatrix[2][1]);
    carRotV[2] = Ogre::Vector3(-carRotMatrix[0][2], -carRotMatrix[1][2], carRotMatrix[2][2]);

    Ogre::Matrix3 carRotPS;
    carRotPS.FromAxes(carRotV[0], carRotV[1], carRotV[2]);

    Ogre::Vector3 carPos (initialVehicleSetup.mCarGlobalPos);
    carPos.z = -carPos.z;//original data is left hand

    Ogre::Vector3 camPos (mCamera->getPosition());
    camPos.z = -camPos.z;//original data is left hand

    Ogre::Vector3 cog(initialVehicleSetup.mCOG);
    cog.z = -cog.z;//original data is left hand

    Ogre::Vector3 cogRot(carRotPS * cog);

    Ogre::Vector3 cogGlobal(carPos + cogRot);

    if(mCamPositonType != CameraPosition_Bumper)
    {
        if(mCamTypeSwitched)
        {
            recalcCamParams(initialVehicleSetup);

            mCamRotZ.x = carRotV[2].x;
            mCamRotZ.z = carRotV[2].z;
        }
        else
        {
            Ogre::Vector3 shift = cogGlobal - mCamShift;
            shift.normalise();

            Ogre::Real diff = Ogre::Math::Abs(acos(mCamRotZ_y) - acos(carRotV[2].y));
            if(diff < 0.01f)
                diff = 0.01f;
            diff = 1.0f - diff * -100.0f;
            mCamRotZ.x = shift.x - mCamParam3D.z * carRotV[2].x * -2.0f / diff;
            mCamRotZ.z = shift.z - mCamParam3D.z * carRotV[2].z * -2.0f / diff;
        }

        if(mCamRotZ.x != 0.0f || mCamRotZ.z != 0.0f)
        {
            mCamRotZ.y = 0.0f;
            mCamRotZ.normalise();
        }
        else
        {
            mCamRotZ = Ogre::Vector3::UNIT_X;
        }

        mCamRotZ_y = carRotV[2].y;
        mCamShift = cogGlobal - mCamRotZ * 50.0f;

        Ogre::Vector3 diff = cogGlobal - mCamRotZ * mCamParam2D.y;

        Ogre::Vector3 camVal(cogGlobal);
        camVal.x += carRotV[0].x * mCameraOffset.x;
        camVal.x += carRotV[1].x * mCameraOffset.y;
        camVal.x += carRotV[2].x * mCameraOffset.z;

        camVal.y += carRotV[0].y * mCameraOffset.x;
        camVal.y += carRotV[1].y * mCameraOffset.y;
        camVal.y += carRotV[2].y * mCameraOffset.z;

        camVal.z += carRotV[0].z * mCameraOffset.x;
        camVal.z += carRotV[1].z * mCameraOffset.y;
        camVal.z += carRotV[2].z * mCameraOffset.z;

        Ogre::Vector3 camDiff = diff + Ogre::Vector3(0.0f, mCamParam2D.x , 0.0f) - camVal;
        camDiff.normalise();

        Ogre::Vector3 camValue;

        if(false)//d.polubotko: TODO - check collision with static
        {
        }
        else
        {
            camValue = camDiff * mCamParam + camVal;
        }

        camVal.x -= carRotV[0].x * mCameraOffset.x;
        camVal.x -= carRotV[1].x * mCameraOffset.y;
        camVal.x -= carRotV[2].x * mCameraOffset.z;

        camVal.y -= carRotV[0].y * mCameraOffset.x;
        camVal.y -= carRotV[1].y * mCameraOffset.y;
        camVal.y -= carRotV[2].y * mCameraOffset.z;

        camVal.z -= carRotV[0].z * mCameraOffset.x;
        camVal.z -= carRotV[1].z * mCameraOffset.y;
        camVal.z -= carRotV[2].z * mCameraOffset.z;

        Ogre::Vector3 rotMatrixAxis = camVal - camValue;
        rotMatrixAxis.normalise();

        Ogre::Vector3 camAxis(rotMatrixAxis.z, 0.0f, -rotMatrixAxis.x);
        camAxis.normalise();

        PhysicsVehicle::createRotMatrix(rotMatrixAxis, camAxis, mCamAngle);
        rotMatrixAxis.normalise();

        Ogre::Vector3 camDiff2(-rotMatrixAxis.z, 0.0f, rotMatrixAxis.x);
        camDiff2.normalise();

        Ogre::Vector3 camAxis2 = camDiff2.crossProduct(rotMatrixAxis);

        if(mCamTypeSwitched)
        {
            mCamTypeSwitched = false;
            camPos = camValue;
            mCamRot[0] = camDiff2;
            mCamRot[1] = camAxis2;
            mCamRot[2] = rotMatrixAxis;
        }
        else
        {
            camPos += (camValue - camPos) * mCamParam3D.x;
            mCamRot[1] += (camAxis2 - mCamRot[1]) * mCamParam3D.y;
            mCamRot[2] += (rotMatrixAxis - mCamRot[2]) * mCamParam3D.y;
            mCamRot[2].normalise();
            mCamRot[0] = mCamRot[1].crossProduct(mCamRot[2]);
            mCamRot[0].normalise();
            mCamRot[1] = mCamRot[2].crossProduct(mCamRot[0]);
            mCamRot[1].normalise();
        }
    }
    else
    {
        Ogre::Vector3 cockpit(initialVehicleSetup.mCockpit);
        cockpit.z = -cockpit.z;//original data is left hand

        camPos = cogGlobal + carRotPS * cockpit;
        mCamRot[0] = carRotV[0];
        mCamRot[1] = carRotV[1];
        mCamRot[2] = carRotV[2];
    }

    Ogre::Vector3 camRotTmp[3];
    camRotTmp[0] = mCamRot[0];
    camRotTmp[1] = mCamRot[1];
    camRotTmp[2] = mCamRot[2];

    camRotTmp[0].z = -camRotTmp[0].z;//original data is left hand
    camRotTmp[1].z = -camRotTmp[1].z;//original data is left hand
    camRotTmp[2].x = -camRotTmp[2].x;//original data is left hand
    camRotTmp[2].y = -camRotTmp[2].y;//original data is left hand

    camPos.z = -camPos.z;//original data is left hand

    Ogre::Quaternion camRot;
    camRot.FromAxes(camRotTmp[0], camRotTmp[1], camRotTmp[2]);
    mCamera->setOrientation(camRot);
    mCamera->setPosition(camPos);

    if(mRearCamera)
    {
        Ogre::Quaternion rotationRear(Ogre::Quaternion::IDENTITY);
        rotationRear.FromAngleAxis(Ogre::Degree(180.0f), Ogre::Vector3::UNIT_Y);
        mRearCamera->setOrientation(carRot * rotationRear);
        mRearCamera->setPosition(camPos + carRot * Ogre::Vector3(0.0f, 10.0f, 0.0f));
    }
}

void CameraMan::setCameraPositionType(const CameraPositions& type)
{
    mCamTypeSwitched = true;
    mCamPositonType = type;
}

void CameraMan::recalcCamParams(const InitialVehicleSetup& initialVehicleSetup)
{

    mCamRot[0] = Ogre::Vector3::ZERO;
    mCamRot[1] = Ogre::Vector3::ZERO;
    mCamRot[2] = Ogre::Vector3::ZERO;

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

    //for software renderer
#if 0
    someVal -= 5.0f;
    mCamParam2D.x -= 3.0f;

    if(mCamPositonType == CameraPosition_ChassisA)
    {
        someVal -= 2.0f;
        mCamParam2D.x += 1.0f;
    }
#endif
    Ogre::Vector3 coreBase(initialVehicleSetup.mCoreBase);
    coreBase.z = -coreBase.z;//original data is left hand

    Ogre::Vector3 COG(initialVehicleSetup.mCOG);
    COG.z = -COG.z;//original data is left hand

    mCameraOffset = coreBase - COG;
    mCamParam = Ogre::Vector3(mCameraOffset.x, mCamParam2D.x - mCameraOffset.y, -mCamParam2D.y - mCameraOffset.z).length();
    mCamAngle = atan2(someVal - mCamParam2D.x, someVal2 + mCamParam2D.y) +
                atan2(mCamParam2D.x, mCamParam2D.y);
}