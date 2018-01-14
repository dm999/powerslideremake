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

class Physics;
class StaticMeshProcesser;
class PSAICar;

class PhysicsVehicle
{
    friend class PhysicsWheels;
public:

    PhysicsVehicle(Physics* physics, 
                    StaticMeshProcesser * meshProesser,
                    InitialVehicleSetup& initialVehicleSetup, 
                    Ogre::SceneNode *wheelNodes[InitialVehicleSetup::mWheelsAmount],
                    Ogre::SceneNode *chassis,
                    InputType type);
    virtual ~PhysicsVehicle();

    void timeStep(const GameState& gameState);

    void adjustImpulseInc(const Ogre::Vector3& rotAxis, const Ogre::Vector3& impulse);

    void setSteeringLeft(bool value) { mIsSteeringLeft = value; }
    void setSteeringRight(bool value) { mIsSteeringRight = value; }
    void setSteering(Ogre::Real value) { mSteeringOriginal = value; }
    void setThrottle(Ogre::Real value){ mThrottle = value;}
    void setBrakes(Ogre::Real value){ mBreaks = value;}
    void gearUp();
    void gearDown();

    const PSCarEngine& getCarEngine()const{return mCarEngine;}
    PSCarEngine& getCarEngine(){return mCarEngine;}

    void setVehicleType(VehicleType type){mVehicleType = type;}
    VehicleType getVehicleType() const {return mVehicleType;}

    void setRaceStarted(){mIsRaceStarted = true;}

    bool getWheelBackLCollision() const {return mPhysicsWheels.getWheelCollision(1);}
    bool getWheelBackRCollision() const {return mPhysicsWheels.getWheelCollision(0);}
    char getWheelBackLTerrainIndex() const {return mPhysicsWheels.getWheelTerainIndex(1);}
    char getWheelBackRTerrainIndex() const {return mPhysicsWheels.getWheelTerainIndex(0);}
    Ogre::Real getWheelBackLVelocity() const {return mPhysicsWheels.getWheelVelocity(1);}
    Ogre::Real getWheelBackRVelocity() const {return mPhysicsWheels.getWheelVelocity(0);}

    Ogre::Real getAlignedVelocity()const {return mVehicleVelocityMod;}
    Ogre::Real getAlignedVelocitySpeedometer()const {return mVehicleVelocityMod * 58.0f;}
    Ogre::Vector3 getLinearVelocitySpeedometer() const;
    Ogre::Vector3 getAngularVelocitySpeedometer() const;
    Ogre::Vector3 getLinearImpulse() const;//ai & multiplayer
    void setLinearImpulse(const Ogre::Vector3& val);//multiplayer
    Ogre::Vector3 getAngularImpulse() const;//multiplayer
    void setAngularImpulse(const Ogre::Vector3& val);//multiplayer

    void zeroImpulses();
    void setSlipStreamFactor(Ogre::Real val){mSlipStreamFactor = val;}
    Ogre::Real getSlipStreamFactor() const {return mSlipStreamFactor;}
    void setCollisionSteeringAdditionalParam(Ogre::Real val) {mCollisionSteeringAdditionalParam = val;}

    Ogre::Real getOriginalSteering() const {return mSteeringOriginal;}

    Ogre::Real getSuspensionHeight(size_t wheelIndex) const{return mPhysicsWheels.getSuspensionHeight(wheelIndex);}

    const InitialVehicleSetup& getVehicleSetup()const{ return mInitialVehicleSetup;}

    static Ogre::Vector3 findTangent(const Ogre::Vector3& normal, const Ogre::Vector3& input);
    static Ogre::Vector2 findTexCoordinates(const Ogre::Vector3& normal, 
        const Ogre::Vector3& point,
        const Ogre::Vector3& pA, const Ogre::Vector3& pB, const Ogre::Vector3& pC,
        const Ogre::Vector2& tA, const Ogre::Vector2& tB, const Ogre::Vector2& tC);
    static void createRotMatrix(Ogre::Vector3& matAxis, const Ogre::Vector3& normalisedImpulse, Ogre::Real angle);

protected:

    virtual void doAIStep(const GameState& gameState) {(void)gameState;}

    virtual Ogre::Real doAdjustAISteering(Ogre::Real val)const {return val;}
    virtual Ogre::Real doGetVelocityScale()const {return 1.0f;}
    virtual Ogre::Real doGetThrottleScale()const {return 1.0f;}
    virtual Ogre::Real doGetTractionScale()const {return 1.0f;}

    VehicleType mVehicleType;

    Ogre::Real mThrottle;
    Ogre::Real mBreaks;
    Ogre::Real mSteeringOriginal;

    Ogre::uint8 mThrottleAdjusterCounter;
    Ogre::Real mThrottleAdjuster;

    PSAICar * mAICar;

    InitialVehicleSetup& mInitialVehicleSetup;

    Ogre::Real mVehicleVelocityMod;

    Ogre::Vector3 mImpulseLinear;
    Ogre::Vector3 mImpulseLinearInc;
    Ogre::Vector3 mImpulseRot;
    Ogre::Vector3 mImpulseRotPrev;
    Ogre::Vector3 mImpulseRotInc;

    Physics* mPhysics;
    PSCarEngine mCarEngine;

private:

    void calcTransmission();

    void reposition();
    void rerotation();

    void integrate();

    Ogre::Real momentOfInertiaProj(const Ogre::Vector3& axis)const;

    void calcWheelRoofImpulses();

    void turnOverRestore(bool isTurnOver);

    Ogre::Real adjustSteering();

    StaticMeshProcesser* mMeshProcesser;

    Ogre::SceneNode *mChassis;

    PhysicsWheels mPhysicsWheels;
    PhysicsRoofs mPhysicsRoofs;
    PhysicsBody mPhysicsBody;

    Ogre::Vector3 mCoreBaseGlobal;
    Ogre::Vector3 mCoreBaseGlobalPrev;

    Ogre::Vector3 mCOGShift;
    static const int mShiftValuesAmount = 6;
    Ogre::Vector3 mCOGShiftValues[mShiftValuesAmount];
    Ogre::uint8 mCOGShiftIndex;

    Ogre::Real mMaxCollisionDistance;

    Ogre::Real mSlipStreamFactor;

    Ogre::Real mSteeringAdditionalParam;
    Ogre::Real mCollisionSteeringAdditionalParam;
    bool mIsSteeringLeft;
    bool mIsSteeringRight;
    Ogre::Real mSteeringIncrement;

    int mTurnOverValue;

    bool mIsRaceStarted;

    InputType mInputType;

};

#endif