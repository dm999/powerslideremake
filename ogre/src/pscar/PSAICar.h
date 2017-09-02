
#ifndef PSAICAR
#define PSAICAR

#include "../includes/OgreInclude.h"
#include "PSControllableCar.h"
#include "../gamelogic/AIUtils.h"

class PSAICar : public PSControllableCar
{
public:

    PSAICar();

    virtual ~PSAICar(){}

    virtual void initModel( lua_State * pipeline, 
                            const GameState& gameState,
                            Ogre::SceneManager* sceneMgr, Ogre::SceneNode* mainNode,
                            ModelsPool* modelsPool,
                            OgreBulletDynamics::DynamicsWorld * world,
                            const std::string& characterName,
                            const Ogre::Matrix4& transform,
                            const Ogre::Vector3& initialForcesLinear,
                            bool isPossesCamera)override;

    virtual void processInternalTick(float timeStep, bool isRaceStarted)override;

    void setLateralStabilizationCoeff(Ogre::Real linearStabilizationCoeff){mLateralStabilizationCoeff = linearStabilizationCoeff;}
    void setSpeedCoeff(Ogre::Real speedCoeff){mAIUtils.setSpeedCoeff(speedCoeff);}

    void setSteeringUmpulse(Ogre::Real impulse){mSteeringImpulse = impulse;}

    virtual void processWheelsCollision(    btManifoldPoint& cp, 
                                    const btCollisionObjectWrapper* colObj0Wrap, const btCollisionObjectWrapper* colObj1Wrap,
                                    StaticMeshProcesser& processer,
                                    int triIndex);

    void performAICorrection(const GameState& gameState, bool isRaceStarted, bool isGamePaused);
    void clearAIData();
    void setAIData(const AIWhole& aiWhole, Ogre::SceneManager* sceneMgr, bool isDebugAI);
    void raceStarted();

private:

    //virtual void adjustWheelsFriction(StaticMeshProcesser& processer);

    AIUtils mAIUtils;

    Ogre::Real mSteeringImpulse;
};

#endif