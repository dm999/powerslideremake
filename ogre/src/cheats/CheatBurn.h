#ifndef CHEATBURN_H
#define CHEATBURN_H

#include "../includes/OgreInclude.h"
#include "../listeners/PhysicsListener.h"

class SoundsProcesser;
class StaticMeshProcesser;

/*all calculations in left hand - like original data*/
class CheatBurn : public PhysicsListener
{
public:

    CheatBurn(StaticMeshProcesser * meshProesser, Ogre::SceneManager* sceneManager, bool isFog
#ifndef NO_OPENAL
        , SoundsProcesser * soundProcesser
#endif
        );
    ~CheatBurn();

    void createBurnByPlayer(PhysicsVehicle * vehicle);

    void timeStepForVehicle(PhysicsVehicle * vehicle, const vehicles& vehiclesMap) override;

    bool isInProgress() const {return mIsBurnInProgress;}

    //deathmatch: when set, a direct burn hit also drains the target's life.
    void setDeathmatch(bool v){ mIsDeathmatch = v; }

private:

    void stopBurn();

    StaticMeshProcesser * mMeshProesser;
    Ogre::SceneManager* mSceneMgr;
#ifndef NO_OPENAL
    SoundsProcesser * mSoundProcesser;
#endif

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
    bool mIsDeathmatch;

    static Ogre::NameGenerator nameGenNodes;
    static Ogre::NameGenerator nameGenParticleMaterials;
};

class CheatBurns : public PhysicsListener
{
public:
    CheatBurns(StaticMeshProcesser * meshProesser, Ogre::SceneManager* sceneManager, bool isFog
#ifndef NO_OPENAL
        , SoundsProcesser * soundProcesser
#endif
        , size_t burnsMaxAmount = 100);

    void createBurnByPlayer(PhysicsVehicle * vehicle);

    void timeStepForVehicle(PhysicsVehicle * vehicle, const vehicles& vehiclesMap) override;

    //deathmatch: propagate the damage-enabled flag to every burn instance.
    void setDeathmatch(bool v){ for(size_t q = 0; q < mBurns.size(); ++q) mBurns[q].setDeathmatch(v); }

private:

    StaticMeshProcesser * mMeshProesser;
    Ogre::SceneManager* mSceneMgr;

    std::vector<CheatBurn> mBurns;
};

#endif