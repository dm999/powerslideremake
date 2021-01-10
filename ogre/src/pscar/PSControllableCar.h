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

    virtual void processFrameBeforePhysics(const StaticMeshProcesser& processer, bool isRaceStarted);

    virtual void processCamera(GameState& gameState) override;

    void setAcceleration(bool isEnable){mAccelEnabled = isEnable;}
    bool getAcceleration() const{return mAccelEnabled;} // multiplayer

    void setBrake(bool isEnable){mBrakeEnabled = isEnable;}
    bool getBrake() const {return mBrakeEnabled;} // multiplayer

    bool getAcceleration(){return mAccelEnabled;}
    bool getBrake(){return mBrakeEnabled;}

    LapUtils& getLapUtils(){return mLapUtils;}
    const LapUtils& getLapUtils()const{return mLapUtils;}

    size_t getCurrentLap() const override {return mLapUtils.getCurrentLap();}
    Ogre::Real getLapPosition() const override {return mLapUtils.getLapPosition();}

    bool getDisableMouse() const {return mIsDisableMouse;}
    void setDisableMouse(bool disableMouse) {mIsDisableMouse = disableMouse ;}

    void setParticlesVisibility(bool isVisible);

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

protected:

    Ogre::ParticleSystem* mWheelBackLParticle;
    Ogre::ParticleSystem* mWheelBackRParticle;
    Ogre::ParticleSystem* mDeadParticle;
    Ogre::ParticleSystem* mDeadParticle2;

    Ogre::SceneNode* mParticleNodeWheelBackL;
    Ogre::SceneNode* mParticleNodeWheelBackR;
    Ogre::SceneNode* mParticleDead;
    Ogre::SceneNode* mParticleDead2;

private:

    std::vector<Particle> mParticles;

    Ogre::String mParticleMaterialName;
    Ogre::String mParticleMaterialNameDead;
    Ogre::String mParticleMaterialNameDead2;

    static Ogre::NameGenerator nameGenMaterialsParticles;

    void adjustWheelsParticles(const Ogre::Quaternion& rot);

protected:

    bool mBrakeEnabled;
    bool mAccelEnabled;

    CameraMan * mCameraMan;

};

#endif