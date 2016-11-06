#ifndef _OGREBULLETDYNAMICS_SixDofSpring2Constraint_H
#define _OGREBULLETDYNAMICS_SixDofSpring2Constraint_H

#include "OgreBulletDynamicsPreRequisites.h"

#include "OgreBulletDynamicsConstraint.h"


namespace OgreBulletDynamics
{
    // -------------------------------------------------------------------------
    // SixDofSpringConstraint  class
    class SixDofSpring2Constraint : public TypedConstraint
    {
    public:
        SixDofSpring2Constraint(RigidBody *rbA,
                               RigidBody *rbB,
                               const Ogre::Vector3 &FrameInAVector,
                               const Ogre::Quaternion &FrameInAOrientation,
                               const Ogre::Vector3 &FrameInBVector,
                               const Ogre::Quaternion &FrameInBOrientation);

        void setLinearLowerLimit(const Ogre::Vector3 &linearLower);
        void setLinearUpperLimit(const Ogre::Vector3 &linearUpper);
        void setAngularLowerLimit(const Ogre::Vector3 &angularLower);
        void setAngularUpperLimit(const Ogre::Vector3 &angularUpper);

        void setLimit(const int axis,
                      const Ogre::Real lo,
                      const Ogre::Real hi);

        void enableSpring(int index, bool onOff);
        void setStiffness(int index, const Ogre::Real stiffness, bool limitIfNeeded = true);
        void setDamping(int index, const Ogre::Real damping, bool limitIfNeeded = true);
        void setEquilibriumPoint(int index);
        void setEquilibriumPoint(int index, const Ogre::Real val);
        void setEquilibriumPoint();
        void setParam(int num, const Ogre::Real value, int axis = -1);
		//! Test limit
		/*!
		- free means upper < lower,
		- locked means upper == lower
		- limited means upper > lower
		- limitIndex: first 3 are linear, next 3 are angular
		*/
        bool isLimited(int limitIndex);

        virtual ~SixDofSpring2Constraint();

    };
}
#endif //_OGREBULLETDYNAMICS_SixDofSpring2Constraint_H

