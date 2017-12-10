
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

void PSCarEngine::process(Ogre::Real vehicleVelocityMod, Ogre::Real throttle, Ogre::Real wheelsAverageVel, bool isTraction)
{
    refreshEngineRPM(vehicleVelocityMod, throttle, wheelsAverageVel, isTraction);
}

void PSCarEngine::refreshEngineRPM(Ogre::Real vehicleVelocityMod, Ogre::Real throttle, Ogre::Real wheelsAverageVel, bool isTraction)
{
    bool isReverse = false;
    if(vehicleVelocityMod < 0.0f)
    {
        vehicleVelocityMod = -vehicleVelocityMod;
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
                if(vehicleVelocityMod > velocityIdle)
                {
                    mEngineRPM = vehicleVelocityMod / (mInitialVehicleSetup.mGearRatioMain * mInitialVehicleSetup.mGearRatio[0]);
                }
                else
                {
                    mEngineRPM = vehicleVelocityMod / velocityIdle * (mInitialVehicleSetup.mEngineIdleRevsEnd - mInitialVehicleSetup.mEngineIdleRevsStart) + mInitialVehicleSetup.mEngineIdleRevsStart;
                }
                
            }
        }
        else
        {
            if(vehicleVelocityMod > velocityIdle)
            {
                mEngineRPM = vehicleVelocityMod / (mInitialVehicleSetup.mGearRatioMain * mInitialVehicleSetup.mGearRatio[mCurrentGear - 1]);
            }
            else
            {
                mEngineRPM = vehicleVelocityMod / velocityIdle * (mInitialVehicleSetup.mEngineIdleRevsEnd - mInitialVehicleSetup.mEngineIdleRevsStart) + mInitialVehicleSetup.mEngineIdleRevsStart;
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

Ogre::Real PSCarEngine::getPower(Ogre::Real throttle, Ogre::Real impulseLinearMod) const
{
    Ogre::Real power = 0.0f;

    if(mCurrentGear > 0)//forward
    {
        Ogre::Real revs = mEngineRPM - mInitialVehicleSetup.mEngineIdleRevsStart;
        power = mInitialVehicleSetup.mPower.getPoint(revs);
        power *= mInitialVehicleSetup.mAccelerationFactor;
        power *= throttle;
        power /= mInitialVehicleSetup.mGearRatioMain * mInitialVehicleSetup.mGearRatio[mCurrentGear];

        if(mCurrentGear < mInitialVehicleSetup.mGearCount && mEngineRPM > 7500.0f)
        {
            Ogre::Real revsRescale = (10000.0f - mEngineRPM) * 0.0004f;
            if(revsRescale < 0.0f) revsRescale = 0.0f;
            power *= revsRescale;
        }
    }

    if(mCurrentGear == -1)//reverse
    {
        Ogre::Real revs = mEngineRPM - mInitialVehicleSetup.mEngineIdleRevsStart;
        power = mInitialVehicleSetup.mPower.getPoint(revs);
        power *= mInitialVehicleSetup.mAccelerationFactor;
        power *= throttle;
        power /= mInitialVehicleSetup.mGearRatioMain * mInitialVehicleSetup.mGearRatio[0];
    }
    
    if(impulseLinearMod < 40.0f)
        power *= (1.0f - (40.0f - impulseLinearMod) * -0.0125f);

    if(power < 0.0f)
        power = 0.0f;

    return power;
}