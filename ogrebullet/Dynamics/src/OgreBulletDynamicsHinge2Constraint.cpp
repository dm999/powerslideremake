#include "OgreBulletDynamics.h"

#include "OgreBulletDynamicsWorld.h"
#include "OgreBulletDynamicsRigidBody.h"
#include "OgreBulletDynamicsConstraint.h"
#include "OgreBulletDynamicsHinge2Constraint.h"

using namespace Ogre;

namespace OgreBulletDynamics
{
    Hinge2Constraint::Hinge2Constraint(RigidBody *rbA,
                                     RigidBody *rbB,
                                     const Vector3 &anchor,
                                     const Vector3 &axisInA,
                                     const Vector3 &axisInB)
        : TypedConstraint(rbA, rbB)
    {
        btVector3 btAnchor;
        btVector3 axis1;
        btVector3 axis2;
        btAnchor = OgreBulletCollisions::convert(anchor);
        axis1 = OgreBulletCollisions::convert(axisInA);
        axis2 = OgreBulletCollisions::convert(axisInB);
       
        mConstraint = new btHinge2Constraint(
            *rbA->getBulletRigidBody(),
            *rbB->getBulletRigidBody(),
            btAnchor, axis1, axis2);
    }
    // -------------------------------------------------------------------------
    Hinge2Constraint::~Hinge2Constraint()
    {
    }
    void Hinge2Constraint::setLinearLowerLimit(const Ogre::Vector3& linearLower)
	{
        static_cast<btHinge2Constraint *>(mConstraint)->setLinearLowerLimit(OgreBulletCollisions::convert(linearLower));
	}
	// -------------------------------------------------------------------------
    void Hinge2Constraint::setLinearUpperLimit(const Ogre::Vector3& linearUpper)
	{
        static_cast<btHinge2Constraint *>(mConstraint)->setLinearUpperLimit(OgreBulletCollisions::convert(linearUpper));
	}
	// -------------------------------------------------------------------------
    void Hinge2Constraint::setAngularLowerLimit(const Ogre::Vector3& angularLower)
	{
        static_cast<btHinge2Constraint *>(mConstraint)->setAngularLowerLimit(OgreBulletCollisions::convert(angularLower));
	}
	// -------------------------------------------------------------------------
    void Hinge2Constraint::setAngularUpperLimit(const Ogre::Vector3& angularUpper)
	{
        static_cast<btHinge2Constraint *>(mConstraint)->setAngularUpperLimit(OgreBulletCollisions::convert(angularUpper));
	}
	// -------------------------------------------------------------------------
    void Hinge2Constraint::setLimit(const int axis, const Ogre::Real lo, const Ogre::Real hi)
	{
        static_cast<btHinge2Constraint *>(mConstraint)->setLimit(axis, lo, hi);
	}
	// -------------------------------------------------------------------------
    bool Hinge2Constraint::isLimited(int limitIndex)
	{
        return static_cast<btHinge2Constraint *>(mConstraint)->isLimited(limitIndex);
	}
	// -------------------------------------------------------------------------
    void Hinge2Constraint::enableSpring(int index, bool onOff)
	{
        static_cast<btHinge2Constraint *>(mConstraint)->enableSpring(index, onOff);
	}
	// -------------------------------------------------------------------------
    void Hinge2Constraint::setStiffness(int index, const Ogre::Real stiffness, bool limitIfNeeded)
	{
        static_cast<btHinge2Constraint *>(mConstraint)->setStiffness(index, stiffness, limitIfNeeded);
	}
	// -------------------------------------------------------------------------
    void Hinge2Constraint::setDamping(int index, const Ogre::Real damping, bool limitIfNeeded)
	{
        static_cast<btHinge2Constraint *>(mConstraint)->setDamping(index, damping, limitIfNeeded);
	}
	// -------------------------------------------------------------------------
    void Hinge2Constraint::setEquilibriumPoint(int index)
	{
        static_cast<btHinge2Constraint *>(mConstraint)->setEquilibriumPoint(index);
	}
    void Hinge2Constraint::setEquilibriumPoint(int index, const Ogre::Real val)
	{
        static_cast<btHinge2Constraint *>(mConstraint)->setEquilibriumPoint(index, val);
	}
	// -------------------------------------------------------------------------
    void Hinge2Constraint::setEquilibriumPoint()
	{
        static_cast<btHinge2Constraint *>(mConstraint)->setEquilibriumPoint();
	}
    void Hinge2Constraint::setParam(int num, const Ogre::Real value, int axis)
    {
        static_cast<btHinge2Constraint *>(mConstraint)->setParam(num, value, axis);
    }
}