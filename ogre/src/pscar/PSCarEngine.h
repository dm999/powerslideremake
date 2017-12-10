
#ifndef PSCARENGINE
#define PSCARENGINE

#include "../includes/OgreInclude.h"

#include "../tools/LinearController.h"

#include "../physics/InitialVehicleSetup.h"

class PSCarEngine
{
public:

    PSCarEngine(const InitialVehicleSetup& setup);

    void process(Ogre::Real vehicleVelocityMod, Ogre::Real throttle, Ogre::Real wheelsAverageVel, bool isTraction);
    Ogre::Real getPower(Ogre::Real throttle, Ogre::Real impulseLinearMod) const;

    int getCurrentGear()const{return mCurrentGear;}
    Ogre::Real getEngineRPM()const{return mEngineRPM;}

    void gearUp();
    void gearDown();

private:

    int mCurrentGear;//0 N, -1 R
    Ogre::Real mEngineRPM;

    void refreshEngineRPM(Ogre::Real vehicleVelocityMod, Ogre::Real throttle, Ogre::Real wheelsAverageVel, bool isTraction);

    const InitialVehicleSetup& mInitialVehicleSetup;
};

#endif