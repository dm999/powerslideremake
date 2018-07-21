
#include <set>
#include <map>

#include "DE2Loader.h"
#include "../tools/Conversions.h"

namespace DE2
{
    DE2_File::DE2_File()
    {
        Parts=0;
        Vertexes=0;
        TexCoords=0;
        TexCoordsDecal=0;
        TexturePathCount=0;
        TerranPathCount=0;
    }

    DE2_File::~DE2_File()
    {
        Clear();
    }

    void DE2_File::Clear()
    {
        Parts=0;
        Vertexes=0;
        TexCoords=0;
        TexCoordsDecal=0;
        TexturePathCount=0;
        TerranPathCount=0;
                
        Data_Vertexes.clear();
        Data_Texture_Coord.clear();
        Data_Texture_Coord_Decal.clear();
        Data_Parts.clear();
        Data_TexturePath.clear();
        Data_TerranName.clear();
        CollisionInfo_Parts.clear();
        CollisionInfo_Global.clear();
    }

    void Find_xV4(const Ogre::DataStreamPtr& stream, int count)
    {
        typedef unsigned char BYTE;
        typedef unsigned int DWORD;

        DWORD ch=0;
        DWORD temp;
        DWORD xV4=0x12345678;//305419896
        while(ch!=count)
        {
            stream->read(&temp,4);
            if(temp==xV4) ch++;
            else stream->seek(stream->tell() - 3 );
        }
    }

    int loadCollisionInfo(DE2_File & DE2, DE2_CollisionInfo& collisionInfo, const AABB& aabbPrev, const Ogre::DataStreamPtr& stream, int curTri)
    {
        typedef unsigned char BYTE;
        typedef unsigned short WORD;
        typedef unsigned int DWORD;


        float ss_1;
        BYTE ss_2;
        BYTE ss_3;
        BYTE ss_4;
        BYTE ss_5;
        BYTE ss_6;
        BYTE ss_7;
        WORD subCount;

        stream->read(&ss_1,4);
        stream->read(&ss_2,1);
        stream->read(&ss_3,1);
        stream->read(&ss_4,1);
        stream->read(&ss_5,1);
        stream->read(&ss_6,1);
        stream->read(&ss_7,1);
        stream->read(&subCount,2);

        DE2_CollisionInfo collisionInfoLocal;
        collisionInfoLocal.aabb.min.x = aabbPrev.min.x + ss_1 * static_cast<float>(ss_2);
        collisionInfoLocal.aabb.min.y = aabbPrev.min.y + ss_1 * static_cast<float>(ss_3);
        collisionInfoLocal.aabb.min.z = aabbPrev.min.z + ss_1 * static_cast<float>(ss_4);
        collisionInfoLocal.aabb.max.x = aabbPrev.min.x + ss_1 * static_cast<float>(ss_5);
        collisionInfoLocal.aabb.max.y = aabbPrev.min.y + ss_1 * static_cast<float>(ss_6);
        collisionInfoLocal.aabb.max.z = aabbPrev.min.z + ss_1 * static_cast<float>(ss_7);

        int totalsum = 0;

        if(subCount)
        {
            for(int q = 0; q < subCount; ++q)
                totalsum += loadCollisionInfo(DE2, collisionInfoLocal, collisionInfoLocal.aabb, stream, q);
        }
        else
        {
            stream->read(&collisionInfoLocal.triIndex,2);
            totalsum = 1;
        }

        //collisionInfoLocal.aabb.min.z = -collisionInfoLocal.aabb.min.z;//original data is left hand
        //collisionInfoLocal.aabb.max.z = -collisionInfoLocal.aabb.max.z;//original data is left hand
        //std::swap(collisionInfoLocal.aabb.min.z, collisionInfoLocal.aabb.max.z);
        collisionInfo.subparts.push_back(collisionInfoLocal);

        return totalsum;
    }

    void loadViewHierInfo(DE2_File & DE2, DE2_CollisionInfo& collisionInfo, const Ogre::DataStreamPtr& stream)
    {

        typedef unsigned short WORD;

        DE2_CollisionInfo collisionInfoLocal;

        stream->read(&collisionInfoLocal.aabb, sizeof(AABB));

        WORD subCount;
        stream->read(&subCount,2);

        if(subCount)
        {
            for(int q = 0; q < subCount; ++q)
                loadViewHierInfo(DE2, collisionInfoLocal, stream);
        }
        else
        {
            stream->read(&collisionInfoLocal.triIndex,2);//part index here
        }

        //collisionInfoLocal.aabb.min.z = -collisionInfoLocal.aabb.min.z;//original data is left hand
        //collisionInfoLocal.aabb.max.z = -collisionInfoLocal.aabb.max.z;//original data is left hand
        //std::swap(collisionInfoLocal.aabb.min.z, collisionInfoLocal.aabb.max.z);
        collisionInfo.subparts.push_back(collisionInfoLocal);
    }

    void readDE2(const Ogre::DataStreamPtr& stream, DE2_File & DataDE2)
    {
        typedef unsigned char BYTE;
        typedef unsigned short WORD;
        typedef unsigned int DWORD;

        if(stream.get() && stream->isReadable())
        {
            BYTE ch=1;
            DWORD temp;

            while(ch!=0)
            stream->read(&ch,1);
            stream->read(&temp,4);

            stream->read(&DataDE2.Vertexes,4);
            DataDE2.Data_Vertexes.clear();
            DataDE2.Data_Vertexes.resize(DataDE2.Vertexes);

            for(DWORD q=0;q<DataDE2.Vertexes;q++)
            {
                stream->read(&DataDE2.Data_Vertexes[q], 4 * 3);
            }

            //Decal TexVerts
            Find_xV4(stream,1);
            stream->read(&DataDE2.TexCoordsDecal,4);
            if(DataDE2.TexCoordsDecal!=0)
            {
                DataDE2.Data_Texture_Coord_Decal.clear();
                DataDE2.Data_Texture_Coord_Decal.resize(DataDE2.TexCoordsDecal);
                for(DWORD q=0;q<DataDE2.TexCoordsDecal;q++)
                {
                    stream->read(&DataDE2.Data_Texture_Coord_Decal[q].uv, 4 * 2);
                    DataDE2.Data_Texture_Coord_Decal[q].uz = 0.0f;
                    DataDE2.Data_Texture_Coord_Decal[q].r = 0xFF;
                    DataDE2.Data_Texture_Coord_Decal[q].g = 0xFF;
                    DataDE2.Data_Texture_Coord_Decal[q].b = 0xFF;
                }
            }

            //Prelit TexVerts
            Find_xV4(stream,1);
            stream->read(&DataDE2.TexCoords,4);
            if(DataDE2.TexCoords==0)
            {
                DataDE2.TexCoorTypeOne=false;
            }
            else
            {
                DataDE2.TexCoorTypeOne=true;
                DataDE2.Data_Texture_Coord.clear();
                DataDE2.Data_Texture_Coord.resize(DataDE2.TexCoords);

                for(DWORD q=0;q<DataDE2.TexCoords;q++)
                {
                    stream->read(&DataDE2.Data_Texture_Coord[q].uv, 4 * 3);
                    DWORD colorMask = *reinterpret_cast<DWORD*>(&DataDE2.Data_Texture_Coord[q].uz);
                    DataDE2.Data_Texture_Coord[q].r = colorMask & 0xFF;
                    DataDE2.Data_Texture_Coord[q].g = (colorMask >> 8) & 0xFF;
                    DataDE2.Data_Texture_Coord[q].b = (colorMask >> 16 ) & 0xFF;
               }
            }

            //Lit TexVerts
            Find_xV4(stream,1);
            stream->read(&temp,4);
            if(!DataDE2.TexCoorTypeOne)
            {
                DataDE2.TexCoords=temp;
                DataDE2.Data_Texture_Coord.clear();
                DataDE2.Data_Texture_Coord.resize(DataDE2.TexCoords);

                for(DWORD q=0;q<DataDE2.TexCoords;q++)
                {
                    stream->read(&DataDE2.Data_Texture_Coord[q].uv, 4 * 3 + 3);
                }
            }

            //Lights
            Find_xV4(stream,1);
            DWORD ligthsCount;
            stream->read(&ligthsCount,4);
            struct LightsHeader
            {
                float x,y,z;
                float r,g,b;
                float radius_in,radius_out;
                DWORD someCount;
            }lightsHeader;

            for(DWORD q = 0; q < ligthsCount; ++q)
            {
                stream->read(&lightsHeader,sizeof(LightsHeader));
                DE2_Light light;
                light.position.x = lightsHeader.x;
                light.position.y = lightsHeader.y;
                light.position.z = lightsHeader.z;
                light.r = lightsHeader.r;
                light.g = lightsHeader.g;
                light.b = lightsHeader.b;
                light.rangeIn = lightsHeader.radius_in;
                light.rangeOut = lightsHeader.radius_out;
                DataDE2.lights.push_back(light);
            }

            //Meshes
            Find_xV4(stream,1);
            stream->read(&DataDE2.Parts,4);
            DataDE2.Data_Parts.clear();
            DataDE2.Data_Parts.resize(DataDE2.Parts);
            DataDE2.CollisionInfo_Parts.clear();
            DataDE2.CollisionInfo_Parts.resize(DataDE2.Parts);

            for(DWORD q=0;q<DataDE2.Parts;q++)
            {
                if(q!=0)Find_xV4(stream,1);

                stream->read(&DataDE2.Data_Parts[q].IsMainPart,4);
                stream->read(&DataDE2.Data_Parts[q].Triangles,4);
                DataDE2.Data_Parts[q].Data_Triangles.clear();
                DataDE2.Data_Parts[q].Data_Triangles.resize(DataDE2.Data_Parts[q].Triangles);

                for(DWORD w=0;w<DataDE2.Data_Parts[q].Triangles;w++)
                {
                    stream->read(&DataDE2.Data_Parts[q].Data_Triangles[w].v0,2);
                    stream->read(&DataDE2.Data_Parts[q].Data_Triangles[w].v1,2);
                    stream->read(&DataDE2.Data_Parts[q].Data_Triangles[w].v2,2);

                    stream->read(&DataDE2.Data_Parts[q].Data_Triangles[w].t0,2);
                    stream->read(&DataDE2.Data_Parts[q].Data_Triangles[w].t1,2);
                    stream->read(&DataDE2.Data_Parts[q].Data_Triangles[w].t2,2);

                    stream->read(&DataDE2.Data_Parts[q].Data_Triangles[w].hz0,2);
                    stream->read(&DataDE2.Data_Parts[q].Data_Triangles[w].hz1,2);
                }

                DWORD switcher;
                stream->read(&switcher,4);

                if(switcher)
                {
                    DE2_CollisionInfo collisionInfo;
                    stream->read(&collisionInfo.aabb,sizeof(AABB));

                    WORD triCount;
                    stream->read(&triCount,2);

                    if(triCount)
                    {
                        int totalSum = 0;
                        for(int ll = 0; ll < triCount; ++ll)
                        {
                            totalSum += loadCollisionInfo(DataDE2, collisionInfo, collisionInfo.aabb, stream, ll);
                        }
                    }
                    else
                    {
                        stream->read(&collisionInfo.triIndex,2);
                    }

                    DataDE2.CollisionInfo_Parts[q] = collisionInfo;
                }
            }

            //viewHierarchy
            Find_xV4(stream,2);
            DataDE2.CollisionInfo_Global.clear();
            DataDE2.CollisionInfo_Global.resize(1);//root element
            loadViewHierInfo(DataDE2, DataDE2.CollisionInfo_Global[0], stream);

            //texture names
            Find_xV4(stream,1);
            stream->read(&DataDE2.TexturePathCount,4);

            if(DataDE2.TexturePathCount)
            {
                DataDE2.Data_TexturePath.clear();
                DataDE2.Data_TexturePath.resize(DataDE2.TexturePathCount);
                for(DWORD q=0;q<DataDE2.TexturePathCount;q++)
                {
                    stream->read(&temp,4);
                    char buf[4096];
                    stream->read(buf, temp);
                    buf[temp] = 0;
                    DataDE2.Data_TexturePath[q] = buf;;
                }
            }

            //terrain names
            Find_xV4(stream,1);
            stream->read(&DataDE2.TerranPathCount,4);
            if(DataDE2.TerranPathCount)
            {
                DataDE2.Data_TerranName.clear();
                DataDE2.Data_TerranName.resize(DataDE2.TerranPathCount);
                for(DWORD q=0;q<DataDE2.TerranPathCount;q++)
                {
                    stream->read(&temp,4);
                    char buf[4096];
                    stream->read(buf, temp);
                    buf[temp] = 0;

                    std::string terrainName(buf);
                    std::string newFileName = terrainName.substr(terrainName.find_last_of("/\\") + 1);
                    std::transform(newFileName.begin(), newFileName.end(), newFileName.begin(), ::tolower);
                    //std::replace(newFileName.begin(), newFileName.end(), ' ', '_');

                    DataDE2.Data_TerranName[q] = newFileName;
                }
            }

            //LODs
            Find_xV4(stream,1);
            stream->read(&DataDE2.RealPartsCount,4);
            DataDE2.indexesForHighestLODS.clear();
            DataDE2.indexesForHighestLODS.resize(DataDE2.RealPartsCount);
            for(DWORD q=0;q<DataDE2.RealPartsCount;q++)
            {
                DWORD subLODSCount;
                stream->read(&subLODSCount,4);
                for(DWORD w = 0; w < subLODSCount; ++w)
                {
                    DWORD partIndex;
                    float something3,something4;
                    DWORD lodTriCount;
                    stream->read(&partIndex,4);
                    stream->read(&something3,4);
                    stream->read(&something4,4);
                    stream->read(&lodTriCount,4);

                    if(w == 0)
                    {
                        DataDE2.indexesForHighestLODS[q] = partIndex;
                    }
                }
            }

            //transformation matrix

            //visibility cells more

            //vis bound box

            //soundregions

        }
    }

    int FindVertex(const std::vector<DE2_Vertex>& Vertexes, const DE2_Vertex& Vertex, size_t From)
    {
        for (size_t q = From; q < Vertexes.size(); q++)
        {
            if(
                Vertexes[q].x == Vertex.x      &&
                Vertexes[q].y == Vertex.y      &&
                Vertexes[q].z == Vertex.z
            ) return q;
        }

    return -1;
    }

    int FindTexCoor(const std::vector<DE2_TextureCoord>& TexCoord, const DE2_TextureCoord& TriCoor, int From)
    {
        for (size_t q = From; q < TexCoord.size(); q++)
        {
                if(
                    TexCoord[q].uv == TriCoor.uv   &&
                    TexCoord[q].uw == TriCoor.uw   &&
                    TexCoord[q].uz == TriCoor.uz
                ) return q;
        }

    return -1;
    }

    void processPart(const DE2_File& DataDE2, size_t PartIndex, MSHData& mshData)
    {

        mshData.clear();

        typedef unsigned char BYTE;
        typedef unsigned short WORD;
        typedef unsigned int DWORD;

        if(PartIndex >= 0 && PartIndex < DataDE2.Parts)
        {
            std::vector<DE2_Vertex> Vertexes;
            std::vector<DE2_TextureCoord> TexC;
            std::vector<DE2_Triangle> TriTex;

            for(size_t q=0;q<DataDE2.Data_Parts[PartIndex].Triangles;q++)
            {
                WORD hz_0 = DataDE2.Data_Parts[PartIndex].Data_Triangles[q].hz0;
                bool isdecal = !(hz_0 & 3);

                DWORD v0=DataDE2.Data_Parts[PartIndex].Data_Triangles[q].v0;
                DWORD v1=DataDE2.Data_Parts[PartIndex].Data_Triangles[q].v1;
                DWORD v2=DataDE2.Data_Parts[PartIndex].Data_Triangles[q].v2;

                DWORD t0=DataDE2.Data_Parts[PartIndex].Data_Triangles[q].t0;
                DWORD t1=DataDE2.Data_Parts[PartIndex].Data_Triangles[q].t1;
                DWORD t2=DataDE2.Data_Parts[PartIndex].Data_Triangles[q].t2;


                int It_0=FindTexCoor(TexC, DataDE2.Data_Texture_Coord[t0],0);
                int It_1=FindTexCoor(TexC, DataDE2.Data_Texture_Coord[t1],0);
                int It_2=FindTexCoor(TexC, DataDE2.Data_Texture_Coord[t2],0);

                if(isdecal)
                {
                    It_0=FindTexCoor(TexC, DataDE2.Data_Texture_Coord_Decal[t0],0);
                    It_1=FindTexCoor(TexC, DataDE2.Data_Texture_Coord_Decal[t1],0);
                    It_2=FindTexCoor(TexC, DataDE2.Data_Texture_Coord_Decal[t2],0);
                }

                int Iv_0=FindVertex(Vertexes, DataDE2.Data_Vertexes[v0],0);
                int Iv_1=FindVertex(Vertexes, DataDE2.Data_Vertexes[v1],0);
                int Iv_2=FindVertex(Vertexes, DataDE2.Data_Vertexes[v2],0);

                if(It_0 == -1)
                {
                    if(isdecal)
                    {
                        TexC.push_back(DataDE2.Data_Texture_Coord_Decal[t0]);
                    }
                    else
                    {
                        TexC.push_back(DataDE2.Data_Texture_Coord[t0]);
                    }

                    It_0 = TexC.size() - 1;
                }

                if(It_1==-1)
                {
                    if(isdecal)
                    {
                        TexC.push_back(DataDE2.Data_Texture_Coord_Decal[t1]);
                    }
                    else
                    {
                        TexC.push_back(DataDE2.Data_Texture_Coord[t1]);
                    }

                    It_1 = TexC.size() - 1;
                }


                if(It_2==-1)
                {
                    if(isdecal)
                    {
                        TexC.push_back(DataDE2.Data_Texture_Coord_Decal[t2]);
                    }
                    else
                    {
                        TexC.push_back(DataDE2.Data_Texture_Coord[t2]);
                    }

                    It_2 = TexC.size() - 1;
                }


                Iv_0=FindVertex(Vertexes, DataDE2.Data_Vertexes[v0],0);
                Iv_1=FindVertex(Vertexes, DataDE2.Data_Vertexes[v1],0);
                Iv_2=FindVertex(Vertexes, DataDE2.Data_Vertexes[v2],0);


                if(Iv_0==-1)
                {
                    Vertexes.push_back(DataDE2.Data_Vertexes[v0]);
                    Iv_0 = Vertexes.size() - 1;
                }

                if(Iv_1==-1)
                {
                    Vertexes.push_back(DataDE2.Data_Vertexes[v1]);
                    Iv_1 = Vertexes.size() - 1;
                }

                if(Iv_2==-1)
                {
                    Vertexes.push_back(DataDE2.Data_Vertexes[v2]);
                    Iv_2 = Vertexes.size() - 1;
                }


                DE2_Triangle TriTemp;
                TriTemp.t0=It_0;
                TriTemp.t1=It_1;
                TriTemp.t2=It_2;
                TriTemp.v0=Iv_0;
                TriTemp.v1=Iv_1;
                TriTemp.v2=Iv_2;
                TriTex.push_back(TriTemp);
            }

            //store data

            mshData.vertCount = Vertexes.size();
            mshData.triCount = DataDE2.Data_Parts[PartIndex].Triangles;
            mshData.texCount = TexC.size();

            assert(mshData.vertCount != 0);
            assert(mshData.triCount != 0);
            assert(mshData.texCount != 0);

            mshData.preallocateRawData();

            for(size_t q = 0; q < Vertexes.size(); q++)
            {
                mshData.vertexes[q].x = Vertexes[q].x;
                mshData.vertexes[q].y = Vertexes[q].y;
                mshData.vertexes[q].z = -Vertexes[q].z;//original data is left hand
            }

            for(size_t q=0;q<DataDE2.Data_Parts[PartIndex].Triangles;q++)
            {
                DWORD v0=DataDE2.Data_Parts[PartIndex].Data_Triangles[q].v0;
                DWORD v1=DataDE2.Data_Parts[PartIndex].Data_Triangles[q].v1;
                DWORD v2=DataDE2.Data_Parts[PartIndex].Data_Triangles[q].v2;

                v0=FindVertex(Vertexes, DataDE2.Data_Vertexes[v0],0);
                v1=FindVertex(Vertexes, DataDE2.Data_Vertexes[v1],0);
                v2=FindVertex(Vertexes, DataDE2.Data_Vertexes[v2],0);

                mshData.triIndexes[q].a = v0;
                mshData.triIndexes[q].b = v1;
                mshData.triIndexes[q].c = v2;
            }

            for(size_t q = 0; q < TexC.size(); q++)
            {
                mshData.texcoords[q].x = TexC[q].uv;
                mshData.texcoords[q].y = TexC[q].uw;
                mshData.texcoords[q].z = TexC[q].uz;

                mshData.colors[q] = Ogre::ColourValue(TexC[q].r / 255.0f, TexC[q].g / 255.0f, TexC[q].b / 255.0f);
            }

            for(size_t q=0;q<DataDE2.Data_Parts[PartIndex].Triangles;q++)
            {
                mshData.texCoordsIndexes[q].a = TriTex[q].t0;
                mshData.texCoordsIndexes[q].b = TriTex[q].t1;
                mshData.texCoordsIndexes[q].c = TriTex[q].t2;
            }

            //find unique texture names for part
            std::set<std::string> texNames;
            std::set<std::string> texNamesNew;

            typedef std::map<std::string, bool> mBool;
            mBool texNamesToDecals;

            for(size_t q=0;q<DataDE2.Data_Parts[PartIndex].Triangles;q++)
            {

                    std::string textureName = DataDE2.Data_TexturePath[DataDE2.Data_Parts[PartIndex].Data_Triangles[q].hz1];

                    texNames.insert(textureName);


                    std::string newFileName = textureName.substr(textureName.find_last_of("/\\") + 1);
                    newFileName = newFileName.substr(0, newFileName.length() - 3) + "tex";
                    std::transform(newFileName.begin(), newFileName.end(), newFileName.begin(), ::tolower);
                    //std::replace(newFileName.begin(), newFileName.end(), ' ', '_');
                    texNamesNew.insert(newFileName);

                    bool isdecal = !(DataDE2.Data_Parts[PartIndex].Data_Triangles[q].hz0 & 3);
                    mBool::iterator found = texNamesToDecals.find(newFileName);
                    if(found == texNamesToDecals.end())
                    {
                        texNamesToDecals.insert(std::make_pair(newFileName, isdecal));
                    }
                    else if(isdecal && found != texNamesToDecals.end())
                    {
                        (*found).second = isdecal;
                    }
            }

            //textures writing
            mshData.texturesCount = texNamesNew.size();

            assert(mshData.texturesCount != 0);
            mshData.preallocateTextureNames();

            {
                size_t texIndex = 0;
                for(std::set<std::string>::iterator i = texNamesNew.begin(),j = texNamesNew.end();i != j; ++i)
                {
                    mshData.textureNames[texIndex] = (*i);

                    mBool::const_iterator found = texNamesToDecals.find(*i);
                    mshData.isDecalTexture[texIndex++] = (*found).second;
                }
            }

            for(size_t q=0;q<DataDE2.Data_Parts[PartIndex].Triangles;q++)
            {
                int index = 0;
                std::string textureName = DataDE2.Data_TexturePath[DataDE2.Data_Parts[PartIndex].Data_Triangles[q].hz1];
                std::string newFileName = textureName.substr(textureName.find_last_of("/\\") + 1);
                newFileName = newFileName.substr(0, newFileName.length() - 3) + "tex";
                std::transform(newFileName.begin(), newFileName.end(), newFileName.begin(), ::tolower);
                //std::replace(newFileName.begin(), newFileName.end(), ' ', '_');

                std::set<std::string>::iterator i = texNamesNew.find(newFileName);
                index = std::distance(texNamesNew.begin(), i);

                mshData.textureForTriangleIndex[q] = index;
            }

            //fprintf(f,"PartIndex: %d\n",PartIndex);
        }//if(PartIndex >= 0 && PartIndex < DataDE2.Parts)
    }

}//DE2 namespace

bool DE2Loader::load(std::vector<MSHData>& parts, const Ogre::DataStreamPtr& fileToLoad)
{
    bool res = false;

    parts.clear();

    if(fileToLoad.get() && fileToLoad->isReadable())
    {
        mDE2.Clear();
        readDE2(fileToLoad, mDE2);

        for(size_t q = 0; q < mDE2.RealPartsCount; q++)
        {
            MSHData mshData;
            int partIndex = mDE2.indexesForHighestLODS[q];
            processPart(mDE2, partIndex, mshData);

            mshData.preallocatePlainData();
            for(size_t q = 0; q < mshData.triCount; ++q)
            {
                Ogre::Vector3 A = mshData.vertexes[mshData.triIndexes[q].a];
                Ogre::Vector3 B = mshData.vertexes[mshData.triIndexes[q].b];
                Ogre::Vector3 C = mshData.vertexes[mshData.triIndexes[q].c];

                Ogre::Vector3 texA = mshData.texcoords[mshData.texCoordsIndexes[q].a];
                Ogre::Vector3 texB = mshData.texcoords[mshData.texCoordsIndexes[q].b];
                Ogre::Vector3 texC = mshData.texcoords[mshData.texCoordsIndexes[q].c];

                Ogre::ColourValue colA = mshData.colors[mshData.texCoordsIndexes[q].a];
                Ogre::ColourValue colB = mshData.colors[mshData.texCoordsIndexes[q].b];
                Ogre::ColourValue colC = mshData.colors[mshData.texCoordsIndexes[q].c];

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

                mshData.plainColors[q * 3 + 0] = colA;
                mshData.plainColors[q * 3 + 1] = colB;
                mshData.plainColors[q * 3 + 2] = colC;
            }

            parts.push_back(mshData);
        }
        res = true;
    }

    return res;
}