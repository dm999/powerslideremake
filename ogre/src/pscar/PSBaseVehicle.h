#ifndef PSBASEVEHICLE_H
#define PSBASEVEHICLE_H

#include "../includes/OgreInclude.h"
#include "../includes/CommonIncludes.h"

#include "../physics/InitialVehicleSetup.h"

class CustomRigidBody;
class CustomRigidBodyWheel;
class Physics;
class PhysicsVehicle;

/**
 * Class to manipulate with bullet objects
 */
class PSBaseVehicle
{
public:

    PSBaseVehicle(){}
    virtual ~PSBaseVehicle(){}

    virtual void clear();

protected:

    void initPhysicalModel(Physics * world, 
        Ogre::SceneNode* modelNode, Ogre::SceneNode *wheelNodes[InitialVehicleSetup::mWheelsAmount], 
        const InitialVehicleSetup& initialVehicleSetup);

    /**
     * Could be slow due to total removal from world
     * http://www.bulletphysics.org/Bullet/phpBB3/viewtopic.php?f=9&t=4517
     */
    void repositionVehicle(const Ogre::Vector3& chassisPos, const Ogre::Quaternion& chassisRot, Ogre::SceneNode* modelNode, Ogre::SceneNode *wheelNodes[4]);

    Physics * mWorld;
    PhysicsVehicle * mPhysicsVehicle;

    InitialVehicleSetup mInitialVehicleSetup;

    static Ogre::NameGenerator nameGenNodes;

private:

    void addToWorld(Ogre::SceneNode* modelNode, Ogre::SceneNode *wheelNodes[4]);

};

#endif