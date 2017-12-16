#ifndef PSCONTROLLABLECAR
#define PSCONTROLLABLECAR

#include "../includes/OgreInclude.h"
#include "PSBaseCar.h"

#include "../tools/LinearController.h"

#include "../gamelogic/LapUtils.h"

#include "../tools/Tools.h"

#include "PSCarEngine.h"

class CameraMan;

class PSControllableCar : public PSBaseCar
{
public:
    PSControllableCar();

    virtual ~PSControllableCar(){}

    virtual void initModel( lua_State * pipeline, 
                            const GameState& gameState,
                            Ogre::SceneManager* sceneMgr, Ogre::SceneNode* mainNode,
                            ModelsPool* modelsPool,
                            Physics * world,
                            const std::string& characterName,
                            InitialVehicleSetup& initialVehicleSetup,
                            bool isPossesCamera,
                            bool isAI = true);

    void setCameraMan(CameraMan * cameraMan){mCameraMan = cameraMan;}

    virtual void processFrameBeforePhysics(const Ogre::FrameEvent &evt, const StaticMeshProcesser& processer, bool isRaceStarted);
    virtual void processFrameAfterPhysics(const Ogre::FrameEvent &evt, bool isRaceStarted);

    void setAcceleration(bool isEnable){mAccelEnabled = isEnable;}
    bool getAcceleration() const{return mAccelEnabled;} // multiplayer

    void setBrake(bool isEnable){mBrakeEnabled = isEnable;}
    bool getBrake() const {return mBrakeEnabled;} // multiplayer

    void setSteerLeft(bool isEnable){mSteeringLeft = isEnable;}
    bool getSteerLeft()const {return mSteeringLeft;} // multiplayer

    void setSteerRight(bool isEnable){mSteeringRight = isEnable;}
    bool getSteerRight() const {return mSteeringRight;} // multiplayer

    bool getAcceleration(){return mAccelEnabled;}
    bool getBrake(){return mBrakeEnabled;}

    LapUtils& getLapUtils(){return mLapUtils;}
    const LapUtils& getLapUtils()const{return mLapUtils;}

    bool checkRearCollision(bool isBoth = false);
    bool checkFrontCollision();
    bool checkChassisCollision()const{return mChassisCollision;}

    size_t getCurrentLap() const override {return mLapUtils.getCurrentLap();}
    Ogre::Real getLapPosition() const override {return mLapUtils.getLapPosition();}

    bool getDisableMouse() const {return mIsDisableMouse;}
    void setDisableMouse(bool disableMouse) {mIsDisableMouse = disableMouse ;}

    void processSounds();

protected:

    LinearController<float> mPitchValueLow;
    LinearController<float> mPitchValueMid;
    LinearController<float> mPitchValueHigh;
    LinearController<float> mGainValueLow;
    LinearController<float> mGainValueMid;
    LinearController<float> mGainValueHigh;

    


private:

    LapUtils mLapUtils;

    bool mIsDisableMouse;
    bool mIsPossesCamera;

    bool mWheelCollisionFrontL;
    bool mWheelCollisionFrontR;
    bool mWheelCollisionBackL;
    bool mWheelCollisionBackR;
    bool mChassisCollision;

    Ogre::Timer mTimerJumpHappenFrontL;
    Ogre::Timer mTimerJumpHappenFrontR;
    Ogre::Timer mTimerJumpHappenRearL;
    Ogre::Timer mTimerJumpHappenRearR;

    Ogre::Timer mTimerCollisionHappenFront;
    Ogre::Timer mTimerCollisionHappenRear;

    LinearController<float> mDriveImpulse;
    LinearController<float> mResistanceImpulse;     //terrain back friction
    LinearController<float> mGroundSpoilerImpulse;
    LinearController<float> mAirSpoilerImpulse;
    LinearController<float> mWheelsRotationByEngineAddition;

    Ogre::Real mWheelRotationalAngleF;
    Ogre::Real mWheelRotationalAngleB;

protected:

    Ogre::ParticleSystem* mWheelBackLParticle;
    Ogre::ParticleSystem* mWheelBackRParticle;

    Ogre::SceneNode* mParticleNodeWheelBackL;
    Ogre::SceneNode* mParticleNodeWheelBackR;

private:

    std::vector<Particle> mParticles;

    Ogre::String mParticleMaterialName;

    static Ogre::NameGenerator nameGenMaterialsParticles;

    void adjustWheelsParticles(const Ogre::Quaternion& rot, Ogre::Real rotAngleAddition);

protected:

    float mSteering;//steering in angles (deg)
    bool mSteeringLeft;
    bool mSteeringRight;
    bool mBrakeEnabled;
    bool mAccelEnabled;

    float mSteeringMin;
    float mSteeringMinRad;
    float mSteeringMax;
    float mSteeringMaxRad;
    float mSteeringIncrement;   // steering sensivity

    Ogre::Real mLateralStabilizationCoeff;   // additional impulse to prevent turn over
    Ogre::Vector2 mAIImpulseHelper;

    CameraMan * mCameraMan;

    void cleanWheelsCollisionsFlags()
    {
        mWheelCollisionFrontL = false;
        mWheelCollisionFrontR = false;
        mWheelCollisionBackL = false;
        mWheelCollisionBackR = false;
        mChassisCollision = false;
    }

    void setWheelCollisionHappenFrontL(){mWheelCollisionFrontL = true;}
    void setWheelCollisionHappenFrontR(){mWheelCollisionFrontR = true;}
    void setWheelCollisionHappenBackL(){mWheelCollisionBackL = true;}
    void setWheelCollisionHappenBackR(){mWheelCollisionBackR = true;}

    private:

    virtual void adjustFrontWheelsAngle(const Ogre::FrameEvent &evt);

};

#endif