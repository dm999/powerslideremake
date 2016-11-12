
#ifndef MSHDATA
#define MSHDATA

#include "OgreInclude.h"
#include <vector>
#include <stdio.h>

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
    std::vector<MSHIndixes> texCoordsIndexes;

    size_t texturesCount;
    std::vector<std::string> textureNames;
    std::vector<bool> isDecalTexture;
    size_t terrainMapCount;
    std::vector<std::string> terrainMapNames;

    std::vector<size_t> textureForTriangleIndex;
    std::vector<size_t> terrainMapForTriangleIndex;

    void preallocateRawData()
    {
        vertexes.resize(vertCount);
        triIndexes.resize(triCount);
        texcoords.resize(texCount);
        texCoordsIndexes.resize(triCount);
        textureForTriangleIndex.resize(triCount);
        terrainMapForTriangleIndex.resize(triCount);
    }

    void preallocateTextureNames()
    {
        textureNames.resize(texturesCount);
        isDecalTexture.resize(texturesCount);
    }

    void preallocateTerrainMaps()
    {
        terrainMapNames.resize(terrainMapCount);
    }


    //processed data
    std::vector<Ogre::Real> plainBuffer;

    //assumed plain indexes
    std::vector<Ogre::Vector3> plainVertices;
    std::vector<Ogre::Vector3> plainNormals;
    std::vector<Ogre::Vector3> plainTexCoords;

    std::vector<std::vector<unsigned short> > submeshesTriangleIndixesDiffuse;
    std::vector<std::vector<size_t> > submeshesTriangleIndixesTerrainMaps;

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
            Ogre::Root::getSingleton().convertColourValue(Ogre::ColourValue(), &colour);

            plainBuffer[q * 10 + 9] = static_cast<float>(colour);
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

        submeshesTriangleIndixesDiffuse.clear();
        submeshesTriangleIndixesTerrainMaps.clear();


        vertexes.clear();
        triIndexes.clear();
        texcoords.clear();
        texCoordsIndexes.clear();

        textureNames.clear();
        isDecalTexture.clear();
        terrainMapNames.clear();

        textureForTriangleIndex.clear();
        terrainMapForTriangleIndex.clear();
    }

    void serialize(FILE * f)
    {
        if(f)
        {
            fwrite(&vertCount, sizeof(size_t), 1, f);
            fwrite(&triCount, sizeof(size_t), 1, f);
            fwrite(&texCount, sizeof(size_t), 1, f);
            fwrite(&texturesCount, sizeof(size_t), 1, f);
            fwrite(&terrainMapCount, sizeof(size_t), 1, f);

            fwrite(&vertexes[0], sizeof(Ogre::Vector3), vertCount, f);
            fwrite(&triIndexes[0], sizeof(MSHIndixes), triCount, f);
            fwrite(&texcoords[0], sizeof(Ogre::Vector3), vertCount, f);
            fwrite(&texCoordsIndexes[0], sizeof(MSHIndixes), triCount, f);

            fwrite(&textureForTriangleIndex[0], sizeof(size_t), triCount, f);
            fwrite(&terrainMapForTriangleIndex[0], sizeof(size_t), triCount, f);

            for(size_t q = 0; q < texturesCount; ++q)
            {
                serialize(textureNames[q], f);
            }

            for(size_t q = 0; q < terrainMapCount; ++q)
            {
                serialize(terrainMapNames[q], f);
            }

            fwrite(&plainVertices[0], sizeof(Ogre::Vector3), triCount * 3, f);
            fwrite(&plainNormals[0], sizeof(Ogre::Vector3), triCount * 3, f);
            fwrite(&plainTexCoords[0], sizeof(Ogre::Vector3), triCount * 3, f);
        }
    }

    void serialize(const std::string& str, FILE * f)
    {
        if(f)
        {
            size_t strSize = str.size();
            fwrite(&strSize, sizeof(size_t), 1, f);
            fwrite(str.c_str(), sizeof(char), strSize, f);
        }
    }

    void deserialize(Ogre::DataStream * stream)
    {
        clear();
        if(stream)
        {
            stream->read(&vertCount, sizeof(size_t));
            stream->read(&triCount, sizeof(size_t));
            stream->read(&texCount, sizeof(size_t));
            stream->read(&texturesCount, sizeof(size_t));
            stream->read(&terrainMapCount, sizeof(size_t));

            preallocateRawData();
            preallocateTextureNames();
            preallocateTerrainMaps();
            preallocatePlainData();

            stream->read(&vertexes[0], sizeof(Ogre::Vector3) * vertCount);
            stream->read(&triIndexes[0], sizeof(MSHIndixes) * triCount);
            stream->read(&texcoords[0], sizeof(Ogre::Vector3) * vertCount);
            stream->read(&texCoordsIndexes[0], sizeof(MSHIndixes) * triCount);

            stream->read(&textureForTriangleIndex[0], sizeof(size_t) * triCount);
            stream->read(&terrainMapForTriangleIndex[0], sizeof(size_t) * triCount);

            for(size_t q = 0; q < texturesCount; ++q)
            {
                deserialize(textureNames[q], stream);
            }

            for(size_t q = 0; q < terrainMapCount; ++q)
            {
                deserialize(terrainMapNames[q], stream);
            }

            stream->read(&plainVertices[0], sizeof(Ogre::Vector3) * triCount * 3);
            stream->read(&plainNormals[0], sizeof(Ogre::Vector3) * triCount * 3);
            stream->read(&plainTexCoords[0], sizeof(Ogre::Vector3) * triCount * 3);
        }
    }

    void deserialize(std::string& str, Ogre::DataStream * stream)
    {
        if(stream)
        {
            size_t strSize;
            stream->read(&strSize, sizeof(size_t));
            char buf[256] = {0};
            stream->read(buf, sizeof(char) * strSize);
            str = buf;
        }
    }
};

#endif