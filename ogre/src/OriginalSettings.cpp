
#include "OriginalSettings.h"

#include "loaders/STRLoader.h"

#include "tools/Tools.h"
#include "tools/OgreTools.h"

void STRSettings::parse(const PFLoader& pfLoaderStore, const std::string& relativeDir, const std::string& fileName)
{
    STRLoader strLoader;
    std::string str = strLoader.load(pfLoaderStore, relativeDir, fileName);

    //assert(!str.empty());

    //https://github.com/brofield/simpleini
    mSTR.Reset();
    mIsSTRLoaded = false;

    SI_Error rc = mSTR.LoadData(str.c_str(), str.size());
    if (rc >= 0)
    {
        mIsSTRLoaded = true;
#if 0
        CSimpleIniA::TNamesDepend sections;
        ini.GetAllSections(sections);

        CSimpleIniA::TNamesDepend keys;
        ini.GetAllKeys("", keys);


        const char * pszValue = ini.GetValue("", "championship tracks", NULL /*default*/);
        //CSimpleIniA::TNamesDepend values;
        //ini.GetAllValues("", "championship tracks", values);

#endif
    }
}

std::string STRSettings::getValue(const std::string& section, const std::string& key, const std::string& defaultVal) const
{
    std::string res = defaultVal;

    if(mIsSTRLoaded)
    {
        res = mSTR.GetValue(section.c_str(), key.c_str(), defaultVal.c_str());
    }

    return res;
}

std::vector<std::string> STRSettings::getArrayValue(const std::string& section, const std::string& key, bool& isFound) const
{
    std::vector<std::string> res;
    isFound = false;

    if(mIsSTRLoaded)
    {
        std::string val = mSTR.GetValue(section.c_str(), key.c_str(), "");
        if(!val.empty() && val.length() > 2)
        {
            std::string::iterator begin = val.begin();
            std::string::iterator end = val.end();
            if(val[0] == '{') begin = val.begin() + 1;
            if(val[val.length() - 1] == '}') end = val.end() - 1;
            std::string buffer(begin, end);
            std::set<char> delims;
            delims.insert(',');
            res = Tools::splitpath(buffer, delims);
            isFound = true;
        }
        //else {assert(false && "STRSettings::getArrayValue: not found");}
    }

    return res;
}

std::vector<std::string> STRSettings::getArrayValue(const std::string& section, const std::string& key) const
{
    bool isFound;
    return getArrayValue(section, key, isFound);
}

Ogre::Vector4 STRSettings::getArray4Value(const std::string& section, const std::string& key, bool& isFound) const
{
    Ogre::Vector4 res = Ogre::Vector4::ZERO;
    isFound = false;

    if(mIsSTRLoaded)
    {
        std::string val = mSTR.GetValue(section.c_str(), key.c_str(), "");
        if(!val.empty())
        {
            sscanf(val.c_str(), "{%f,%f,%f,%f}", &res.x, &res.y, &res.z, &res.w);
            isFound = true;
        }
        //else {assert(false && "STRSettings::getArray4Value: not found");}
    }

    return res;
}

Ogre::Vector4 STRSettings::getArray4Value(const std::string& section, const std::string& key) const
{
    bool isFound;
    return getArray4Value(section, key, isFound);
}

Ogre::Vector3 STRSettings::getArray3Value(const std::string& section, const std::string& key, bool& isFound) const
{
    Ogre::Vector3 res = Ogre::Vector3::ZERO;
    isFound = false;

    if(mIsSTRLoaded)
    {
        std::string val = mSTR.GetValue(section.c_str(), key.c_str(), "");
        if(!val.empty())
        {
            sscanf(val.c_str(), "{%f,%f,%f}", &res.x, &res.y, &res.z);
            isFound = true;
        }
        //else {assert(false && "STRSettings::getArray3Value: not found");}
    }

    return res;
}

Ogre::Vector3 STRSettings::getArray3Value(const std::string& section, const std::string& key) const
{
    bool isFound;
    return getArray3Value(section, key, isFound);
}

Ogre::Vector2 STRSettings::getArray2Value(const std::string& section, const std::string& key) const
{
    Ogre::Vector2 res = Ogre::Vector2::ZERO;

    if(mIsSTRLoaded)
    {
        std::string val = mSTR.GetValue(section.c_str(), key.c_str(), "");
        if(!val.empty())
        {
            sscanf(val.c_str(), "{%f,%f}", &res.x, &res.y);
        }
        else {assert(false && "STRSettings::getArray2Value: not found");}
    }

    return res;
}

float STRSettings::getFloatValue(const std::string& section, const std::string& key, bool& isFound) const
{
    float res = 0.0f;
    isFound = false;

    if(mIsSTRLoaded)
    {
        std::string val = mSTR.GetValue(section.c_str(), key.c_str(), "");
        if(!val.empty())
        {
            isFound = Conversions::DMFromString(val, res);
        }
        //else {assert(false && "STRSettings::getFloatValue: not found");}
    }

    return res;
}

float STRSettings::getFloatValue(const std::string& section, const std::string& key) const
{
    bool isFound;
    return getFloatValue(section, key, isFound);
}

float STRSettings::getFloatValue(const std::string& section, const std::string& key, float defaultVal) const
{
    bool isFound;
    float ret = getFloatValue(section, key, isFound);
    if (!isFound)
    {
        ret = defaultVal;
    }

    return ret;
}

int STRSettings::getIntValue(const std::string& section, const std::string& key, bool& isFound) const
{
    int res = 0;
    isFound = false;

    if(mIsSTRLoaded)
    {
        std::string val = mSTR.GetValue(section.c_str(), key.c_str(), "");
        if(!val.empty())
        {
            isFound = Conversions::DMFromString(val, res);
        }
        //else {assert(false && "STRSettings::getIntValue: not found");}
    }

    return res;
}

int STRSettings::getIntValue(const std::string& section, const std::string& key) const
{
    bool isFound;
    return getIntValue(section, key, isFound);
}

int STRSettings::getIntValue(const std::string& section, const std::string& key, int defaultVal) const
{
    bool isFound;
    int ret =  getIntValue(section, key, isFound);
    if(!isFound)
    {
        ret = defaultVal;
    }

    return ret;
}

std::string STRSettings::arrayToString(const std::vector<std::string>& data)const
{
    std::string ret = "{";

    for(size_t q = 0; q < data.size(); ++q)
    {
        ret += data[q];
        if(q != data.size() - 1)
        {
            ret += ",";
        }
    }

    ret += "}";

    return ret;
}

void STRPowerslide::parse(const PFLoader& pfLoaderStore)
{
    STRSettings::parse(pfLoaderStore, "", "powerslide.str");
}

std::string STRPowerslide::getBaseCarFromCar(const std::string& car)const
{
    std::string res = car;

    std::string baseCar = getValue(car + " parameters", "base car", "unknown");
    if(baseCar != "unknown")
    {
        res = baseCar;
    }

    return res;
}

std::string STRPowerslide::getCarFromCharacter(const std::string& character)const
{
    std::string res = getValue(character + " parameters", "car", "feral max");

    return res;
}

std::vector<std::string> STRPowerslide::getCharactersByBaseCar(const std::string& car)const
{
    std::vector<std::string> res;

    std::vector<std::string> availChars = getArrayValue("", "available characters");

    for(size_t q = 0; q < availChars.size(); ++q)
    {
        std::string baseCar = getBaseCarFromCar(getCarFromCharacter(availChars[q]));
        if(baseCar == car)
            res.push_back(availChars[q]);
    }

    return res;
}

std::string STRPowerslide::getTrackTitle(const std::string& track)const
{
    std::string res = getValue(track + " parameters", "english", track);

    return res;
}

std::string STRPowerslide::getCarTitle(const std::string& car)
{
    static std::map<std::string, std::string> originalToTitle;
    if(originalToTitle.empty())
    {
        originalToTitle["feral max"] = "Warthog";
        originalToTitle["mustang"] = "Big Heaver";
        originalToTitle["pickup"] = "Pickup";
        originalToTitle["exotic"] = "Sabre";
        originalToTitle["orc"] = "Orc";
        originalToTitle["skeeto"] = "Skeeto";
        originalToTitle["supercar"] = "Supercar";
    }

    std::string res = originalToTitle[car];

    return res;
}

std::string STRPowerslide::getCharacterTitle(const std::string& character)
{
    static std::map<std::string, std::string> originalToTitle;

    if(originalToTitle.empty())
    {
        originalToTitle["frantic"] = "Fran Tick";
        originalToTitle["cyber"] = "Cyberasta";
        originalToTitle["radiation"] = "Radiation Boy";
        originalToTitle["zig"] = "Blade";
        originalToTitle["zag"] = "Chain";
        originalToTitle["beryl"] = "Feryl";
        originalToTitle["stig"] = "Stig Mata";

        originalToTitle["dodgy"] = "Dodgy Dave";
        originalToTitle["pepper"] = "Boris";
        originalToTitle["colonel"] = "Colonel Slanders";
        originalToTitle["furnace"] = "Furnace Chan";
        originalToTitle["pablo"] = "Pablo Martinez";
        originalToTitle["dwayne"] = "Dwayne Schmitt";
        originalToTitle["massie"] = "Chopper";

        originalToTitle["jean"] = "Jean-Marc Laplace";
        originalToTitle["hemp"] = "Hemp Girl";
        originalToTitle["hardy"] = "Hardy Buckmeister";
        originalToTitle["bill"] = "William Grates";
        originalToTitle["upjohn"] = "Upjohn Worthington";
        originalToTitle["bendito"] = "Bendito Mescalini";

        originalToTitle["marl"] = "Mal Burro";
        originalToTitle["yukio"] = "Yukio";
        originalToTitle["bimbeau"] = "Bimbeau Escargeaugeau";
        originalToTitle["miki"] = "Miki Yamamoto";
        originalToTitle["rosie"] = "Rosie Crippler";
        originalToTitle["cinnamon"] = "Cinnamon Conrad";

        originalToTitle["crayz"] = "CrayZ 1000";
        originalToTitle["mad"] = "Mad Vlad Alexovich";
        originalToTitle["max"] = "Max Alexovich";

        originalToTitle["driverx"] = "Driver X";
        originalToTitle["chuck"] = "Chuck Schmitt";
        originalToTitle["punch"] = "Punch";

        originalToTitle["mach"] = "Mach";
        originalToTitle["armadillo"] = "Armadillo";
        originalToTitle["alter"] = "Alter";
        originalToTitle["komodo"] = "Komodo";
        originalToTitle["panther"] = "Panther";
        originalToTitle["volt"] = "Volt";
        originalToTitle["razer"] = "Razer";
        originalToTitle["argon"] = "Argon";
    }


    std::string res = originalToTitle[character];

    return res;
}

std::string STRPowerslide::getCharacterFolderForBio(const std::string& character)
{
    static std::map<std::string, std::string> originalToTitle;

    if(originalToTitle.empty())
    {
        originalToTitle["frantic"] = "twitch";
        originalToTitle["cyber"] = "rasta!";
        originalToTitle["radiation"] = "the kid";
        originalToTitle["zig"] = "blade";
        originalToTitle["zag"] = "chain";
        originalToTitle["beryl"] = "feryl";
        originalToTitle["stig"] = "stig";

        originalToTitle["dodgy"] = "dodgy";
        originalToTitle["pepper"] = "boris";
        originalToTitle["colonel"] = "slur";
        originalToTitle["furnace"] = "burner";
        originalToTitle["pablo"] = "diablo";
        originalToTitle["dwayne"] = "gimp";
        originalToTitle["massie"] = "chopper";

        originalToTitle["jean"] = "skidkid";
        originalToTitle["hemp"] = "roach";
        originalToTitle["hardy"] = "hardbuck";
        originalToTitle["bill"] = "bill";
        originalToTitle["upjohn"] = "upjohn";
        originalToTitle["bendito"] = "trip";

        originalToTitle["marl"] = "snort";
        originalToTitle["yukio"] = "yukio";
        originalToTitle["bimbeau"] = "bimbeau";
        originalToTitle["miki"] = "buggirl";
        originalToTitle["rosie"] = "tootsie";
        originalToTitle["cinnamon"] = "spicey";

        originalToTitle["crayz"] = "crayz";
        originalToTitle["mad"] = "mad vlad";
        originalToTitle["max"] = "the axe";

        originalToTitle["driverx"] = "x";
        originalToTitle["chuck"] = "schmuck";
        originalToTitle["punch"] = "punch";

        originalToTitle["mach"] = "";
        originalToTitle["armadillo"] = "";
        originalToTitle["alter"] = "";
        originalToTitle["komodo"] = "";
        originalToTitle["panther"] = "";
        originalToTitle["volt"] = "";
        originalToTitle["razer"] = "";
        originalToTitle["argon"] = "";
    }


    std::string res = originalToTitle[character];

    return res;
}

Ogre::Vector2 STRPowerslide::getFogStartEnd(const std::string& trackName) const
{
    Ogre::Vector2 ret;

    if(mIsSTRLoaded)
    {
        ret.x = getFloatValue(std::string(trackName + " parameters").c_str(), "fog start");
        ret.y = getFloatValue(std::string(trackName + " parameters").c_str(), "fog stop");
#if 0
        if(ret.x < 1000000.0f)
        {
            ret.x = 20.0f;
            ret.y = 100.0f;
        }
#endif
    }

    return ret;
}

Ogre::ColourValue STRPowerslide::getTrackSkyColor(const std::string& trackName) const
{
    Ogre::ColourValue ret;

    if(mIsSTRLoaded)
    {
        std::string value = mSTR.GetValue(std::string(trackName + " parameters").c_str(), "sky colour", "");
        ret = parseColor(value);
    }

    return ret;
}

Ogre::ColourValue STRPowerslide::getTrackAmbientColor(const std::string& trackName) const
{
    Ogre::ColourValue ret(0.25f, 0.25f, 0.25f, 1.0f);//from game

    if(mIsSTRLoaded)
    {
        float val;

        std::string value = mSTR.GetValue(std::string(trackName + " parameters").c_str(), "ambient red", "");
        if(value != "")
        {
            Conversions::DMFromString(value, val);
            ret.r = val;
        }

        value = mSTR.GetValue(std::string(trackName + " parameters").c_str(), "ambient green", "");
        if(value != "")
        {
            Conversions::DMFromString(value, val);
            ret.g = val;
        }

        value = mSTR.GetValue(std::string(trackName + " parameters").c_str(), "ambient blue", "");
        if(value != "")
        {
            Conversions::DMFromString(value, val);
            ret.b = val;
        }
    }

    return ret;
}

Ogre::ColourValue STRPowerslide::getTrackTimeTrialColor(const std::string& trackName) const
{
    Ogre::ColourValue ret(0.0f, 0.0f, 0.0f, 1.0f);//from game

    if(mIsSTRLoaded)
    {
        std::string value = mSTR.GetValue(std::string(trackName + " parameters").c_str(), "timetrial lap colour", "");
        if(!value.empty())
            ret = parseColor(value);
    }

    return ret;
}

Ogre::ColourValue STRPowerslide::getCharacterSpecularColor(const std::string& trackName, const std::string& characterName) const
{
    Ogre::ColourValue ret(0.5f, 0.5f, 0.5f, 1.0f);//from game

    //d.polubotko(TODO): to be implemented

    return ret;
}

Ogre::ColourValue STRPowerslide::parseColor(const std::string& val) const
{
    Ogre::ColourValue ret;

    if(val != "")
    {
        int r,g,b;
        sscanf(val.c_str(), "{%d,%d,%d}", &r, &g, &b);
        ret.r = r / 255.0f;
        ret.g = g / 255.0f;
        ret.b = b / 255.0f;
        ret.a = 1.0f;
    }

    return ret;
}

std::string STRPowerslide::getExclusionFile(const std::string& trackName) const
{
    std::string ret = "exclusion.txt";

    if(mIsSTRLoaded)
    {
        std::string value = mSTR.GetValue(std::string(trackName + " parameters").c_str(), "exclusion file", "");
        if(value != "") ret = value;
    }

    return ret;
}

std::string STRPowerslide::getBaseDir(const std::string& trackName) const
{
    std::string ret = "";

    if(mIsSTRLoaded)
    {
        std::string value = mSTR.GetValue(std::string(trackName + " parameters").c_str(), "base directory", "");
        if(value != "")
        {
            std::set<char> delim;
            delim.insert('\\');
            std::vector<std::string> pathComponents = Tools::splitpath(value, delim, false);
            ret = pathComponents[pathComponents.size() - 1];
        }
    }

    return ret;
}

std::string STRPowerslide::getDataSubDir(const std::string& trackName) const
{
    std::string ret = "";

    if(mIsSTRLoaded)
    {
        std::string value = mSTR.GetValue(std::string(trackName + " parameters").c_str(), "data subdirectory", "");
        if(value != "")
        {
            std::set<char> delim;
            delim.insert('\\');
            std::vector<std::string> pathComponents = Tools::splitpath(value, delim, false);
            ret = pathComponents[pathComponents.size() - 1];
        }
    }

    return ret;
}

size_t STRPowerslide::getLapsCount(const std::string& trackName) const
{
    size_t ret = 10;

    if(mIsSTRLoaded)
    {
        std::string value = mSTR.GetValue(std::string(trackName + " parameters").c_str(), "num laps", "");
        if(value != "") Conversions::DMFromString(value, ret);
    }

    return ret;
}

size_t STRPowerslide::getCDTrack(const std::string& trackName) const
{
    size_t ret = 0;

    if(mIsSTRLoaded)
    {
        std::string value = mSTR.GetValue(std::string(trackName + " parameters").c_str(), "cd track", "");
        if(value != "") Conversions::DMFromString(value, ret);
    }

    return ret;
}

Ogre::Vector3 STRPowerslide::getFinishCameraPos(const std::string& trackName) const
{
    Ogre::Vector3 ret(0.0f, 0.0f, 0.0f);

    if(mIsSTRLoaded)
    {
        ret = getArray3Value(trackName + " parameters", "finish camera pos");
        ret.z = -ret.z;
    }

    return ret;
}

void STRRacecrud::parse(const PFLoader& pfLoaderStore)
{
    STRSettings::parse(pfLoaderStore, "data/misc", "racecrud.str");
}

void STRRacetimes::parse(const PFLoader& pfLoaderStore)
{
    STRSettings::parse(pfLoaderStore, "data/misc", "racetimes.str");
}

std::string WritableSettings::parse(const std::string& dataDir)
{
    std::string ret = "";

    Ogre::DataStreamPtr stream = getReadableFile(dataDir, mFileName);

    if(stream.get() && stream->isReadable())
    {
        size_t size = stream->size();

        if(size > 0)
        {
            std::vector<char> buf(size);

            stream->read(&buf[0],size);

            ret = STRLoader().decode(buf);
        }
    }

    if(stream.get())
    {
        stream->close();
    }
    else
    {
        Ogre::LogManager::getSingleton().logMessage(Ogre::LML_CRITICAL, "[WritableSettings::parse]: Error - no stream available");
    }

    return ret;
}

void WritableSettings::writeFile(const std::string& dataDir, const std::string& str)
{
    mIsSaved = false;

    std::string strDecoded = STRLoader().decode(std::vector<char>(str.begin(), str.end()));

    Ogre::DataStreamPtr stream = getWritibleFile(dataDir, mFileName);

    if(stream.get() && stream->isWriteable())
    {
        stream->write(strDecoded.c_str(), strDecoded.length());
        mIsSaved = true;
    }

    if(stream.get())
    {
        stream->close();
    }
    else
    {
        Ogre::LogManager::getSingleton().logMessage(Ogre::LML_CRITICAL, "[WritableSettings::writeFile]: Error - no stream available");
    }
}

void STRHiscores::parse(const PFLoader& pfLoaderStore, const std::string& dataDir)
{
    STRSettings::parse(pfLoaderStore, "data/gamestructure", getFileName());

    std::string data = WritableSettings::parse(dataDir);
    if(!data.empty())
    {
        mSTR.Reset();
        SI_Error rc = mSTR.LoadData(data.c_str(), data.size());
        if (rc >= 0)
        {
            mIsSTRLoaded = true;
        }
    }
}

void STRHiscores::save(const std::string& dataDir)
{
    std::string str;

    SI_Error rc = mSTR.Save(str);

    if (rc >= 0)
        writeFile(dataDir, str);
}

bool STRHiscores::updateTrackTime(const std::string& trackName, const std::string& character, const std::string& playerName, Ogre::Real timeNew)
{
    bool isBestBeaten = false;

    if(mIsSTRLoaded)
    {
        const std::string sectionName = trackName + " parameters";

        size_t entries = getIntValue(sectionName, "Number of Entries");
        std::vector<std::string> names = getArrayValue(sectionName, "names");
        std::vector<std::string> times = getArrayValue(sectionName, "lap times");
        std::vector<std::string> characters = getArrayValue(sectionName, "characters");
        std::vector<std::string> dimCoeff = getArrayValue(sectionName, "4th dimension coefficient");

        bool isUpdated = false;
        for(size_t q = 0; q < entries; ++q)
        {
            float time;
            Conversions::DMFromString(times[q], time);

            if(time > timeNew)
            {
                if(q == 0) isBestBeaten = true;

                for(size_t w = entries - 1; w > q; --w)
                {
                    times[w] = times[w - 1];
                    names[w] = names[w - 1];
                    characters[w] = characters[w - 1];
                    dimCoeff[w] = dimCoeff[w - 1];
                }

                isUpdated = true;
                times[q] = Conversions::DMToString(timeNew);
                names[q] = playerName;
                characters[q] = character;
                dimCoeff[q] = "1";//d.polubotko: unclear value, 0 assumed to be untuched
                break;
            }
        }

        if(isUpdated)
        {
            std::string plainNames = arrayToString(names);
            std::string plainTimes = arrayToString(times);
            std::string plainChars = arrayToString(characters);
            std::string plainDimCoeffs = arrayToString(dimCoeff);

            mSTR.SetValue(sectionName.c_str(), "names", plainNames.c_str());
            mSTR.SetValue(sectionName.c_str(), "lap times", plainTimes.c_str());
            mSTR.SetValue(sectionName.c_str(), "characters", plainChars.c_str());
            mSTR.SetValue(sectionName.c_str(), "4th dimension coefficient", plainDimCoeffs.c_str());
        }
    }

    return isBestBeaten;
}

void STRHiscores::remove(const std::string& trackName, size_t removeIndex)
{
    if (mIsSTRLoaded)
    {
        const std::string sectionName = trackName + " parameters";

        size_t entries = getIntValue(sectionName, "Number of Entries");

        std::vector<std::string> names = getArrayValue(sectionName, "names");
        std::vector<std::string> times = getArrayValue(sectionName, "lap times");
        std::vector<std::string> characters = getArrayValue(sectionName, "characters");
        std::vector<std::string> dimCoeff = getArrayValue(sectionName, "4th dimension coefficient");

        if (removeIndex < names.size())
        {
            names.erase(names.begin() + removeIndex);
            times.erase(times.begin() + removeIndex);
            characters.erase(characters.begin() + removeIndex);
            dimCoeff.erase(dimCoeff.begin() + removeIndex);

            names.push_back(names[names.size() - 1]);
            times.push_back(times[times.size() - 1]);
            characters.push_back(characters[characters.size() - 1]);
            dimCoeff.push_back(dimCoeff[dimCoeff.size() - 1]);

            std::string plainNames = arrayToString(names);
            std::string plainTimes = arrayToString(times);
            std::string plainChars = arrayToString(characters);
            std::string plainDimCoeffs = arrayToString(dimCoeff);

            mSTR.SetValue(sectionName.c_str(), "names", plainNames.c_str());
            mSTR.SetValue(sectionName.c_str(), "lap times", plainTimes.c_str());
            mSTR.SetValue(sectionName.c_str(), "characters", plainChars.c_str());
            mSTR.SetValue(sectionName.c_str(), "4th dimension coefficient", plainDimCoeffs.c_str());
        }
    }
}

void STRPlayerSettings::parse(const std::string& dataDir)
{
    mIsSTRLoaded = false;

    std::string data = WritableSettings::parse(dataDir);

    if(!data.empty())
    {
        mSTR.Reset();

        SI_Error rc = mSTR.LoadData(data.c_str(), data.size());

        if (rc >= 0)
        {
            mIsSTRLoaded = true;
        }
    }
}

void STRPlayerSettings::save(const std::string& dataDir, const GlobalData& globalData, const PlayerData& playerData)
{
    mSTR.SetValue("", "player name", globalData.playerName.c_str());
    mSTR.SetValue("", "track choice", globalData.track.c_str());
    mSTR.SetValue("", "character choice", globalData.character.c_str());
    mSTR.SetValue("", "num opponents", Conversions::DMToString(globalData.numOpponents).c_str());
    mSTR.SetValue("", "resolution", globalData.resolution.c_str());
    mSTR.SetValue("", "vsync", Conversions::DMToString(globalData.vsync).c_str());
    mSTR.SetValue("", "fullscreen", Conversions::DMToString(globalData.fullscreen).c_str());
    mSTR.SetValue("", "shadows", Conversions::DMToString(globalData.shadows).c_str());
    mSTR.SetValue("", "mirror", Conversions::DMToString(globalData.mirror).c_str());
    mSTR.SetValue("", "ghost", Conversions::DMToString(globalData.ghost).c_str());
    mSTR.SetValue("", "adv lighting player", Conversions::DMToString(globalData.adv_lightinig_player).c_str());
    mSTR.SetValue("", "attenuation player", Conversions::DMToString(globalData.attenuation_player).c_str());
    mSTR.SetValue("", "adv lighting ai", Conversions::DMToString(globalData.adv_lightinig_ai).c_str());
    mSTR.SetValue("", "gamma", Conversions::DMToString(globalData.gamma, 3, 1).c_str());
    mSTR.SetValue("", "bicubic upscale", Conversions::DMToString(globalData.bicubic_upscale).c_str());
    mSTR.SetValue("", "speedo", Conversions::DMToString(globalData.kmph).c_str());
    mSTR.SetValue("", "transmission", Conversions::DMToString(globalData.transmission).c_str());
    mSTR.SetValue("", "input", Conversions::DMToString(globalData.input).c_str());
    mSTR.SetValue("", "camera setting", Conversions::DMToString(globalData.cameraPos).c_str());
    mSTR.SetValue("", "sound fx volume", Conversions::DMToString(static_cast<size_t>(globalData.fxVolume * 9.0f)).c_str());
    mSTR.SetValue("", "sound cd volume", Conversions::DMToString(static_cast<size_t>(globalData.musicVolume * 9.0f)).c_str());
    mSTR.SetValue("", "key mappers", arrayToString(globalData.keyMappers).c_str());
    mSTR.SetValue("", "key mappers 2", arrayToString(globalData.keyMappers2).c_str());
    mSTR.SetValue("", "mouse mappers", arrayToString(globalData.mouseMappers).c_str());

    const std::string section = globalData.playerName + " parameters";

    mSTR.SetValue(section.c_str(), "Level", Conversions::DMToString(playerData.level).c_str());

    {
        std::vector<std::string> fruits;
        for(size_t q = 0; q < playerData.fruit.size(); ++q)
        {
            fruits.push_back(Conversions::DMToString(playerData.fruit[q]));
        }
        mSTR.SetValue(section.c_str(), "fruit", arrayToString(fruits).c_str());
    }

    std::string str;

    SI_Error rc = mSTR.Save(str);

    if (rc >= 0)
        writeFile(dataDir, str);
}

