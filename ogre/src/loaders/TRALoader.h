#ifndef TRALOADER_H
#define TRALOADER_H

#include "../CommonIncludes.h"

/**
 * Loader of terrain (surface mappings)[*.tra] files
 */
class TRALoader
{
public:
    TRALoader(){}

    bool load(FILE * fileToLoad, CommonIncludes::shared_ptr<Ogre::Image>& image) const;

private:
};

#endif
