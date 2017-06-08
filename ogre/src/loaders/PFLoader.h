#ifndef PFLOADER_H
#define PFLOADER_H

#include <string>
#include <vector>
#include <set>

#include "Ogre.h"

#include "PFItem.h"

class PFLoader
{
public:

    PFLoader();

    /**
     * file - data.pf
     */
    bool init(const std::string& file);

    /**
     * opens packed file and offsets to proper location
     * should be closed externaly
    */
    Ogre::DataStreamPtr getFile(const std::string& relativeDir, const std::string& file) const;

    size_t getFileSize(const std::string& relativeDir, const std::string& file) const;

private:

#if !defined(__ANDROID__)
    typedef unsigned int DWORD;

    std::string mFileName;

    std::vector<PF::PackedFileItem> fileSystem;

    size_t findFile(const std::string& relativeDir, const std::string& file, size_t& fileSize) const;

    static std::string readString(const Ogre::DataStreamPtr& stream, DWORD& FilePos);
#endif
};

#endif