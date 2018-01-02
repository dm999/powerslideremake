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

    bool isInProgress() const {return mIsBombInProgress;}

private:

    void stopBomb();

    StaticMeshProcesser * mMeshProesser;
    Ogre::SceneManager* mSceneMgr;

    bool mIsBombInProgress;
    bool mIsBombJumpsInProgress;
    bool mIsBombExplosionInProgress;
    Ogre::uint8 mBlowCounter;
    Ogre::uint8 mExplosionCounter;
    PhysicsVehicle * mPlayerVehicle;

    Ogre::Vector3 mBombPosition;
    Ogre::Vector3 mBombVelocity;

    Ogre::SceneNode * mSphereNode;
    Ogre::ParticleSystem* mParticle;

    Ogre::String mNodeName;

    bool mIsFog;

    static Ogre::NameGenerator nameGenNodes;
    static Ogre::NameGenerator nameGenParticleMaterials;
};

class CheatBombs : public PhysicsListener
{
public:
    CheatBombs(StaticMeshProcesser * meshProesser, Ogre::SceneManager* sceneManager, size_t bombsMaxAmount = 100);

    void createBombByPlayer(PhysicsVehicle * vehicle);

    void timeStepForVehicle(PhysicsVehicle * vehicle, const vehicles& vehiclesMap) override;

private:

    StaticMeshProcesser * mMeshProesser;
    Ogre::SceneManager* mSceneMgr;

    std::vector<CheatBomb> mBombs;
};

#endif