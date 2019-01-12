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

    Ogre::TexturePtr generate(const std::string& texturename, Ogre::uint width = 64, Ogre::uint height = 64, const Ogre::ColourValue& keyCol = Ogre::ColourValue::Black, const Ogre::String& group = TEMP_RESOURCE_GROUP_NAME, Ogre::PixelFormat targetTextureFormat = Ogre::PF_A8R8G8B8) const;

    Ogre::TexturePtr load(const PFLoader& pfLoader, const std::string& subfolder, const std::string& filename, const std::string& texturename, const Ogre::String& group = TEMP_RESOURCE_GROUP_NAME, Ogre::Real gamma = 1.0f, bool isHighRes = false) const;
    Ogre::TexturePtr loadChroma(    const PFLoader& pfLoader, 
                                    const std::string& subfolder, const std::string& filename, 
                                    const std::string& texturename, const Ogre::String& group = TEMP_RESOURCE_GROUP_NAME,
                                    const Ogre::ColourValue& keyCol = Ogre::ColourValue::Black,
                                    float threshold = 0.003f,
                                    bool resize = false,
                                    size_t newSize = 64,
                                    bool blackForTransparent = false) const;

    static void adjustTextureSizeIfNecessary(Ogre::Image& image);

};

#endif
