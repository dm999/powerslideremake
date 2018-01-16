
#ifndef PSPLAYERCAR_H
#define PSPLAYERCAR_H

#include "PSControllableCar.h"
#include "../includes/OISInclude.h"
#include "OgreInput.h"

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

    void keyDown(OgreBites::Keycode key);
    void keyUp(OgreBites::Keycode key);

    void mousePressed(Uint8 id);
    void mouseReleased(Uint8 id);
    void mouseMoved(const Ogre::Vector2& pos, Ogre::Real windowWidth);

    void raceStarted();

    bool isCollideChassis(const PSBaseCar& otherCar)const;

    Ogre::Real getSteering() const;

private:

};

#endif