
#include "PSCarEngine.h"

PSCarEngine::PSCarEngine(const InitialVehicleSetup& setup) :
    mInitialVehicleSetup(setup),
    mCurrentGear(0),
    mEngineRPM(0.0f)
{}

void PSCarEngine::process(Ogre::Real wheelsAverageVel, Ogre::Real throttle)
{
    
    if(mCurrentGear <= 0)
    {
        if(mCurrentGear == 0)//N
        {
            mEngineRPM = mEngineRPM * 0.975f - (mInitialVehicleSetup.mEngineIdleRevsStart - throttle * -11000.0f) * -0.02f;
        }
        else//R
        {
            Ogre::Real absVel = Ogre::Math::Abs(wheelsAverageVel);
            Ogre::Real velocityIdle = mInitialVehicleSetup.mGearRatioMain * mInitialVehicleSetup.mGearRatio[0] * mInitialVehicleSetup.mEngineIdleRevsEnd;

            if(absVel > velocityIdle)
            {
                mEngineRPM = absVel / (mInitialVehicleSetup.mGearRatioMain * mInitialVehicleSetup.mGearRatio[0]);
            }
            else
            {
                mEngineRPM = absVel / velocityIdle * (mInitialVehicleSetup.mEngineIdleRevsEnd - mInitialVehicleSetup.mEngineIdleRevsStart) + mInitialVehicleSetup.mEngineIdleRevsStart;
            }
        }
    }
    else//1, 2, 3, 4
    {
        Ogre::Real absVel = Ogre::Math::Abs(wheelsAverageVel);
        Ogre::Real velocityIdle = mInitialVehicleSetup.mGearRatioMain * mInitialVehicleSetup.mGearRatio[0] * mInitialVehicleSetup.mEngineIdleRevsEnd;

        if(absVel >= velocityIdle)
        {
            mEngineRPM = absVel / (mInitialVehicleSetup.mGearRatioMain * mInitialVehicleSetup.mGearRatio[mCurrentGear - 1]);
        }
        else
        {
            mEngineRPM = absVel / velocityIdle * (mInitialVehicleSetup.mEngineIdleRevsEnd - mInitialVehicleSetup.mEngineIdleRevsStart) + mInitialVehicleSetup.mEngineIdleRevsStart;
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
        
        mCurrentGear = Ogre::Math::Clamp(mCurrentGear, 1, mInitialVehicleSetup.mGearCount);
    }
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
        power /= mInitialVehicleSetup.mGearRatioMain * mInitialVehicleSetup.mGearRatio[mCurrentGear - 1];

        if(mCurrentGear < mInitialVehicleSetup.mGearCount && mEngineRPM > 7500.0f)
        {
            Ogre::Real revsRescale = (10000.0f - mEngineRPM) * 0.0004f;
            if(revsRescale < 0.0f) revsRescale = 0.0f;
            power *= revsRescale;
        }
        
        if(impulseLinearMod < 40.0f)
            power *= (1.0f - (40.0f - impulseLinearMod) * -0.0125f);
    }

    if(mCurrentGear == -1)//reverse
    {
        Ogre::Real revs = mEngineRPM - mInitialVehicleSetup.mEngineIdleRevsStart;
        power = mInitialVehicleSetup.mPower.getPoint(revs);
        power *= mInitialVehicleSetup.mAccelerationFactor;
        power *= throttle;
        power /= mInitialVehicleSetup.mGearRatioMain * mInitialVehicleSetup.mGearRatio[0];
        
        if(impulseLinearMod < 40.0f)
            power *= (1.0f - (40.0f - impulseLinearMod) * -0.0125f);
        
        if(power < 0.0f)
            power = 0.0f;
    }
    

    return power;
}