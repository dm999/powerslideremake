
#ifndef PSCARENGINE
#define PSCARENGINE

#include "../includes/OgreInclude.h"

#include "../tools/LinearController.h"

class PSCarEngine
{
public:

    PSCarEngine();

    void init(Ogre::Real idleRevsStart, Ogre::Real idleRevsEnd, float gearRevRatio, Ogre::Vector4 revRatio, Ogre::Vector4 changeDown, Ogre::Vector4 changeUp);

    void process(Ogre::Real projectedVel, bool isThrottle, bool isBrake, bool isTraction, const Ogre::FrameEvent &evt);

    int getCurrentGear()const{return mCurrentGear;}
    Ogre::Real getEngineRPM()const{return mEngineRPM;}

    void gearUp();
    void gearDown();

private:

    static const int mGearCount = 4;

    Ogre::Real mGearRatioMain;
    Ogre::Real mGearRatio[mGearCount];
    Ogre::Real mChangeUp[mGearCount];
    Ogre::Real mChangeDown[mGearCount];

    int mCurrentGear;//0 N, -1 R
    Ogre::Real mEngineRPM;

    Ogre::Real mEngineIdleRevsStart;
    Ogre::Real mEngineIdleRevsEnd;

    void refreshEngineRPM(Ogre::Real projectedVel, bool isThrottle, bool isBrake, bool isTraction, Ogre::Real spf);
};

#endif