#ifndef TEXTURELOADER_H
#define TEXTURELOADER_H

#include "PFLoader.h"

#include "../includes/CommonIncludes.h"

/**
 * Load texture from arbitrary format standard (non *.tex)
 */
class TextureLoader
{
public:
    TextureLoader(){}

    Ogre::TexturePtr load(const PFLoader& pfLoader, const std::string& subfolder, const std::string& filename, const std::string& texturename, const Ogre::String& group = TEMP_RESOURCE_GROUP_NAME) const;
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
