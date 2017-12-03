
#ifndef PSPLAYERCAR_H
#define PSPLAYERCAR_H

#include "PSControllableCar.h"
#include "../includes/OISInclude.h"

#include "../tools/LinearController.h"

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
                            ModelsPool* modelsPool,
                            Physics * world,
                            const std::string& characterName,
                            InitialVehicleSetup& initialVehicleSetup,
                            bool isPossesCamera);

    virtual void processInternalTick(float timeStep, bool isRaceStarted) override;

    void keyDown(OIS::KeyCode key);
    void keyUp(OIS::KeyCode key);

    bool isCollideChassis(const PSBaseCar& otherCar)const;

private:

    float mSteeringAngleVelocity;
    LinearController<float> mRotationIntensity;

    void processSteering(bool isRaceStarted);

    bool checkOverSteer();
    Ogre::Timer mTimerOversteer;
};

#endif