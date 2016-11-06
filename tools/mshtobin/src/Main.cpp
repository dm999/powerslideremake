
#include <stdio.h>

#include "MshData.h"
#include "Conversions.h"

#ifdef _MSC_VER
    #pragma warning(disable:4996)
#endif

void parseMsh(FILE *f, MSHData& mshData)
{
    fscanf(f, "%d %d %d\n", &mshData.vertCount, &mshData.triCount, &mshData.texCount);

    assert(mshData.vertCount != 0);
    assert(mshData.triCount != 0);
    assert(mshData.texCount != 0);

    mshData.preallocateRawData();

    for(size_t q = 0; q < mshData.vertCount; ++q)
    {
        Ogre::Real localX, localY, localZ;
        fscanf(f, "%f %f %f\n", &localX, &localY, &localZ);
        mshData.vertexes[q].x = localX;
        mshData.vertexes[q].y = localZ;
        mshData.vertexes[q].z = -localY;
    }

    for(size_t q = 0; q < mshData.triCount; ++q)
    {
        fscanf(f, "%d %d %d\n", &mshData.triIndexes[q].a, &mshData.triIndexes[q].b, &mshData.triIndexes[q].c);
    }

    for(size_t q = 0; q < mshData.texCount; ++q)
    {
        Ogre::Real localU, localV, localW;
        fscanf(f, "%f %f %f\n", &localU, &localV, &localW);
        mshData.texcoords[q].x = localU;
        mshData.texcoords[q].y = 1.0f - localV;
        mshData.texcoords[q].z = localW;
    }

    for(size_t q = 0; q < mshData.triCount; ++q)
    {
        fscanf(f, "%d %d %d\n", &mshData.texCoordsIndexes[q].a, &mshData.texCoordsIndexes[q].b, &mshData.texCoordsIndexes[q].c);
    }


    fscanf(f, "%d\n", &mshData.texturesCount);

    assert(mshData.texturesCount != 0);

    mshData.preallocateTextureNames();

    for(size_t q = 0; q < mshData.texturesCount; ++q)
    {
        char buf[256] = {0};
        fscanf(f, "%s\n", buf);
        mshData.textureNames[q] = buf;
    }



    for(size_t q = 0; q < mshData.triCount; ++q)
    {
        fscanf(f, "%d\n", &mshData.textureForTriangleIndex[q]);
    }

    fscanf(f, "%d\n", &mshData.terrainMapCount);

    assert(mshData.terrainMapCount != 0);


    mshData.preallocateTerrainMaps();

    for(size_t q = 0; q < mshData.terrainMapCount; ++q)
    {
        char buf[256] = {0};
        fscanf(f, "%s\n", buf);
        mshData.terrainMapNames[q] = buf;
    }


    for(size_t q = 0; q < mshData.triCount; ++q)
    {
        fscanf(f, "%d\n", &mshData.terrainMapForTriangleIndex[q]);
    }
}

int main(int argc, char* argv[])
{

    MSHData mshData;

    if(argc > 2)
    {
        std::string fileNameOriginal = argv[1];
        std::string maxAmount = argv[2];
        size_t maxSize = 0;
        Conversions::DMFromString(maxAmount, maxSize);

        for(size_t w = 0; w <= maxSize; ++w)
        {
            std::string fileName = fileNameOriginal + Conversions::DMToString(w) + ".msh";
            std::string fileNameBin = fileName.substr(0, fileName.size() - 4) + ".bin";
            FILE * f = fopen(fileName.c_str(), "r");
            if(f)
            {
                parseMsh(f, mshData);
                fclose(f);

                mshData.preallocatePlainData();

                for(size_t q = 0; q < mshData.triCount; ++q)
                {
                    Ogre::Vector3 A = mshData.vertexes[mshData.triIndexes[q].a];
                    Ogre::Vector3 B = mshData.vertexes[mshData.triIndexes[q].b];
                    Ogre::Vector3 C = mshData.vertexes[mshData.triIndexes[q].c];

                    Ogre::Vector3 texA = mshData.texcoords[mshData.texCoordsIndexes[q].a];
                    Ogre::Vector3 texB = mshData.texcoords[mshData.texCoordsIndexes[q].b];
                    Ogre::Vector3 texC = mshData.texcoords[mshData.texCoordsIndexes[q].c];

                    Ogre::Vector3 normal = (B - A).crossProduct(C - A);
                    normal.normalise();

                    mshData.plainVertices[q * 3 + 0] = A;
                    mshData.plainVertices[q * 3 + 1] = B;
                    mshData.plainVertices[q * 3 + 2] = C;

                    mshData.plainNormals[q * 3 + 0] = normal;
                    mshData.plainNormals[q * 3 + 1] = normal;
                    mshData.plainNormals[q * 3 + 2] = normal;

                    mshData.plainTexCoords[q * 3 + 0] = texA;
                    mshData.plainTexCoords[q * 3 + 1] = texB;
                    mshData.plainTexCoords[q * 3 + 2] = texC;
                }

                f = fopen(fileNameBin.c_str(), "wb");
                if(f)
                {
                    mshData.serialize(f);
                    fclose(f);
                }
            }
        }
    }
}