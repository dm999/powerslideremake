
#include "CameraMan.h"

#include "physics/InitialVehicleSetup.h"
#include "physics/PhysicsVehicle.h"

#include "mesh/StaticMeshProcesser.h"

CameraMan::CameraMan(Ogre::Camera* cam, StaticMeshProcesser& staticMeshProcesser, CameraPositions camPos) 
: mRearCamera(NULL),
    mCamera(cam),
    mCamPositonType(camPos),
    mStaticMeshProcesser(staticMeshProcesser),
    mCamTypeSwitched(true)
{}

void CameraMan::setYawPitchDist(const InitialVehicleSetup& initialVehicleSetup,
    Ogre::Light * globalLight, Ogre::Light * shadowLight)
{

    Ogre::Matrix3 carRotMatrix;

    initialVehicleSetup.mCarRot.ToRotationMatrix(carRotMatrix);
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

    Ogre::Vector3 cogGlobal(initialVehicleSetup.mCOGGlobal);
    cogGlobal.z = -cogGlobal.z;//original data is left hand

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

        bool isCollisionFound = false;
        short collisionApproach = 0;
        Ogre::Vector3 collisionPoint;
        std::vector<std::pair<short, short> > averaging;
        averaging.reserve(15);
        do
        {
            short partIndex;
            short triangleIndex;
            Ogre::Vector3 rayDir = camDiff * (mCamParam + 7.0f);
            bool isCollided = mStaticMeshProcesser.performPointCollisionDetection(Ogre::Ray(camVal, rayDir), collisionPoint, partIndex, triangleIndex);
            if(!isCollided)
            {
                isCollisionFound = false;
                break;
            }

            if(!averaging.empty())
            {
                short prevPartIndex = averaging[averaging.size() - 1].first;
                short prevTriangleIndex = averaging[averaging.size() - 1].second;
                if(partIndex != prevPartIndex && triangleIndex != prevTriangleIndex)
                {
                    Ogre::int32 averageIndex = averaging.size() - 2;
                    if(averageIndex >= 0)
                    {
                        do
                        {
                            if(partIndex == averaging[averageIndex].first && triangleIndex == averaging[averageIndex].second)
                                break;
                            --averageIndex;
                        }while(averageIndex >= 0);

                        if(averageIndex >= 0)
                        {
                            isCollisionFound = true;
                            break;
                        }
                    }
                    averaging.push_back(std::make_pair(partIndex, triangleIndex));
                }
            }
            else
            {
                averaging.push_back(std::make_pair(partIndex, triangleIndex));
            }

            isCollisionFound = true;

            Ogre::Vector3 pointOnStaticA, pointOnStaticB, pointOnStaticC;
            mStaticMeshProcesser.getGeoverts(partIndex, triangleIndex, pointOnStaticA, pointOnStaticC, pointOnStaticB);

            Ogre::Vector3 normal = Ogre::Vector3(pointOnStaticB - pointOnStaticA).crossProduct(Ogre::Vector3(pointOnStaticC - pointOnStaticA));
            normal.normalise();

            Ogre::Real dotP = Ogre::Math::Abs(camVal.dotProduct(normal) - pointOnStaticA.dotProduct(normal));

            Ogre::Vector3 camProj = camVal - dotP * normal;
            Ogre::Vector3 camValDiff = camVal - camProj;
            Ogre::Real camValSqrt = Ogre::Math::Sqrt(((mCamParam + 7.0f) * (mCamParam + 7.0f)) - camValDiff.dotProduct(camValDiff));

            Ogre::Vector3 camCollisionPointDiff = collisionPoint - camProj;
            Ogre::Real camCollsisionPointDiffDotP = camCollisionPointDiff.dotProduct(camCollisionPointDiff);
            if(camCollsisionPointDiffDotP < 0.01f)break;

            camCollisionPointDiff /= Ogre::Math::Sqrt(camCollsisionPointDiffDotP);

            Ogre::Vector3 camSomeAxis = camValSqrt * camCollisionPointDiff + camProj;

            Ogre::Vector3 normalAbs = Ogre::Vector3(Ogre::Math::Abs(normal.x), Ogre::Math::Abs(normal.y), Ogre::Math::Abs(normal.z));

            Ogre::Vector2 paramA, paramC, paramE, paramG, paramI;

            if(normalAbs.x <= normalAbs.y || normalAbs.x <= normalAbs.z)//y,z max
            {
                if(normalAbs.y > normalAbs.z)//y max
                {
                    if(normal.y <= 0.0f)
                    {
                        paramA.x = pointOnStaticA.x;
                        paramA.y = pointOnStaticA.z;
                        paramC.x = pointOnStaticB.x;
                        paramC.y = pointOnStaticB.z;
                        paramE.x = pointOnStaticC.x;
                        paramE.y = pointOnStaticC.z;
                        paramG.y = camSomeAxis.z;
                        paramG.x = camSomeAxis.x;
                        paramI.y = collisionPoint.z;
                        paramI.x = collisionPoint.x;
                    }
                    else
                    {
                        paramA.x = pointOnStaticA.z;
                        paramA.y = pointOnStaticA.x;
                        paramC.x = pointOnStaticB.z;
                        paramC.y = pointOnStaticB.x;
                        paramE.x = pointOnStaticC.z;
                        paramE.y = pointOnStaticC.x;
                        paramG.y = camSomeAxis.x;
                        paramG.x = camSomeAxis.z;
                        paramI.y = collisionPoint.x;
                        paramI.x = collisionPoint.z;
                    }
                }
                else//z max
                {
                    if(normal.z <= 0.0f)
                    {
                        paramA.x = pointOnStaticA.y;
                        paramA.y = pointOnStaticA.x;
                        paramC.x = pointOnStaticB.y;
                        paramC.y = pointOnStaticB.x;
                        paramE.x = pointOnStaticC.y;
                        paramE.y = pointOnStaticC.x;
                        paramG.y = camSomeAxis.x;
                        paramG.x = camSomeAxis.y;
                        paramI.y = collisionPoint.x;
                        paramI.x = collisionPoint.y;
                    }
                    else
                    {
                        paramA.x = pointOnStaticA.x;
                        paramA.y = pointOnStaticA.y;
                        paramC.x = pointOnStaticB.x;
                        paramC.y = pointOnStaticB.y;
                        paramE.x = pointOnStaticC.x;
                        paramE.y = pointOnStaticC.y;
                        paramG.y = camSomeAxis.y;
                        paramG.x = camSomeAxis.x;
                        paramI.y = collisionPoint.y;
                        paramI.x = collisionPoint.x;
                    }
                }
            }
            else//x max
            {
                if(normal.x > 0.0f)
                {
                    paramA.x = pointOnStaticA.y;
                    paramA.y = pointOnStaticA.z;
                    paramC.x = pointOnStaticB.y;
                    paramC.y = pointOnStaticB.z;
                    paramE.x = pointOnStaticC.y;
                    paramE.y = pointOnStaticC.z;
                    paramG.y = camSomeAxis.z;
                    paramG.x = camSomeAxis.y;
                    paramI.y = collisionPoint.z;
                    paramI.x = collisionPoint.y;
                }
                else
                {
                    paramA.x = pointOnStaticA.z;
                    paramA.y = pointOnStaticA.y;
                    paramC.x = pointOnStaticB.z;
                    paramC.y = pointOnStaticB.y;
                    paramE.x = pointOnStaticC.z;
                    paramE.y = pointOnStaticC.y;
                    paramG.y = camSomeAxis.y;
                    paramG.x = camSomeAxis.z;
                    paramI.y = collisionPoint.y;
                    paramI.x = collisionPoint.z;
                }
            }

            Ogre::Vector2 compareRes;
            if(
                compareCamParams(paramA, paramC, paramI, paramG, compareRes)    ||
                compareCamParams(paramC, paramE, paramI, paramG, compareRes)    ||
                compareCamParams(paramE, paramA, paramI, paramG, compareRes)
                )
            {
                Ogre::Vector2 diffResI = paramI - compareRes;
                Ogre::Vector2 diffResIG = paramI - paramG;

                Ogre::Real diffSqrt = Ogre::Math::Sqrt((diffResI.x * diffResI.x + diffResI.y * diffResI.y) /
                    (diffResIG.x * diffResIG.x + diffResIG.y * diffResIG.y) * diffResIG.y);

                Ogre::Vector3 newSomeAxis = (camSomeAxis - collisionPoint) * diffSqrt + collisionPoint - camCollisionPointDiff * -0.5f;
                Ogre::Vector3 newSomeAxisDiff = collisionPoint - newSomeAxis;
                Ogre::Vector3 someAxisDiff = collisionPoint - camSomeAxis;

                if(someAxisDiff.dotProduct(someAxisDiff) > newSomeAxisDiff.dotProduct(newSomeAxisDiff))
                {
                    camSomeAxis = newSomeAxis;
                }

            }

            camDiff = camSomeAxis - camVal;
            camDiff.normalise();

            ++collisionApproach;

        }while(collisionApproach < 15);

        if(isCollisionFound)
        {
            Ogre::Vector3 camCollisionPointDiff = collisionPoint - camVal;
            camCollisionPointDiff.normalise();
            camValue = collisionPoint - camCollisionPointDiff * 7.0f;
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
        mRearCamera->setOrientation(initialVehicleSetup.mCarRot * rotationRear);
        mRearCamera->setPosition(camPos + initialVehicleSetup.mCarRot * Ogre::Vector3(0.0f, 5.0f, -10.0f));
    }

    //shadow cam
    if(globalLight && shadowLight)
    {
        const Ogre::Real shadowLightDistanceFromCar = 40.0f;

        Ogre::Vector3 lightPos = globalLight->getDerivedPosition();
        Ogre::Vector3 lightDir = initialVehicleSetup.mCarGlobalPos - lightPos;
        lightDir.normalise();

        Ogre::Vector3 shadowLightPos = initialVehicleSetup.mCarGlobalPos + (-lightDir) * shadowLightDistanceFromCar;
        shadowLight->setPosition(shadowLightPos);
        shadowLight->setDirection(lightDir);
    }
}

bool CameraMan::compareCamParams(Ogre::Vector2 paramA, Ogre::Vector2 paramB, Ogre::Vector2 paramC, Ogre::Vector2 paramD, Ogre::Vector2& res) const
{
    bool ret = false;

    Ogre::Vector2 diffBA = paramB - paramA;
    Ogre::Vector2 diffCD = paramC - paramD;
    Ogre::Vector2 diffAC = paramA - paramC;

    Ogre::Vector2 switcher, switcher2;

    if(diffBA.x < 0.0f)
    {
        switcher.x = paramB.x;
        switcher2.x = paramA.x;
    }
    else
    {
        switcher.x = paramA.x;
        switcher2.x = paramB.x;
    }

    if(diffCD.x <= 0.0f)
    {
        if (switcher2.x < paramC.x || paramD.x < switcher.x)
            return false;
    }
    else
    {
        if (switcher2.x < paramD.x)
            return false;
        if (paramC.x < switcher.x)
            return false;
    }

    if(diffBA.y < 0.0f)
    {
        switcher.y = paramB.y;
        switcher2.y = paramA.y;
    }
    else
    {
        switcher.y = paramA.y;
        switcher2.y = paramB.y;
    }

    if(diffCD.y <= 0.0f)
    {
        if (switcher2.y < paramC.y || paramD.y < switcher.y)
            return false;
    }
    else
    {
        if (switcher2.y < paramD.y)
            return false;
        if (paramC.y < switcher.y)
            return false;
    }

    Ogre::Real dotP_1 = diffBA.y * diffCD.x - diffCD.y * diffBA.x;
    Ogre::Real dotP_2 = diffCD.y * diffAC.x - diffAC.y * diffCD.x;

    if(dotP_1 <= 0.0f)
    {
        if (dotP_2 > 0.0f || dotP_2 < dotP_1)
            return false;
    }
    else
    {
        if (dotP_2 < 0.0f)
            return false;
        if (dotP_2 > dotP_1)
            return false;
    }

    Ogre::Real dotP_3 = diffAC.y * diffBA.x - diffBA.y * diffAC.x;

    if(dotP_1 <= 0.0f)
    {
        if (dotP_3 > 0.0f || dotP_3 < dotP_1)
            return false;
    }
    else
    {
        if (dotP_3 < 0.0f)
            return false;
        if (dotP_3 > dotP_1)
            return false;
    }

    if(dotP_1 != 0.0f)
    {
        ret = true;
        res.x = dotP_2 * diffBA.x / dotP_1 + paramA.x;
        res.y = dotP_2 * diffBA.y / dotP_1 + paramA.y;
    }


    return ret;
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

    Ogre::Real someVal = 0.0f;
    Ogre::Real someVal2 = 0.0f;

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