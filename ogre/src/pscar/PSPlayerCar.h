
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

    void keyDown(OIS::KeyCode key);
    void keyUp(OIS::KeyCode key);

    void mousePressed(OIS::MouseButtonID id);
    void mouseReleased(OIS::MouseButtonID id);
    void mouseMoved(Ogre::Vector2& pos, Ogre::Real windowWidth);

    void raceStarted();

    bool isCollideChassis(const PSBaseCar& otherCar)const;

private:

};

#endif