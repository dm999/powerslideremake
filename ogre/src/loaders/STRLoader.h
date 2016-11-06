#ifndef STRLOADER_H
#define STRLOADER_H

#include <string>

#include "PFLoader.h"

/**
 * Loader & decoder of encoded strings
 */
class STRLoader
{
public:
    STRLoader(){}

    std::string load(const PFLoader& pfLoader, const std::string& relativeDir, const std::string& file) const;

private:
};

#endif
