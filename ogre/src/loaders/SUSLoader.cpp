
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
}//anonymous namespace

void SUSLoader::load(const PFLoader& pfLoader, const std::string& subfolder, const std::string& filename, std::vector<std::vector<Ogre::Vector3> >& suspensionData) const
{
    Ogre::DataStreamPtr fileToLoad = pfLoader.getFile("data/cars/" + subfolder, filename);
    if(fileToLoad.get() && fileToLoad->isReadable())
    {
        suspensionData.clear();

        PartSuspension parts[5];

        size_t firstPartIndex = 0;

        for(size_t q = 0; q < 5; ++q)
        {
            char partName[256];
            char buf[1025];
            fileToLoad->readLine(buf, 1024);
            sscanf(buf, "%s\n", partName);

            parts[q].name = partName;

            //FrontLeft
            parts[q].frontLeft = readWheelSuspension(fileToLoad);

            //FrontRight
            parts[q].frontRight = readWheelSuspension(fileToLoad);

            //BackLeft
            parts[q].backLeft = readWheelSuspension(fileToLoad);

            //BackRight
            parts[q].backRight = readWheelSuspension(fileToLoad);

            if(parts[q].name[parts[q].name.length() - 1] == '0')
            {
                firstPartIndex = q;
            }
        }

        suspensionData.push_back(parts[firstPartIndex].frontLeft);
        suspensionData.push_back(parts[firstPartIndex].frontRight);
        suspensionData.push_back(parts[firstPartIndex].backLeft);
        suspensionData.push_back(parts[firstPartIndex].backRight);

        fileToLoad->close();
    }
    else {assert(false && "No suspension file");}
}

std::vector<Ogre::Vector3> SUSLoader::readWheelSuspension(const Ogre::DataStreamPtr& fileToLoad) const
{
    std::vector<Ogre::Vector3> res;

    size_t pointsAmount;
    char buf[1025];
    fileToLoad->readLine(buf, 1024);
    sscanf(buf, "%d\n", &pointsAmount);
    for(size_t q = 0; q < pointsAmount; ++q)
    {
        Ogre::Vector3 point;
        fileToLoad->readLine(buf, 1024);
        sscanf(buf, "%f,%f,%f\n", &point.x, &point.y, &point.z);
        point.z = -point.z;
        res.push_back(point);
    }

    return res;
}