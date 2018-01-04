
#include "Physics.h"

#include "../mesh/StaticMeshProcesser.h"

#include "PhysicsVehicle.h"
#include "PhysicsVehicleAI.h"

#include "../listeners/PhysicsListener.h"


Physics::Physics(StaticMeshProcesser * meshProesser)
    : 
    mMeshProesser(meshProesser),
    mTimeStep(-1),
    mTimeStep2(0)
{}

Physics::~Physics()
{}

void Physics::timeStep(GameState& gameState)
{
    Ogre::Real timeFromStartSeconds = mTimer.getMilliseconds() / 1000.0f;

    mTimeStep2 = static_cast<Ogre::int32>(timeFromStartSeconds * 60.0f + 1.0f);

    if(mTimeStep2 - mTimeStep > 15 || mTimeStep2 < mTimeStep)
        mTimeStep = mTimeStep2 - 15;

    if(mTimeStep < mTimeStep2)
    {
        do
        {
            internalTimeStep(gameState);
            ++mTimeStep;
        }while(mTimeStep < mTimeStep2);
    }
}

void Physics::internalTimeStep(GameState& gameState)
{
    for (physicsListener::iterator i = mListeners.begin(), j = mListeners.end(); i != j; ++i)
    {
        (*i)->timeStepBefore(this);
    }

    if(gameState.getRaceStarted() && !gameState.isGamePaused())
    {
        for (vehicles::iterator i = mVehicles.begin(), j = mVehicles.end(); i != j; ++i)
        {
            (*i).second->timeStep(gameState);
            (*i).first->processCamera(gameState);
            processCarsCollisions((*i).second.get());

            for (physicsListener::iterator ii = mListeners.begin(), jj = mListeners.end(); ii != jj; ++ii)
            {
                (*ii)->timeStepForVehicle((*i).second.get(), mVehicles);
            }
        }
    }

    if(!gameState.getRaceStarted() && !gameState.isGamePaused())
    {
        for (vehicles::iterator i = mVehicles.begin(), j = mVehicles.end(); i != j; ++i)
        {
            (*i).second->processEngineIdle();
            (*i).first->processCamera(gameState);

            for (physicsListener::iterator ii = mListeners.begin(), jj = mListeners.end(); ii != jj; ++ii)
            {
                (*ii)->timeStepForVehicle((*i).second.get(), mVehicles);
            }
        }
    }

    for (physicsListener::iterator i = mListeners.begin(), j = mListeners.end(); i != j; ++i)
    {
        (*i)->timeStepAfter(this);
    }

}

PhysicsVehicle* Physics::addVehicle(InitialVehicleSetup& initialVehicleSetup, PSBaseVehicle * vehiclePtr,
                        Ogre::SceneNode *wheelNodes[InitialVehicleSetup::mWheelsAmount], Ogre::SceneNode *chassis,
                        InputType type,
                        bool isAI)
{
    PhysicsVehicle* ret = NULL;

    vehicles::const_iterator found = mVehicles.find(vehiclePtr);

    if(found == mVehicles.end())
    {
        CommonIncludes::shared_ptr<PhysicsVehicle> vehicle;
        if(isAI)
            vehicle = CommonIncludes::shared_ptr<PhysicsVehicle>(new PhysicsVehicleAI(this, mMeshProesser, initialVehicleSetup, wheelNodes, chassis, itKeyboard, static_cast<PSAICar*>(vehiclePtr)));
        else
            vehicle = CommonIncludes::shared_ptr<PhysicsVehicle>(new PhysicsVehicle(this, mMeshProesser, initialVehicleSetup, wheelNodes, chassis, type));
        ret = vehicle.get();
        mVehicles.insert(std::make_pair(vehiclePtr, vehicle));
    }

    return ret;
}

void Physics::removeVehicle(PSBaseVehicle * vehiclePtr)
{
    vehicles::const_iterator found = mVehicles.find(vehiclePtr);
    mVehicles.erase(found);
}

const PhysicsVehicle * Physics::getVehicle(PSBaseVehicle * vehiclePtr) const
{
    const PhysicsVehicle * ret = NULL;
    vehicles::const_iterator found = mVehicles.find(vehiclePtr);
    if(found != mVehicles.end())
        ret = (*found).second.get();
    return ret;
}

PhysicsVehicle * Physics::getVehicle(PSBaseVehicle * vehiclePtr)
{
    PhysicsVehicle * ret = NULL;
    vehicles::const_iterator found = mVehicles.find(vehiclePtr);
    if(found != mVehicles.end())
        ret = (*found).second.get();
    return ret;
}

void Physics::addListener(PhysicsListener* listener)
{
    mListeners.push_back(listener);
}

void Physics::removeListener(PhysicsListener* listener)
{
    physicsListener::const_iterator found = std::find(mListeners.begin(), mListeners.end(), listener);
    if(found != mListeners.end())
        mListeners.erase(found);
}

void Physics::processCarsCollisions(PhysicsVehicle* vehicle)
{
    for (vehicles::iterator i = mVehicles.begin(), j = mVehicles.end(); i != j; ++i)
    {
        if((*i).second.get() != vehicle)//don`t collide with yourself
        {
            Ogre::Matrix3 carRotMatrix;
            Ogre::Vector3 carARotZ;//original data is left hand
            Ogre::Vector3 carBRotZ;//original data is left hand

            (*i).second->getVehicleSetup().mCarRot.ToRotationMatrix(carRotMatrix);
            carARotZ = Ogre::Vector3(-carRotMatrix[0][2], -carRotMatrix[1][2], carRotMatrix[2][2]);

            vehicle->getVehicleSetup().mCarRot.ToRotationMatrix(carRotMatrix);
            carBRotZ = Ogre::Vector3(-carRotMatrix[0][2], -carRotMatrix[1][2], carRotMatrix[2][2]);

            Ogre::Vector3 cogAGlobal((*i).second->getVehicleSetup().mCOGGlobal);
            cogAGlobal.z = -cogAGlobal.z;//original data is left hand

            Ogre::Vector3 cogBGlobal(vehicle->getVehicleSetup().mCOGGlobal);
            cogBGlobal.z = -cogBGlobal.z;//original data is left hand

            Ogre::Real collisionRadiusDiff = vehicle->getVehicleSetup().mCollisionRadius + 
                (*i).second->getVehicleSetup().mCollisionRadius - 2.0f + 1.0f;

            Ogre::Vector3 cogDiff(cogBGlobal - cogAGlobal);
            Ogre::Vector3 cogDiffAbs(Ogre::Math::Abs(cogDiff.x), Ogre::Math::Abs(cogDiff.y), Ogre::Math::Abs(cogDiff.z));
            if( collisionRadiusDiff >= cogDiffAbs.x &&
                collisionRadiusDiff >= cogDiffAbs.y &&
                collisionRadiusDiff >= cogDiffAbs.z &&
                collisionRadiusDiff * collisionRadiusDiff > cogDiff.squaredLength()
                )
            {
                Ogre::Real collisionRadiusDiffMore = vehicle->getVehicleSetup().mCollisionRadius + 
                (*i).second->getVehicleSetup().mCollisionRadius - 6.0f - 2.0f + 1.0f;
                collisionRadiusDiffMore *= collisionRadiusDiffMore;

                Ogre::Vector3 cogDiffSub = Ogre::Vector3::ZERO;
                Ogre::Vector3 cogDiffAdd = Ogre::Vector3::ZERO;
                Ogre::Real distance = 0.0f;
                Ogre::uint32 threshold = 0;
                Ogre::int8 counter = 0;

                Ogre::Vector3 aVal;
                Ogre::Vector3 bVal;
                do
                {
                    switch(counter)
                    {
                    case 0:
                        aVal = cogAGlobal - carARotZ * -3.0f;
                        bVal = cogBGlobal - carBRotZ * -3.0f;
                        break;
                    case 1:
                        bVal = cogBGlobal - carBRotZ * 3.0f;
                        break;
                    case 2:
                        aVal = cogAGlobal - carARotZ * 3.0f;
                        break;
                    case 3:
                        bVal = cogBGlobal - carBRotZ * -3.0f;
                        break;
                    default:
                        break;
                    }

                    Ogre::Vector3 diff(bVal - aVal);
                    Ogre::Real dotP = diff.dotProduct(diff);
                    if(dotP < collisionRadiusDiffMore)
                    {
                        cogDiffSub -= cogDiff * -0.5f;

                        if(dotP > distance)
                            distance = collisionRadiusDiffMore - dotP;

                        ++threshold;
                        cogDiffAdd += (cogDiff * -1.0f / Ogre::Math::Sqrt(dotP));
                    }

                    ++counter;
                }while(counter < 4);

                if(threshold > 0)
                {
                    cogDiffSub *= 1.0f / static_cast<float>(threshold);
                    cogDiffAdd.normalise();

                    Ogre::Vector3 impulseDiff(vehicle->getLinearImpulse() - (*i).second->getLinearImpulse());

                    Ogre::Real velocity = impulseDiff.dotProduct(cogDiffAdd);
                    if(velocity < 0.0f) velocity = 0.0f;

                    Ogre::Real impulseDiffReciprocalLen = impulseDiff.squaredLength();
                    if(impulseDiffReciprocalLen > 0.0f)
                        impulseDiffReciprocalLen = 1.0f / Ogre::Math::Sqrt(impulseDiffReciprocalLen);
                    impulseDiff.normalise();

                    Ogre::Real velocityMul = velocity * impulseDiffReciprocalLen * 0.7f;
                    cogDiffAdd = cogDiffAdd * (1.0f - velocityMul) + impulseDiff * velocityMul;
                    cogDiffAdd.normalise();

                    Ogre::Real vdv = mCollisionVDV.getPoint(velocity);
                    Ogre::Real ddv = mCollisionDDV.getPoint(distance) * vdv;
                    Ogre::Real dd = mCollisionDD.getPoint(distance) + ddv;
                    Ogre::Real finalImpulse = mCollisionVV.getPoint(velocity) + dd;
                    if(finalImpulse > 100.0f) finalImpulse = 100.0f;

                    (*i).second->adjustImpulseInc(cogDiffSub, cogDiffAdd * finalImpulse);
                }
            }

        }
    }
}