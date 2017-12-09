#ifndef PHYSICSVEHICLE_H
#define PHYSICSVEHICLE_H

#include "../includes/OgreInclude.h"
#include "../includes/CommonIncludes.h"

#include "InitialVehicleSetup.h"
#include "PhysicsWheels.h"
#include "PhysicsRoofs.h"
#include "PhysicsBody.h"

#include "../pscar/PSCarEngine.h"

class btSphereShape;
class btCollisionObject;

class Physics;
class StaticMeshProcesser;

class PhysicsVehicle
{
public:

    PhysicsVehicle(Physics* physics, 
                    StaticMeshProcesser * meshProesser,
                    const InitialVehicleSetup& initialVehicleSetup, 
                    Ogre::SceneNode *wheelNodes[InitialVehicleSetup::mWheelsAmount],
                    Ogre::SceneNode *chassis);
    virtual ~PhysicsVehicle();

    void timeStep();
    void processEngineIdle();

    void adjustImpulseInc(const Ogre::Vector3& rotAxis, const Ogre::Vector3& impulse);

    void setThrottle(Ogre::Real value){mThrottle = value;}
    void setBrakes(Ogre::Real value){mBreaks = value;}

    const PSCarEngine& getCarEngine()const{return mCarEngine;}
    PSCarEngine& getCarEngine(){return mCarEngine;}

    static Ogre::Vector3 findTangent(const Ogre::Vector3& normal, const Ogre::Vector3& input);

private:

    void calcTransmission();

    void calcPhysics();

    void reposition(const Ogre::Vector3& posDiff);
    void rerotation(const Ogre::Quaternion& rot);

    void integrate();

    Ogre::Real momentOfInertiaProj(const Ogre::Vector3& axis)const;
    void createRotMatrix(Ogre::Vector3& matAxis, const Ogre::Vector3& normalisedImpulse, Ogre::Real angle) const;

    void calcWheelRoofImpulses();

    Physics* mPhysics;
    StaticMeshProcesser* mMeshProcesser;

    Ogre::Vector3 mImpulseLinear;
    Ogre::Vector3 mImpulseLinearInc;
    Ogre::Vector3 mImpulseRot;
    Ogre::Vector3 mImpulseRotPrev;
    Ogre::Vector3 mImpulseRotInc;

    Ogre::Real mVehicleVelocityMod;

    Ogre::SceneNode *mChassis;

    const InitialVehicleSetup& mInitialVehicleSetup;

    PhysicsWheels mPhysicsWheels;
    PhysicsRoofs mPhysicsRoofs;
    PhysicsBody mPhysicsBody;

    Ogre::Vector3 mCoreBaseGlobal;
    Ogre::Vector3 mCoreBaseGlobalPrev;

    Ogre::Real mMaxCollisionDistance;

    PSCarEngine mCarEngine;
    Ogre::Real mThrottle;
    Ogre::Real mBreaks;

};

#endif