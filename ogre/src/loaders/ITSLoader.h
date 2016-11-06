#ifndef ITSLOADER_H
#define ITSLOADER_H

#include <vector>

#include "PFLoader.h"

/**
 * Sound data loader
 */
class ITSLoader
{
public:
    ITSLoader(){}

    size_t load(const PFLoader& pfLoader, const std::string& subfolder, const std::string& filename, std::vector<unsigned char>& rawData) const;

private:

};

#endif
