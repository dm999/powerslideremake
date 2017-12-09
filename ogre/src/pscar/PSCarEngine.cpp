
#include "PSCarEngine.h"

namespace{
    const Ogre::Real engineMinRPM = 0.0f;
    const Ogre::Real engineMaxRPM = 10000.0f;
}

PSCarEngine::PSCarEngine(const InitialVehicleSetup& setup) :
    mInitialVehicleSetup(setup),
    mCurrentGear(1),
    mEngineRPM(0.0f)
{}

void PSCarEngine::process(Ogre::Real projectedVel, Ogre::Real throttle, Ogre::Real brakes, bool isTraction)
{
    refreshEngineRPM(projectedVel, throttle, brakes, isTraction);
}

void PSCarEngine::refreshEngineRPM(Ogre::Real projectedVel, Ogre::Real throttle, Ogre::Real brakes, bool isTraction)
{
    bool isReverse = false;
    if(projectedVel < 0.0f)
    {
        projectedVel = -projectedVel;
        isReverse = true;
    }

    if(isTraction)
    {
        Ogre::Real velocityIdle = mInitialVehicleSetup.mGearRatioMain * mInitialVehicleSetup.mGearRatio[0] * mInitialVehicleSetup.mEngineIdleRevsEnd;

        if(mCurrentGear <= 0)
        {
            if(mCurrentGear == 0) // N
            {
                mEngineRPM = mEngineRPM * 0.975f - (mInitialVehicleSetup.mEngineIdleRevsStart - throttle * -11000.0f) * -0.02f;
            }
            else // R
            {
                if(projectedVel > velocityIdle)
                {
                    mEngineRPM = projectedVel / (mInitialVehicleSetup.mGearRatioMain * mInitialVehicleSetup.mGearRatio[0]);
                }
                else
                {
                    mEngineRPM = projectedVel / velocityIdle * (mInitialVehicleSetup.mEngineIdleRevsEnd - mInitialVehicleSetup.mEngineIdleRevsStart) + mInitialVehicleSetup.mEngineIdleRevsStart;
                }
                
            }
        }
        else
        {
            if(projectedVel > velocityIdle)
            {
                mEngineRPM = projectedVel / (mInitialVehicleSetup.mGearRatioMain * mInitialVehicleSetup.mGearRatio[mCurrentGear - 1]);
            }
            else
            {
                mEngineRPM = projectedVel / velocityIdle * (mInitialVehicleSetup.mEngineIdleRevsEnd - mInitialVehicleSetup.mEngineIdleRevsStart) + mInitialVehicleSetup.mEngineIdleRevsStart;
            }
        }

        //auto transmission
        if(mCurrentGear > 0)
        {
            if(mEngineRPM < mInitialVehicleSetup.mChangeDown[mCurrentGear - 1])
            {
                --mCurrentGear;
            }
            if(mEngineRPM > mInitialVehicleSetup.mChangeUp[mCurrentGear - 1])
            {
                ++mCurrentGear;
            }
        }
        if(mCurrentGear >= 1)
        {
            mCurrentGear = Ogre::Math::Clamp(mCurrentGear, 1, mInitialVehicleSetup.mGearCount);
        }
    }
    else //N
    {
        mEngineRPM = mEngineRPM * 0.975f - (mInitialVehicleSetup.mEngineIdleRevsStart - throttle * -11000.0f) * -0.02f;
    }

    mEngineRPM = Ogre::Math::Clamp(mEngineRPM, engineMinRPM, engineMaxRPM);

}

void PSCarEngine::gearUp()
{
    if(mCurrentGear <= 0 && mCurrentGear <= mInitialVehicleSetup.mGearCount) ++mCurrentGear;
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