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

private:

    void doBicubicUpscale(Ogre::Image& img) const;

    //LUTs
    void doLUTUpscale(Ogre::Image& img, const LUTs& luts) const;
    void doLSB(size_t x, size_t y, const Ogre::Image& src, const LUTs& luts, std::vector<Ogre::ColourValue>& res) const;
    void doMSB(size_t x, size_t y, const Ogre::Image& src, const LUTs& luts, std::vector<Ogre::ColourValue>& res) const;
    Ogre::ColourValue getPixel(int x, int y, const Ogre::Image& src) const;

    std::vector<int16_t> getLUTValsLSB(const std::vector<int8_t>& lut, const Ogre::ColourValue& valA, const Ogre::ColourValue& valB) const;
    std::vector<int16_t> getLUTValsMSB(const std::vector<int8_t>& lut, const Ogre::ColourValue& valA, const Ogre::ColourValue& valB, const Ogre::ColourValue& valC) const;
    void rotateBack(std::vector<int16_t>& res) const;
};

#endif
