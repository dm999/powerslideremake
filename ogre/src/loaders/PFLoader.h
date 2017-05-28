#ifndef PFLOADER_H
#define PFLOADER_H

#include <string>
#include <vector>
#include <set>

#include "Ogre.h"

namespace PF
{
    struct PackedFileItem
    {
        typedef unsigned int DWORD;

        std::string Name;
        DWORD Next;
        DWORD FileFolder;
        DWORD Offset;
        DWORD Length;

        //Folder
        PackedFileItem(const std::string& N,DWORD Num,DWORD FF)
        {
            Name=N;

            if(Name == ".")
            {
                Next = FF+1;
                FileFolder = FF;
            }
            else
            {
                Next = Num;
                FileFolder = FF;
            }
        }

        //File
        PackedFileItem(const std::string& N,DWORD Num,DWORD O,DWORD L)
        {
            Name = N;
            Next = Num;
            FileFolder = -1;
            Offset = O;
            Length = L;
        }
    };

}//PF namespace

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

    typedef unsigned int DWORD;

    std::string mFileName;

    std::vector<PF::PackedFileItem> fileSystem;

    size_t findFile(const std::string& relativeDir, const std::string& file, size_t& fileSize) const;

    static std::string readString(const Ogre::DataStreamPtr& stream, DWORD& FilePos);
};

#endif