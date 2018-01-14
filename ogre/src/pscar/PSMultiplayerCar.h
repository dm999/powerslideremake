#ifndef PSMULTIPLAYERCAR_H
#define PSMULTIPLAYERCAR_H

#include "PSPlayerCar.h"

namespace Ogre
{
    class MovableText;
}

class PSMultiplayerCar : public PSPlayerCar
{
public:
    PSMultiplayerCar();
    virtual ~PSMultiplayerCar(){}

    void initModel( lua_State * pipeline, 
                            const GameState& gameState,
                            Ogre::SceneManager* sceneMgr, Ogre::SceneNode* mainNode,
                            ModelsPool* modelsPool,
                            Physics * world,
                            const std::string& characterName,
                            InitialVehicleSetup& initialVehicleSetup,
                            bool isPossesCamera, const std::string& humanName, bool isHuman);

    /**
     * Should delete title for multiplayer car
     */
    void clear()override;

    /**
     * Positioning of multiplayer car based on impulses
     */
    void setModelImpulse(const Ogre::Vector3& linear, const Ogre::Vector3& angular);

    void removeFromScene(Ogre::SceneManager* sceneMgr);

    void setCurrentLap(size_t currentLap){mCurrentLap = currentLap;}
    size_t getCurrentLap()const override{return mCurrentLap;}

    void setLapPosition(Ogre::Real lapPosition){mLapPosition = lapPosition;}
    Ogre::Real getLapPosition()const override{return mLapPosition;}

    void setLastTimeOfUpdate(Ogre::uint64 lastTimeOfUpdate){mLastTimeOfUpdate = lastTimeOfUpdate;}
    Ogre::uint64 getLastTimeOfUpdate()const {return mLastTimeOfUpdate;}

private:

    Ogre::uint64 mLastTimeOfUpdate;//other client timestamp

    size_t mCurrentLap;
    Ogre::Real mLapPosition;

    CommonIncludes::shared_ptr<Ogre::MovableText> mPlayerTitle;
};

#endif