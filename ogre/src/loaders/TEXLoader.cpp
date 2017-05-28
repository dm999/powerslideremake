#include "../pcheader.h"

#include "TEXLoader.h"

#include "../tools/OgreTools.h"

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

Ogre::TexturePtr TEXLoader::load(const Ogre::DataStreamPtr& fileToLoad, const std::string& texturename, const Ogre::String& group) const
{
    Ogre::TexturePtr res;

    typedef unsigned char BYTE;
    typedef unsigned short WORD;
    typedef unsigned int DWORD;

    if(fileToLoad.get() && fileToLoad->isReadable())
    {
        header head;
        fileToLoad->read(&head, sizeof(header));

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

        fileToLoad->read(pDest, sizeofformat * sizeOfBuffer);

        //try to adjust brightness/contrast of texture
#if 0

        Ogre::ulong pxDataIndex = 0, pxDataIndexStep = Ogre::PixelUtil::getNumElemBytes (targetFormat); 

        for(Ogre::uint y = 0; y < head.height; ++y) 
        {
            for(Ogre::uint x = 0; x < head.width; ++x) 
            {
                Ogre::ColourValue pixCol;
                Ogre::PixelUtil::unpackColour (&pixCol, targetFormat, static_cast<void*> (pDest + pxDataIndex) ); 

#if 0
                {
                    Ogre::Vector3 xyz = RGBToXYZ(Ogre::Vector3(pixCol.r, pixCol.g, pixCol.b));
                    //xyz.x *= 1.2f;
                    //xyz.z *= 0.5f;

                    xyz.y *= 0.8f;
                    xyz.z *= 0.5f;

                    Ogre::Vector3 rgb = XYZToRGB(xyz);
                    pixCol.r = rgb.x;
                    pixCol.g = rgb.y;
                    pixCol.b = rgb.z;
                }
#endif
#if 1
                {
                    Ogre::Real hue, saturation, brightness;
                    pixCol.getHSB(&hue, &saturation, &brightness);
                    //hue *= 0.96f;
                    //hue *= 1.1f;
                    //hue *= 0.98f;
                    saturation *= 1.5f;
                    brightness *= 1.0f;
                    pixCol.setHSB(hue, saturation, brightness);
                }
#endif
#if 1
                {
                    Ogre::Real brightness = 0.0f;
                    Ogre::Real contrast = 1.4f;
                    Ogre::Real gamma = 2.4f;

                    pixCol.r /= pixCol.a;
                    pixCol.g /= pixCol.a;
                    pixCol.b /= pixCol.a;
                    pixCol.r = (pixCol.r - 0.5f) * contrast + 0.5f;
                    pixCol.g = (pixCol.g - 0.5f) * contrast + 0.5f;
                    pixCol.b = (pixCol.b - 0.5f) * contrast + 0.5f;
                    pixCol.r += brightness;
                    pixCol.g += brightness;
                    pixCol.b += brightness;
                    pixCol.r = pow(pixCol.r, 1.0f / gamma);
                    pixCol.g = pow(pixCol.g, 1.0f / gamma);
                    pixCol.b = pow(pixCol.b, 1.0f / gamma);
                    pixCol.r *= pixCol.a;
                    pixCol.g *= pixCol.a;
                    pixCol.b *= pixCol.a;
                }
#endif
                pixCol.saturate();
                Ogre::PixelUtil::packColour (pixCol, targetFormat, static_cast<void*> (pDest + pxDataIndex) ); 
                pxDataIndex += pxDataIndexStep; 
            }
        }
#endif
        pixelBuffer->unlock();

        Ogre::LogManager::getSingleton().logMessage(Ogre::LML_NORMAL, "[TEXLoader::load]: [" + texturename + "]");
    }

    return res;
}

Ogre::TexturePtr TEXLoader::load(const PFLoader& pfLoader, const std::string& subfolder, const std::string& filename, const std::string& texturename, const Ogre::String& group) const
{
    Ogre::TexturePtr res;
    Ogre::DataStreamPtr fileToLoad = pfLoader.getFile(subfolder, filename);
    if(fileToLoad.get() && fileToLoad->isReadable())
    {
        res = load(fileToLoad, texturename, group);
        fileToLoad->close();
    }
    return res;
}

Ogre::TexturePtr TEXLoader::loadChroma( const PFLoader& pfLoader, 
                                            const std::string& subfolder, const std::string& filename, 
                                            const std::string& texturename, const Ogre::String& group,
                                            const Ogre::ColourValue& keyCol,
                                            float threshold,
                                            bool resize,
                                            size_t newSize) const
{
    Ogre::TexturePtr res;

    typedef unsigned char BYTE;
    typedef unsigned short WORD;
    typedef unsigned int DWORD;

    Ogre::DataStreamPtr fileToLoad = pfLoader.getFile(subfolder, filename);
    if(fileToLoad.get() && fileToLoad->isReadable())
    {
        header head;
        fileToLoad->read(&head, sizeof(header));

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

        fileToLoad->read(pDest, sizeofformat * sizeOfBuffer);

        Ogre::ulong pxDataIndex = 0, pxDataIndexStep = Ogre::PixelUtil::getNumElemBytes (targetFormat); 

        for(Ogre::uint y = 0; y < head.height; ++y) 
        {
            for(Ogre::uint x = 0; x < head.width; ++x) 
            {
                Ogre::ColourValue pixCol;
                Ogre::PixelUtil::unpackColour (&pixCol, targetFormat, static_cast<void*> (pDest + pxDataIndex) ); 
                Ogre::ColourValue diffCol = pixCol - keyCol; 
                if(pixCol.a == 1.0f)
                    pixCol.a = ( (fabs(diffCol.r)<threshold) && (fabs(diffCol.g)<threshold) && (fabs(diffCol.b)<threshold) ) ? 0.0f : 1.0f; 
                Ogre::PixelUtil::packColour (pixCol, targetFormat, static_cast<void*> (pDest + pxDataIndex) ); 
                pxDataIndex += pxDataIndexStep; 
            }
        }
        pixelBuffer->unlock();

        Ogre::LogManager::getSingleton().logMessage(Ogre::LML_NORMAL, "[TEXLoader::loadChroma]: [" + texturename + "]");
    }
    else {assert(false && "No texture file");}

    return res;
}