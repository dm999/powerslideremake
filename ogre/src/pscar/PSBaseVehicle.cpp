
#include "PSBaseVehicle.h"

#include "../physics/Physics.h"

Ogre::NameGenerator PSBaseVehicle::nameGenNodes("Scene/Node/Vehicle/Name");

void PSBaseVehicle::initPhysicalModel(Physics * world, 
    Ogre::SceneNode* modelNode, Ogre::SceneNode *wheelNodes[InitialVehicleSetup::mWheelsAmount], 
    const InitialVehicleSetup& initialVehicleSetup)
{
    mWorld = world;

    mInitialVehicleSetup = initialVehicleSetup;

    addToWorld(modelNode, wheelNodes);
}

void PSBaseVehicle::repositionVehicle(const Ogre::Vector3& chassisPos, const Ogre::Quaternion& chassisRot, Ogre::SceneNode* modelNode, Ogre::SceneNode *wheelNodes[4])
{
    //d.polubotko: reimplement
    mInitialVehicleSetup.mCarGlobalPos = chassisPos;
    mInitialVehicleSetup.mCarRot = chassisRot;
}

void PSBaseVehicle::clear()
{
    mWorld->removeVehicle(this);
}

void PSBaseVehicle::addToWorld(Ogre::SceneNode* modelNode, Ogre::SceneNode *wheelNodes[4])
{
    mPhysicsVehicle = mWorld->addVehicle(mInitialVehicleSetup, this, wheelNodes, modelNode);
}
