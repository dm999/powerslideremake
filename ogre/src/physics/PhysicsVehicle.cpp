
#include "PhysicsVehicle.h"

PhysicsVehicle::PhysicsVehicle(Ogre::SceneNode *wheelNodes[mWheelsAmount], Ogre::SceneNode *chassis)
: mChassis(chassis)
{
    for(int q = 0; q < mWheelsAmount; ++q)
    {
        mWheelNodes[q] = wheelNodes[q];
    }
}

PhysicsVehicle::~PhysicsVehicle(){}

void PhysicsVehicle::shiftPos(const Ogre::Vector3& shiftAmount)
{
    for(int q = 0; q < mWheelsAmount; ++q)
    {
        mWheelNodes[q]->translate(shiftAmount);
    }

    mChassis->translate(shiftAmount);
}