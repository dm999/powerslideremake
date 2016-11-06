/***************************************************************************

This source file is part of OGREBULLET
(Object-oriented Graphics Rendering Engine Bullet Wrapper)

Copyright (c) 2007 tuan.kuranes@gmail.com (Use it Freely, even Statically, but have to contribute any changes)



Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
-----------------------------------------------------------------------------
*/

#include "OgreBulletDynamics.h"

#include "OgreBulletCollisionsShape.h"

#include "OgreBulletDynamicsWorld.h"
#include "OgreBulletDynamicsObjectState.h"
#include "OgreBulletDynamicsRigidBody.h"
#include "OgreBulletDynamicsConstraint.h"

#include "OgreBulletDynamicsRaycastVehicle.h"

#include "BulletCollision/Gimpact/btGImpactShape.h"
#include "BulletCollision/Gimpact/btGImpactCollisionAlgorithm.h"

#include "BulletDynamics/ConstraintSolver/btNNCGConstraintSolver.h"
#include "BulletDynamics/MLCPSolvers/btMLCPSolver.h"
#include "BulletDynamics/MLCPSolvers/btSolveProjectedGaussSeidel.h"

using namespace Ogre;
using namespace OgreBulletCollisions;

namespace OgreBulletDynamics
{

    DynamicsWorld::DynamicsWorld(Ogre::SceneManager *mgr,
                                 const Ogre::AxisAlignedBox &bounds,
                                 const Ogre::Vector3 &gravity,
                                 bool init,
                                 bool set32BitAxisSweep,
                                 unsigned int maxHandles)
        : CollisionsWorld(mgr, bounds, false, set32BitAxisSweep, maxHandles)
    {
        //btSequentialImpulseConstraintSolver
        //btSequentialImpulseConstraintSolver3

        //d.polubotko: adjust solver if necessary
        mConstraintsolver = new btSequentialImpulseConstraintSolver();
        //mConstraintsolver = new btNNCGConstraintSolver();

        //mSolverInterface = new btSolveProjectedGaussSeidel();
        //mConstraintsolver = new btMLCPSolver(mSolverInterface);

        //only if init is true, otherwise you have to create mWorld manually later on
        if (init)
        {
            mWorld = new btDiscreteDynamicsWorld(mDispatcher, mBroadphase, mConstraintsolver, &mDefaultCollisionConfiguration);
            static_cast<btDiscreteDynamicsWorld *>(mWorld)->setGravity(convert(gravity));
            
            //http://bulletphysics.org/mediawiki-1.5.8/index.php/BtContactSolverInfo#Split_Impulse
            btContactSolverInfo& info = static_cast<btDiscreteDynamicsWorld *>(mWorld)->getSolverInfo();
            info.m_numIterations = 2;
            info.m_splitImpulse = 1; //enable split impulse feature
            //optionally set the m_splitImpulsePenetrationThreshold (only used when m_splitImpulse  is enabled)
            //only enable split impulse position correction when the penetration is deeper than this m_splitImpulsePenetrationThreshold, otherwise use the regular velocity/position constraint coupling (Baumgarte).
            info.m_splitImpulsePenetrationThreshold = -0.002f;

            info.m_solverMode |= SOLVER_USE_2_FRICTION_DIRECTIONS;

			//btCollisionDispatcher * dispatcher = static_cast<btCollisionDispatcher *>(mWorld->getDispatcher());
			//btGImpactCollisionAlgorithm::registerAlgorithm(dispatcher);
		}

    }
    // -------------------------------------------------------------------------
    DynamicsWorld::~DynamicsWorld()
    {
        delete mConstraintsolver;
        mConstraintsolver = NULL;

        //delete mSolverInterface;
        //mSolverInterface = NULL;
    }

    // -------------------------------------------------------------------------
    void DynamicsWorld::addRigidBody(RigidBody *rb, short collisionGroup, short collisionMask)
    {
        mObjects.push_back(static_cast <Object *>(rb));

		if (collisionGroup == 0 && collisionMask == 0)
		{
			// use default collision group/mask values (dynamic/kinematic/static)
            static_cast<btDiscreteDynamicsWorld *>(mWorld)->addRigidBody(rb->getBulletRigidBody());
		}
		else
		{
            static_cast<btDiscreteDynamicsWorld *>(mWorld)->addRigidBody(rb->getBulletRigidBody(), collisionGroup, collisionMask);
		}
    }

    void DynamicsWorld::removeRigidBody(RigidBody *rb)
    {
        getBulletDynamicsWorld()->removeRigidBody(rb->getBulletRigidBody());
    }
    // -------------------------------------------------------------------------
    void DynamicsWorld::stepSimulation(const Ogre::Real elapsedTime, int maxSubSteps, const Ogre::Real fixedTimestep)
    {
        // Reset Debug Lines
        if (mDebugDrawer)
        {
            mDebugDrawer->clear();
        }

        if(mDebugContactPoints)
        {
            mDebugContactPoints->clear();
        }

        static_cast<btDiscreteDynamicsWorld *> (mWorld)->stepSimulation(elapsedTime, maxSubSteps, fixedTimestep);

		if (mDebugContactPoints) 
		{
			///one way to draw all the contact points is iterating over contact manifolds / points:
            const unsigned int numManifolds = mWorld->getDispatcher()->getNumManifolds();
            for (unsigned int i = 0; i < numManifolds; ++i)
			{
				btPersistentManifold* contactManifold = mWorld->getDispatcher()->getManifoldByIndexInternal(i);

                const btCollisionObject* obA = contactManifold->getBody0();
                const btCollisionObject* obB = contactManifold->getBody1();

                contactManifold->refreshContactPoints(obA->getWorldTransform(), obB->getWorldTransform());

				const unsigned int numContacts = contactManifold->getNumContacts();
                for (unsigned int j = 0; j < numContacts; j++)
				{
					btManifoldPoint& pt = contactManifold->getContactPoint(j);

					if (mShowDebugContactPoints)
					{
						btVector3 ptA = pt.getPositionWorldOnA();
						btVector3 ptB = pt.getPositionWorldOnB();
                        btVector3 ptDistB = ptB  + pt.m_normalWorldOnB * 100;

                        mDebugContactPoints->addLine(ptA.x(), ptA.y(), ptA.z(),
                                                     ptB.x(), ptB.y(), ptB.z());

                        mDebugContactPoints->addLine(ptB.x(), ptB.y(), ptB.z(),
                                                     ptDistB.x(), ptDistB.y(), ptDistB.z());
					}
				}
				//you can un-comment out this line, and then all points are removed
				//contactManifold->clearManifold();	
			}
			// draw lines that step Simulation sent.
			mDebugContactPoints->draw();
		}

		if (mDebugDrawer) 
		{
			// draw lines that step Simulation sent.
			mDebugDrawer->draw();

            /** TODO: */
            /*
            const bool drawFeaturesText = ((mDebugDrawer->getDebugMode () & btIDebugDraw::DBG_DrawFeaturesText) != 0);
			if (drawFeaturesText)
			{
				// on all bodies we have
				// we get all shapes and draw more information
				//depending on mDebugDrawer mode.
				std::deque<Object*>::iterator it = mObjects.begin();
				while (it != mObjects.end())
				{
					//(*it)->drawFeaturesText();
					++it;
				}
			}
            */
		}
    }
    // -------------------------------------------------------------------------
    void DynamicsWorld::removeConstraint(TypedConstraint *constraint)
    {
        getBulletDynamicsWorld()->removeConstraint(constraint->getBulletTypedConstraint());
        std::deque <TypedConstraint*>::iterator it = mConstraints.begin();
        while (it != mConstraints.end())
        {
            if ((*it) == constraint)
            {
                mConstraints.erase(it);
                break;
            }
            ++it;
        }
    }
    // -------------------------------------------------------------------------
    void DynamicsWorld::addConstraint(TypedConstraint *constraint, bool disableCollisionsBetweenLinkedBodies)
    {
        getBulletDynamicsWorld()->addConstraint(constraint->getBulletTypedConstraint(), disableCollisionsBetweenLinkedBodies);
        mConstraints.push_back(constraint);
    }
    // -------------------------------------------------------------------------
    void DynamicsWorld::addVehicle(RaycastVehicle *v)
    {
        getBulletDynamicsWorld()->addVehicle(v->getBulletVehicle());
        mActionInterface.push_back(static_cast<ActionInterface *>(v));

        //mVehicles.push_back(v);
    }
    // -------------------------------------------------------------------------
    bool DynamicsWorld::isConstraintRegistered(TypedConstraint *constraint) const
    {
        std::deque <TypedConstraint*>::const_iterator it = mConstraints.begin();
        while (it != mConstraints.end())
        {
            if ((*it) == constraint)
            {
                return true;
            }
            ++it;
        }
        return false;
    }
}

