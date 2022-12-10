
#ifndef PSCARENGINE
#define PSCARENGINE

#include "../includes/OgreInclude.h"

#include "../tools/LinearController.h"

#include "../physics/InitialVehicleSetup.h"

#include "../Enums.h"

class PSCarEngine
{
public:

    PSCarEngine(const InitialVehicleSetup& setup);

    void process(Ogre::Real wheelsAverageVel, Ogre::Real throttle, bool isNitro);
    Ogre::Real getPower(Ogre::Real throttle, Ogre::Real impulseLinearMod) const;

    int getCurrentGear()const{return mCurrentGear;}

    Ogre::Real getEngineRPM()const{return mEngineRPM;}
    void setEngineRPM(Ogre::Real val) {mEngineRPM = val;}

    void gearUp();
    void gearDown();

    void setTransmissionType(TransmissionType type);

private:

    TransmissionType mTransmissionType;

    int mCurrentGear;//0 N, -1 R
    Ogre::Real mEngineRPM;

    const InitialVehicleSetup& mInitialVehicleSetup;
    std::vector<Ogre::Real> mChangeDown;
    std::vector<Ogre::Real> mChangeUp;
    std::vector<Ogre::Real> mGearRatio;
    int mGearCount;

    void initDefaultGearBox();
    void initAdvancedAutoGearBox();
};

#endif