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
                               PSAICar * aiCar);

    virtual ~PhysicsVehicleAI(){}

    void setSteering(Ogre::Real value);
    void setAcceleration(Ogre::Real value);
    void setBreaks(Ogre::Real value);

protected:

    virtual void doAIStep(const GameState& gameState) override;
};

#endif