#ifndef DE2_H
#define DE2_H

#include <vector>
#include <string>

namespace DE2
{

    /**
     * Original DE2 mesh data is trinagle oriented (Glide, early DX, OpenGL 1.0)
     * Whole mesh is splitted into parts, each of which could have different LODs
     * Each triangle in mesh could have decal or lit texture
     * Corresponding decal/prelit/lit texture coordinates are presented
     * Conversion to batch oriented approach performed through intermidiate state of MSH file
     * MSH file is separated by parts with merged texture coordinates
     * MSH useful to load scene into editors (blender, 3dsmax, etc.)
     * In MSH information about texture type (lit or decal) captured in isDecalTexture array
     */

    /**
     * MSH file structure:
     * Consist of several Parts
     * Each Part has number of triangles possible with different textures
     * For each Part Vertexes & TexCoordinates stored in separated lists with different sizes
     * Besides of diffuse textures there are terrain textures to store surface type (16 different surface types possible - 4bits texture)
     * It is possible to have different amount of diffuse and terrain textures
     * There are two tables: triangle -> texture association for diffuse and terrain textures
     */

    /**
     * Part sample (MSH file, see mshtobin -> parseMsh for details)
    8 6 5
    734.285278 324.411255 85.555847
    734.285278 324.411255 106.337563
    681.484375 382.012878 84.415154
    681.484375 382.012878 105.192406
    581.830261 452.145813 79.230354
    581.830261 452.145813 100.003151
    583.426514 464.856445 79.244682
    583.426514 464.856445 100.006660
    0 1 2
    2 1 3
    2 3 4
    4 3 5
    4 5 6
    6 5 7
    2.000000 13.851574 0.000000
    2.000000 13.851574 0.000000
    2.000000 13.626436 0.000000
    2.000000 13.401299 0.000000
    2.000000 12.854749 0.000000
    0 1 2
    2 0 2
    2 2 3
    3 2 3
    3 3 4
    4 3 4
    3
    dam_tx_corporate_door_lit.bmp
    dam_tx_steel_wall_middle.bmp
    dam_tx_steel_wall_top.bmp
    0
    0
    1
    1
    2
    2
    1
    defaultterrain.bmp
    0
    0
    0
    0
    0
    0
    */

    struct DE2_Vertex
    {
        float x,y,z;

        bool operator == (const DE2_Vertex& other)
        {
            bool ret = false;

            if(
            other.x==x   &&
            other.y==y   &&
            other.z==z
            )ret = true;

            return ret;
        }
    };

    struct DE2_TextureCoord
    {
        float uv,uw,uz;
        char hz0,hz1,hz2;

        bool operator == (const DE2_TextureCoord& other)
        {
            bool ret = false;

            if(
            other.uv==uv   &&
            other.uw==uw   &&
            other.uz==uz
            )ret = true;

            return ret;
        }
    };

    struct DE2_Triangle
    {
        typedef unsigned short WORD;

        WORD v0,v1,v2;
        WORD t0,t1,t2;
        WORD hz0;   // decal flag
        WORD hz1;   // index for texture and terrain
    };

    struct DE2_Part
    {
        typedef unsigned int DWORD;

        DWORD Triangles;
        DWORD IsMainPart;


        std::vector<DE2_Triangle> Data_Triangles;

        DE2_Part()
        {
            Triangles=0;
            IsMainPart=0;
        }

        ~DE2_Part()
        {
            Clear();
        }

        void Clear()
        {
            Triangles=0;
            IsMainPart=0;
                    
            Data_Triangles.clear();
        }
    };

    struct DE2_Light
    {
        DE2_Vertex position;
        float r,g,b;
        float rangeOut, rangeIn;
    };

    struct AABB{
        DE2_Vertex min, max;
        AABB()
        {
            min.x = 0.0f;
            min.y = 0.0f;
            min.z = 0.0f;
            max.x = 0.0f;
            max.y = 0.0f;
            max.z = 0.0f;
        }
    };

    struct DE2_CollisionInfo
    {
        AABB aabb;
        std::vector<DE2_CollisionInfo> subparts;
        short triIndex;//or part index for view Hierarchy

        DE2_CollisionInfo()
        {
            triIndex = -1;
        }
    };

    struct DE2_File
    {
        typedef unsigned int DWORD;

        DWORD Parts;            // total number of parts (low and high poly)
        DWORD Vertexes;
        DWORD TexCoords;
        DWORD TexCoordsDecal;
        DWORD TexturePathCount; // always same count as TerranPathCount
        DWORD TerranPathCount;  // always same count as TexturePathCount
        DWORD RealPartsCount;   // semantical amount of parts

        bool TexCoorTypeOne;

        std::vector<DE2_Vertex> Data_Vertexes;
        std::vector<DE2_TextureCoord> Data_Texture_Coord;
        std::vector<DE2_TextureCoord> Data_Texture_Coord_Decal;
        std::vector<DE2_Part> Data_Parts;           // amount = parts
        std::vector<std::string> Data_TexturePath;  // unique names
        std::vector<std::string> Data_TerranName;   // repeated names (to align size with Data_TexturePath)

        std::vector<int> indexesForHighestLODS; // maps all parts to high poly only (size == RealPartsCount)

        std::vector<DE2_Light> lights;

        std::vector<DE2_CollisionInfo> CollisionInfo_Parts; // amount = parts
        std::vector<DE2_CollisionInfo> CollisionInfo_Global;


        DE2_File();
        ~DE2_File();
        void Clear();
    };

}//DE2 namespace

#endif