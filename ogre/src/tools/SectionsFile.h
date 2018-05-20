#ifndef SECTIONSFILE_H
#define SECTIONSFILE_H

#include <vector>
#include <string>

#include "../includes/OgreInclude.h"

#include "../gamelogic/TrialGhost.h"

class SectionsFile
{
public:

    void load(const std::string& dataDir, const std::string& fileName);
    void save(const std::string& dataDir, const std::string& fileName) const;
    void updateSection(const std::string& sectionName, Ogre::Real bestTime, const TrialGhost::GhostData& data, const std::string& characterName);
    bool isSectionExists(const std::string& sectionName) const;
    Ogre::Real getSectionTime(const std::string& sectionName)const;
    const TrialGhost::GhostData& getSectionData(const std::string& sectionName)const;
    std::string getSectionCharacter(const std::string& sectionName)const;

private:

    std::string readString(Ogre::DataStreamPtr stream) const;
    void writeString(Ogre::DataStreamPtr stream, const std::string& str) const;


    std::vector<std::string> mSectionNames;
    std::vector<Ogre::Real> mBestTimes;
    std::vector<TrialGhost::GhostData> mSectionData;
    std::vector<std::string> mCharacterNames;
};

#endif
