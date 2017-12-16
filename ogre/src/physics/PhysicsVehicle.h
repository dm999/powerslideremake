#ifndef PHYSICSVEHICLE_H
#define PHYSICSVEHICLE_H

#include "../includes/OgreInclude.h"
#include "../includes/CommonIncludes.h"

#include "InitialVehicleSetup.h"
#include "PhysicsWheels.h"
#include "PhysicsRoofs.h"
#include "PhysicsBody.h"

#include "../pscar/PSCarEngine.h"

#include "../GameState.h"

class btSphereShape;
class btCollisionObject;

class Physics;
class StaticMeshProcesser;

class PhysicsVehicle
{
    friend class PhysicsWheels;
public:

    PhysicsVehicle(Physics* physics, 
                    StaticMeshProcesser * meshProesser,
                    InitialVehicleSetup& initialVehicleSetup, 
                    Ogre::SceneNode *wheelNodes[InitialVehicleSetup::mWheelsAmount],
                    Ogre::SceneNode *chassis);
    virtual ~PhysicsVehicle();

    void timeStep();
    void processEngineIdle();

    void adjustImpulseInc(const Ogre::Vector3& rotAxis, const Ogre::Vector3& impulse);

    void setSteering(Ogre::Real value) { mSteeringOriginal = value; }
    void setThrottle(Ogre::Real value){ mThrottle = value;}
    void setBrakes(Ogre::Real value){ mBreaks = value;}
    void gearUp();
    void gearDown();

    const PSCarEngine& getCarEngine()const{return mCarEngine;}

    void setVehicleType(VehicleType type){mVehicleType = type;}

    void setRaceStarted(){mIsRaceStarted = true;}

    static Ogre::Vector3 findTangent(const Ogre::Vector3& normal, const Ogre::Vector3& input);

private:

    void calcTransmission();

    void reposition();
    void rerotation();

    void integrate();

    Ogre::Real momentOfInertiaProj(const Ogre::Vector3& axis)const;
    void createRotMatrix(Ogre::Vector3& matAxis, const Ogre::Vector3& normalisedImpulse, Ogre::Real angle) const;

    void calcWheelRoofImpulses();

    void turnOverRestore(bool isTurnOver);

    Ogre::Real adjustSteering();

    Physics* mPhysics;
    StaticMeshProcesser* mMeshProcesser;

    Ogre::Vector3 mImpulseLinear;
    Ogre::Vector3 mImpulseLinearInc;
    Ogre::Vector3 mImpulseRot;
    Ogre::Vector3 mImpulseRotPrev;
    Ogre::Vector3 mImpulseRotInc;

    Ogre::Real mVehicleVelocityMod;

    Ogre::SceneNode *mChassis;

    InitialVehicleSetup& mInitialVehicleSetup;

    PhysicsWheels mPhysicsWheels;
    PhysicsRoofs mPhysicsRoofs;
    PhysicsBody mPhysicsBody;

    Ogre::Vector3 mCoreBaseGlobal;
    Ogre::Vector3 mCoreBaseGlobalPrev;

    Ogre::Real mMaxCollisionDistance;

    PSCarEngine mCarEngine;
    Ogre::Real mThrottle;
    Ogre::Real mBreaks;
    Ogre::Real mSteeringOriginal;
    Ogre::Real mSteeringAdditionalParam;

    int mTurnOverValue;

    bool mIsRaceStarted;
    VehicleType mVehicleType;

};

#endif