
#include "Physics.h"

#include "BulletCollision/BroadphaseCollision/btAxisSweep3.h"
#include "BulletCollision/CollisionDispatch/btCollisionDispatcher.h"
#include "BulletCollision/CollisionDispatch/btCollisionObject.h"
#include "BulletCollision/CollisionDispatch/btCollisionWorld.h"
#include "BulletCollision/CollisionDispatch/btDefaultCollisionConfiguration.h"
#include "BulletCollision/CollisionDispatch/btInternalEdgeUtility.h"
#include "BulletCollision/CollisionShapes/btBvhTriangleMeshShape.h"
#include "BulletCollision/CollisionShapes/btEmptyShape.h"
#include "BulletCollision/CollisionShapes/btSphereShape.h"
#include "BulletCollision/CollisionShapes/btTriangleMesh.h"

#include "../tools/PhysicsTools.h"

#include "../mesh/StaticMeshProcesser.h"

#include "PhysicsVehicle.h"

namespace
{
    const float aabbMax = 10000.0f; // world collisions max aabb
}//namespace

struct GameWorld
{
    GameWorld() :
        mDispatcher(&mCollisionConfiguration),
        mBroadphase(btVector3(-aabbMax, -aabbMax, -aabbMax), btVector3(aabbMax, aabbMax, aabbMax)),
        mCollisionWorld(&mDispatcher, &mBroadphase, &mCollisionConfiguration)
    {}

    btDefaultCollisionConfiguration mCollisionConfiguration;
    btCollisionDispatcher mDispatcher;
    btAxisSweep3 mBroadphase;
    btCollisionWorld mCollisionWorld;

};

Physics::Physics()
    : mGameWorld(new GameWorld())
{}

void Physics::timeStep()
{
    
    for(vehicles::iterator i = mVehicles.begin(), j = mVehicles.end(); i != j; ++i)
    {
        (*i).second->shiftPos(Ogre::Vector3(0.0f, 0.01f, 0.0f));
    }
}

void Physics::addPart(const DE2Part& part, const DE2SingleBatch& batch,
                        size_t partIndex, size_t batchIndex,
                        const Ogre::Real bodyRestitution, 
                        const Ogre::Real bodyFriction,
                        const Ogre::Real bodyRollingFriction)
{
    createTrimesh(part, batch);

    btBvhTriangleMeshShape * pGroundShape = static_cast<btBvhTriangleMeshShape*>(mStaticCollisionShapes[mStaticCollisionShapes.size() - 1].get());

    btTriangleInfoMap* triangleInfoMap = new btTriangleInfoMap();
    btGenerateInternalEdgeInfo(pGroundShape, triangleInfoMap);

    pGroundShape->setMargin(0.2f);

    CommonIncludes::shared_ptr<btCollisionObject> collisionObject = CommonIncludes::shared_ptr<btCollisionObject>(new btCollisionObject());
    collisionObject->setCollisionShape(pGroundShape);
    collisionObject->setRestitution(bodyRestitution);
    collisionObject->setFriction(bodyFriction);
    collisionObject->setRollingFriction(bodyRollingFriction);
    collisionObject->getWorldTransform().setOrigin(PhysicsTools::convert(part.offset));
    collisionObject->getWorldTransform().setRotation(PhysicsTools::convert(Ogre::Quaternion::IDENTITY));
    collisionObject->setCollisionFlags(collisionObject->getCollisionFlags() | btCollisionObject::CF_CUSTOM_MATERIAL_CALLBACK);

    mStaticCollisionObjects.push_back(collisionObject);

    mGameWorld->mCollisionWorld.addCollisionObject(collisionObject.get());

    mStaticBodies.insert(std::make_pair(collisionObject.get(), std::make_pair(partIndex, batchIndex)));
}

bool Physics::isRigidBodyStatic(const btCollisionObject * object, std::pair<int, int>& address)const
{
    bool res = false;

    if(!mStaticBodies.empty())
    {
        bodies::const_iterator i = mStaticBodies.find(object);

        if(i != mStaticBodies.end())
        {
            address = (*i).second;
            res = true;
        }
    }

    return res;
}

void Physics::launchRay(btCollisionWorld::AllHitsRayResultCallback &rayresult) const
{
    mGameWorld->mCollisionWorld.rayTest(rayresult.m_rayFromWorld, rayresult.m_rayToWorld, rayresult);
}

void Physics::createTrimesh(const DE2Part& part, const DE2SingleBatch& batch)
{
    unsigned int numFaces = batch.mIndexBuffer.size() / 3;

    CommonIncludes::shared_ptr<btTriangleMesh> triMesh = CommonIncludes::shared_ptr<btTriangleMesh>(new btTriangleMesh(true));

    for (unsigned int n = 0; n < numFaces; n += 3)
    {
        triMesh->addTriangle(
            PhysicsTools::convert(part.mVertexBuffer[batch.mIndexBuffer[n + 0]]), 
            PhysicsTools::convert(part.mVertexBuffer[batch.mIndexBuffer[n + 1]]), 
            PhysicsTools::convert(part.mVertexBuffer[batch.mIndexBuffer[n + 2]])
            );
    }

    CommonIncludes::shared_ptr<btCollisionShape> shape = CommonIncludes::shared_ptr<btCollisionShape>(new btBvhTriangleMeshShape(triMesh.get(), true));

    mStaticTriMeshes.push_back(triMesh);
    mStaticCollisionShapes.push_back(shape);
}

void Physics::addVehicle(const PSBaseVehicle * vehiclePtr,
                        Ogre::SceneNode *wheelNodes[PhysicsVehicle::mWheelsAmount], Ogre::SceneNode *chassis)
{
    vehicles::const_iterator found = mVehicles.find(vehiclePtr);

    if(found == mVehicles.end())
    {
        CommonIncludes::shared_ptr<PhysicsVehicle> vehicle = CommonIncludes::shared_ptr<PhysicsVehicle>(new PhysicsVehicle(wheelNodes, chassis));
        mVehicles.insert(std::make_pair(vehiclePtr, vehicle));
    }
}

void Physics::removeVehicle(const PSBaseVehicle * vehiclePtr)
{
    vehicles::const_iterator found = mVehicles.find(vehiclePtr);
    mVehicles.erase(found);
}