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

    std::string arrayToString(std::vector<std::string>& data)const;

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
    static std::string getCharacterFolderForBio(const std::string& character);

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


class WritableSettings
{
public:

    WritableSettings(const std::string& fileName) : mFileName(fileName), mIsSaved(false){}

    bool getIsSaved() const {return mIsSaved;}

protected:

    std::string parse(const std::string& dataDir);

    void writeFile(const std::string& dataDir, const std::string& str);

    const std::string& getFileName() const {return mFileName;}

private:

    const std::string mFileName;

    bool mIsSaved;
};

/**
 * Parser of original hiscores.str
 */
class STRHiscores : public STRSettings, public WritableSettings
{
public:
    STRHiscores() : WritableSettings("hiscores.str"){}

    void parse(const PFLoader& pfLoaderStore, const std::string& dataDir);

    void save(const std::string& dataDir);

    /**
     * return - is best time beaten
     */
    bool updateTrackTime(const std::string& trackName, const std::string& character, const std::string& playerName, Ogre::Real timeNew);
};

/**
 * 
 */
class STRPlayerSettings : public STRSettings, public WritableSettings
{
public:

    struct GlobalData
    {
        std::string playerName;
        std::string track;
        std::string character;
        Ogre::uint32 numOpponents;
        std::string resolution;
        bool vsync;
        bool fullscreen;
        bool shadows;
        bool mirror;
        bool kmph;
        Ogre::uint32 transmission;
        Ogre::uint32 input;
        Ogre::uint32 cameraPos;
        Ogre::Real fxVolume;
    };

    struct PlayerData
    {
        PlayerData() : level(Easy) {}
        AIStrength level;
    };

    STRPlayerSettings() : WritableSettings("player.str"){}

    void parse(const std::string& dataDir);
    void save(const std::string& dataDir, const GlobalData& globalData, const PlayerData& playerData);

    bool getIsLoaded() const {return mIsSTRLoaded;}

private:

};

#endif
