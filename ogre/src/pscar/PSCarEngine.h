
#ifndef PSCARENGINE
#define PSCARENGINE

#include "../includes/OgreInclude.h"

#include "../tools/LinearController.h"

class PSCarEngine
{
public:

    PSCarEngine();
    ~PSCarEngine();

    void init(float gearRevRatio, Ogre::Vector4 revRatio, Ogre::Vector4 changeDown, Ogre::Vector4 changeUp);

    void process(Ogre::Real projectedVel, bool isThrottle, bool isBrake, bool isTraction, const Ogre::FrameEvent &evt);

    int getCurrentGear()const{return mCurrentGear;}
    Ogre::Real getEngineRPM()const{return mEngineRPM;}

private:

    static const int mGearCount = 4;

    Ogre::Real mGearRatioMain;
    Ogre::Real mGearRatio[mGearCount];
    Ogre::Real mChangeUp[mGearCount];
    Ogre::Real mChangeDown[mGearCount];

    int mCurrentGear;
    Ogre::Real mEngineRPM;
    Ogre::Real mEngineMinRPM;
    Ogre::Real mEngineMaxRPM;

    void refreshEngineRPM(Ogre::Real projectedVel, bool isThrottle, bool isBrake, bool isTraction, Ogre::Real spf);
};

#endif