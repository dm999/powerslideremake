#ifndef PHYSICS_H
#define PHYSICS_H

#include <map>
#include <vector>

#include "../includes/CommonIncludes.h"

#include "BulletCollision/CollisionDispatch/btCollisionWorld.h"

struct GameWorld;

struct DE2SingleBatch;
struct DE2Part;

class btCollisionObject;
class btCollisionShape;
class btTriangleMesh;
class btCollisionObject;

class PSBaseVehicle;
class PhysicsVehicle;
struct InitialVehicleSetup;

class Physics
{
public:

    Physics();
    ~Physics();

    void timeStep();

    void addPart(const DE2Part& part, const DE2SingleBatch& batch,
                    size_t partIndex, size_t batchIndex,
                    const Ogre::Real bodyRestitution, 
                    const Ogre::Real bodyFriction,
                    const Ogre::Real bodyRollingFriction);

    bool isRigidBodyStatic(const btCollisionObject * object, std::pair<int, int>& address) const;

    void launchRay(btCollisionWorld::AllHitsRayResultCallback &rayresult) const;

    void addVehicle(const InitialVehicleSetup& initialVehicleSetup, const PSBaseVehicle * vehiclePtr, 
        Ogre::SceneNode *wheelNodes[4], Ogre::SceneNode *chassis
        );
    void removeVehicle(const PSBaseVehicle * vehiclePtr);

    void addCollisionObject(btCollisionObject* object);
    void removeCollisionObject(btCollisionObject* object);

private:

    void createTrimesh(const DE2Part& part, const DE2SingleBatch& batch);

    std::vector<CommonIncludes::shared_ptr<btTriangleMesh>> mStaticTriMeshes;
    std::vector<CommonIncludes::shared_ptr<btCollisionShape>> mStaticCollisionShapes;
    std::vector<CommonIncludes::shared_ptr<btCollisionObject>> mStaticCollisionObjects;

    typedef std::map<const btCollisionObject *, std::pair<int, int> > bodies;
    bodies mStaticBodies;

    CommonIncludes::shared_ptr<GameWorld> mGameWorld;

    typedef std::map<const PSBaseVehicle *, CommonIncludes::shared_ptr<PhysicsVehicle> > vehicles;
    vehicles mVehicles;
};

#endif
