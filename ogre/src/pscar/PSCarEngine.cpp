
#include "PSCarEngine.h"

namespace{
    const Ogre::Real engineMinRPM = 0.0f;
    const Ogre::Real engineMaxRPM = 10000.0f;
}

PSCarEngine::PSCarEngine() :
    mCurrentGear(1),
    mEngineRPM(0.0f)
{}


void PSCarEngine::init(Ogre::Real idleRevsStart, Ogre::Real idleRevsEnd, float gearRevRatio, Ogre::Vector4 revRatio, Ogre::Vector4 changeDown, Ogre::Vector4 changeUp)
{
    mEngineIdleRevsStart = idleRevsStart;
    mEngineIdleRevsEnd = idleRevsEnd;

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

    float throttle = isThrottle ? 1.0f : 0.0f;

    if(isTraction)
    {
        Ogre::Real velocityIdle = mGearRatioMain * mGearRatio[0] * mEngineIdleRevsEnd;

        if(mCurrentGear <= 0)
        {
            if(mCurrentGear == 0) // N
            {
                mEngineRPM = mEngineRPM * 0.975f - (mEngineIdleRevsStart - throttle * -11000.0f) * -0.02f;
            }
            else // R
            {
                if(projectedVel > velocityIdle)
                {
                    mEngineRPM = projectedVel / (mGearRatioMain * mGearRatio[0]);
                }
                else
                {
                    mEngineRPM = projectedVel / velocityIdle * (mEngineIdleRevsEnd - mEngineIdleRevsStart) + mEngineIdleRevsStart;
                }
                
            }
        }
        else
        {
            if(projectedVel > velocityIdle)
            {
                mEngineRPM = projectedVel / (mGearRatioMain * mGearRatio[mCurrentGear - 1]);
            }
            else
            {
                mEngineRPM = projectedVel / velocityIdle * (mEngineIdleRevsEnd - mEngineIdleRevsStart) + mEngineIdleRevsStart;
            }
        }

        //auto transmission
        if(mCurrentGear > 0)
        {
            if(mEngineRPM < mChangeDown[mCurrentGear - 1])
            {
                --mCurrentGear;
            }
            if(mEngineRPM > mChangeUp[mCurrentGear - 1])
            {
                ++mCurrentGear;
            }
        }
        if(mCurrentGear >= 1)
        {
            mCurrentGear = Ogre::Math::Clamp(mCurrentGear, 1, mGearCount);
        }

/*
        if(!isBrake)
        {
            mCurrentGear = Ogre::Math::Clamp(mCurrentGear, 0, mGearCount);
        }
        else
        {
            mCurrentGear = -1;
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
        */
    }
    else //N
    {
        mEngineRPM = mEngineRPM * 0.975f - (mEngineIdleRevsStart - throttle * -11000.0f) * -0.02f;
    }

    mEngineRPM = Ogre::Math::Clamp(mEngineRPM, engineMinRPM, engineMaxRPM);
}

void PSCarEngine::gearUp()
{
    if(mCurrentGear <= 0 && mCurrentGear <= mGearCount) ++mCurrentGear;
}

void PSCarEngine::gearDown()
{
    if(mCurrentGear > 1)
    {
        mCurrentGear = 0;
    }
    else
    {
        if(mCurrentGear > -1) --mCurrentGear;
    }
}