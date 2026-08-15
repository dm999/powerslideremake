#ifndef CHEATBOMB_H
#define CHEATBOMB_H

#include "../includes/OgreInclude.h"
#include "../listeners/PhysicsListener.h"

class SoundsProcesser;
class StaticMeshProcesser;

/*all calculations in left hand - like original data*/
class CheatBomb : public PhysicsListener
{
public:

    CheatBomb(StaticMeshProcesser * meshProesser, Ogre::SceneManager* sceneManager
#ifndef NO_OPENAL
        , SoundsProcesser * soundProcesser
#endif
        );
    ~CheatBomb();

    void createBombByPlayer(PhysicsVehicle * vehicle);

    void timeStepForVehicle(PhysicsVehicle * vehicle, const vehicles& vehiclesMap) override;

    bool isInProgress() const {return mIsBombInProgress;}

    //deathmatch: when set, the bomb explosion also drains nearby cars' life.
    void setDeathmatch(bool v){ mIsDeathmatch = v; }

private:

    void stopBomb();

    StaticMeshProcesser * mMeshProesser;
    Ogre::SceneManager* mSceneMgr;
#ifndef NO_OPENAL
    SoundsProcesser * mSoundProcesser;
#endif

    bool mIsBombInProgress;
    bool mIsBombJumpsInProgress;
    bool mIsBombExplosionInProgress;
    Ogre::uint16 mBlowCounter;
    Ogre::uint8 mExplosionCounter;
    PhysicsVehicle * mPlayerVehicle;

    Ogre::Vector3 mBombPosition;
    Ogre::Vector3 mBombVelocity;

    Ogre::SceneNode * mSphereNode;
    Ogre::ParticleSystem* mParticle;

    Ogre::String mNodeName;

    bool mIsFog;
    bool mIsDeathmatch;

    static Ogre::NameGenerator nameGenNodes;
    static Ogre::NameGenerator nameGenParticleMaterials;
};

class CheatBombs : public PhysicsListener
{
public:
    CheatBombs(StaticMeshProcesser * meshProesser, Ogre::SceneManager* sceneManager
#ifndef NO_OPENAL
        , SoundsProcesser * soundProcesser
#endif
        , size_t bombsMaxAmount = 100);

    void createBombByPlayer(PhysicsVehicle * vehicle);

    void timeStepForVehicle(PhysicsVehicle * vehicle, const vehicles& vehiclesMap) override;

    //deathmatch: propagate the damage-enabled flag to every bomb instance.
    void setDeathmatch(bool v){ for(size_t q = 0; q < mBombs.size(); ++q) mBombs[q].setDeathmatch(v); }

private:

    StaticMeshProcesser * mMeshProesser;
    Ogre::SceneManager* mSceneMgr;

    std::vector<CheatBomb> mBombs;
};

#endif