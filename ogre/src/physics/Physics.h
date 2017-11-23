#ifndef PHYSICS_H
#define PHYSICS_H

#include <map>
#include <vector>

#include "../includes/CommonIncludes.h"

#include "BulletCollision/CollisionDispatch/btCollisionWorld.h"

#include "InitialVehicleSetup.h"

struct GameWorld;

struct DE2SingleBatch;
struct DE2Part;

class btCollisionObject;
class btCollisionShape;
class btTriangleMesh;
class btCollisionObject;

class PSBaseVehicle;
class PhysicsVehicle;

class StaticMeshProcesser;

class Physics
{
public:

    Physics(StaticMeshProcesser * meshProesser);
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
        Ogre::SceneNode *wheelNodes[InitialVehicleSetup::mWheelsAmount], Ogre::SceneNode *chassis
        );
    void removeVehicle(const PSBaseVehicle * vehiclePtr);

    void addKinematicObject(btCollisionObject* object);
    void removeCollisionObject(btCollisionObject* object);
    bool findCollision(const btCollisionObject* const object, const btCollisionObject*& staticObj, 
        Ogre::Vector3& pointOnStatic,
        int& triIndex,
        Ogre::Vector3& worldNormal, Ogre::Real& distance);

private:

    StaticMeshProcesser * mMeshProesser;

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
