
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

    CommonIncludes::shared_ptr<btMotionState> motionState = CommonIncludes::shared_ptr<btMotionState>(new btDefaultMotionState());
    mStaticMotionStates.push_back(motionState);

    const btScalar zeroMass(0.f);
    CommonIncludes::shared_ptr<btRigidBody> body = CommonIncludes::shared_ptr<btRigidBody>(new btRigidBody(zeroMass, motionState.get(), pGroundShape));
    body->setRestitution(bodyRestitution);
    body->setFriction(bodyFriction);
    body->setRollingFriction(bodyRollingFriction);
    body->getWorldTransform().setOrigin(PhysicsTools::convert(part.offset));
    body->getWorldTransform().setRotation(PhysicsTools::convert(Ogre::Quaternion::IDENTITY));
    body->setCollisionFlags(body->getCollisionFlags() | btCollisionObject::CF_CUSTOM_MATERIAL_CALLBACK);
    mStaticRigidBodies.push_back(body);

    mStaticBodies.insert(std::make_pair(body.get(), std::make_pair(partIndex, batchIndex)));
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