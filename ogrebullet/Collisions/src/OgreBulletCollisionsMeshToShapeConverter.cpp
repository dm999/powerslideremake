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

#include "OgreBulletCollisions.h"

#include "OgreBulletCollisionsMeshToShapeConverter.h"

#include "ConvexDecomposition/ConvexBuilder.h"

#include "BulletCollision/CollisionShapes/btShapeHull.h"

#include "OgreBulletCollisionsTrimeshShape.h"
#include "OgreBulletCollisionsCylinderShape.h"
#include "OgreBulletCollisionsSphereShape.h"
#include "OgreBulletCollisionsBoxShape.h"
#include "OgreBulletCollisionsConvexHullShape.h"
#include "OgreBulletCollisionsGImpactShape.h"
#include "OgreBulletCollisionsCapsuleShape.h"
#include "OgreBulletCollisionsCompoundShape.h"

using namespace OgreBulletCollisions;

//------------------------------------------------------------------------------------------------
void VertexIndexToShape::addStaticVertexData(const Ogre::VertexData *vertex_data)
{
    if (!vertex_data)
    {
        return;
    }

    const Ogre::VertexData *data = vertex_data;

    const unsigned int prev_size = mVertexCount;
    mVertexCount += (unsigned int)data->vertexCount;

    Ogre::Vector3 *tmp_vert = new Ogre::Vector3[mVertexCount];
    if (mVertexBuffer)
    {
        memcpy(tmp_vert, mVertexBuffer, sizeof(Ogre::Vector3) * prev_size);
        delete[] mVertexBuffer;
    }
    mVertexBuffer = tmp_vert;

    // Get the positional buffer element
    {	
        const Ogre::VertexElement *posElem = data->vertexDeclaration->findElementBySemantic(Ogre::VES_POSITION);
        Ogre::HardwareVertexBufferSharedPtr vbuf = data->vertexBufferBinding->getBuffer(posElem->getSource());
        const unsigned int vSize = (unsigned int)vbuf->getVertexSize();

        unsigned char *vertex = static_cast<unsigned char*>(vbuf->lock(Ogre::HardwareBuffer::HBL_READ_ONLY));
        float *pReal = NULL;
        Ogre::Vector3 * curVertices = &mVertexBuffer[prev_size];
        const unsigned int vertexCount = (unsigned int)data->vertexCount;
        for (unsigned int j = 0; j < vertexCount; ++j)
        {
            posElem->baseVertexPointerToElement(vertex, &pReal);
            vertex += vSize;

            curVertices->x = (*pReal++);
            curVertices->y = (*pReal++);
            curVertices->z = (*pReal++);

            *curVertices = mTransform * (*curVertices);
            
            curVertices++;
        }
        vbuf->unlock();
    }
}

//------------------------------------------------------------------------------------------------
void VertexIndexToShape::addAnimatedVertexData(const Ogre::VertexData *vertex_data,
                                               const Ogre::VertexData *blend_data,
                                               const Ogre::Mesh::IndexMap *indexMap)
{
    // Get the bone index element
    assert(vertex_data);

    const Ogre::VertexData *data = blend_data;
    const unsigned int prev_size = mVertexCount;
    mVertexCount += (unsigned int)data->vertexCount;
    Ogre::Vector3 *tmp_vert = new Ogre::Vector3[mVertexCount];
    if (mVertexBuffer)
    {
        memcpy(tmp_vert, mVertexBuffer, sizeof(Ogre::Vector3) * prev_size);
        delete[] mVertexBuffer;
    }
    mVertexBuffer = tmp_vert;

    // Get the positional buffer element
    {	
        const Ogre::VertexElement *posElem = data->vertexDeclaration->findElementBySemantic(Ogre::VES_POSITION);
        assert(posElem);
        Ogre::HardwareVertexBufferSharedPtr vbuf = data->vertexBufferBinding->getBuffer(posElem->getSource());
        const unsigned int vSize = (unsigned int)vbuf->getVertexSize();

        unsigned char *vertex = static_cast<unsigned char*>(vbuf->lock(Ogre::HardwareBuffer::HBL_READ_ONLY));
        float *pReal;
        Ogre::Vector3 * curVertices = &mVertexBuffer[prev_size];
        const unsigned int vertexCount = (unsigned int)data->vertexCount;
        for (unsigned int j = 0; j < vertexCount; ++j)
        {
            posElem->baseVertexPointerToElement(vertex, &pReal);
            vertex += vSize;

            curVertices->x = (*pReal++);
            curVertices->y = (*pReal++);
            curVertices->z = (*pReal++);

            *curVertices = mTransform * (*curVertices);

            curVertices++;
        }
        vbuf->unlock();
    }

    {
        const Ogre::VertexElement *bneElem = vertex_data->vertexDeclaration->findElementBySemantic(Ogre::VES_BLEND_INDICES);
        assert(bneElem);
		
        Ogre::HardwareVertexBufferSharedPtr vbuf = vertex_data->vertexBufferBinding->getBuffer(bneElem->getSource());
        const unsigned int vSize = (unsigned int)vbuf->getVertexSize();
        unsigned char *vertex = static_cast<unsigned char*>(vbuf->lock(Ogre::HardwareBuffer::HBL_READ_ONLY));

        unsigned char *pBone = NULL;

        if (!mBoneIndex)
        {
            mBoneIndex = new BoneIndex();	
        }

        Ogre::Vector3 *curVertices = &mVertexBuffer[prev_size];

        const unsigned int vertexCount = (unsigned int)vertex_data->vertexCount;
        for (unsigned int j = 0; j < vertexCount; ++j)
        {
            bneElem->baseVertexPointerToElement(vertex, &pBone);
            vertex += vSize;

            const unsigned char currBone = (indexMap) ? (*indexMap)[*pBone] : *pBone;
            const BoneIndex::iterator i = mBoneIndex->find(currBone);
            Vector3Array *l = NULL;
            if (i == mBoneIndex->end())
            {
                l = new Vector3Array;
                mBoneIndex->insert(BoneKeyIndex(currBone, l));
            }
            else 
            {
                  l = i->second;
            }

            l->push_back(*curVertices);

            curVertices++;
        }

        vbuf->unlock();
    }
}

//------------------------------------------------------------------------------------------------
void VertexIndexToShape::addIndexData(Ogre::IndexData *data, const unsigned int offset)
{
    const unsigned int prev_size = mIndexCount;
    mIndexCount += (unsigned int)data->indexCount;

    unsigned int *tmp_ind = new unsigned int[mIndexCount];
    if (mIndexBuffer)
    {
        memcpy (tmp_ind, mIndexBuffer, sizeof(unsigned int) * prev_size);
        delete[] mIndexBuffer;
    }
    mIndexBuffer = tmp_ind;

    const unsigned int numTris = (unsigned int) data->indexCount / 3;
    Ogre::HardwareIndexBufferSharedPtr ibuf = data->indexBuffer;
    const bool use32bitindexes = (ibuf->getType() == Ogre::HardwareIndexBuffer::IT_32BIT);
    unsigned int index_offset = prev_size;

    if (use32bitindexes) 
    {
        const unsigned int *pInt = static_cast<unsigned int*>(ibuf->lock(Ogre::HardwareBuffer::HBL_READ_ONLY));
        for (unsigned int k = 0; k < numTris; ++k)
        {
            mIndexBuffer[index_offset ++] = offset + *pInt++;
            mIndexBuffer[index_offset ++] = offset + *pInt++;
            mIndexBuffer[index_offset ++] = offset + *pInt++;
        }
        ibuf->unlock();
    }
    else 
    {
        const unsigned short *pShort = static_cast<unsigned short*>(ibuf->lock(Ogre::HardwareBuffer::HBL_READ_ONLY));
        for(unsigned int k = 0; k < numTris; ++k)
        {
            mIndexBuffer[index_offset ++] = offset + static_cast<unsigned int>(*pShort++);
            mIndexBuffer[index_offset ++] = offset + static_cast<unsigned int>(*pShort++);
            mIndexBuffer[index_offset ++] = offset + static_cast<unsigned int>(*pShort++);
        }
        ibuf->unlock();
    }
}
//------------------------------------------------------------------------------------------------
Ogre::Real VertexIndexToShape::getRadius()
{
    if (mBoundRadius == Ogre::Real(-1))
    {
        getSize();
        mBoundRadius = (std::max(mBounds.x, std::max(mBounds.y, mBounds.z)) * 0.5f);
    }
    return mBoundRadius;
}
//------------------------------------------------------------------------------------------------
Ogre::Vector3 VertexIndexToShape::getSize()
{
    const unsigned int vCount = getVertexCount();
    if (mBounds == Ogre::Vector3(-1, -1, -1) && vCount > 0)
    {
        const Ogre::Vector3 * const v = getVertices();

        Ogre::Vector3 vmin(v[0]);
        Ogre::Vector3 vmax(v[0]);

        for (unsigned int j = 1; j < vCount; ++j)
        {
            vmin.x = std::min(vmin.x, v[j].x);
            vmin.y = std::min(vmin.y, v[j].y);
            vmin.z = std::min(vmin.z, v[j].z);

            vmax.x = std::max(vmax.x, v[j].x);
            vmax.y = std::max(vmax.y, v[j].y);
            vmax.z = std::max(vmax.z, v[j].z);
        }

	mBounds.x = vmax.x - vmin.x;
	mBounds.y = vmax.y - vmin.y;
	mBounds.z = vmax.z - vmin.z;
    }
    return mBounds;
}
//------------------------------------------------------------------------------------------------
SphereCollisionShape *VertexIndexToShape::createSphere()
{
    const Ogre::Real rad = getRadius();
    assert(rad > Ogre::Real(0.f) && "Sphere radius must be greater than zero");
    return new SphereCollisionShape(rad);
}
//------------------------------------------------------------------------------------------------
BoxCollisionShape *VertexIndexToShape::createBox()
{
    const Ogre::Vector3 sz = getSize();

    assert((sz.x > Ogre::Real(0.f)) && (sz.y > Ogre::Real(0.f)) && (sz.y > Ogre::Real(0.f)) &&
           "Size of box must be greater than zero on all axes");

    return new BoxCollisionShape(sz);
}
//------------------------------------------------------------------------------------------------
CylinderCollisionShape *VertexIndexToShape::createCylinder()
{
    const Ogre::Vector3 sz = getSize();

    assert((sz.x > Ogre::Real(0.f)) && (sz.y > Ogre::Real(0.f)) && (sz.y > Ogre::Real(0.f)) &&
           "Size of Cylinder must be greater than zero on all axes");

    return new CylinderCollisionShape(sz);
}
//------------------------------------------------------------------------------------------------
ConvexHullCollisionShape *VertexIndexToShape::createConvex()
{
    assert(mVertexCount && (mIndexCount >= 6) &&
           "Mesh must have some vertices and at least 6 indices (2 triangles)");

    //TriangleMeshCollisionShape *trimesh = createTrimesh();

    btConvexShape *tmpConvexShape = new btConvexHullShape((btScalar *)&mVertexBuffer[0].x,
                                                          mVertexCount,
                                                          sizeof(Ogre::Vector3));
    //new btConvexTriangleMeshShape();

/*
    Ogre::LogManager::getSingleton().logMessage(Ogre::String("Old numTriangles = ") + Ogre::StringConverter::toString(mVertexCount / 3));
    Ogre::LogManager::getSingleton().logMessage(Ogre::String("Old numIndices = ")   + Ogre::StringConverter::toString(mIndexCount));
    Ogre::LogManager::getSingleton().logMessage(Ogre::String("Old numVertices = ")  + Ogre::StringConverter::toString(mVertexCount));

    Ogre::LogManager::getSingleton().logMessage(Ogre::String("Reducing vertices by creating a convex hull"));
*/

    //create a hull approximation
    btShapeHull *hull = new btShapeHull(tmpConvexShape);
    btScalar margin = 0.001f;//tmpConvexShape->getMargin();
    hull->buildHull(margin);
    tmpConvexShape->setUserPointer(hull);
/*
    Ogre::LogManager::getSingleton().logMessage(String("New numTriangles = ") + Ogre::StringConverter::toString(hull->numTriangles()));
    Ogre::LogManager::getSingleton().logMessage(String("New numIndices = ")   + Ogre::StringConverter::toString(hull->numIndices()));
    Ogre::LogManager::getSingleton().logMessage(String("New numVertices = ")  + Ogre::StringConverter::toString(hull->numVertices()));
*/
    btConvexHullShape *convexShape = new btConvexHullShape();
    for (unsigned int i = 0; i < (unsigned int)hull->numVertices(); ++i)
    {
        convexShape->addPoint(hull->getVertexPointer()[i]);
    }

    delete tmpConvexShape;
    delete hull;
    return new ConvexHullCollisionShape(convexShape);
}


//------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------
class MyConvexDecomposition : public ConvexDecomposition::ConvexDecompInterface
{
    //ConvexDecompositionDemo*	m_convexDemo;

public:

    btAlignedObjectArray<btConvexHullShape*> m_convexShapes;
    btAlignedObjectArray<btVector3> m_convexCentroids;

    int mBaseCount;
    int mHullCount;

    btVector3 mCentroid;
    btVector3 convexDecompositionObjectOffset;
    //FILE* mOutputFile;

    MyConvexDecomposition(
    //FILE *outputFile
    //,ConvexDecompositionDemo *demo
                         )
        : //m_convexDemo(demo)
          mBaseCount(0),
          mHullCount(0)
		//, mOutputFile(outputFile)
    {
        mCentroid = btVector3(0, 0 ,0);
        convexDecompositionObjectOffset = btVector3(10, 0, 0);
    }

    virtual void ConvexDecompResult(ConvexDecomposition::ConvexResult &result)
    {
        btTriangleMesh *trimesh = new btTriangleMesh();
        //m_convexDemo->m_trimeshes.push_back(trimesh);

        btVector3 localScaling(1.0f, 1.0f, 1.0f);

        //export data to .obj

        Ogre::LogManager::getSingleton().logMessage("ConvexResult. ");
        //if (mOutputFile)
        {
            //fprintf(mOutputFile,"## Hull Piece %d with %d vertices and %d triangles.\r\n", mHullCount, result.mHullVcount, result.mHullTcount );

            //fprintf(mOutputFile,"usemtl Material%i\r\n",mBaseCount);
            //fprintf(mOutputFile,"o Object%i\r\n",mBaseCount);

            //for (unsigned int i=0; i<result.mHullVcount; i++)
            //{
                //const float *p = &result.mHullVertices[i*3];
                //fprintf(mOutputFile,"v %0.9f %0.9f %0.9f\r\n", p[0], p[1], p[2] );
            //}

            //calc centroid, to shift vertices around center of mass
            mCentroid.setValue(0,0,0);

            btAlignedObjectArray<btVector3> vertices;
            {
                //const unsigned int *src = result.mHullIndices;
                for (unsigned int i = 0; i < result.mHullVcount; ++i)
                {
                    btVector3 vertex(result.mHullVertices[i*3], result.mHullVertices[i*3+1], result.mHullVertices[i*3+2]);
                    vertex *= localScaling;
                    mCentroid += vertex;
                }
            }

            mCentroid *= 1.0f / (float(result.mHullVcount));

            {
                //const unsigned int *src = result.mHullIndices;
                for (unsigned int i = 0; i < result.mHullVcount; ++i)
                {
                    btVector3 vertex(result.mHullVertices[i*3], result.mHullVertices[i*3+1], result.mHullVertices[i*3+2]);
                    vertex *= localScaling;
                    vertex -= mCentroid;
                    vertices.push_back(vertex);
                }
            }

            {
                const unsigned int *src = result.mHullIndices;
                for (unsigned int i = 0; i<result.mHullTcount; i++)
                {
                    unsigned int index0 = *src++;
                    unsigned int index1 = *src++;
                    unsigned int index2 = *src++;


                    btVector3 vertex0(result.mHullVertices[index0*3], result.mHullVertices[index0*3+1], result.mHullVertices[index0*3+2]);
                    btVector3 vertex1(result.mHullVertices[index1*3], result.mHullVertices[index1*3+1], result.mHullVertices[index1*3+2]);
                    btVector3 vertex2(result.mHullVertices[index2*3], result.mHullVertices[index2*3+1], result.mHullVertices[index2*3+2]);

                    vertex0 *= localScaling;
                    vertex1 *= localScaling;
                    vertex2 *= localScaling;

                    vertex0 -= mCentroid;
                    vertex1 -= mCentroid;
                    vertex2 -= mCentroid;

                    trimesh->addTriangle(vertex0, vertex1, vertex2);

                    index0 += mBaseCount;
                    index1 += mBaseCount;
                    index2 += mBaseCount;

                    //fprintf(mOutputFile,"f %d %d %d\r\n", index0+1, index1+1, index2+1 );
                }
            }

            float mass = 1.0f;
            //float collisionMargin = 0.01f;

            //this is a tools issue: due to collision margin, convex objects overlap, compensate for it here:
            //#define SHRINK_OBJECT_INWARDS 1
#ifdef SHRINK_OBJECT_INWARDS


            std::vector<btVector3> planeEquations;
            btGeometryUtil::getPlaneEquationsFromVertices(vertices, planeEquations);

            std::vector<btVector3> shiftedPlaneEquations;
            for (int p = 0; p < planeEquations.size(); ++p)
            {
                btVector3 plane = planeEquations[p];
                plane[3] += 5*collisionMargin;
                shiftedPlaneEquations.push_back(plane);
            }
            std::vector<btVector3> shiftedVertices;
            btGeometryUtil::getVerticesFromPlaneEquations(shiftedPlaneEquations, shiftedVertices);


            btConvexHullShape *convexShape = new btConvexHullShape(&(shiftedVertices[0].getX()), shiftedVertices.size());

#else //SHRINK_OBJECT_INWARDS

            btConvexHullShape *convexShape = new btConvexHullShape(&(vertices[0].getX()), vertices.size());
#endif 

            convexShape->setMargin(btScalar(0.f));
            m_convexShapes.push_back(convexShape);
            m_convexCentroids.push_back(mCentroid);

            //m_convexDemo->m_collisionShapes.push_back(convexShape);

            mBaseCount += result.mHullVcount; // advance the 'base index' counter.
        }
    }
};


//------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------
CompoundCollisionShape *VertexIndexToShape::createConvexDecomposition(unsigned int depth,
                                                                      float cpercent,
                                                                      float ppercent,
                                                                      unsigned int maxv,
                                                                      float skinWidth)
{
    assert(mVertexCount && (mIndexCount >= 6) &&
           "Mesh must have some vertices and at least 6 indices (2 triangles)");

/*
	{
		//TriangleMeshCollisionShape *trimesh = createTrimesh();

        btConvexShape *tmpConvexShape = new btConvexHullShape((btScalar*) &mVertexBuffer[0].x, mVertexCount, sizeof(Vector3));
			//new btConvexTriangleMeshShape();


		Ogre::LogManager::getSingleton().logMessage(String("old numTriangles= %d\n") + Ogre::StringConverter::toString(mVertexCount / 3));
		Ogre::LogManager::getSingleton().logMessage(String("old numIndices = %d\n") + Ogre::StringConverter::toString(mIndexCount));
		Ogre::LogManager::getSingleton().logMessage(String("old numVertices = %d\n") + Ogre::StringConverter::toString(mVertexCount));

		Ogre::LogManager::getSingleton().logMessage(String("reducing vertices by creating a convex hull\n"));

		//create a hull approximation
        btShapeHull *hull = new btShapeHull(tmpConvexShape);
		btScalar margin = 0.0;//tmpConvexShape->getMargin();
		hull->buildHull(margin);
		tmpConvexShape->setUserPointer(hull);


		Ogre::LogManager::getSingleton().logMessage(String("new numTriangles = %d\n") + Ogre::StringConverter::toString(hull->numTriangles ()));
		Ogre::LogManager::getSingleton().logMessage(String("new numIndices = %d\n") + Ogre::StringConverter::toString(hull->numIndices ()));
		Ogre::LogManager::getSingleton().logMessage(String("new numVertices = %d\n") + Ogre::StringConverter::toString(hull->numVertices ()));

        btConvexHullShape *convexShape = new btConvexHullShape();
		for (unsigned int i=0;i<(unsigned int )hull->numVertices();i++)
		{
			convexShape->addPoint(hull->getVertexPointer()[i]);	
		}

		delete tmpConvexShape;
		delete hull;
	}
*/

	// options
    ConvexDecomposition::DecompDesc desc;

    desc.mVcount       = mVertexCount;
    //desc.mVertices     = mVertexBuffer;
    desc.mVertices     = (const float *)mVertexBuffer;
    desc.mTcount       = mIndexCount / 3;
    desc.mIndices      = (unsigned int *)mIndexBuffer;
    desc.mDepth        = depth;
    desc.mCpercent     = cpercent;
    desc.mPpercent     = ppercent;
    desc.mMaxVertices  = maxv;
    desc.mSkinWidth    = skinWidth;

    MyConvexDecomposition convexDecomposition;

    desc.mCallback = &convexDecomposition;

    //convexDecomposition.performConvexDecomposition(desc);

    ConvexBuilder cb(desc.mCallback);
    cb.process(desc);
    //now create some bodies

    btCompoundShape *compound = new btCompoundShape;

    //m_collisionShapes.push_back (compound);

    btTransform trans;
    trans.setIdentity();
    for (int i = 0; i < convexDecomposition.m_convexShapes.size(); ++i)
    {
        btVector3 centroid = convexDecomposition.m_convexCentroids[i];
        trans.setOrigin(centroid);
        btConvexHullShape *convexShape = convexDecomposition.m_convexShapes[i];
        compound->addChildShape(trans, convexShape);
    }
	
    return new CompoundCollisionShape(compound);
}
//------------------------------------------------------------------------------------------------
TriangleMeshCollisionShape *VertexIndexToShape::createTrimesh()
{
    assert(mVertexCount && (mIndexCount >= 6) &&
           "Mesh must have some vertices and at least 6 indices (2 triangles)");

    return new TriangleMeshCollisionShape(mVertexBuffer, mVertexCount,mIndexBuffer, mIndexCount);
}
//------------------------------------------------------------------------------------------------
GImpactConcaveShape *VertexIndexToShape::createConcave()
{
    assert(mVertexCount && (mIndexCount >= 6) &&
           "Mesh must have some vertices and at least 6 indices (2 triangles)");

    return new GImpactConcaveShape(mVertexBuffer, mVertexCount,mIndexBuffer, mIndexCount);
}

//------------------------------------------------------------------------------------------------
VertexIndexToShape::~VertexIndexToShape()
{
    delete[] mVertexBuffer;
    delete[] mIndexBuffer;

    if (mBoneIndex)
    {
        for(BoneIndex::iterator i = mBoneIndex->begin(); i != mBoneIndex->end(); ++i)
        {
            delete i->second;
        }
        delete mBoneIndex;
    }
}
//------------------------------------------------------------------------------------------------
VertexIndexToShape::VertexIndexToShape(const Ogre::Matrix4 &transform)
    : mVertexBuffer(NULL),
      mIndexBuffer(NULL),
      mVertexCount(0),
      mIndexCount(0),
      mBounds(Ogre::Vector3(-1.0f, -1.0f, -1.0f)),
      mBoundRadius(-1.0f),
      mBoneIndex(NULL),
      mTransform(transform)
{	
}
//------------------------------------------------------------------------------------------------
StaticMeshToShapeConverter::StaticMeshToShapeConverter()
    : VertexIndexToShape(),
      mEntity(NULL),
      mNode(NULL)
{
}
//------------------------------------------------------------------------------------------------
StaticMeshToShapeConverter::~StaticMeshToShapeConverter()
{
}
//------------------------------------------------------------------------------------------------
StaticMeshToShapeConverter::StaticMeshToShapeConverter(Ogre::Entity *entity,
                                                       const Ogre::Matrix4 &transform)
    : VertexIndexToShape(transform),
      mEntity(NULL),
      mNode(NULL)
{
    addEntity(entity, transform);	
}
//------------------------------------------------------------------------------------------------
StaticMeshToShapeConverter::StaticMeshToShapeConverter(Ogre::Renderable *rend,
                                                       const Ogre::Matrix4 &transform)
    : VertexIndexToShape(transform),
      mEntity(NULL),
      mNode(NULL)
{
    Ogre::RenderOperation op;
    rend->getRenderOperation(op);
    VertexIndexToShape::addStaticVertexData(op.vertexData);
    if (op.useIndexes)
    {
        VertexIndexToShape::addIndexData(op.indexData);
    }

}
//------------------------------------------------------------------------------------------------
void StaticMeshToShapeConverter::addEntity(const Ogre::Entity *entity,
                                           const Ogre::Matrix4 &transform)
{
    // Each entity added need to reset size and radius
    // next time getRadius and getSize are asked, they're computed.
    mBounds  = Ogre::Vector3(-1.0f, -1.0f, -1.0f);
    mBoundRadius = -1.0f;

    mEntity = entity;
    if (!mEntity->isParentTagPoint())
    {
        mNode = mEntity->getParentSceneNode();
    }
    mTransform = transform;

    if (mEntity->getMesh()->sharedVertexData)
    {
        VertexIndexToShape::addStaticVertexData (mEntity->getMesh()->sharedVertexData);
    }

    for (unsigned int i = 0; i < mEntity->getNumSubEntities(); ++i)
    {
        Ogre::SubMesh *sub_mesh = mEntity->getSubEntity(i)->getSubMesh();

        if (!sub_mesh->useSharedVertices)
        {
            VertexIndexToShape::addIndexData(sub_mesh->indexData, mVertexCount);
            VertexIndexToShape::addStaticVertexData(sub_mesh->vertexData);
        }
        else 
        {
            VertexIndexToShape::addIndexData(sub_mesh->indexData);
        }
    }
}
//------------------------------------------------------------------------------------------------
void StaticMeshToShapeConverter::addMesh(const Ogre::MeshPtr &mesh,
                                         const Ogre::Matrix4 &transform)
{
    // Each entity added need to reset size and radius
    // next time getRadius and getSize are asked, they're computed.
    mBounds = Ogre::Vector3(-1.0f, -1.0f, -1.0f);
    mBoundRadius = -1;

    //_entity = entity;
    //_node = (SceneNode*)(_entity->getParentNode());
    mTransform = transform;

    if (mesh->hasSkeleton())
    {
        Ogre::LogManager::getSingleton().logMessage("MeshToShapeConverter::addMesh : Mesh " + mesh->getName() + " as skeleton but added to trimesh non animated");
    }

    if (mesh->sharedVertexData)
    {
        VertexIndexToShape::addStaticVertexData (mesh->sharedVertexData);
    }

    for (unsigned int i = 0; i < mesh->getNumSubMeshes(); ++i)
    {
        Ogre::SubMesh *sub_mesh = mesh->getSubMesh(i);

        if (!sub_mesh->useSharedVertices)
        {
            VertexIndexToShape::addIndexData(sub_mesh->indexData, mVertexCount);
            VertexIndexToShape::addStaticVertexData(sub_mesh->vertexData);
        }
        else 
        {
            VertexIndexToShape::addIndexData(sub_mesh->indexData);
        }
    }
}
//------------------------------------------------------------------------------------------------
AnimatedMeshToShapeConverter::AnimatedMeshToShapeConverter(Ogre::Entity *entity,
                                                           const Ogre::Matrix4 &transform)
    : VertexIndexToShape(transform),
      mEntity(NULL),
      mNode(NULL),
      mTransformedVerticesTemp(NULL),
      mTransformedVerticesTempSize(0)
{
    addEntity(entity, transform);	
}
//------------------------------------------------------------------------------------------------
AnimatedMeshToShapeConverter::AnimatedMeshToShapeConverter()
    : VertexIndexToShape(),
      mEntity(NULL),
      mNode(NULL),
      mTransformedVerticesTemp(NULL),
      mTransformedVerticesTempSize(0)
{
}
//------------------------------------------------------------------------------------------------
AnimatedMeshToShapeConverter::~AnimatedMeshToShapeConverter()
{
    delete[] mTransformedVerticesTemp;
}
//------------------------------------------------------------------------------------------------
void AnimatedMeshToShapeConverter::addEntity(Ogre::Entity *entity,
                                             const Ogre::Matrix4 &transform)
{
    // Each entity added need to reset size and radius
    // next time getRadius and getSize are asked, they're computed.
    mBounds = Ogre::Vector3(-1.0f, -1.0f, -1.0f);
    mBoundRadius = -1.0f;

    mEntity = entity;
    if (!mEntity->isParentTagPoint())
    {
        mNode = mEntity->getParentSceneNode();
    }
    mTransform = transform;

    assert(entity->getMesh()->hasSkeleton());

    mEntity->addSoftwareAnimationRequest(false);
    mEntity->_updateAnimation();

    if (mEntity->getMesh()->sharedVertexData)
    {
        VertexIndexToShape::addAnimatedVertexData(mEntity->getMesh()->sharedVertexData,
                                                  mEntity->_getSkelAnimVertexData(),
                                                  &mEntity->getMesh()->sharedBlendIndexToBoneIndexMap);
    }

    for (unsigned int i = 0; i < mEntity->getNumSubEntities(); ++i)
    {
        Ogre::SubMesh *sub_mesh = mEntity->getSubEntity(i)->getSubMesh();

        if (!sub_mesh->useSharedVertices)
        {
            VertexIndexToShape::addIndexData(sub_mesh->indexData, mVertexCount);

            VertexIndexToShape::addAnimatedVertexData(sub_mesh->vertexData,
                                                      mEntity->getSubEntity(i)->_getSkelAnimVertexData(),
                                                      &sub_mesh->blendIndexToBoneIndexMap);
        }
        else 
        {
            VertexIndexToShape::addIndexData(sub_mesh->indexData);
        }
    }

    mEntity->removeSoftwareAnimationRequest(false);
}
//------------------------------------------------------------------------------------------------
void AnimatedMeshToShapeConverter::addMesh(const Ogre::MeshPtr &mesh,
                                           const Ogre::Matrix4 &transform)
{
    // Each entity added need to reset size and radius
    // next time getRadius and getSize are asked, they're computed.
    mBounds = Ogre::Vector3(-1.0f , -1.0f, -1.0f);
    mBoundRadius = -1.0f;

    //_entity = entity;
    //_node = (SceneNode*)(_entity->getParentNode());
    mTransform = transform;

    assert(mesh->hasSkeleton());

    if (mesh->sharedVertexData)
    {
        VertexIndexToShape::addAnimatedVertexData(mesh->sharedVertexData,
                                                  NULL,
                                                  &mesh->sharedBlendIndexToBoneIndexMap);
    }

    for (unsigned int i = 0; i < mesh->getNumSubMeshes(); ++i)
    {
        Ogre::SubMesh *sub_mesh = mesh->getSubMesh(i);

        if (!sub_mesh->useSharedVertices)
        {
            VertexIndexToShape::addIndexData(sub_mesh->indexData, mVertexCount);

            VertexIndexToShape::addAnimatedVertexData(sub_mesh->vertexData,
                                                      NULL,
                                                      &sub_mesh->blendIndexToBoneIndexMap);
        }
        else 
        {
            VertexIndexToShape::addIndexData(sub_mesh->indexData);
        }
    }
}
//------------------------------------------------------------------------------------------------
bool AnimatedMeshToShapeConverter::getBoneVertices(unsigned char bone, 
                                                   unsigned int &vertex_count,
                                                   Ogre::Vector3 *&vertices,
                                                   const Ogre::Vector3 &bonePosition)
{
    BoneIndex::iterator i = mBoneIndex->find(bone);

    if (i == mBoneIndex->end())
    {
        return false;
    }

    Vector3Array *array = i->second;
    const Vector3Array::size_type size = array->size();
    if (size == 0)
    {
        return false;
    }

    vertex_count = size + 1;
    if (vertex_count > mTransformedVerticesTempSize)
    {
        if (mTransformedVerticesTemp)
        {
            delete[] mTransformedVerticesTemp;
        }

        mTransformedVerticesTemp = new Ogre::Vector3[vertex_count];

    }

    vertices = mTransformedVerticesTemp;
    vertices[0] = bonePosition;
    //mEntity->_getParentNodeFullTransform() * 
    //    mEntity->getSkeleton()->getBone(bone)->_getDerivedPosition();

    //mEntity->getSkeleton()->getBone(bone)->_getDerivedOrientation()
    unsigned int currBoneVertex = 1;
    Vector3Array::iterator j = array->begin();
    while (j != array->end())
    {
        vertices[currBoneVertex] = (*j);
        ++j;
        ++currBoneVertex; 
    }       
    return true;
}
//------------------------------------------------------------------------------------------------
BoxCollisionShape *AnimatedMeshToShapeConverter::createAlignedBox(unsigned char bone,
                                                                  const Ogre::Vector3 &bonePosition,
                                                                  const Ogre::Quaternion &boneOrientation)
{
    unsigned int vertex_count;
    Ogre::Vector3 *vertices;

    if (!getBoneVertices(bone, vertex_count, vertices, bonePosition))
    {
        return NULL;
    }

    Ogre::Vector3 min_vec(vertices[0]);
    Ogre::Vector3 max_vec(vertices[0]);

    for (unsigned int j = 1; j < vertex_count; ++j)
    {
        min_vec.x = std::min(min_vec.x, vertices[j].x);
        min_vec.y = std::min(min_vec.y, vertices[j].y);
        min_vec.z = std::min(min_vec.z, vertices[j].z);

        max_vec.x = std::max(max_vec.x, vertices[j].x);
        max_vec.y = std::max(max_vec.y, vertices[j].y);
        max_vec.z = std::max(max_vec.z, vertices[j].z);
    }

    const Ogre::Vector3 maxMinusMin(max_vec - min_vec);
    return new BoxCollisionShape(maxMinusMin);

    /*
    const Ogre::Vector3 pos(min_vec.x + (maxMinusMin.x * 0.5),
                            min_vec.y + (maxMinusMin.y * 0.5),
                            min_vec.z + (maxMinusMin.z * 0.5));

    box->setPosition(pos);
    */
}
//------------------------------------------------------------------------------------------------
bool AnimatedMeshToShapeConverter::getOrientedBox(unsigned char bone,
                                                  const Ogre::Vector3 &bonePosition,
                                                  const Ogre::Quaternion &boneOrientation,
                                                  Ogre::Vector3 &box_afExtent,
                                                  Ogre::Vector3 *box_akAxis,
                                                  Ogre::Vector3 &box_kCenter)
{
    unsigned int vertex_count;
    Ogre::Vector3 *vertices;

    if (!getBoneVertices(bone, vertex_count, vertices, bonePosition))
    {
        return false;
    }

    box_kCenter = Ogre::Vector3::ZERO;

    {
        for (unsigned int c = 0 ; c < vertex_count; ++c)
        {
            box_kCenter += vertices[c];
        }
        const Ogre::Real invVertexCount = 1.0f / vertex_count;
        box_kCenter *= invVertexCount;
    }

    Ogre::Quaternion orient = boneOrientation;
    orient.ToAxes(box_akAxis);

    // Let C be the box center and let U0, U1, and U2 be the box axes.  Each
    // input point is of the form X = C + y0*U0 + y1*U1 + y2*U2.  The
    // following code computes min(y0), max(y0), min(y1), max(y1), min(y2),
    // and max(y2).  The box center is then adjusted to be
    //   C' = C + 0.5*(min(y0)+max(y0))*U0 + 0.5*(min(y1)+max(y1))*U1 +
    //        0.5*(min(y2)+max(y2))*U2

    Ogre::Vector3 kDiff (vertices[1] - box_kCenter);
    Ogre::Real fY0Min = kDiff.dotProduct(box_akAxis[0]), fY0Max = fY0Min;
    Ogre::Real fY1Min = kDiff.dotProduct(box_akAxis[1]), fY1Max = fY1Min;
    Ogre::Real fY2Min = kDiff.dotProduct(box_akAxis[2]), fY2Max = fY2Min;

    for (unsigned int i = 2; i < vertex_count; ++i)
    {
        kDiff = vertices[i] - box_kCenter;

        const Ogre::Real fY0 = kDiff.dotProduct(box_akAxis[0]);
        if (fY0 < fY0Min)
        {
            fY0Min = fY0;
        }
        else
        if (fY0 > fY0Max)
        {
            fY0Max = fY0;
        }

        const Ogre::Real fY1 = kDiff.dotProduct(box_akAxis[1]);
        if (fY1 < fY1Min)
        {
            fY1Min = fY1;
        }
        else
        if (fY1 > fY1Max)
        {
            fY1Max = fY1;
        }

        const Ogre::Real fY2 = kDiff.dotProduct(box_akAxis[2]);
        if (fY2 < fY2Min)
        {
            fY2Min = fY2;
        }
        else
        if (fY2 > fY2Max)
        {
            fY2Max = fY2;
        }
    }

    box_afExtent.x = (0.5f) * (fY0Max - fY0Min);
    box_afExtent.y = (0.5f) * (fY1Max - fY1Min);
    box_afExtent.z = (0.5f) * (fY2Max - fY2Min);

    box_kCenter += (0.5f * (fY0Max+fY0Min)) * box_akAxis[0] +
                   (0.5f * (fY1Max+fY1Min)) * box_akAxis[1] +
                   (0.5f * (fY2Max+fY2Min)) * box_akAxis[2];

    box_afExtent *= 2.0f;

    return true;
}
//------------------------------------------------------------------------------------------------
BoxCollisionShape *AnimatedMeshToShapeConverter::createOrientedBox(unsigned char bone,
                                                                   const Ogre::Vector3 &bonePosition,
                                                                   const Ogre::Quaternion &boneOrientation)
{
    Ogre::Vector3 box_akAxis[3];
    Ogre::Vector3 box_afExtent;
    Ogre::Vector3 box_afCenter;

    if (!getOrientedBox(bone, bonePosition, boneOrientation, box_afExtent, box_akAxis, box_afCenter))
    {
        return NULL;
    }

    return new BoxCollisionShape(box_afExtent);
    //geom->setOrientation(Quaternion(box_akAxis[0],box_akAxis[1],box_akAxis[2]));
    //geom->setPosition(box_afCenter);
}
//------------------------------------------------------------------------------------------------
CapsuleCollisionShape *AnimatedMeshToShapeConverter::createOrientedCapsuleCollisionShape(
        unsigned char bone,
        const Ogre::Vector3 &bonePosition,
        const Ogre::Quaternion &boneOrientation)
{
    Ogre::Vector3 box_akAxis[3];
    Ogre::Vector3 box_afExtent;
    Ogre::Vector3 box_afCenter;

    if (!getOrientedBox(bone, bonePosition, boneOrientation, box_afExtent, box_akAxis, box_afCenter))
    {
        return NULL;
    }

    // find axes with longest length

    Ogre::Vector3 cap_dir;    //std::max(box_afExtent.x,std::max(box_afExtent.y,box_afExtent.z));
    Ogre::Real cap_dirLength;  //std::max(box_afExtent.x,std::max(box_afExtent.y,box_afExtent.z));
    Ogre::Real cap_radius;	 // max axe length
    if (box_afExtent.x - box_afExtent.y > 0 && box_afExtent.x - box_afExtent.z > 0)
    {
        cap_dir = Ogre::Vector3::UNIT_X;
                  cap_dirLength = box_afExtent.x;
        cap_radius = std::max(box_afExtent.y, box_afExtent.z);
    }
    else
    if (box_afExtent.y - box_afExtent.x > 0 && box_afExtent.y - box_afExtent.z > 0)
    {
        cap_dir = Ogre::Vector3::UNIT_Y;
                  cap_dirLength = box_afExtent.y;
        cap_radius = std::max(box_afExtent.x, box_afExtent.z);
    }
    else
    if (box_afExtent.z - box_afExtent.y > 0 && box_afExtent.z - box_afExtent.y > 0)
    {
        cap_dir = Ogre::Vector3::UNIT_Z;
                  cap_dirLength = box_afExtent.z;
        cap_radius = std::max(box_afExtent.x, box_afExtent.z);
    }

    return new CapsuleCollisionShape(cap_radius, cap_dirLength, cap_dir);

    //geom->setOrientation(orient);
    //geom->setPosition(cap_orig + (boneOrientation * (cap_dirLength * 0.5)));

    //return geom;
}
