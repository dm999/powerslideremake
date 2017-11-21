#ifndef PHYSICSVEHICLE_H
#define PHYSICSVEHICLE_H

#include "../includes/OgreInclude.h"
#include "../includes/CommonIncludes.h"

#include "InitialVehicleSetup.h"

class btSphereShape;
class btCollisionObject;

class Physics;
class StaticMeshProcesser;

class PhysicsVehicle
{
public:

    static const int mWheelsAmount = 4;

    PhysicsVehicle(Physics* physics, 
                    StaticMeshProcesser * meshProesser,
                    const InitialVehicleSetup& initialVehicleSetup, 
                    Ogre::SceneNode *wheelNodes[mWheelsAmount], Ogre::SceneNode *chassis);
    virtual ~PhysicsVehicle();

    void timeStep();

protected:

    Physics* mPhysics;
    StaticMeshProcesser* mMeshProcesser;

    Ogre::Vector3 mImpulseLinear;
    Ogre::Vector3 mImpulseLinearInc;
    Ogre::Vector3 mImpulseRot;
    Ogre::Vector3 mImpulseRotInc;

    Ogre::Real mVehicleVelocityMod;

    Ogre::SceneNode *mWheelNodes[mWheelsAmount];
    Ogre::SceneNode *mChassis;

    CommonIncludes::shared_ptr<btSphereShape> mWheelShapeRL;
    CommonIncludes::shared_ptr<btSphereShape> mWheelShapeRR;
    CommonIncludes::shared_ptr<btSphereShape> mWheelShapeFL;
    CommonIncludes::shared_ptr<btSphereShape> mWheelShapeFR;

    CommonIncludes::shared_ptr<btCollisionObject> mWheelRL;
    CommonIncludes::shared_ptr<btCollisionObject> mWheelRR;
    CommonIncludes::shared_ptr<btCollisionObject> mWheelFL;
    CommonIncludes::shared_ptr<btCollisionObject> mWheelFR;

    CommonIncludes::shared_ptr<btSphereShape> mBodyShape;

    CommonIncludes::shared_ptr<btCollisionObject> mBody;

private:

    void initStep();

    void reposition(const Ogre::Vector3& posDiff);
    void rerotation(const Ogre::Quaternion& rot);

    void integrateLinear();
    void integrateRot();

    Ogre::Real momentOfInertiaProj(const Ogre::Vector3& axis)const;
    Ogre::Vector3 findTangent(const Ogre::Vector3& normal, const Ogre::Vector3& input) const;
    void createRotMatrix(Ogre::Vector3& matAxis, const Ogre::Vector3& normalisedImpulse, Ogre::Real angle) const;

    void processBody();

    void adjustImpulseInc(const Ogre::Vector3& rotAxis, const Ogre::Vector3& impulse);

    Ogre::Vector3 mBodyGlobal;
    Ogre::Vector3 mBodyGlobalPrev;

    const InitialVehicleSetup& mInitialVehicleSetup;
};

#endif