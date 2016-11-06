#include "OgreBulletDynamics.h"

#include "OgreBulletDynamicsWorld.h"
#include "OgreBulletDynamicsRigidBody.h"

#include "OgreBulletDynamicsConstraint.h"
#include "OgreBulletDynamics6DofSpring2Constraint.h"

using namespace Ogre;

namespace OgreBulletDynamics
{
    // -------------------------------------------------------------------------
    SixDofSpring2Constraint::SixDofSpring2Constraint(RigidBody *rbA,
                                                   RigidBody *rbB,
                                                   const Vector3 &FrameInAVector3,
                                                   const Quaternion& FrameInAOrientation,
                                                   const Vector3 &FrameInBVector3,
                                                   const Quaternion& FrameInBOrientation)
        : TypedConstraint(rbA, rbB)
    {
        btTransform frameInA(OgreBulletCollisions::convert(FrameInAOrientation),
                             OgreBulletCollisions::convert(FrameInAVector3));
        btTransform frameInB(OgreBulletCollisions::convert(FrameInBOrientation),
                             OgreBulletCollisions::convert(FrameInBVector3));

        mConstraint = new btGeneric6DofSpring2Constraint(
            *rbA->getBulletRigidBody(),
            *rbB->getBulletRigidBody(),
            frameInA,
            frameInB);
    }
    // -------------------------------------------------------------------------
    SixDofSpring2Constraint::~SixDofSpring2Constraint()
    {
	}
	// -------------------------------------------------------------------------
    void SixDofSpring2Constraint::setLinearLowerLimit(const Ogre::Vector3& linearLower)
	{
        static_cast<btGeneric6DofSpring2Constraint *>(mConstraint)->setLinearLowerLimit(OgreBulletCollisions::convert(linearLower));
	}
	// -------------------------------------------------------------------------
    void SixDofSpring2Constraint::setLinearUpperLimit(const Ogre::Vector3& linearUpper)
	{
        static_cast<btGeneric6DofSpring2Constraint *>(mConstraint)->setLinearUpperLimit(OgreBulletCollisions::convert(linearUpper));
	}
	// -------------------------------------------------------------------------
    void SixDofSpring2Constraint::setAngularLowerLimit(const Ogre::Vector3& angularLower)
	{
        static_cast<btGeneric6DofSpring2Constraint *>(mConstraint)->setAngularLowerLimit(OgreBulletCollisions::convert(angularLower));
	}
	// -------------------------------------------------------------------------
    void SixDofSpring2Constraint::setAngularUpperLimit(const Ogre::Vector3& angularUpper)
	{
        static_cast<btGeneric6DofSpring2Constraint *>(mConstraint)->setAngularUpperLimit(OgreBulletCollisions::convert(angularUpper));
	}
	// -------------------------------------------------------------------------
    void SixDofSpring2Constraint::setLimit(const int axis, const Ogre::Real lo, const Ogre::Real hi)
	{
        static_cast<btGeneric6DofSpring2Constraint *>(mConstraint)->setLimit(axis, lo, hi);
	}
	// -------------------------------------------------------------------------
    bool SixDofSpring2Constraint::isLimited(int limitIndex)
	{
        return static_cast<btGeneric6DofSpring2Constraint *>(mConstraint)->isLimited(limitIndex);
	}
	// -------------------------------------------------------------------------
    void SixDofSpring2Constraint::enableSpring(int index, bool onOff)
	{
        static_cast<btGeneric6DofSpring2Constraint *>(mConstraint)->enableSpring(index, onOff);
	}
	// -------------------------------------------------------------------------
    void SixDofSpring2Constraint::setStiffness(int index, const Ogre::Real stiffness, bool limitIfNeeded)
	{
        static_cast<btGeneric6DofSpring2Constraint *>(mConstraint)->setStiffness(index, stiffness, limitIfNeeded);
	}
	// -------------------------------------------------------------------------
    void SixDofSpring2Constraint::setDamping(int index, const Ogre::Real damping, bool limitIfNeeded)
	{
        static_cast<btGeneric6DofSpring2Constraint *>(mConstraint)->setDamping(index, damping, limitIfNeeded);
	}
	// -------------------------------------------------------------------------
    void SixDofSpring2Constraint::setEquilibriumPoint(int index)
	{
        static_cast<btGeneric6DofSpring2Constraint *>(mConstraint)->setEquilibriumPoint(index);
	}
    void SixDofSpring2Constraint::setEquilibriumPoint(int index, const Ogre::Real val)
	{
        static_cast<btGeneric6DofSpring2Constraint *>(mConstraint)->setEquilibriumPoint(index, val);
	}
	// -------------------------------------------------------------------------
    void SixDofSpring2Constraint::setEquilibriumPoint()
	{
        static_cast<btGeneric6DofSpring2Constraint *>(mConstraint)->setEquilibriumPoint();
	}
    void SixDofSpring2Constraint::setParam(int num, const Ogre::Real value, int axis)
    {
        static_cast<btGeneric6DofSpring2Constraint *>(mConstraint)->setParam(num, value, axis);
    }
}
