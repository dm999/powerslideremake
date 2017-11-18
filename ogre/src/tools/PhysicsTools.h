#ifndef PHYSICSTOOLS_H
#define PHYSICSTOOLS_H

#include "LinearMath/btQuaternion.h"
#include "LinearMath/btVector3.h"

#include "OgreVector3.h"
#include "OgreQuaternion.h"

class PhysicsTools
{
public:
    inline static btVector3 convert(const Ogre::Vector3 &vec) { return btVector3(vec.x, vec.y, vec.z); }

    inline static Ogre::Vector3 convert(const btVector3 &vec) { return Ogre::Vector3(vec.x(), vec.y(), vec.z()); }

    inline static btQuaternion convert(const Ogre::Quaternion &quat) { return btQuaternion(quat.x, quat.y, quat.z, quat.w); }

    inline static Ogre::Quaternion convert(const btQuaternion &quat) { return Ogre::Quaternion(quat.w(), quat.x(), quat.y(), quat.z()); }
};
#endif
