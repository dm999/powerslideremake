#ifndef TEXLOADER_H
#define TEXLOADER_H

#include "../CommonIncludes.h"

#include "PFLoader.h"
/**
 * Load texture from *.tex format
 */
class TEXLoader
{
public:
    TEXLoader(){}

    Ogre::TexturePtr load(const PFLoader& pfLoader, const std::string& subfolder, const std::string& filename, const std::string& texturename, const Ogre::String& group = TEMP_RESOURCE_GROUP_NAME) const;
    Ogre::TexturePtr load(FILE * fileToLoad, const std::string& texturename, const Ogre::String& group = TEMP_RESOURCE_GROUP_NAME) const;

private:
};

#endif
