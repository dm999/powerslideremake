
#include "PSBaseVehicle.h"

#include "../physics/Physics.h"

Ogre::NameGenerator PSBaseVehicle::nameGenNodes("Scene/Node/Vehicle/Name");

void PSBaseVehicle::initPhysicalModel(Physics * world, 
    Ogre::SceneNode* modelNode, Ogre::SceneNode *wheelNodes[InitialVehicleSetup::mWheelsAmount], 
    const InitialVehicleSetup& initialVehicleSetup,
    InputType type,
    bool isUltraInsane)
{
    mWorld = world;

    mInitialVehicleSetup = initialVehicleSetup;

    mPhysicsVehicle = mWorld->addVehicle(mInitialVehicleSetup, this, wheelNodes, modelNode, type, mIsAI, isUltraInsane);
}

void PSBaseVehicle::repositionVehicle(const Ogre::Vector3& chassisPos, const Ogre::Quaternion& chassisRot, Ogre::SceneNode* modelNode, Ogre::SceneNode *wheelNodes[4])
{
    mInitialVehicleSetup.mCarGlobalPos = chassisPos;
    mInitialVehicleSetup.mCarRot = chassisRot;
}

void PSBaseVehicle::clear()
{
    mWorld->removeVehicle(this);
}
