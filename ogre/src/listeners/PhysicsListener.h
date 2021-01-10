#ifndef PHYSICSLISTENER_H
#define PHYSICSLISTENER_H

#include <map>
#include "../includes/CommonIncludes.h"

class Physics;
class PhysicsVehicle;
class PSBaseVehicle;

class PhysicsListener
{
public:

    virtual ~PhysicsListener(){}

    typedef std::map<PSBaseVehicle *, CommonIncludes::shared_ptr<PhysicsVehicle> > vehicles;

    virtual void timeStepForVehicle(PhysicsVehicle * vehicle, const vehicles& vehiclesMap){}
    virtual void timeStepBefore(Physics * physics){}
    virtual void timeStepAfter(Physics * physics){}

    virtual void carDead(PhysicsVehicle* vehicle) {}
};

#endif