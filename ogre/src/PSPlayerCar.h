
#ifndef PSPLAYERCAR_H
#define PSPLAYERCAR_H

#include "PSControllableCar.h"
#include "OISInclude.h"

#include "LinearController.h"

class StaticMeshProcesser;
class ModelsPool;

class PSPlayerCar : public PSControllableCar
{
public:

    PSPlayerCar();
    virtual ~PSPlayerCar(){}

    virtual void initModel( lua_State * pipeline, 
                            const GameState& gameState,
                            Ogre::SceneManager* sceneMgr, Ogre::SceneNode* mainNode,
                            CameraMan * cameraMan,
                            ModelsPool* modelsPool,
                            OgreBulletDynamics::DynamicsWorld * world,
                            GameCars gameCar,
                            const Ogre::Matrix4& transform,
                            bool isPossesCamera);

    void keyDown(OIS::KeyCode key);
    void keyUp(OIS::KeyCode key);

    bool isCollideChassis(const PSBaseCar& otherCar, const btCollisionObjectWrapper* colObj0Wrap, const btCollisionObjectWrapper* colObj1Wrap)const;

protected:

    virtual void calculateSteering(const Ogre::FrameEvent &evt, bool isRaceStarted);

private:

    LinearController<float> mSteeringImpulse;
    LinearController<float> mOverSteeringImpulseLength;
    //LinearController<float> mParticlesEmmisionRate;
    //LinearController<float> mJumpImpulse;



    Ogre::Real mSideImpulse;    // oversteer simulation
    Ogre::Timer mSteerTimer;
    unsigned long mLastSteerLength;
    bool mIsTimerFirstTime;
    bool mIsLastTurnLeft;
    Ogre::Real mSmoothSideImpulse;
};

#endif