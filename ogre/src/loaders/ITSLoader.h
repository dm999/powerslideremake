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

    size_t load(const PFLoader& pfLoader, const std::string& subfolder, const std::string& filename, std::vector<signed short>& rawData) const;

private:

};

#endif
