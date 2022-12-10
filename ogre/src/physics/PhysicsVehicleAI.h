#ifndef PHYSICSVEHICLEAI_H
#define PHYSICSVEHICLEAI_H

#include "PhysicsVehicle.h"

class PhysicsVehicleAI : public PhysicsVehicle
{
public:

    PhysicsVehicleAI(Physics* physics, 
                               StaticMeshProcesser * meshProesser,
                               InitialVehicleSetup& initialVehicleSetup, 
                               Ogre::SceneNode *wheelNodes[InitialVehicleSetup::mWheelsAmount],
                               Ogre::SceneNode *chassis,
                               InputType type,
                               PSAICar * aiCar,
                               bool isUltraInsane);

    virtual ~PhysicsVehicleAI(){}

    void setSteering(Ogre::Real value);
    void setAcceleration(Ogre::Real value);
    void setBreaks(Ogre::Real value);

    Ogre::Real getLinearVelocity() const;
    void setLinearVelocity(Ogre::Real val);
    Ogre::Vector3 getLinearImpulseInc() const;
    void setLinearImpulseInc(const Ogre::Vector3& val);

protected:

    virtual void doAIStep(const GameState& gameState) override;

    virtual Ogre::Real doAdjustAISteering(Ogre::Real val) const override;
    virtual Ogre::Real doGetVelocityScale() const override;
    virtual Ogre::Real doGetThrottleScale() const override;
    virtual Ogre::Real doGetTractionScale() const override;
};

#endif