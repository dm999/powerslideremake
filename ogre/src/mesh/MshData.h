
#ifndef MSHDATA
#define MSHDATA

#include "../includes/OgreInclude.h"
#include <vector>
#include <cstdio>

struct MSHIndixes
{
    size_t a, b, c;
};

struct MSHData
{
    //source data
    size_t vertCount, triCount, texCount;

    std::vector<Ogre::Vector3> vertexes;
    std::vector<MSHIndixes> triIndexes;
    std::vector<Ogre::Vector3> texcoords;
    std::vector<Ogre::ColourValue> colors;
    std::vector<MSHIndixes> texCoordsIndexes;

    size_t texturesCount;
    std::vector<std::string> textureNames;
    std::vector<bool> isDecalTexture;

    std::vector<size_t> textureForTriangleIndex;

    void preallocateRawData()
    {
        vertexes.resize(vertCount);
        triIndexes.resize(triCount);
        texcoords.resize(texCount);
        colors.resize(texCount);
        texCoordsIndexes.resize(triCount);
        textureForTriangleIndex.resize(triCount);
    }

    void preallocateTextureNames()
    {
        textureNames.resize(texturesCount);
        isDecalTexture.resize(texturesCount);
    }


    //processed data
    std::vector<Ogre::Real> plainBuffer;

    //assumed plain indexes
    std::vector<Ogre::Vector3> plainVertices;
    std::vector<Ogre::Vector3> plainNormals;
    std::vector<Ogre::Vector3> plainTexCoords;
    std::vector<Ogre::ColourValue> plainColors;

    std::vector<std::vector<unsigned short> > submeshesTriangleIndixesDiffuse;
    std::vector<std::vector<unsigned int> > submeshesTriangleIndixesTerrainMaps;

    void preallocatePlainBuffer(bool isTerrain)
    {
        if(isTerrain)
            plainBuffer.resize(triCount * 3 * (3 + 3 + 1));//pos+tex+color
        else
            plainBuffer.resize(triCount * 3 * (3 + 3 + 3 + 1));//pos+norm+tex+color
    }

    void preallocatePlainData(bool isTerrain)
    {
        preallocatePlainBuffer(isTerrain);

        plainVertices.resize(triCount * 3);
        if(!isTerrain) plainNormals.resize(triCount * 3);
        plainTexCoords.resize(triCount * 3);
        plainColors.resize(triCount * 3);
    }



    void makePlain(const Ogre::Vector3& centroid, bool isTerrain)
    {
        size_t offset = 10;
        if(isTerrain) offset = 7;

        //create interleaved buffer
        for(size_t q = 0; q < triCount * 3; ++q)
        {
            size_t localOffset = 0;

            plainBuffer[q * offset + localOffset + 0] = plainVertices[q].x - centroid.x;
            plainBuffer[q * offset + localOffset + 1] = plainVertices[q].y - centroid.y;
            plainBuffer[q * offset + localOffset + 2] = plainVertices[q].z - centroid.z;
            localOffset += 3;

            if(!isTerrain)
            {
                plainBuffer[q * offset + localOffset + 0] = plainNormals[q].x;
                plainBuffer[q * offset + localOffset + 1] = plainNormals[q].y;
                plainBuffer[q * offset + localOffset + 2] = plainNormals[q].z;
                localOffset += 3;
            }

            plainBuffer[q * offset + localOffset + 0] = plainTexCoords[q].x;
            plainBuffer[q * offset + localOffset + 1] = plainTexCoords[q].y;
            plainBuffer[q * offset + localOffset + 2] = plainTexCoords[q].z;
            localOffset += 3;

            Ogre::RGBA colour;
            Ogre::Root::getSingleton().convertColourValue(plainColors[q], &colour);

            *reinterpret_cast<Ogre::RGBA *>(&plainBuffer[q * offset + localOffset + 0]) = colour;
        }
    }

    Ogre::Vector3 getCentroid()const
    {
        Ogre::Vector3 res = Ogre::Vector3::ZERO;

        for(size_t j = 0; j < vertCount; ++j)
        {
            res += vertexes[j];
        }

        res /= static_cast<float>(vertCount);

        return res;
    }

    void getMinMax(Ogre::Vector3& min, Ogre::Vector3& max, const Ogre::Vector3& centroid)const
    {
        min = Ogre::Vector3(std::numeric_limits<float>::max());
        max = Ogre::Vector3(std::numeric_limits<float>::min());

        for(size_t j = 0; j < vertCount; ++j)
        {
            min.x = min.x < (vertexes[j].x - centroid.x) ? min.x : (vertexes[j].x - centroid.x);
            min.y = min.y < (vertexes[j].y - centroid.y) ? min.y : (vertexes[j].y - centroid.y);
            min.z = min.z < (vertexes[j].z - centroid.z) ? min.z : (vertexes[j].z - centroid.z);

            max.x = max.x > (vertexes[j].x - centroid.x) ? max.x : (vertexes[j].x - centroid.x);
            max.y = max.y > (vertexes[j].y - centroid.y) ? max.y : (vertexes[j].y - centroid.y);
            max.z = max.z > (vertexes[j].z - centroid.z) ? max.z : (vertexes[j].z - centroid.z);
        }
    }

    void clear()
    {
        plainBuffer.clear();

        plainVertices.clear();
        plainNormals.clear();
        plainTexCoords.clear();
        plainColors.clear();

        submeshesTriangleIndixesDiffuse.clear();
        submeshesTriangleIndixesTerrainMaps.clear();


        vertexes.clear();
        triIndexes.clear();
        texcoords.clear();
        texCoordsIndexes.clear();

        textureNames.clear();
        isDecalTexture.clear();

        textureForTriangleIndex.clear();
    }

};

#endif