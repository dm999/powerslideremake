
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

    void preallocatePlainBuffer()
    {
        plainBuffer.resize(triCount * 3 * (3 + 3 + 3 + 1));//pos+norm+tex+color
    }

    void preallocatePlainData()
    {
        preallocatePlainBuffer();

        plainVertices.resize(triCount * 3);
        plainNormals.resize(triCount * 3);
        plainTexCoords.resize(triCount * 3);
        plainColors.resize(triCount * 3);
    }



    void makePlain(const Ogre::Vector3& centroid)
    {
        //create interleaved buffer
        for(size_t q = 0; q < triCount * 3; ++q)
        {
            plainBuffer[q * 10 + 0] = plainVertices[q].x - centroid.x;
            plainBuffer[q * 10 + 1] = plainVertices[q].y - centroid.y;
            plainBuffer[q * 10 + 2] = plainVertices[q].z - centroid.z;

            plainBuffer[q * 10 + 3] = plainNormals[q].x;
            plainBuffer[q * 10 + 4] = plainNormals[q].y;
            plainBuffer[q * 10 + 5] = plainNormals[q].z;

            plainBuffer[q * 10 + 6] = plainTexCoords[q].x;
            plainBuffer[q * 10 + 7] = plainTexCoords[q].y;
            plainBuffer[q * 10 + 8] = plainTexCoords[q].z;

            Ogre::RGBA colour;
            Ogre::Root::getSingleton().convertColourValue(plainColors[q], &colour);

            *reinterpret_cast<Ogre::RGBA *>(&plainBuffer[q * 10 + 9]) = colour;
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