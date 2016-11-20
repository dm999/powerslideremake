#include "../pcheader.h"

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
        Data_TerranPath.clear();
    }

    void Find_xV4(FILE *f, int count)
    {
        typedef unsigned char BYTE;
        typedef unsigned int DWORD;

        DWORD ch=0;
        DWORD temp;
        DWORD xV4=0x12345678;//305419896
        while(ch!=count)
        {
            fread(&temp,4,1,f);
            if(temp==xV4) ch++;
            else fseek(f,-3,SEEK_CUR);
        }
    }

    int loadTriInfo(DE2_File & DE2, int partIndex, FILE * f, int curTri)
    {
        typedef unsigned char BYTE;
        typedef unsigned short WORD;
        typedef unsigned int DWORD;

        //lightmap per vertex???
        float ss_1;
        BYTE ss_2;
        BYTE ss_3;
        BYTE ss_4;
        BYTE bright;
        BYTE ss_6;
        BYTE ss_7;
        WORD subCount;

        fread(&ss_1,1,4,f);
        fread(&ss_2,1,1,f);
        fread(&ss_3,1,1,f);
        fread(&ss_4,1,1,f);
        fread(&bright,1,1,f);
        fread(&ss_6,1,1,f);
        fread(&ss_7,1,1,f);
        fread(&subCount,1,2,f);

        int totalsum = 0;

        if(subCount)
        {
            for(int q = 0; q < subCount; ++q)
                totalsum += loadTriInfo(DE2, partIndex, f, q);
        }
        else
        {
            WORD triIndex;
            fread(&triIndex,1,2,f);

            int vertIndexA = DE2.Data_Parts[partIndex].Data_Triangles[triIndex].v0;
            int vertIndexB = DE2.Data_Parts[partIndex].Data_Triangles[triIndex].v1;
            int vertIndexC = DE2.Data_Parts[partIndex].Data_Triangles[triIndex].v2;

            int texIndexA = DE2.Data_Parts[partIndex].Data_Triangles[triIndex].t0;
            int texIndexB = DE2.Data_Parts[partIndex].Data_Triangles[triIndex].t1;
            int texIndexC = DE2.Data_Parts[partIndex].Data_Triangles[triIndex].t2;

            totalsum = 1;
        }

        return totalsum;
    }

    void readDE2(FILE * f, DE2_File & DataDE2)
    {
        typedef unsigned char BYTE;
        typedef unsigned short WORD;
        typedef unsigned int DWORD;

        if(f)
        {
            BYTE ch=1;
            DWORD temp;

            while(ch!=0)
            fread(&ch,1,1,f);
            fread(&temp,4,1,f);

            fread(&DataDE2.Vertexes,4,1,f);
            DataDE2.Data_Vertexes.clear();
            DataDE2.Data_Vertexes.resize(DataDE2.Vertexes);

            for(DWORD q=0;q<DataDE2.Vertexes;q++)
            {
                fread(&DataDE2.Data_Vertexes[q].x,1,4,f);
                fread(&DataDE2.Data_Vertexes[q].y,1,4,f);
                fread(&DataDE2.Data_Vertexes[q].z,1,4,f);
            }

            //Decal TexVerts
            Find_xV4(f,1);
            fread(&DataDE2.TexCoordsDecal,4,1,f);
            if(DataDE2.TexCoordsDecal!=0)
            {
                DataDE2.Data_Texture_Coord_Decal.clear();
                DataDE2.Data_Texture_Coord_Decal.resize(DataDE2.TexCoordsDecal);
                for(DWORD q=0;q<DataDE2.TexCoordsDecal;q++)
                {
                    fread(&DataDE2.Data_Texture_Coord_Decal[q].uv,1,4,f);
                    fread(&DataDE2.Data_Texture_Coord_Decal[q].uw,1,4,f);
                    DataDE2.Data_Texture_Coord_Decal[q].uz = 0.0f;
                }
            }

            //Prelit TexVerts
            Find_xV4(f,1);
            fread(&DataDE2.TexCoords,4,1,f);
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
                    fread(&DataDE2.Data_Texture_Coord[q].uv,1,4,f);
                    fread(&DataDE2.Data_Texture_Coord[q].uw,1,4,f);
                    fread(&DataDE2.Data_Texture_Coord[q].uz,1,4,f);
                    DataDE2.Data_Texture_Coord[q].hz0=0;
                    DataDE2.Data_Texture_Coord[q].hz1=0;
                    DataDE2.Data_Texture_Coord[q].hz2=0;
                }
            }

            //Lit TexVerts
            Find_xV4(f,1);
            fread(&temp,4,1,f);
            if(!DataDE2.TexCoorTypeOne)
            {
                DataDE2.TexCoords=temp;
                DataDE2.Data_Texture_Coord.clear();
                DataDE2.Data_Texture_Coord.resize(DataDE2.TexCoords);

                for(DWORD q=0;q<DataDE2.TexCoords;q++)
                {
                    fread(&DataDE2.Data_Texture_Coord[q].uv,1,4,f);
                    fread(&DataDE2.Data_Texture_Coord[q].uw,1,4,f);
                    fread(&DataDE2.Data_Texture_Coord[q].uz,1,4,f);
                    fread(&DataDE2.Data_Texture_Coord[q].hz0,1,1,f);
                    fread(&DataDE2.Data_Texture_Coord[q].hz1,1,1,f);
                    fread(&DataDE2.Data_Texture_Coord[q].hz2,1,1,f);
                }
            }

            //Lights
            Find_xV4(f,1);
            DWORD ligthsCount;
            fread(&ligthsCount,4,1,f);
            struct LightsHeader
            {
                float x,y,z;
                float r,g,b;
                float radius_in,radius_out;
                DWORD someCount;
            }lightsHeader;

            for(DWORD q = 0; q < ligthsCount; ++q)
            {
                fread(&lightsHeader,sizeof(LightsHeader),1,f);
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
            Find_xV4(f,1);
            fread(&DataDE2.Parts,4,1,f);
            DataDE2.Data_Parts.clear();
            DataDE2.Data_Parts.resize(DataDE2.Parts);

            for(DWORD q=0;q<DataDE2.Parts;q++)
            {
                if(q!=0)Find_xV4(f,1);

                fread(&DataDE2.Data_Parts[q].IsMainPart,4,1,f);
                fread(&DataDE2.Data_Parts[q].Triangles,4,1,f);
                DataDE2.Data_Parts[q].Data_Triangles.clear();
                DataDE2.Data_Parts[q].Data_Triangles.resize(DataDE2.Data_Parts[q].Triangles);

                for(DWORD w=0;w<DataDE2.Data_Parts[q].Triangles;w++)
                {
                    fread(&DataDE2.Data_Parts[q].Data_Triangles[w].v0,1,2,f);
                    fread(&DataDE2.Data_Parts[q].Data_Triangles[w].v1,1,2,f);
                    fread(&DataDE2.Data_Parts[q].Data_Triangles[w].v2,1,2,f);

                    fread(&DataDE2.Data_Parts[q].Data_Triangles[w].t0,1,2,f);
                    fread(&DataDE2.Data_Parts[q].Data_Triangles[w].t1,1,2,f);
                    fread(&DataDE2.Data_Parts[q].Data_Triangles[w].t2,1,2,f);

                    fread(&DataDE2.Data_Parts[q].Data_Triangles[w].hz0,1,2,f);
                    fread(&DataDE2.Data_Parts[q].Data_Triangles[w].hz1,1,2,f);
                }

                DWORD switcher;
                fread(&switcher,1,4,f);

                if(switcher)
                {
                    struct switcher_1{ float x,y,z;} sw_1, sw_2;
                    fread(&sw_1,1,sizeof(switcher_1),f);
                    fread(&sw_2,1,sizeof(switcher_1),f);
                    WORD triCount;
                    fread(&triCount,1,2,f);

                    if(triCount)
                    {
                        int totalSum = 0;
                        for(int ll = 0; ll < triCount; ++ll)
                        {
                            totalSum += loadTriInfo(DataDE2, q, f, ll);
                        }
                    }else
                    {
                        WORD triIndex;
                        fread(&triIndex,1,2,f);
                    }
                }
            }

            //texture names
            Find_xV4(f,3);
            fread(&DataDE2.TexturePathCount,4,1,f);

            if(DataDE2.TexturePathCount)
            {
                DataDE2.Data_TexturePath.clear();
                DataDE2.Data_TexturePath.resize(DataDE2.TexturePathCount);
                for(DWORD q=0;q<DataDE2.TexturePathCount;q++)
                {
                    fread(&temp,4,1,f);
                    char buf[4096];
                    fread(buf, 1, temp, f);
                    buf[temp] = 0;
                    DataDE2.Data_TexturePath[q] = buf;;
                }
            }

            //terrain names
            Find_xV4(f,1);
            fread(&DataDE2.TerranPathCount,4,1,f);
            if(DataDE2.TerranPathCount)
            {
                DataDE2.Data_TerranPath.clear();
                DataDE2.Data_TerranPath.resize(DataDE2.TerranPathCount);
                for(DWORD q=0;q<DataDE2.TerranPathCount;q++)
                {
                    fread(&temp,4,1,f);
                    char buf[4096];
                    fread(buf, 1, temp, f);
                    buf[temp] = 0;
                    DataDE2.Data_TerranPath[q] = buf;
                }
            }

            //LODs
            Find_xV4(f,1);
            fread(&DataDE2.RealPartsCount,4,1,f);
            DataDE2.indexesForHighestLODS.clear();
            DataDE2.indexesForHighestLODS.resize(DataDE2.RealPartsCount);
            for(DWORD q=0;q<DataDE2.RealPartsCount;q++)
            {
                DWORD subLODSCount;
                fread(&subLODSCount,4,1,f);
                for(DWORD w = 0; w < subLODSCount; ++w)
                {
                    DWORD partIndex;
                    float something3,something4;
                    DWORD lodTriCount;
                    fread(&partIndex,4,1,f);
                    fread(&something3,4,1,f);
                    fread(&something4,4,1,f);
                    fread(&lodTriCount,4,1,f);

                    if(w == 0)
                    {
                        DataDE2.indexesForHighestLODS[q] = partIndex;
                    }
                }
            }
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
                mshData.vertexes[q].z = -Vertexes[q].z;
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

            std::set<std::string> terrainNames;
            std::set<std::string> terrainNamesNew;

            typedef std::map<std::string, bool> mBool;
            mBool texNamesToDecals;

            for(size_t q=0;q<DataDE2.Data_Parts[PartIndex].Triangles;q++)
            {

                    std::string textureName = DataDE2.Data_TexturePath[DataDE2.Data_Parts[PartIndex].Data_Triangles[q].hz1];
                    std::string terrainName = DataDE2.Data_TerranPath[DataDE2.Data_Parts[PartIndex].Data_Triangles[q].hz1];

                    texNames.insert(textureName);
                    terrainNames.insert(terrainName);


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

                    newFileName = terrainName.substr(terrainName.find_last_of("/\\") + 1);
                    std::transform(newFileName.begin(), newFileName.end(), newFileName.begin(), ::tolower);
                    //newFileName = newFileName.substr(0, newFileName.length() - 3) + "bmp";
                    std::replace(newFileName.begin(), newFileName.end(), ' ', '_');
                    terrainNamesNew.insert(newFileName);
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


            //terrains writing
            mshData.terrainMapCount = terrainNamesNew.size();

            assert(mshData.terrainMapCount != 0);
            mshData.preallocateTerrainMaps();

            {
                size_t terrainIndex = 0;
                for(std::set<std::string>::iterator i = terrainNamesNew.begin(),j = terrainNamesNew.end();i != j; ++i)
                {
                    mshData.terrainMapNames[terrainIndex++] = (*i);
                }
            }

            for(size_t q = 0; q < DataDE2.Data_Parts[PartIndex].Triangles; q++)
            {
                    int index = 0;
                    std::string terrainName = DataDE2.Data_TerranPath[DataDE2.Data_Parts[PartIndex].Data_Triangles[q].hz1];
                    std::string newFileName = terrainName.substr(terrainName.find_last_of("/\\") + 1);
                    std::transform(newFileName.begin(), newFileName.end(), newFileName.begin(), ::tolower);
                    //newFileName = newFileName.substr(0, newFileName.length() - 3) + "bmp";
                    std::replace(newFileName.begin(), newFileName.end(), ' ', '_');

                    std::set<std::string>::iterator i = terrainNamesNew.find(newFileName);
                    index = std::distance(terrainNamesNew.begin(), i);

                    mshData.terrainMapForTriangleIndex[q] = index;
            }

            //fprintf(f,"PartIndex: %d\n",PartIndex);
        }//if(PartIndex >= 0 && PartIndex < DataDE2.Parts)
    }

}//DE2 namespace

bool DE2Loader::load(std::vector<MSHData>& parts, FILE * fileToLoad)
{
    bool res = false;

    parts.clear();

    if(fileToLoad)
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

            parts.push_back(mshData);
        }
        res = true;
    }

    return res;
}