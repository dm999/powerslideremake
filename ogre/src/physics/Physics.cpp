
#include "Physics.h"

#include "BulletCollision/BroadphaseCollision/btAxisSweep3.h"
#include "BulletCollision/CollisionDispatch/btCollisionDispatcher.h"
#include "BulletCollision/CollisionDispatch/btCollisionObject.h"
#include "BulletCollision/CollisionDispatch/btCollisionWorld.h"
#include "BulletCollision/CollisionDispatch/btDefaultCollisionConfiguration.h"
#include "BulletCollision/CollisionDispatch/btInternalEdgeUtility.h"
#include "BulletCollision/CollisionShapes/btBvhTriangleMeshShape.h"
#include "BulletCollision/CollisionShapes/btEmptyShape.h"
#include "BulletCollision/CollisionShapes/btTriangleMesh.h"
#include "BulletCollision/Gimpact/btGImpactCollisionAlgorithm.h"

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
    {
        //btCollisionDispatcher * dispatcher = static_cast<btCollisionDispatcher *>(mCollisionWorld.getDispatcher());
        //btGImpactCollisionAlgorithm::registerAlgorithm(dispatcher);
    }

    ~GameWorld()
    {}

    btDefaultCollisionConfiguration mCollisionConfiguration;
    btCollisionDispatcher mDispatcher;
    btAxisSweep3 mBroadphase;
    //bt32BitAxisSweep3 mBroadphase;
    btCollisionWorld mCollisionWorld;

};

Physics::Physics(StaticMeshProcesser * meshProesser)
    : mMeshProesser(meshProesser),
    mGameWorld(new GameWorld())
{}

Physics::~Physics()
{}

void Physics::timeStep()
{
    mGameWorld->mCollisionWorld.performDiscreteCollisionDetection();

    for(vehicles::iterator i = mVehicles.begin(), j = mVehicles.end(); i != j; ++i)
    {
        (*i).second->timeStep();
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

    //btTriangleInfoMap* triangleInfoMap = new btTriangleInfoMap();
    //btGenerateInternalEdgeInfo(pGroundShape, triangleInfoMap);

    pGroundShape->setMargin(0.2f);

    CommonIncludes::shared_ptr<btCollisionObject> collisionObject = CommonIncludes::shared_ptr<btCollisionObject>(new btCollisionObject());
    collisionObject->setCollisionShape(pGroundShape);
    collisionObject->setRestitution(bodyRestitution);
    collisionObject->setFriction(bodyFriction);
    collisionObject->setRollingFriction(bodyRollingFriction);
    collisionObject->getWorldTransform().setOrigin(PhysicsTools::convert(part.offset));
    collisionObject->getWorldTransform().setRotation(PhysicsTools::convert(Ogre::Quaternion::IDENTITY));
    //collisionObject->setCollisionFlags(collisionObject->getCollisionFlags() | btCollisionObject::CF_CUSTOM_MATERIAL_CALLBACK);
    //collisionObject->setActivationState(DISABLE_DEACTIVATION);

    mStaticCollisionObjects.push_back(collisionObject);

    mGameWorld->mCollisionWorld.addCollisionObject(
        collisionObject.get(), 
        btBroadphaseProxy::StaticFilter, 
        btBroadphaseProxy::AllFilter ^ btBroadphaseProxy::StaticFilter
    );

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

    for (unsigned int n = 0; n < numFaces; ++n)
    {
        triMesh->addTriangle(
            PhysicsTools::convert(part.mVertexBuffer[batch.mIndexBuffer[n * 3 + 0]]), 
            PhysicsTools::convert(part.mVertexBuffer[batch.mIndexBuffer[n * 3 + 1]]), 
            PhysicsTools::convert(part.mVertexBuffer[batch.mIndexBuffer[n * 3 + 2]])
            );
    }

    CommonIncludes::shared_ptr<btCollisionShape> shape = CommonIncludes::shared_ptr<btCollisionShape>(new btBvhTriangleMeshShape(triMesh.get(), true));

    mStaticTriMeshes.push_back(triMesh);
    mStaticCollisionShapes.push_back(shape);
}

void Physics::addVehicle(const InitialVehicleSetup& initialVehicleSetup, const PSBaseVehicle * vehiclePtr,
                        Ogre::SceneNode *wheelNodes[InitialVehicleSetup::mWheelsAmount], Ogre::SceneNode *chassis)
{
    vehicles::const_iterator found = mVehicles.find(vehiclePtr);

    if(found == mVehicles.end())
    {
        CommonIncludes::shared_ptr<PhysicsVehicle> vehicle = CommonIncludes::shared_ptr<PhysicsVehicle>(new PhysicsVehicle(this, mMeshProesser, initialVehicleSetup, wheelNodes, chassis));
        mVehicles.insert(std::make_pair(vehiclePtr, vehicle));
    }
}

void Physics::removeVehicle(const PSBaseVehicle * vehiclePtr)
{
    vehicles::const_iterator found = mVehicles.find(vehiclePtr);
    mVehicles.erase(found);
}

void Physics::addCollisionObject(btCollisionObject* object)
{
    mGameWorld->mCollisionWorld.addCollisionObject(
        object,
        btBroadphaseProxy::KinematicFilter,
        btBroadphaseProxy::AllFilter ^ btBroadphaseProxy::KinematicFilter
        );

    //object->setActivationState(DISABLE_DEACTIVATION);

}

void Physics::removeCollisionObject(btCollisionObject* object)
{
    mGameWorld->mCollisionWorld.removeCollisionObject(object);
}

bool Physics::findCollision(const btCollisionObject* const object, const btCollisionObject*& staticObj, 
                            Ogre::Vector3& pointOnStatic,
                            int& triIndex,
                            Ogre::Vector3& worldNormal, Ogre::Real& distance)
{
    bool ret = false;

    const int numManifolds = mGameWorld->mCollisionWorld.getDispatcher()->getNumManifolds();
    for (int i = 0; i < numManifolds; ++i)
    {
        btPersistentManifold * contact_manifold = mGameWorld->mCollisionWorld.getDispatcher()->getManifoldByIndexInternal(i);

        const btCollisionObject * obj_0 = contact_manifold->getBody0();
        const btCollisionObject * obj_1 = contact_manifold->getBody1();

        if(object == obj_0 || object == obj_1)
        {
            bool swap = false;
            if(object == obj_0)
            {
                swap = true;
                staticObj = obj_1;
            }
            else
            {
                staticObj = obj_0;
            }

            int numContacts = contact_manifold->getNumContacts();

            bool contactsFound = false;

            for (int j = 0; j < numContacts; ++j)
            {
                btManifoldPoint& manifold_point = contact_manifold->getContactPoint(j);
                distance = manifold_point.getDistance();

                if(distance < 0.0f)
                {
                    contactsFound = true;
                    worldNormal = PhysicsTools::convert(manifold_point.m_normalWorldOnB);
                    if(swap)
                    {
                        triIndex = manifold_point.m_index1;
                        pointOnStatic = PhysicsTools::convert(manifold_point.getPositionWorldOnB());
                    }
                    else
                    {
                        triIndex = manifold_point.m_index0;
                        pointOnStatic = PhysicsTools::convert(manifold_point.getPositionWorldOnA());
                    }
                    break;
                }
            }

            if(contactsFound)
            {
                ret = true;
                break;
            }
        }
    }

    return ret;
}