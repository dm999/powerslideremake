#ifndef PHYSICSWHEELS_H
#define PHYSICSWHEELS_H

#include "../includes/OgreInclude.h"
#include "../includes/CommonIncludes.h"

#include "InitialVehicleSetup.h"

class Physics;
class StaticMeshProcesser;
class PhysicsVehicle;

/*all calculations in left hand - like original data*/
class PhysicsWheels
{
public:
    PhysicsWheels(const InitialVehicleSetup& initialVehicleSetup,
        Physics* physics,
        const StaticMeshProcesser* const meshProcesser);

    ~PhysicsWheels();

    void init(Ogre::SceneNode *wheelNodes[InitialVehicleSetup::mWheelsAmount]);
    void init(const InitialVehicleSetup& vehicleSetup);//to restore falloff

    void initStep();
    void calcImpulses(const Ogre::Vector3& impulseRot, const Ogre::Vector3& impulseRotPrev, const Ogre::Vector3& normalisedImpulseRot,
                        const Ogre::Vector3& impulseLinear,
                        Ogre::Real recipMomentProj);
    void process(PhysicsVehicle& vehicle, bool& processShift);
    void reposition();
    void rerotation();

    Ogre::Real calcVelocity(Ogre::Real vehicleVelocityMod, Ogre::Real throttle, Ogre::Real breaks, Ogre::Real handBreaks);
    void calcVelocityMore(Ogre::Real power, int gear);
    
    bool getWheelCollision(size_t index) const;
    char getWheelTerainIndex(size_t index) const;
    Ogre::Real getWheelVelocity(size_t index) const;

    void calcPhysics(PhysicsVehicle& vehicle, Ogre::Real throttle, Ogre::Real breaks, Ogre::Real handBreaks, Ogre::Real tractionScale, Ogre::uint8& throttleAdjusterCounter, Ogre::Real throttleAdjuster, Ogre::Vector3& gravityVelocity);

    void setSteering(Ogre::Real value);

    void setBackVelocity(Ogre::Real velocity){mVelocity[0] = mVelocity[1] = velocity;}

    const Ogre::Vector3& getWorldNormalWeighted(size_t wheelIndex) const{return mWorldNormalWeighted[wheelIndex];}

    Ogre::Real getSuspensionHeight(size_t wheelIndex) const{return mSuspensionHeight[wheelIndex];}

private:

    Ogre::Real averageCollisionNormal(const Ogre::Vector3& matrixYColumn, size_t wheelIndex, Ogre::Vector3& averagedNormal) const;

    float calcSuspensionLength(const PhysicsVehicle& vehicle, float len, size_t wheelIndex);

    Physics* mPhysics;
    const StaticMeshProcesser* const mMeshProcesser;

    const InitialVehicleSetup& mInitialVehicleSetup;

    Ogre::SceneNode *mWheelNodes[InitialVehicleSetup::mWheelsAmount];//RR, RL, FR, FL

    Ogre::Vector3 mGlobalPos[InitialVehicleSetup::mWheelsAmount];//RR, RL, FR, FL

    Ogre::Vector3 mWheelsSuspensionPoint[InitialVehicleSetup::mWheelsAmount];//RR, RL, FR, FL
    Ogre::Vector3 mWheelsSuspensionPointGlobalPrev[InitialVehicleSetup::mWheelsAmount];//RR, RL, FR, FL
    Ogre::Vector3 mWheelsSuspensionPointGlobal[InitialVehicleSetup::mWheelsAmount];//RR, RL, FR, FL
    Ogre::Vector3 mWheelsSuspensionPoint2Global[InitialVehicleSetup::mWheelsAmount];//RR, RL, FR, FL

    Ogre::Vector3 mWheelsSuspensionRot[InitialVehicleSetup::mWheelsAmount];//RR, RL, FR, FL
    Ogre::Vector3 mWheelsSuspensionGlobal[InitialVehicleSetup::mWheelsAmount];//RR, RL, FR, FL

    Ogre::Real mSuspensionHeight[InitialVehicleSetup::mWheelsAmount];//RR, RL, FR, FL;
    Ogre::Real mSuspensionHeightPrev[InitialVehicleSetup::mWheelsAmount];//RR, RL, FR, FL;
    Ogre::Vector3 mWorldNormalWeighted[InitialVehicleSetup::mWheelsAmount];//RR, RL, FR, FL;

    Ogre::Real mSpringVal[InitialVehicleSetup::mWheelsAmount];//RR, RL, FR, FL;
    
    Ogre::Real mSteering[InitialVehicleSetup::mWheelsAmount];//RR, RL, FR, FL;
    Ogre::Real mVelocity[InitialVehicleSetup::mWheelsAmount];//RR, RL, FR, FL;
    Ogre::Real mWheelRotationalAngle[InitialVehicleSetup::mWheelsAmount];//RR, RL, FR, FL;

    Ogre::Vector3 mWheelsImpulseLinear[InitialVehicleSetup::mWheelsAmount];//RR, RL, FR, FL
    Ogre::Vector3 mWheelsImpulseTangent[InitialVehicleSetup::mWheelsAmount];//RR, RL, FR, FL
    Ogre::Vector3 mWheelsAveragedNormal[InitialVehicleSetup::mWheelsAmount];//RR, RL, FR, FL
    Ogre::Real mWheelsImpulseResulted[InitialVehicleSetup::mWheelsAmount];//RR, RL, FR, FL

    char mTerrainIndex[InitialVehicleSetup::mWheelsAmount];//RR, RL, FR, FL;

    bool mIsCollided[InitialVehicleSetup::mWheelsAmount];//RR, RL, FR, FL;
};


#endif