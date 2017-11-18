
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
                            Physics * world,
                            const std::string& characterName,
                            const Ogre::Matrix4& transform,
                            const Ogre::Vector3& initialForcesLinear,
                            bool isPossesCamera)override;

    virtual void processInternalTick(float timeStep, bool isRaceStarted)override;

    void setLateralStabilizationCoeff(Ogre::Real linearStabilizationCoeff){mLateralStabilizationCoeff = linearStabilizationCoeff;}
    void setAIImpulseHelper(const Ogre::Vector2& aiImpulseHelper){mAIImpulseHelper = aiImpulseHelper;}
    void setSpeedCoeff(Ogre::Real speedCoeff){mAIUtils.setSpeedCoeff(speedCoeff);}

    void setSteeringUmpulse(Ogre::Real impulse){mSteeringImpulse = impulse;}

    virtual void processWheelsCollision(    btManifoldPoint& cp, 
                                    const btCollisionObjectWrapper* colObj0Wrap, const btCollisionObjectWrapper* colObj1Wrap,
                                    const Physics * physicsProcesser,
                                    const StaticMeshProcesser& processer,
                                    int triIndex);

    void performAICorrection(const GameState& gameState, bool isRaceStarted, bool isGamePaused);
    void setAIData(const AIWhole& aiWhole, Ogre::SceneManager* sceneMgr, bool isDebugAI);
    void raceStarted();

    void setSlotIndex(size_t index){mSlotIndex = index;}
    size_t getSlotIndex() const {return mSlotIndex;}

    void setSteering(float value){mSteeringValue = value;}


private:

    virtual void adjustFrontWheelsAngle(const Ogre::FrameEvent &evt) override;

    float mSteeringValue;

    AIUtils mAIUtils;

    size_t mSlotIndex;

    Ogre::Real mSteeringImpulse;
};

#endif