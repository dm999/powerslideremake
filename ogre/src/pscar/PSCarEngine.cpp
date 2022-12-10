
#include "PSCarEngine.h"

PSCarEngine::PSCarEngine(const InitialVehicleSetup& setup) :
    mInitialVehicleSetup(setup),
    mCurrentGear(0),
    mEngineRPM(0.0f),
    mTransmissionType(trManual)
{
    initDefaultGearBox();
}

void PSCarEngine::initDefaultGearBox()
{
    mGearCount = mInitialVehicleSetup.mGearCount;
    
    mChangeDown.resize(mGearCount);
    mChangeUp.resize(mGearCount);
    mGearRatio.resize(mGearCount);

    for(size_t q = 0; q < static_cast<size_t>(mGearCount); ++q)
    {
        mChangeDown[q] = mInitialVehicleSetup.mChangeDown[q];
        mChangeUp[q] = mInitialVehicleSetup.mChangeUp[q];
        mGearRatio[q] = mInitialVehicleSetup.mGearRatio[q];
    }
}

void PSCarEngine::initAdvancedAutoGearBox()
{
    mGearCount = 9;

    mChangeDown.resize(mGearCount);
    mChangeUp.resize(mGearCount);
    mGearRatio.resize(mGearCount);

    mChangeDown[0] = 0.0f;
    mChangeUp[mGearCount - 1] = 1000000.00f;
    mGearRatio[mGearCount - 1] = 1.0f;

    // keep revs near 7k (max power)
    for(size_t q = 0; q < static_cast<size_t>(mGearCount - 1); ++q)
    {
        mChangeDown[q + 1] = 6500.0f;
        mChangeUp[q] = 7500.0f;
        mGearRatio[q] = 0.3f + 0.7f * static_cast<float>(q) / static_cast<float>(mGearCount - 1);
    }
}

void PSCarEngine::process(Ogre::Real wheelsAverageVel, Ogre::Real throttle, bool isNitro)
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
            Ogre::Real velocityIdle = mInitialVehicleSetup.mGearRatioMain * mGearRatio[0] * mInitialVehicleSetup.mEngineIdleRevsEnd;

            if(absVel > velocityIdle)
            {
                mEngineRPM = absVel / (mInitialVehicleSetup.mGearRatioMain * mGearRatio[0]);
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
        Ogre::Real velocityIdle = mInitialVehicleSetup.mGearRatioMain * mGearRatio[0] * mInitialVehicleSetup.mEngineIdleRevsEnd;

        if(absVel >= velocityIdle)
        {
            mEngineRPM = absVel / (mInitialVehicleSetup.mGearRatioMain * mGearRatio[mCurrentGear - 1]);
        }
        else
        {
            mEngineRPM = absVel / velocityIdle * (mInitialVehicleSetup.mEngineIdleRevsEnd - mInitialVehicleSetup.mEngineIdleRevsStart) + mInitialVehicleSetup.mEngineIdleRevsStart;
        }
    }

    if(isNitro)
        mEngineRPM *= 1.5f;

    if(mTransmissionType == trAuto || mTransmissionType == trAutoAdvanced)
    {
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
            
            mCurrentGear = Ogre::Math::Clamp(mCurrentGear, 1, mGearCount);
        }
    }
}

void PSCarEngine::gearUp()
{
    if(mTransmissionType == trAuto || mTransmissionType == trAutoAdvanced)
    {
        if(mCurrentGear <= 0 && mCurrentGear < mInitialVehicleSetup.mGearCount) ++mCurrentGear;
    }
    else
        if(mCurrentGear < mInitialVehicleSetup.mGearCount) ++mCurrentGear;
}

void PSCarEngine::gearDown()
{
    if(mTransmissionType == trAuto || mTransmissionType == trAutoAdvanced)
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
    else
        if(mCurrentGear > -1) --mCurrentGear;
}

void PSCarEngine::setTransmissionType(TransmissionType type)
{
    mTransmissionType = type;

    if(mTransmissionType != trAutoAdvanced) initDefaultGearBox();
    else initAdvancedAutoGearBox();
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
        power /= mInitialVehicleSetup.mGearRatioMain * mGearRatio[mCurrentGear - 1];

        if(mCurrentGear < mGearCount && mEngineRPM > 7500.0f)
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
        power /= mInitialVehicleSetup.mGearRatioMain * mGearRatio[0];
        
        if(impulseLinearMod < 40.0f)
            power *= (1.0f - (40.0f - impulseLinearMod) * -0.0125f);
        
        if(power < 0.0f)
            power = 0.0f;
    }
    

    return power;
}