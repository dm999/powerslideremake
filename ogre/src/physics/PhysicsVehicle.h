#ifndef PHYSICSVEHICLE_H
#define PHYSICSVEHICLE_H

#include "../includes/OgreInclude.h"

class PhysicsVehicle
{
public:

    static const int mWheelsAmount = 4;

    PhysicsVehicle(Ogre::SceneNode *wheelNodes[mWheelsAmount], Ogre::SceneNode *chassis);
    virtual ~PhysicsVehicle();

    void shiftPos(const Ogre::Vector3& shiftAmount);

protected:

    Ogre::SceneNode *mWheelNodes[mWheelsAmount];
    Ogre::SceneNode *mChassis;

private:
};

#endif