#include "pcheader.h"

#include "OriginalSettings.h"

#include "loaders/STRLoader.h"

#include "Conversions.h"

void STRSettings::parse(const PFLoader& pfLoaderStore, const std::string& relativeDir, const std::string& fileName)
{
    STRLoader strLoader;
    std::string str = strLoader.load(pfLoaderStore, relativeDir, fileName);

    assert(!str.empty());

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

Ogre::Vector3 STRSettings::getArray3Value(const std::string& section, const std::string& key) const
{
    Ogre::Vector3 res = Ogre::Vector3::ZERO;

    if(mIsSTRLoaded)
    {
        std::string val = mSTR.GetValue(section.c_str(), key.c_str(), "");
        if(!val.empty())
        {
            sscanf(val.c_str(), "{%f,%f,%f}", &res.x, &res.y, &res.z);
        }
        else {assert(false && "STRSettings::getArray3Value: not found");}
    }

    return res;
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

OriginalSettings::OriginalSettings()
{
    mCustomToOriginalSections.insert(std::make_pair("deserttrack.de2", "desert track parameters"));
    mCustomToOriginalSections.insert(std::make_pair("speedway.de2", "speedway track parameters"));
    mCustomToOriginalSections.insert(std::make_pair("speedway_night.de2", "speedway night track parameters"));
    mCustomToOriginalSections.insert(std::make_pair("dam.de2", "dam parameters"));
    mCustomToOriginalSections.insert(std::make_pair("mf.de2", "mountain forest track parameters"));
    mCustomToOriginalSections.insert(std::make_pair("mineshaft.de2", "mineshaft parameters"));
    mCustomToOriginalSections.insert(std::make_pair("alpine.de2", "alpine track parameters"));
    mCustomToOriginalSections.insert(std::make_pair("citytrack.de2", "city track parameters"));
    mCustomToOriginalSections.insert(std::make_pair("nutopia.de2", "nutopia track parameters"));
    mCustomToOriginalSections.insert(std::make_pair("luge.de2", "luge track parameters"));
    mCustomToOriginalSections.insert(std::make_pair("stunt.de2", "stunt track parameters"));
    mCustomToOriginalSections.insert(std::make_pair("fnh.de2", "Foxnhound1 track parameters"));
    mCustomToOriginalSections.insert(std::make_pair("fnh2.de2", "Foxnhound2 track parameters"));
}

void OriginalSettings::parse(const PFLoader& pfLoaderStore)
{
    STRSettings::parse(pfLoaderStore, "", "powerslide.str");
}

Ogre::ColourValue OriginalSettings::getTrackSkyColor(const std::string& trackDE2FileName) const
{
    Ogre::ColourValue ret;

    if(mIsSTRLoaded)
    {
        std::map<std::string, std::string>::const_iterator found = mCustomToOriginalSections.find(trackDE2FileName);
        if(found != mCustomToOriginalSections.end())
        {
            std::string value = mSTR.GetValue((*found).second.c_str(), "sky colour", "");
            ret = parseColor(value);
        }
    }

    return ret;
}

Ogre::ColourValue OriginalSettings::getTrackAmbientColor(const std::string& trackDE2FileName) const
{
    Ogre::ColourValue ret(0.25f, 0.25f, 0.25f, 1.0f);//from game

    if(mIsSTRLoaded)
    {
        std::map<std::string, std::string>::const_iterator found = mCustomToOriginalSections.find(trackDE2FileName);
        if(found != mCustomToOriginalSections.end())
        {
            float val;

            std::string value = mSTR.GetValue((*found).second.c_str(), "ambient red", "");
            if(value != "")
            {
                Conversions::DMFromString(value, val);
                ret.r = val;
            }

            value = mSTR.GetValue((*found).second.c_str(), "ambient green", "");
            if(value != "")
            {
                Conversions::DMFromString(value, val);
                ret.g = val;
            }

            value = mSTR.GetValue((*found).second.c_str(), "ambient blue", "");
            if(value != "")
            {
                Conversions::DMFromString(value, val);
                ret.b = val;
            }
        }
    }

    return ret;
}

Ogre::ColourValue OriginalSettings::getCharacterSpecularColor(const std::string& trackDE2FileName, const std::string& characterName) const
{
    Ogre::ColourValue ret(0.5f, 0.5f, 0.5f, 1.0f);//from game

    //d.polubotko(TODO): to be implemented

    return ret;
}

Ogre::ColourValue OriginalSettings::parseColor(const std::string& val) const
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

std::string OriginalSettings::getExclusionFile(const std::string& trackDE2FileName) const
{
    std::string ret = "exclusion.txt";

    if(mIsSTRLoaded)
    {
        std::map<std::string, std::string>::const_iterator found = mCustomToOriginalSections.find(trackDE2FileName);
        if(found != mCustomToOriginalSections.end())
        {
            std::string value = mSTR.GetValue((*found).second.c_str(), "exclusion file", "");
            if(value != "") ret = value;
        }
    }

    return ret;
}

size_t OriginalSettings::getLapsCount(const std::string& trackDE2FileName) const
{
    size_t ret = 10;

    if(mIsSTRLoaded)
    {
        std::map<std::string, std::string>::const_iterator found = mCustomToOriginalSections.find(trackDE2FileName);
        if(found != mCustomToOriginalSections.end())
        {
            std::string value = mSTR.GetValue((*found).second.c_str(), "num laps", "");
            if(value != "") Conversions::DMFromString(value, ret);
        }
    }

    return ret;
}