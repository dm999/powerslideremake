#include "../pcheader.h"

#include "TEXLoader.h"

namespace
{
    struct header
    {
        typedef unsigned char BYTE;
        typedef unsigned int DWORD;

        BYTE unknown[20];
        DWORD width;
        DWORD height;
        DWORD depth;
    };
}

Ogre::TexturePtr TEXLoader::load(FILE * fileToLoad, const std::string& texturename, const Ogre::String& group) const
{
    Ogre::TexturePtr res;

    typedef unsigned char BYTE;
    typedef unsigned short WORD;
    typedef unsigned int DWORD;

    if(fileToLoad)
    {
        header head;
        fread(&head, sizeof(header), 1, fileToLoad);

        size_t sizeOfBuffer = head.width * head.height;

        Ogre::PixelFormat targetFormat = head.depth == 16 ? Ogre::PF_R5G6B5 : Ogre::PF_BYTE_BGRA;

        res = Ogre::TextureManager::getSingleton().createManual(
            texturename,
            group,
            Ogre::TEX_TYPE_2D,
            head.width, head.height,
            Ogre::MIP_UNLIMITED,
            targetFormat,
            Ogre::TU_DEFAULT);

        Ogre::HardwarePixelBufferSharedPtr pixelBuffer = res->getBuffer();
        pixelBuffer->lock(Ogre::HardwareBuffer::HBL_NORMAL);
        const Ogre::PixelBox& pixelBox = pixelBuffer->getCurrentLock();
        BYTE* pDest = static_cast<BYTE*>(pixelBox.data);

        size_t sizeofformat = Ogre::PixelUtil::getNumElemBytes(targetFormat);

        fread(pDest, sizeofformat * sizeOfBuffer, 1, fileToLoad);
        pixelBuffer->unlock();
    }

    return res;
}

Ogre::TexturePtr TEXLoader::load(const PFLoader& pfLoader, const std::string& subfolder, const std::string& filename, const std::string& texturename, const Ogre::String& group) const
{
    Ogre::TexturePtr res;
    FILE * fileToLoad = pfLoader.getFile(subfolder, filename);
    if(fileToLoad)
    {
        res = load(fileToLoad, texturename, group);
        fclose(fileToLoad);

        Ogre::LogManager::getSingleton().logMessage(Ogre::LML_NORMAL, "[TEXLoader::load]: [" + filename + "]");
    }
    return res;
}
