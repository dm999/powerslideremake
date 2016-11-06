#ifndef ORIGINALSETTINGS_H
#define ORIGINALSETTINGS_H

#include <string>
#include <map>

#include "loaders/PFLoader.h"

#define SI_SUPPORT_IOSTREAMS
#include "SimpleIni.h"

/**
 * Parser of str files
 */
class STRSettings
{
public:

    void parse(const PFLoader& pfLoaderStore, const std::string& relativeDir, const std::string& fileName);

    std::string getValue(const std::string& section, const std::string& key, const std::string& defaultVal = "") const;

    /**
     * extract value from string {a, b, c, d}
     */
    Ogre::Vector4 getArray4Value(const std::string& section, const std::string& key, bool& isFound) const;

    /**
     * extract value from string {a, b, c}
     */
    Ogre::Vector3 getArray3Value(const std::string& section, const std::string& key) const;

    /**
     * extract value from string {a, b}
     */
    Ogre::Vector2 getArray2Value(const std::string& section, const std::string& key) const;

    float getFloatValue(const std::string& section, const std::string& key, bool& isFound) const;


protected:

    CSimpleIniA mSTR;
    bool mIsSTRLoaded;
};

/**
 * Parser of original Powerslide.str
 */
class OriginalSettings : public STRSettings
{
public:
    OriginalSettings();

    void parse(const PFLoader& pfLoaderStore);

    Ogre::ColourValue getTrackSkyColor(const std::string& trackDE2FileName) const;
    Ogre::ColourValue getTrackAmbientColor(const std::string& trackDE2FileName) const;
    Ogre::ColourValue getCharacterSpecularColor(const std::string& trackDE2FileName, const std::string& characterName) const;
    std::string getExclusionFile(const std::string& trackDE2FileName) const;
    size_t getLapsCount(const std::string& trackDE2FileName) const;

private:

    std::map<std::string, std::string> mCustomToOriginalSections;

    Ogre::ColourValue parseColor(const std::string& val) const;
};

#endif
