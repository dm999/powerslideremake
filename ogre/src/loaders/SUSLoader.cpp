#include "../pcheader.h"

#include <string>

#include "SUSLoader.h"

namespace
{
    struct PartSuspension
    {
        std::string name;
        std::vector<Ogre::Vector3> frontLeft;
        std::vector<Ogre::Vector3> frontRight;
        std::vector<Ogre::Vector3> backLeft;
        std::vector<Ogre::Vector3> backRight;
    };
}//anonimoys namespace

void SUSLoader::load(const PFLoader& pfLoader, const std::string& subfolder, const std::string& filename, std::vector<std::vector<Ogre::Vector3> >& suspensionData) const
{
    FILE * fileToLoad = pfLoader.getFile("data/cars/" + subfolder, filename);
    if(fileToLoad)
    {
        suspensionData.clear();

        PartSuspension parts[4];

        size_t maxAmount = 0;

        size_t maxAmountPartIndex = 0;

        for(size_t q = 0; q < 4; ++q)
        {
            char partName[256];
            fgets(partName, 256, fileToLoad);

            parts[q].name = partName;

            //FrontLeft
            parts[q].frontLeft = readWheelSuspension(fileToLoad);

            //FrontRight
            parts[q].frontRight = readWheelSuspension(fileToLoad);

            //BackLeft
            parts[q].backLeft = readWheelSuspension(fileToLoad);

            //BackRight
            parts[q].backRight = readWheelSuspension(fileToLoad);

            if(parts[q].frontLeft.size() > maxAmount)
            {
                maxAmount = parts[q].frontLeft.size();
                maxAmountPartIndex = q;
            }
        }

        suspensionData.push_back(parts[maxAmountPartIndex].frontLeft);
        suspensionData.push_back(parts[maxAmountPartIndex].frontRight);
        suspensionData.push_back(parts[maxAmountPartIndex].backLeft);
        suspensionData.push_back(parts[maxAmountPartIndex].backRight);

        fclose(fileToLoad);
    }
    else {assert(false && "No suspension file");}
}

std::vector<Ogre::Vector3> SUSLoader::readWheelSuspension(FILE * fileToLoad) const
{
    std::vector<Ogre::Vector3> res;

    size_t pointsAmount;
    fscanf(fileToLoad, "%d\n", &pointsAmount);
    for(size_t q = 0; q < pointsAmount; ++q)
    {
        Ogre::Vector3 point;
        fscanf(fileToLoad, "%f,%f,%f\n", &point.x, &point.y, &point.z);
        point.z = -point.z;
        res.push_back(point);
    }

    return res;
}