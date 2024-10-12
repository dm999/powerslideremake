#ifndef TEXLOADER_H
#define TEXLOADER_H

#include "../includes/CommonIncludes.h"

#include "PFLoader.h"
#include "LUTs.h"

#include <array>

/**
 * Load texture from *.tex format
 */
class TEXLoader
{
public:
    TEXLoader(){}

    Ogre::TexturePtr load(const PFLoader& pfLoader, const std::string& subfolder, const std::string& filename, const std::string& texturename, const LUTs& lutsX2, const LUTs& lutsX4, const Ogre::String& group = TEMP_RESOURCE_GROUP_NAME, Ogre::Real gamma = 1.0f, bool doUpscale = false) const;
    Ogre::TexturePtr load(const Ogre::DataStreamPtr& fileToLoad, const std::string& texturename, const LUTs& lutsX2, const LUTs& lutsX4, const Ogre::String& group = TEMP_RESOURCE_GROUP_NAME, Ogre::Real gamma = 1.0f, bool doUpscale = false) const;

    Ogre::TexturePtr loadChroma(    const PFLoader& pfLoader, 
                                    const std::string& subfolder, const std::string& filename, 
                                    const std::string& texturename, const Ogre::String& group = TEMP_RESOURCE_GROUP_NAME,
                                    const Ogre::ColourValue& keyCol = Ogre::ColourValue::Black,
                                    float threshold = 0.003f,
                                    bool resize = false,
                                    size_t newSize = 64) const;

    typedef std::tuple<uint8_t, uint8_t, uint8_t> Pixel;
    typedef std::array<uint8_t, 15> PixelsRow;
    typedef std::tuple<int16_t, int16_t, int16_t> PixelSigned;

private:

    void doBicubicUpscale(Ogre::Image& img) const;

    //LUTs
    void doLUTUpscale(Ogre::Image& img, const LUTs& luts, bool convertoRGB, bool swapRGB) const;
    void doLSBMSB(size_t x, size_t y, const uint8_t* src, size_t stride, const LUTs& luts, PixelSigned* resLSB, PixelSigned* resMSB) const;
    Pixel getPixel(int x, int y, const uint8_t* src, size_t stride) const
    {
        return std::make_tuple(src[y * stride * 3 + x * 3 + 0], src[y * stride * 3 + x * 3 + 1], src[y * stride * 3 + x * 3 + 2]);
    }

    PixelsRow getRow(int x, int y, const uint8_t* src, size_t stride) const
    {
        PixelsRow ret;
        std::copy(&src[y * stride * 3 + x * 3 + 0], &src[y * stride * 3 + x * 3 + 15], ret.begin());
        return ret;
    }

    void rotateBack(std::array<int16_t, 12> & res) const;

    std::vector<uint8_t> toRGB(Ogre::Image& img) const;
    std::vector<uint8_t> AddPadding(const uint8_t * inBuf, size_t width, size_t height, size_t top, size_t bottom, size_t left, size_t right) const;
};

#endif
