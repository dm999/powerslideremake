#ifndef ORIGINALSETTINGS_H
#define ORIGINALSETTINGS_H

#include <string>
#include <map>

#include "loaders/PFLoader.h"

#define SI_SUPPORT_IOSTREAMS
#include "SimpleIni.h"

#include "Enums.h"

/**
 * Parser of str files
 */
class STRSettings
{
public:

    STRSettings() : mIsSTRLoaded(false) {}

    void parse(const PFLoader& pfLoaderStore, const std::string& relativeDir, const std::string& fileName);

    std::string getValue(const std::string& section, const std::string& key, const std::string& defaultVal = "") const;

    /**
     * extract value from string {a, b, c, d, ..., n}
     */
    std::vector<std::string> getArrayValue(const std::string& section, const std::string& key, bool& isFound) const;
    std::vector<std::string> getArrayValue(const std::string& section, const std::string& key) const;

    /**
     * extract value from string {a, b, c, d}
     */
    Ogre::Vector4 getArray4Value(const std::string& section, const std::string& key, bool& isFound) const;
    Ogre::Vector4 getArray4Value(const std::string& section, const std::string& key) const;

    /**
     * extract value from string {a, b, c}
     */
    Ogre::Vector3 getArray3Value(const std::string& section, const std::string& key, bool& isFound) const;
    Ogre::Vector3 getArray3Value(const std::string& section, const std::string& key) const;

    /**
     * extract value from string {a, b}
     */
    Ogre::Vector2 getArray2Value(const std::string& section, const std::string& key) const;

    float getFloatValue(const std::string& section, const std::string& key, bool& isFound) const;
    float getFloatValue(const std::string& section, const std::string& key) const;

    int getIntValue(const std::string& section, const std::string& key, bool& isFound) const;
    int getIntValue(const std::string& section, const std::string& key) const;
    int getIntValue(const std::string& section, const std::string& key, int defaultVal) const;


protected:

    CSimpleIniA mSTR;
    bool mIsSTRLoaded;
};

/**
 * Parser of original powerslide.str
 */
class STRPowerslide : public STRSettings
{
public:
    STRPowerslide(){}

    void parse(const PFLoader& pfLoaderStore);

    std::string getBaseCarFromCar(const std::string& car) const;
    std::string getCarFromCharacter(const std::string& character) const;
    std::vector<std::string> getCharactersByBaseCar(const std::string& car)const;
    std::string getTrackTitle(const std::string& track)const;
    std::string getCarTitle(const std::string& car)const;
    static std::string getCharacterTitle(const std::string& character);

    Ogre::Vector2 getFogStartEnd(const std::string& trackName) const;
    Ogre::ColourValue getTrackSkyColor(const std::string& trackName) const;
    Ogre::ColourValue getTrackAmbientColor(const std::string& trackName) const;
    Ogre::ColourValue getTrackTimeTrialColor(const std::string& trackName) const;
    Ogre::ColourValue getCharacterSpecularColor(const std::string& trackName, const std::string& characterName) const;
    std::string getExclusionFile(const std::string& trackName) const;
    std::string getBaseDir(const std::string& trackName) const;
    std::string getDataSubDir(const std::string& trackName) const;
    size_t getLapsCount(const std::string& trackName) const;
    Ogre::Vector3 getFinishCameraPos(const std::string& trackName) const;

private:

    Ogre::ColourValue parseColor(const std::string& val) const;
};

/**
 * Parser of original racecrud.str
 */
class STRRacecrud : public STRSettings
{
public:
    STRRacecrud(){}

    void parse(const PFLoader& pfLoaderStore);

private:
};

/**
 * Parser of original racetimes.str
 */
class STRRacetimes : public STRSettings
{
public:
    STRRacetimes(){}

    void parse(const PFLoader& pfLoaderStore);

private:
};

/**
 * 
 */
class STRPlayerSettings : public STRSettings
{
public:

    struct GlobalData
    {
        std::string playerName;
        std::string track;
        Ogre::uint32 numOpponents;
        std::string resolution;
        bool vsync;
        bool fullscreen;
        bool shadows;
        bool mirror;
        bool kmph;
        Ogre::uint32 transmission;
        Ogre::uint32 input;
    };

    struct PlayerData
    {
        PlayerData() : level(Easy) {}
        AIStrength level;
    };

    STRPlayerSettings() : mIsSaved(false), mFileName("player.txt"){}

    void parse(const std::string& dataDir);
    void save(const std::string& dataDir, const GlobalData& globalData, const PlayerData& playerData);

    bool getIsLoaded() const {return mIsSTRLoaded;}
    bool getIsSaved() const {return mIsSaved;}

private:

    void writeFile(const std::string& dataDir);

    const std::string mFileName;

    bool mIsSaved;
};

#endif
