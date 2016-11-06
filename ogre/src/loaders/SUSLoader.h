#ifndef SUSLOADER_H
#define SUSLOADER_H

#include <vector>

#include "PFLoader.h"

/**
 * Suspension data loader
 */
class SUSLoader
{
public:
    SUSLoader(){}

    void load(const PFLoader& pfLoader, const std::string& subfolder, const std::string& filename, std::vector<std::vector<Ogre::Vector3> >& suspensionData) const;

private:

    std::vector<Ogre::Vector3> readWheelSuspension(FILE * fileToLoad) const;

};

#endif
