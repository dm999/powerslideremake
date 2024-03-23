
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
}//anonymous

void TEXLoader::doBicubicUpscale(Ogre::Image& img) const
{
    //https://stackoverflow.com/questions/17640173/implementation-of-bi-cubic-resize

    size_t width = img.getWidth();
    size_t height = img.getHeight();
    
    size_t scaleFactor = 2;

    size_t dest_height = height * scaleFactor;
    size_t dest_width = width * scaleFactor;

    size_t bufSize = Ogre::PixelUtil::getMemorySize(dest_width, dest_height, 1, img.getFormat());
    Ogre::uchar* pixelData = OGRE_ALLOC_T(Ogre::uchar, bufSize, Ogre::MEMCATEGORY_GENERAL);

    Ogre::Image img2;
    img2.loadDynamicImage(pixelData, dest_width, dest_height, 1, img.getFormat(), true);

    float tx = 1.0f / static_cast<float>(scaleFactor);
    float ty = tx;

    const size_t channels = 3;
    const size_t row_stride = dest_width * channels;

    Ogre::ColourValue C[4] = { Ogre::ColourValue::ZERO, Ogre::ColourValue::ZERO, Ogre::ColourValue::ZERO, Ogre::ColourValue::ZERO };

    for(size_t i = 0; i < dest_height; ++i)
    {
        for(size_t j = 0; j < dest_width; ++j)
        {
            const float x = static_cast<float>(tx * j);
            const float y = static_cast<float>(ty * i);

            const float dx = tx * j - x;
            const float dx2 = dx * dx;
            const float dx3 = dx2 * dx;

            const float dy = ty * i - y;
            const float dy2 = dy * dy;
            const float dy3 = dy2 * dy;

            for(int jj = 0; jj < 4; ++jj)
            {
                int idx = static_cast<int>(y - 1 + jj);

                idx = clamp(idx, 0, static_cast<int>(height));

                Ogre::ColourValue a0 = img.getColourAt(static_cast<size_t>(x), idx, 0);
                Ogre::ColourValue d0 = img.getColourAt(clamp(static_cast<int>(x - 1), 0, static_cast<int>(width)), idx, 0) - a0;
                Ogre::ColourValue d2 = img.getColourAt(clamp(static_cast<int>(x + 1), 0, static_cast<int>(width)), idx, 0) - a0;
                Ogre::ColourValue d3 = img.getColourAt(clamp(static_cast<int>(x + 2), 0, static_cast<int>(width)), idx, 0) - a0;

                Ogre::ColourValue a1 = -(1.0f / 3.0f) * d0 + d2 - (1.0f / 6.0f) * d3;
                Ogre::ColourValue a2 = 0.5f  * d0 + 0.5f *  d2;
                Ogre::ColourValue a3 = -(1.0f / 6.0f) * d0 - 0.5f * d2 + (1.0f / 6.0f) * d3;

                C[jj] = a0 + a1 * dx + a2 * dx2 + a3 * dx3;

                d0 = C[0] - C[1];
                d2 = C[2] - C[1];
                d3 = C[3] - C[1];
                a0 = C[1];

                a1 = -(1.0f / 3.0f) * d0 + d2 - (1.0f / 6.0f) * d3;
                a2 = 0.5f  * d0 + 0.5f  * d2;
                a3 = -(1.0f / 6.0f) * d0 - 0.5f * d2 + (1.0f / 6.0f) * d3;

                Ogre::ColourValue colorRes = a0 + a1 * dy + a2 * dy2 + a3 * dy3;
                colorRes.saturate();
                img2.setColourAt(colorRes, j, i, 0);
            }
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