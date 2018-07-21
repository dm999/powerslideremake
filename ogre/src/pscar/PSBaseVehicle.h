#ifndef PSBASEVEHICLE_H
#define PSBASEVEHICLE_H

#include "../includes/OgreInclude.h"
#include "../includes/CommonIncludes.h"

#include "../physics/InitialVehicleSetup.h"

#include "../Enums.h"

class CustomRigidBody;
class CustomRigidBodyWheel;
class Physics;
class PhysicsVehicle;
class GameState;

/**
 * Class to manipulate with bullet objects
 */
class PSBaseVehicle
{
public:

    PSBaseVehicle(){}
    virtual ~PSBaseVehicle(){}

    virtual void clear();

    virtual void processCamera(GameState& gameState) = 0;

    PhysicsVehicle* getPhysicsVehicle() {return mPhysicsVehicle;}
    const PhysicsVehicle* getPhysicsVehicle() const {return mPhysicsVehicle;}

protected:

    void initPhysicalModel(Physics * world, 
        Ogre::SceneNode* modelNode, Ogre::SceneNode *wheelNodes[InitialVehicleSetup::mWheelsAmount], 
        const InitialVehicleSetup& initialVehicleSetup,
        InputType type);

    //for multiplayer
    void repositionVehicle(const Ogre::Vector3& chassisPos, const Ogre::Quaternion& chassisRot, Ogre::SceneNode* modelNode, Ogre::SceneNode *wheelNodes[4]);

    Physics * mWorld;
    PhysicsVehicle * mPhysicsVehicle;

    InitialVehicleSetup mInitialVehicleSetup;

    bool mIsAI;

    static Ogre::NameGenerator nameGenNodes;

};

#endif