
#include "SectionsFile.h"

#include "OgreTools.h"

void SectionsFile::load(const std::string& dataDir, const std::string& fileName)
{
    Ogre::DataStreamPtr stream = getReadableFile(dataDir, fileName);

    if(stream.get() && stream->isReadable())
    {
        Ogre::uint32 sectionsAmount;
        stream->read(&sectionsAmount, sizeof(Ogre::uint32));

        for(Ogre::uint32 q = 0; q < sectionsAmount; ++q)
        {
            std::string sectionName = readString(stream);

            Ogre::Real bestTime;
            stream->read(&bestTime, sizeof(Ogre::Real));

            Ogre::uint32 snapshotsAmount;
            stream->read(&snapshotsAmount, sizeof(Ogre::uint32));

            TrialGhost::GhostData snapshots(snapshotsAmount);
            for(std::size_t w = 0; w < snapshots.size(); ++w)
            {
                stream->read(&snapshots[w], sizeof(std::pair<Ogre::Real, GhostPos>));
            }
            

            mSectionNames.push_back(sectionName);
            mBestTimes.push_back(bestTime);
            mSectionData.push_back(snapshots);
        }
    }

    if(stream.get())
    {
        stream->close();
    }
}

void SectionsFile::save(const std::string& dataDir, const std::string& fileName) const
{
    Ogre::DataStreamPtr stream = getWritibleFile(dataDir, fileName);

    if(stream.get() && stream->isWriteable())
    {
        Ogre::uint32 sectionsAmount = mSectionNames.size();
        stream->write(&sectionsAmount, sizeof(Ogre::uint32));

        for(std::size_t q = 0; q < mSectionNames.size(); ++q)
        {
            writeString(stream, mSectionNames[q]);

            stream->write(&mBestTimes[q], sizeof(Ogre::Real));

            Ogre::uint32 snapshotsAmount = mSectionData[q].size();
            stream->write(&snapshotsAmount, sizeof(Ogre::uint32));

            for(std::size_t w = 0; w < mSectionData[q].size(); ++w)
            {
                stream->write(&mSectionData[q][w], sizeof(std::pair<Ogre::Real, GhostPos>));
            }
        }
    }

    if(stream.get())
    {
        stream->close();
    }
}

void SectionsFile::updateSection(const std::string& sectionName, Ogre::Real bestTime, const TrialGhost::GhostData& data)
{
    std::vector<std::string>::const_iterator i = std::find(mSectionNames.begin(), mSectionNames.end(), sectionName);
    if(i != mSectionNames.end())
    {
        size_t index = i - mSectionNames.begin();
        mSectionNames[index] = sectionName;
        mBestTimes[index] = bestTime;
        mSectionData[index] = data;
    }
    else
    {
        mSectionNames.push_back(sectionName);
        mBestTimes.push_back(bestTime);
        mSectionData.push_back(data);
    }
}

bool SectionsFile::isSectionExists(const std::string& sectionName) const
{
    bool ret = false;
    std::vector<std::string>::const_iterator i = std::find(mSectionNames.begin(), mSectionNames.end(), sectionName);
    if(i != mSectionNames.end())
    {
        ret = true;
    }

    return ret;
}

Ogre::Real SectionsFile::getSectionTime(const std::string& sectionName)const
{
    std::vector<std::string>::const_iterator i = std::find(mSectionNames.begin(), mSectionNames.end(), sectionName);

    assert(i != mSectionNames.end());

    size_t index = i - mSectionNames.begin();

    return mBestTimes[index];
}

const TrialGhost::GhostData& SectionsFile::getSectionData(const std::string& sectionName)const
{
    std::vector<std::string>::const_iterator i = std::find(mSectionNames.begin(), mSectionNames.end(), sectionName);

    assert(i != mSectionNames.end());

    size_t index = i - mSectionNames.begin();

    return mSectionData[index];
}

std::string SectionsFile::readString(Ogre::DataStreamPtr stream) const
{
    Ogre::uint32 length;
    stream->read(&length, sizeof(Ogre::uint32));

    std::vector<char> buf(length);
    stream->read(&buf[0], length);

    std::string ret(buf.begin(), buf.end());

    return ret;
}

void SectionsFile::writeString(Ogre::DataStreamPtr stream, const std::string& str) const
{
    Ogre::uint32 length = str.length();
    stream->write(&length, sizeof(Ogre::uint32));
    stream->write(str.c_str(), length);
}