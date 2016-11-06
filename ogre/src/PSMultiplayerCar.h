#ifndef PSMULTIPLAYERCAR_H
#define PSMULTIPLAYERCAR_H

#include "PSAICar.h"

class PSMultiplayerCar : public PSAICar
{
public:
    PSMultiplayerCar();
    virtual ~PSMultiplayerCar(){}

    void setModelPosition(const Ogre::Vector3& pos, const Ogre::Quaternion& rot, const Ogre::Vector3& wheel0, const Ogre::Vector3& wheel1, const Ogre::Vector3& wheel2, const Ogre::Vector3& wheel3);
    void setModelRotationOnly(const Ogre::Quaternion& rot);
    void setModelVelocity(const Ogre::Vector3& linear, const Ogre::Vector3& angular);

    void removeFromScene(Ogre::SceneManager* sceneMgr);

    void setCurrentLap(size_t currentLap){mCurrentLap = currentLap;}
    size_t getCurrentLap()const override{return mCurrentLap;}

    void setLapPosition(Ogre::Real lapPosition){mLapPosition = lapPosition;}
    Ogre::Real getLapPosition()const override{return mLapPosition;}

private:

    void removeFromPhysicsSimulation();

    size_t mCurrentLap;
    Ogre::Real mLapPosition;
};

#endif