#include "pcheader.h"

#include "PSCarEngine.h"

PSCarEngine::PSCarEngine() :
    mCurrentGear(1),
    mEngineRPM(0.0f),
    mEngineMinRPM(0.0f),
    mEngineMaxRPM(10000.0f)
{
}


PSCarEngine::~PSCarEngine()
{
}

void PSCarEngine::init(float gearRevRatio, Ogre::Vector4 revRatio, Ogre::Vector4 changeDown, Ogre::Vector4 changeUp)
{
    mGearRatioMain = gearRevRatio;

    mGearRatio[0] = revRatio.x;
    mGearRatio[1] = revRatio.y;
    mGearRatio[2] = revRatio.z;
    mGearRatio[3] = revRatio.w;

    mChangeUp[0] = changeUp.x;
    mChangeUp[1] = changeUp.y;
    mChangeUp[2] = changeUp.z;
    mChangeUp[3] = changeUp.w;

    mChangeDown[0] = changeDown.x;
    mChangeDown[1] = changeDown.y;
    mChangeDown[2] = changeDown.z;
    mChangeDown[3] = changeDown.w;

    mCurrentGear = 1;
    mEngineRPM = 0.0f;
}

void PSCarEngine::process(Ogre::Real projectedVel, bool isThrottle, bool isBrake, bool isTraction, const Ogre::FrameEvent &evt)
{
    refreshEngineRPM(projectedVel, isThrottle, isBrake, isTraction, evt.timeSinceLastFrame);
}

void PSCarEngine::refreshEngineRPM(Ogre::Real projectedVel, bool isThrottle, bool isBrake, bool isTraction, Ogre::Real spf)
{
    bool isReverse = false;
    if(projectedVel < 0.0f)
    {
        projectedVel = -projectedVel;
        isReverse = true;
    }

    const Ogre::Real wheelRadius = 3.2f;
    Ogre::Real wheelRotation = projectedVel / (wheelRadius * 2.0f * Ogre::Math::PI / 1000.0f);
    wheelRotation /= 60.0f; // rpm

    if(isTraction)
    {
        if(!isBrake)
        {
            if(mCurrentGear < 0) mCurrentGear = 1;
            mEngineRPM = (wheelRotation / mGearRatioMain) / mGearRatio[mCurrentGear - 1];
            if(mEngineRPM < mEngineMinRPM)
                mEngineRPM = mEngineMinRPM;
        }
        else
        {
            mEngineRPM = (wheelRotation / mGearRatioMain) / 0.4f;
            if(mEngineRPM < mEngineMinRPM)
                mEngineRPM = mEngineMinRPM;
        }

        if(isThrottle || isBrake)
        {
            if(mEngineRPM < 3000.0f)
                mEngineRPM = 3000.0f;
        }

        if(!isBrake)
        {
            if(mEngineRPM > mChangeUp[mCurrentGear - 1])
            {
                ++mCurrentGear;
            }

            if(mEngineRPM < mChangeDown[mCurrentGear - 1])
            {
                --mCurrentGear;
            }

            mCurrentGear = Ogre::Math::Clamp(mCurrentGear, 1, mGearCount);
        }
        else
        {
            mCurrentGear = -1;
        }
    }
    else
    {
        if(isThrottle || isBrake)
        {
            Ogre::Real accel = mEngineMaxRPM / Ogre::Math::Clamp(mEngineRPM, 1000.0f, mEngineMaxRPM);
            mEngineRPM += accel * 5000.0f * spf; 
            
        }
        else
        {
            mEngineRPM -= 5.0f * 5000.0f * spf; 
        }
    }

    mEngineRPM = Ogre::Math::Clamp(mEngineRPM, mEngineMinRPM, mEngineMaxRPM);
}