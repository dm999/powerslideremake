#include "../pcheader.h"

#include "TRALoader.h"

#include <vector>


bool TRALoader::load(FILE * fileToLoad, CommonIncludes::shared_ptr<Ogre::Image>& img) const
{
    bool res = false;

    if(fileToLoad)
    {
        typedef unsigned char BYTE;
        typedef unsigned int DWORD;

        DWORD ver, height, width;
        fread(&ver, 4, 1, fileToLoad);
        fread(&height, 4, 1, fileToLoad);
        fread(&width, 4, 1, fileToLoad);

        if(!ver)//check version
        {
            if(height <= 512 && width <= 512)//Terrain map appears to be dodgy
            {
                DWORD readSize = height * width / 2;
                std::vector<BYTE> buffer(readSize);
                fread(&buffer[0], readSize, 1, fileToLoad);

                DWORD imageSize = height * width;
                BYTE * image = OGRE_ALLOC_T(BYTE, imageSize, Ogre::MEMCATEGORY_GENERAL);


                for(size_t q = 0; q < height; ++q)
                {
                    for(size_t w = 0; w < width; ++w)
                    {
                        BYTE data = buffer[q * width/2 + w/2];
                        data &= 0xF;
                        image[q * width + w] = data;
                    }
                }

                img->loadDynamicImage(image, width, height, 1, Ogre::PF_R8, true);

                res = true;
            }
        }
    }
    else {assert(false && "No TRA file");}

    return res;
}