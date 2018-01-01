#ifndef CHEATBOMB_H
#define CHEATBOMB_H

#include "../includes/OgreInclude.h"
#include "../listeners/PhysicsListener.h"

class StaticMeshProcesser;

/*all calculations in left hand - like original data*/
class CheatBomb : public PhysicsListener
{
public:

    CheatBomb(StaticMeshProcesser * meshProesser, Ogre::SceneManager* sceneManager);
    ~CheatBomb();

    void createBombByPlayer(PhysicsVehicle * vehicle);

    void timeStepForVehicle(PhysicsVehicle * vehicle, const vehicles& vehiclesMap) override;

private:

    void stopBomb();

    StaticMeshProcesser * mMeshProesser;
    Ogre::SceneManager* mSceneMgr;

    bool mIsBombInProgress;
    bool mIsBombJumpsInProgress;
    Ogre::uint8 mBlowCounter;
    PhysicsVehicle * mPlayerVehicle;

    Ogre::Vector3 mBombPosition;
    Ogre::Vector3 mBombVelocity;

    Ogre::SceneNode * mSphereNode;
};

#endif