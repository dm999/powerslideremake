#ifndef _OGREBULLETDYNAMICS_Hinge2Constraint_H
#define _OGREBULLETDYNAMICS_Hinge2Constraint_H

#include "OgreBulletDynamicsPreRequisites.h"

#include "OgreBulletDynamicsConstraint.h"


namespace OgreBulletDynamics
{
    // -------------------------------------------------------------------------
    // Hinge2Constraint  class
    class Hinge2Constraint : public TypedConstraint
    {
    public:
        Hinge2Constraint(RigidBody *rbA,
                        RigidBody *rbB,
                        const Ogre::Vector3 &anchor,
                        const Ogre::Vector3 &axisInA,
                        const Ogre::Vector3 &axisInB);

        virtual ~Hinge2Constraint();

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

    };
}
#endif //_OGREBULLETDYNAMICS_Hinge2Constraint_H

