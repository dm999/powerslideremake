#ifndef PSMULTIPLAYERCAR_H
#define PSMULTIPLAYERCAR_H

#if defined(_MSC_VER) && (_MSC_VER < 1800)
typedef unsigned __int64 uint64_t;
#else
#include <cstdint>
#endif

#include "PSAICar.h"

namespace Ogre
{
    class MovableText;
}

class PSMultiplayerCar : public PSAICar
{
public:
    PSMultiplayerCar();
    virtual ~PSMultiplayerCar(){}

    void initModel( lua_State * pipeline, 
                            const GameState& gameState,
                            Ogre::SceneManager* sceneMgr, Ogre::SceneNode* mainNode,
                            ModelsPool* modelsPool,
                            OgreBulletDynamics::DynamicsWorld * world,
                            const std::string& characterName,
                            const Ogre::Matrix4& transform,
                            bool isPossesCamera, const std::string& humanName, bool isHuman);

    /**
     * Should delete title for multiplayer car
     */
    void clear()override;

    /**
     * Positioning of multiplayer car based on velocities
     */
    void setModelVelocity(const Ogre::Vector3& linear, const Ogre::Vector3& angular);

    void removeFromScene(Ogre::SceneManager* sceneMgr);

    void setCurrentLap(size_t currentLap){mCurrentLap = currentLap;}
    size_t getCurrentLap()const override{return mCurrentLap;}

    void setLapPosition(Ogre::Real lapPosition){mLapPosition = lapPosition;}
    Ogre::Real getLapPosition()const override{return mLapPosition;}

    void setLastTimeOfUpdate(uint64_t lastTimeOfUpdate){mLastTimeOfUpdate = lastTimeOfUpdate;}
    uint64_t getLastTimeOfUpdate()const {return mLastTimeOfUpdate;}

private:

    uint64_t mLastTimeOfUpdate;//other client timestamp

    size_t mCurrentLap;
    Ogre::Real mLapPosition;

    CommonIncludes::shared_ptr<Ogre::MovableText> mPlayerTitle;
};

#endif