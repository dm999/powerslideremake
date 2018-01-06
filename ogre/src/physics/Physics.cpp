
#include "Physics.h"

#include "../mesh/StaticMeshProcesser.h"

#include "PhysicsVehicle.h"
#include "PhysicsVehicleAI.h"

#include "../listeners/PhysicsListener.h"


Physics::Physics(StaticMeshProcesser * meshProesser)
    : 
    mMeshProesser(meshProesser),
    mTimeStep(-1),
    mTimeStep2(0),
    mAfterStartCounter(0)
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

    if(!gameState.isGamePaused())
    {
        //process physics
        for (vehicles::iterator i = mVehicles.begin(), j = mVehicles.end(); i != j; ++i)
        {
            (*i).second->timeStep(gameState);
            (*i).first->processCamera(gameState);
        }

        //process collisions
        for (vehicles::iterator i = mVehicles.begin(), j = mVehicles.end(); i != j; ++i)
        {
               if(gameState.getRaceStarted())
                    processCarsCollisions((*i).second.get());
        }

        //process impulse weighter
        for (vehicles::iterator i = mVehicles.begin(), j = mVehicles.end(); i != j; ++i)
        {
            if(gameState.getRaceStarted())
            {
                processCollisionsImpulseWeighter((*i).second.get());
            }
            else
            {
                if((*i).second->getVehicleType() == AIVehicle)
                    (*i).second->getCarEngine().setEngineRPM(8000.0f);
                (*i).second->zeroImpulses();
            }
        }

        //process listeners
        for (vehicles::iterator i = mVehicles.begin(), j = mVehicles.end(); i != j; ++i)
        {
            if(gameState.getRaceStarted())
            {
                for (physicsListener::iterator ii = mListeners.begin(), jj = mListeners.end(); ii != jj; ++ii)
                {
                    (*ii)->timeStepForVehicle((*i).second.get(), mVehicles);
                }
            }
        }

        if(gameState.getRaceStarted())
            ++mAfterStartCounter;
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
            Ogre::Vector3 carARotX;//original data is left hand
            Ogre::Vector3 carARotZ;//original data is left hand
            Ogre::Vector3 carBRotZ;//original data is left hand

            vehicle->getVehicleSetup().mCarRot.ToRotationMatrix(carRotMatrix);
            carARotX = Ogre::Vector3(carRotMatrix[0][0], carRotMatrix[1][0], -carRotMatrix[2][0]);
            carARotZ = Ogre::Vector3(-carRotMatrix[0][2], -carRotMatrix[1][2], carRotMatrix[2][2]);

            (*i).second->getVehicleSetup().mCarRot.ToRotationMatrix(carRotMatrix);
            carBRotZ = Ogre::Vector3(-carRotMatrix[0][2], -carRotMatrix[1][2], carRotMatrix[2][2]);

            Ogre::Vector3 cogAGlobal(vehicle->getVehicleSetup().mCOGGlobal);
            cogAGlobal.z = -cogAGlobal.z;//original data is left hand

            Ogre::Vector3 cogBGlobal((*i).second->getVehicleSetup().mCOGGlobal);
            cogBGlobal.z = -cogBGlobal.z;//original data is left hand

            Ogre::Real collisionRadiusDiff = (*i).second->getVehicleSetup().mCollisionRadius + 
                vehicle->getVehicleSetup().mCollisionRadius - 2.0f + 1.0f;

            Ogre::Vector3 cogDiff(cogBGlobal - cogAGlobal);
            Ogre::Vector3 cogDiffAbs(Ogre::Math::Abs(cogDiff.x), 
                Ogre::Math::Abs(cogDiff.y), 
                Ogre::Math::Abs(cogDiff.z));

            if( collisionRadiusDiff >= cogDiffAbs.x &&
                collisionRadiusDiff >= cogDiffAbs.y &&
                collisionRadiusDiff >= cogDiffAbs.z &&
                collisionRadiusDiff * collisionRadiusDiff > cogDiff.squaredLength()
                )
            {
                Ogre::Real collisionRadiusDiffMore = (*i).second->getVehicleSetup().mCollisionRadius + 
                vehicle->getVehicleSetup().mCollisionRadius - 6.0f - 2.0f + 1.0f;
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
                    Ogre::Real sqLength = diff.squaredLength();
                    if(sqLength < collisionRadiusDiffMore)
                    {
                        cogDiffSub -= cogDiff * -0.5f;

                        if(sqLength > distance)
                            distance = collisionRadiusDiffMore - sqLength;

                        ++threshold;
                        cogDiffAdd += (cogDiff * -1.0f / Ogre::Math::Sqrt(sqLength));
                    }

                    ++counter;
                }while(counter < 4);

                if(threshold > 0)
                {
                    cogDiffSub *= 1.0f / static_cast<float>(threshold);
                    cogDiffAdd.normalise();

                    Ogre::Vector3 impulseDiff((*i).second->getLinearImpulse() - vehicle->getLinearImpulse());

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

                    Ogre::Vector3 impulseLinear = cogDiffAdd * finalImpulse;
                    vehicle->adjustImpulseInc(cogDiffSub, impulseLinear);
                    vehicle->setCollisionSteeringAdditionalParam(carARotX.dotProduct(impulseLinear) * 0.02f);
                }
            }

        }
    }
}

void Physics::processCollisionsImpulseWeighter(PhysicsVehicle* vehicle)
{
    vehicle->setSlipStreamFactor(0.0f);

    for (vehicles::iterator i = mVehicles.begin(), j = mVehicles.end(); i != j; ++i)
    {
        if((*i).second.get() != vehicle)//don`t collide with yourself
        {
            Ogre::Vector3 carAPos(vehicle->getVehicleSetup().mCarGlobalPos);
            carAPos.z = -carAPos.z;//original data is left hand

            Ogre::Vector3 carBPos((*i).second->getVehicleSetup().mCarGlobalPos);
            carBPos.z = -carBPos.z;//original data is left hand

            Ogre::Vector3 posDiff(carBPos - carAPos);

            if(posDiff.squaredLength() < 4225.0f)
            {
                Ogre::Real posDiffLen = posDiff.length();
                Ogre::Real carBImpulseLen = (*i).second->getLinearImpulse().length();
                Ogre::Real impulseWeight = posDiffLen * carBImpulseLen;
                if(posDiffLen < 10.0f)
                    posDiffLen = 10.0f;

                posDiffLen = (posDiffLen - 10.0f) * 0.018181818f;

                if(impulseWeight > 0.0f)
                {
                    impulseWeight = (*i).second->getLinearImpulse().dotProduct(posDiff) / impulseWeight - 
                    (posDiffLen - (1.0f - posDiffLen) * -0.7f);

                    if(impulseWeight > 0.0f)
                    {
                        impulseWeight *= carBImpulseLen * 0.015f;

                        if(impulseWeight > 0.6f)
                            impulseWeight = 0.6f;
                        if(impulseWeight > vehicle->getSlipStreamFactor())
                            vehicle->setSlipStreamFactor(impulseWeight);
                    }
                }
            }
        }
    }
}