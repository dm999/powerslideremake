
#include "TEXLoader.h"

#include "../tools/OgreTools.h"

#include <algorithm>

namespace
{
    struct header
    {
        typedef unsigned char BYTE;

        BYTE unknown[20];
        Ogre::uint32 width;
        Ogre::uint32 height;
        Ogre::uint32 depth;
    };

    template<class T, class Compare>
    constexpr const T& clamp(const T& v, const T& lo, const T& hi, Compare comp)
    {
        return comp(v, lo) ? lo : comp(hi, v) ? hi : v;
    }

    template<class T>
    constexpr const T& clamp(const T& v, const T& lo, const T& hi)
    {
        return clamp(v, lo, hi, std::less<T>());
    }

    float bicubicW(float x)
    {

        float res = 0.0f;

        float a = -0.5f;

        float abs = std::abs(x);

        if(abs <= 1.0f)
        {
            res = ((a + 2.0f) * (abs * abs * abs)) - ((a + 3.0f) * (abs * abs)) + 1.0f;
        }
        else if(1.0f < abs && abs < 2.0f)
        {
            res = ((a * (abs * abs * abs)) - (5.0f * a * (abs * abs)) + (8.0f * a * abs) - 4.0f * a);
        }

        return res;
    }
}//anonymous

void TEXLoader::doBicubicUpscale(Ogre::Image& img) const
{
    size_t width = img.getWidth();
    size_t height = img.getHeight();

    size_t scaleFactor = 2;
    float scale = 1.0f / scaleFactor;

    size_t dest_height = height * scaleFactor;
    size_t dest_width = width * scaleFactor;

    size_t bufSize = Ogre::PixelUtil::getMemorySize(dest_width, dest_height, 1, img.getFormat());
    Ogre::uchar* pixelData = OGRE_ALLOC_T(Ogre::uchar, bufSize, Ogre::MEMCATEGORY_GENERAL);

    Ogre::Image img2;
    img2.loadDynamicImage(pixelData, dest_width, dest_height, 1, img.getFormat(), true);

    for(size_t y = 0; y < dest_height; ++y)
    {
        for(size_t x = 0; x < dest_width; ++x)
        {
            float xx = ((static_cast<float>(x) + 0.5f) * scale) - 0.5f;
            float yy = ((static_cast<float>(y) + 0.5f) * scale) - 0.5f;

            int xxi = static_cast<int>(xx);
            int yyi = static_cast<int>(yy);

            float u = xx - xxi;
            float v = yy - yyi;

            Ogre::ColourValue pixel = Ogre::ColourValue::ZERO;

            for(int m = -1; m < 3; ++m){
                for(int n = -1; n < 3; ++n){
                    /*
                    if(
                        (xxi + n < 0) ||
                        (xxi + n >= static_cast<int>(width)) ||
                        (yyi + m < 0) ||
                        (yyi + m >= static_cast<int>(height))
                        ) continue;*/
                    if(
                        (xxi + n < 0) &&
                        (yyi + m < 0)
                        ) continue;
                    if(
                        (xxi + n < 0) &&
                        (yyi + m >= static_cast<int>(height))
                        ) continue;
                    if(
                        (yyi + m < 0) &&
                        (xxi + n >= static_cast<int>(width))
                        ) continue;
                    if(
                        (xxi + n >= static_cast<int>(width)) &&
                        (yyi + m >= static_cast<int>(height))
                        ) continue;
                    if(xxi + n < 0 && yyi + m >= 0)
                    {
                        pixel += img.getColourAt(xxi - n, yyi + m, 0) * (bicubicW(u - n) * bicubicW(v - m));
                        continue;
                    }
                    if(xxi + n >= 0 && yyi + m < 0)
                    {
                        pixel += img.getColourAt(xxi + n, yyi - m, 0) * (bicubicW(u - n) * bicubicW(v - m));
                        continue;
                    }
                    if(xxi + n >= static_cast<int>(width) && yyi + m < static_cast<int>(height))
                    {
                        pixel += img.getColourAt(xxi - n, yyi + m, 0) * (bicubicW(u - n) * bicubicW(v - m));
                        continue;
                    }
                    if(xxi + n < static_cast<int>(width) && yyi + m >= static_cast<int>(height))
                    {
                        pixel += img.getColourAt(xxi + n, yyi - m, 0) * (bicubicW(u - n) * bicubicW(v - m));
                        continue;
                    }

                    pixel += img.getColourAt(xxi + n, yyi + m, 0) * (bicubicW(u - n) * bicubicW(v - m));
                }
            }

            pixel.saturate();

            img2.setColourAt(pixel, x, y, 0);
        }
    }

    std::swap(img, img2);
}

Ogre::TexturePtr TEXLoader::load(const Ogre::DataStreamPtr& fileToLoad, const std::string& texturename, const Ogre::String& group, Ogre::Real gamma, bool doUpscale) const
{
    Ogre::TexturePtr res;

    typedef unsigned char BYTE;
    typedef unsigned short WORD;

    if(fileToLoad.get() && fileToLoad->isReadable())
    {
        header head;
        fileToLoad->read(&head, sizeof(header));

#if !defined(__ANDROID__)
        Ogre::PixelFormat targetFormat = head.depth == 16 ? Ogre::PF_R5G6B5 : Ogre::PF_BYTE_BGRA;
#else
        Ogre::PixelFormat targetFormat = head.depth == 16 ? Ogre::PF_B5G6R5 : Ogre::PF_BYTE_BGRA;
#endif

        size_t bufSize = Ogre::PixelUtil::getMemorySize (head.width, head.height, 1, targetFormat);

        // Since Ogre 1.6 Shoggoth, the OGRE_ALLOC_T memory macro must be used:
        Ogre::uchar* pixelData = OGRE_ALLOC_T (Ogre::uchar, bufSize, Ogre::MEMCATEGORY_GENERAL);
        fileToLoad->read(pixelData, bufSize);

        Ogre::Image img;
        img.loadDynamicImage (pixelData, head.width, head.height, 1, targetFormat, true); 

        if (gamma != 1.0f)
        {
#if 0
            if (img.getBPP() == 16)
            {
                size_t bufSize2 = Ogre::PixelUtil::getMemorySize(head.width, head.height, 1, Ogre::PF_BYTE_BGRA);
                Ogre::uchar* pixelData2 = OGRE_ALLOC_T(Ogre::uchar, bufSize2, Ogre::MEMCATEGORY_GENERAL);
                Ogre::PixelBox pbConverted(head.width, head.height, 1, Ogre::PF_BYTE_BGRA, pixelData2);

                Ogre::PixelBox pb(head.width, head.height, 1, targetFormat, img.getData());

                Ogre::PixelUtil::bulkPixelConversion(pb, pbConverted);
                Ogre::Image img2;
                img2.loadDynamicImage(pixelData2, head.width, head.height, 1, Ogre::PF_BYTE_BGRA, true);
                std::swap(img,img2);
            }
            img.applyGamma(img.getData(), gamma, img.getSize(), img.getBPP());
#else
            const Ogre::Real exp = 1.0f / gamma;
            Ogre::Real gammaBuf[256];
            for (size_t q = 0; q < 256; ++q)
            {
                gammaBuf[q] = Ogre::Math::Pow(q / 255.0f, exp);
            }
            for (size_t x = 0; x < head.width; ++x)
            {
                for (size_t y = 0; y < head.height; ++y)
                {
                    Ogre::ColourValue color = img.getColourAt(x, y, 0);
                    color.r = gammaBuf[static_cast<size_t>(color.r * 255.0f)];
                    color.g = gammaBuf[static_cast<size_t>(color.g * 255.0f)];
                    color.b = gammaBuf[static_cast<size_t>(color.b * 255.0f)];
                    img.setColourAt(color, x, y, 0);
                }
            }
#endif
        }

        if(doUpscale) doBicubicUpscale(img);

        res = Ogre::TextureManager::getSingleton().loadImage(texturename, group, img, Ogre::TEX_TYPE_2D);

        Ogre::LogManager::getSingleton().logMessage(Ogre::LML_NORMAL, "[TEXLoader::load]: [" + texturename + "]");
    }

    return res;
}

Ogre::TexturePtr TEXLoader::load(const PFLoader& pfLoader, const std::string& subfolder, const std::string& filename, const std::string& texturename, const Ogre::String& group, Ogre::Real gamma, bool doUpscale) const
{
    Ogre::TexturePtr res;
    Ogre::DataStreamPtr fileToLoad = pfLoader.getFile(subfolder, filename);
    if(fileToLoad.get() && fileToLoad->isReadable())
    {
        res = load(fileToLoad, texturename, group, gamma, doUpscale);
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