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
    void setHandBrakes(Ogre::Real value){ mHandBreaks = value;}
    void gearUp();
    void gearDown();

    static const size_t mNitroFrames = 200;
    void enableNitro(){mIsNitro = true;}
    bool isNitro() const{return mIsNitro;}
    size_t getNitroFrames() const {return mNitroCounter;}

    void enableSticky() { mIsSticky = true; mVehicleSetup.mSuspensionTraction.setVScale(1.5f); }
    void disableSticky() { mIsSticky = false; mVehicleSetup.mSuspensionTraction.setVScale(1.0f); }
    bool isSticky() const{ return mIsSticky; }

    void enableSpider() { mIsSpider = true; }
    void disableSpider() { mIsSpider = false; }
    bool isSpider() const{ return mIsSpider; }

    void enableICBM() { mIsICBM = true; }
    void disableICBM() { mIsICBM = false; }
    bool isICBM() const{ return mIsICBM; }

    void enableApollo() { mIsApollo = true; mApolloAmount = 1; }
    void disableApollo() { mIsApollo = false; }
    bool isApollo() const{ return mIsApollo; }

    void enableLunar() { mIsLunar = true; }
    void disableLunar() { mIsLunar = false; }
    bool isLunar() const{ return mIsLunar; }

    void enableJump(std::function<void(void)>& f) { mIsJump = true; mDisableJump = f; }
    void disableJump() { mIsJump = false; }
    bool isJump() const{ return mIsJump; }

    void enableHover() { mPrevMaxTravel = mVehicleSetup.mMaxTravel; mVehicleSetup.mMaxTravel = 6.0f;  mIsHover = true; }
    void disableHover() { mIsHover = false; mVehicleSetup.mMaxTravel = mPrevMaxTravel; }
    bool isHover() const{ return mIsHover; }

    void enableGlider() { mIsGlider = true; }
    void disableGlider() { mIsGlider = false; }
    bool isGlider() const{ return mIsGlider; }

    const PSCarEngine& getCarEngine()const{return mCarEngine;}
    PSCarEngine& getCarEngine(){return mCarEngine;}

    void setVehicleType(VehicleType type){mVehicleType = type;}
    VehicleType getVehicleType() const {return mVehicleType;}

    void setRaceStarted(){mIsRaceStarted = true;}

    bool getFrontCollision() const;
    bool getBackCollision() const;
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

    const InitialVehicleSetup& getVehicleSetup()const{ return mVehicleSetup;}

    void repositionToStart();

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
    Ogre::Real mHandBreaks;
    Ogre::Real mSteeringOriginal;

    Ogre::uint8 mThrottleAdjusterCounter;
    Ogre::Real mThrottleAdjuster;

    PSAICar * mAICar;

    InitialVehicleSetup& mVehicleSetup;

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
    bool fallOffRestore();

    void adjustRot(const Ogre::Vector3& A, const Ogre::Vector3& B, Ogre::Real val);
    void adjustRot2(const Ogre::Vector3& A, const Ogre::Vector3& B, Ogre::Real val);

    Ogre::Real adjustSteering();

    StaticMeshProcesser* mMeshProcesser;

    Ogre::SceneNode *mChassis;

    const InitialVehicleSetup mInitialVehicleSetup;//to restore if falloff

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

    bool mIsNitro;
    size_t mNitroCounter;

    bool mIsSticky;
    bool mIsSpider;
    bool mIsICBM;
    bool mIsApollo;
    size_t mApolloAmount;
    bool mIsLunar;
    bool mIsJump;
    std::function<void(void)> mDisableJump;
    bool mIsHover;
    Ogre::Real mPrevMaxTravel;
    bool mIsGlider;

};

#endif