#ifndef TEXLOADER_H
#define TEXLOADER_H

#include "../includes/CommonIncludes.h"

#include "PFLoader.h"
/**
 * Load texture from *.tex format
 */
class TEXLoader
{
public:
    TEXLoader(){}

    Ogre::TexturePtr load(const PFLoader& pfLoader, const std::string& subfolder, const std::string& filename, const std::string& texturename, const Ogre::String& group = TEMP_RESOURCE_GROUP_NAME) const;
    Ogre::TexturePtr load(const Ogre::DataStreamPtr& fileToLoad, const std::string& texturename, const Ogre::String& group = TEMP_RESOURCE_GROUP_NAME) const;

    Ogre::TexturePtr loadChroma(    const PFLoader& pfLoader, 
                                    const std::string& subfolder, const std::string& filename, 
                                    const std::string& texturename, const Ogre::String& group = TEMP_RESOURCE_GROUP_NAME,
                                    const Ogre::ColourValue& keyCol = Ogre::ColourValue::Black,
                                    float threshold = 0.003f,
                                    bool resize = false,
                                    size_t newSize = 64) const;

private:
};

#endif
