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
#include "OgreBulletCollisionsGImpactShape.h"
#include "OgreBulletCollisionsTrimeshShape.h"
#include "OgreBulletCollisionsDebugLines.h"
#include "OgreBulletConverter.h"

using namespace Ogre;
using namespace OgreBulletCollisions;

static inline void setVector(btVector3 &vec, float *data)
{
    vec.setValue(data[0], data[1], data[2]);
}

namespace OgreBulletCollisions
{
    GImpactConcaveShape::GImpactConcaveShape(Ogre::Vector3 *_vertices,
                                             unsigned int _vertex_count,
                                             unsigned int *_indices,
                                             unsigned int int_index_count)
        : CollisionShape(),
          mTriMesh(NULL)
	{

#ifndef BVH
		mTriMesh = new btTriangleMesh();

        const unsigned int numFaces = int_index_count / 3;

        btVector3 vertexPos[3];
		for (size_t n = 0; n < numFaces; ++n)
		{
			for (unsigned int i = 0; i < 3; ++i)
			{
				const Vector3 &vec = _vertices[*_indices];
				vertexPos[i][0] = vec.x;
				vertexPos[i][1] = vec.y;
				vertexPos[i][2] = vec.z;
                (*_indices)++;
			}

			mTriMesh->addTriangle(vertexPos[0], vertexPos[1], vertexPos[2]);
		}

        btGImpactMeshShape *trimesh = new btGImpactMeshShape(mTriMesh);
        trimesh->setLocalScaling(btVector3(1.f, 1.f, 1.f));
        trimesh->setMargin(0.f);
		trimesh->updateBound();
#else

		btTriangleIndexVertexArray *m_indexVertexArrays = new btTriangleIndexVertexArray(
			int_index_count,
			_indices,
			4,
			_vertex_count,
			(btScalar*) &_vertices[0][0], 
			12);

		bool useQuantizedAabbCompression = true;

		//comment out the next line to read the BVH from disk (first run the demo once to create the BVH)
#define SERIALIZE_TO_DISK 1
#ifdef SERIALIZE_TO_DISK
        btVector3 aabbMin(-1000, -1000, -1000),aabbMax(1000, 1000, 1000);

        btBvhTriangleMeshShape * trimeshShape  = new btBvhTriangleMeshShape(m_indexVertexArrays, useQuantizedAabbCompression, aabbMin, aabbMax);
		
		///we can serialize the BVH data 
		void* buffer = 0;
		int numBytes = trimeshShape->getOptimizedBvh()->calculateSerializeBufferSize();
		buffer = btAlignedAlloc(numBytes,16);
		bool swapEndian = false;
        trimeshShape->getOptimizedBvh()->serialize(buffer, numBytes, swapEndian);
        FILE *file = fopen("bvh.bin","wb");
		fwrite(buffer,1,numBytes,file);
		fclose(file);
		btAlignedFree(buffer);



#else

        btBvhTriangleMeshShape *trimesh = new btBvhTriangleMeshShape(m_indexVertexArrays, useQuantizedAabbCompression, false);

		char* fileName = "bvh.bin";

		FILE* file = fopen(fileName,"rb");
		int size=0;
		btOptimizedBvh* bvh = 0;

        if (fseek(file, 0, SEEK_END) || (size = ftell(file)) == EOF || fseek(file, 0, SEEK_SET)) /* File operations denied? ok, just close and return failure */
        {
			printf("Error: cannot get filesize from %s\n", fileName);
			exit(0);
        }
        else
		{

			fseek(file, 0, SEEK_SET);

			int buffersize = size+btOptimizedBvh::getAlignmentSerializationPadding();

            void* buffer = btAlignedAlloc(buffersize, 16);
            int read = fread(buffer, 1, size, file);
			fclose(file);
			bool swapEndian = false;
            bvh = btOptimizedBvh::deSerializeInPlace(buffer, buffersize, swapEndian);
		}

		trimesh->setOptimizedBvh(bvh);
#endif
#endif
		mShape = trimesh;
	}

	GImpactConcaveShape::~GImpactConcaveShape()
	{
	}
	// -------------------------------------------------------------------------
    bool GImpactConcaveShape::drawWireFrame(DebugLines *wire,
                                            const Ogre::Vector3 &pos,
                                            const Ogre::Quaternion &quat) const
	{
        const int numTris = mTriMesh->getNumTriangles();
		if (numTris > 0)
		{
            const int numSubParts = mTriMesh->getNumSubParts();
            for (int currSubPart = 0; currSubPart < numSubParts; ++currSubPart)
			{
                const unsigned char *vertexBase = NULL;
				int numVerts;
				PHY_ScalarType vertexType;
				int vertexStride;
                const unsigned char *indexBase = NULL;
				int indexStride;
				int numFaces;
				PHY_ScalarType indexType;

                mTriMesh->getLockedReadOnlyVertexIndexBase(&vertexBase, numVerts,
                                                           vertexType, vertexStride,
                                                           &indexBase, indexStride,
                                                           numFaces, indexType,
                                                           currSubPart);

                float *p = NULL;
				btVector3 vert0;
				btVector3 vert1;
				btVector3 vert2;

                for (int t = 0; t < numFaces; ++t)
				{
					if (indexType == PHY_SHORT)
					{
                        short int *index = (short int *)(indexBase + t * indexStride);

                        p = (float *)(vertexBase + index[0] * vertexStride);
						setVector(vert0, p);						
                        p = (float *)(vertexBase + index[1] * vertexStride);
						setVector(vert1, p);			
                        p = (float *)(vertexBase + index[2] * vertexStride);
						setVector(vert2, p);		
					} 
					else
					{
                        int *index = (int *)(indexBase + t * indexStride);

                        p = (float *)(vertexBase + index[0] * vertexStride);
						setVector(vert0, p);						
                        p = (float *)(vertexBase + index[1] * vertexStride);
						setVector(vert1, p);			
                        p = (float *)(vertexBase + index[2] * vertexStride);
						setVector(vert2, p);		
					}

                    wire->addLine(convert(vert0), convert(vert1));
                    wire->addLine(convert(vert1), convert(vert2));
                    wire->addLine(convert(vert2), convert(vert0));
				}
			}
			return true;
		}
		return false;
	}
}
