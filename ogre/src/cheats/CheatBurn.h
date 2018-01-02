#ifndef CHEATBURN_H
#define CHEATBURN_H

#include "../includes/OgreInclude.h"
#include "../listeners/PhysicsListener.h"

class StaticMeshProcesser;

/*all calculations in left hand - like original data*/
class CheatBurn : public PhysicsListener
{
public:

    CheatBurn(StaticMeshProcesser * meshProesser, Ogre::SceneManager* sceneManager, bool isFog);
    ~CheatBurn();

    void createBurnByPlayer(PhysicsVehicle * vehicle);

    void timeStepForVehicle(PhysicsVehicle * vehicle, const vehicles& vehiclesMap) override;

    bool isInProgress() const {return mIsBurnInProgress;}

private:

    void stopBurn();

    StaticMeshProcesser * mMeshProesser;
    Ogre::SceneManager* mSceneMgr;

    bool mIsBurnInProgress;
    bool mIsBurnExplosionInProgress;
    Ogre::Real mFlyPath;
    Ogre::Real mFlyPathLength;
    Ogre::uint8 mExplosionCounter;
    PhysicsVehicle * mPlayerVehicle;

    Ogre::Vector3 mBurnPosition;
    Ogre::Vector3 mBurnVelocity;

    Ogre::SceneNode * mSphereNode;
    Ogre::ParticleSystem* mParticle;

    Ogre::String mNodeName;

    bool mIsFog;

    static Ogre::NameGenerator nameGenNodes;
    static Ogre::NameGenerator nameGenParticleMaterials;
};

class CheatBurns : public PhysicsListener
{
public:
    CheatBurns(StaticMeshProcesser * meshProesser, Ogre::SceneManager* sceneManager, bool isFog, size_t burnsMaxAmount = 100);

    void createBurnByPlayer(PhysicsVehicle * vehicle);

    void timeStepForVehicle(PhysicsVehicle * vehicle, const vehicles& vehiclesMap) override;

private:

    StaticMeshProcesser * mMeshProesser;
    Ogre::SceneManager* mSceneMgr;

    std::vector<CheatBurn> mBurns;
};

#endif