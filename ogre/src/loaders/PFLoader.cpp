#include "../pcheader.h"

#include <set>

#include "PFLoader.h"

#include "../tools/Tools.h"

PFLoader::PFLoader()
    : mFileName("data.pf")
{ }

bool PFLoader::init(const std::string& file)
{
    bool res = false;

    mFileName = file;

    Ogre::DataStreamPtr stream = Ogre::ResourceGroupManager::getSingleton().openResource( mFileName.c_str(), "PF" );

    if(stream.get() && stream->isReadable())
    {

        fileSystem.clear();

        //read file system
        {
            res = true;

            DWORD From;
            DWORD ElemCount;
            size_t Size;

            Size = stream->size();
            stream->seek(Size - 4);
            stream->read(&From,4);
            stream->seek(From);
            stream->read(&ElemCount,4);

            DWORD FilePos=From+4;

            

            while(1)
            {
                if(FilePos>=Size-4) break;

                DWORD Next,F_F,Offset,Length,Smthing;

                std::string itemName = readString(stream, FilePos);

                stream->read(&Next,4);
                stream->read(&F_F,4);
                FilePos+=8;
                if(F_F==0xffffffff)//file
                {
                    FilePos+=10;
                    stream->read(&Offset,4);
                    stream->read(&Length,4);
                    stream->read(&Smthing,2);
                    fileSystem.push_back(PF::PackedFileItem(itemName,Next,Offset,Length));
                }
                else                // folder
                {
                    fileSystem.push_back(PF::PackedFileItem(itemName,Next,F_F));
                }
            }

        }

        stream->close();
    }

    return res;
}

Ogre::DataStreamPtr PFLoader::getFile(const std::string& relativeDir, const std::string& file) const
{
    Ogre::DataStreamPtr ret;

    if(!fileSystem.empty())
    {
        ret = Ogre::ResourceGroupManager::getSingleton().openResource( mFileName.c_str(), "PF" );
        if(ret.get() && ret->isReadable())
        {
            //find file offset
            size_t fileSize;
            size_t offset = findFile(relativeDir, file, fileSize);
            if(offset != 0)
            {
                ret->seek(offset);
            }
            else
            {
                ret->close();
                ret = Ogre::DataStreamPtr();
            }
        }
    }

    return ret;
}

size_t PFLoader::getFileSize(const std::string& relativeDir, const std::string& file) const
{
    size_t ret = 0;

    if(!fileSystem.empty())
    {
        //find file size
        size_t fileSize;
        size_t offset = findFile(relativeDir, file, fileSize);
        if(offset != 0)
        {
            ret = fileSize;
        }
    }

    return ret;
}

size_t PFLoader::findFile(const std::string& relativeDir, const std::string& file, size_t& fileSize) const
{
    size_t res = 0;
    fileSize = 0;

    std::set<char> delims;
    delims.insert('\\');
    delims.insert('/');

    std::vector<std::string> vPath = Tools::splitpath(relativeDir, delims);
    
    if(relativeDir == "") vPath.clear();

    if(fileSystem[0].Name == ".")
    {
        DWORD next = fileSystem[0].Next;

        //folder
        for(size_t q = 0; q < vPath.size(); ++q)
        {
            while(next < fileSystem.size())
            {
                if(fileSystem[next].Name == vPath[q])
                {
                    next = fileSystem[next].FileFolder;
                    break;
                }
                else
                {
                    next = fileSystem[next].Next;
                }
            }
        }

        //file
        if(next < fileSystem.size())
        {
            while(next < fileSystem.size())
            {
                if(fileSystem[next].FileFolder != 0xFFFFFFFF || fileSystem[next].Name != file)
                {
                    next = fileSystem[next].Next;
                }
                else break;
            }

            if(next < fileSystem.size() && fileSystem[next].FileFolder == 0xFFFFFFFF && fileSystem[next].Name == file)
            {
                res = fileSystem[next].Offset;
                fileSize = fileSystem[next].Length;
            }
        }
    }

    return res;
}

std::string PFLoader::readString(const Ogre::DataStreamPtr& stream, DWORD& FilePos)
{
    std::string ret = "";
    char ch='!';
    char buffer[1024];
    DWORD charpos=0;
    while(ch)
    {
        stream->read(&ch,1);
        buffer[charpos] = ch;
        charpos++;
        FilePos++;
    }
    ret = buffer;
    return ret;
}
