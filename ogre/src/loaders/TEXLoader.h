#ifndef TEXLOADER_H
#define TEXLOADER_H

#include "../includes/CommonIncludes.h"

#include "PFLoader.h"
#include "LUTs.h"

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
    typedef std::tuple<int8_t, int8_t, int8_t> PixelSigned;

private:

    void doBicubicUpscale(Ogre::Image& img) const;

    //LUTs
    void doLUTUpscale(Ogre::Image& img, const LUTs& luts) const;
    void doLSB(size_t x, size_t y, const uint8_t* src, size_t stride, const LUTs& luts, std::vector<PixelSigned>& res) const;
    void doMSB(size_t x, size_t y, const uint8_t* src, size_t stride, const LUTs& luts, std::vector<PixelSigned>& res) const;
    Pixel getPixel(int x, int y, const uint8_t* src, size_t stride) const;

    std::vector<int8_t> getLUTValsLSB(const std::vector<int8_t>& lut, const Pixel& valA, const Pixel& valB) const;
    std::vector<int8_t> getLUTValsMSB(const std::vector<int8_t>& lut, const Pixel& valA, const Pixel& valB, const Pixel& valC) const;
    void rotateBack(std::vector<int16_t>& res) const;

    std::vector<uint8_t> toRGB(Ogre::Image& img) const;
    std::vector<uint8_t> AddPadding(const uint8_t * inBuf, size_t width, size_t height, size_t top, size_t bottom, size_t left, size_t right) const;
};

#endif
