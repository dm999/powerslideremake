#include "../pcheader.h"

#include <set>

#include "PFLoader.h"

PFLoader::PFLoader()
    : mPath("."), mFileName("data.pf")
{
    mIsPathCorrect = checkPathCorrect(mPath, mFileName);
}

void PFLoader::init(const std::string& path, const std::string& file)
{
    mPath = path;
    mFileName = file;
    mIsPathCorrect = checkPathCorrect(path, file);

    if(mIsPathCorrect)
    {
        fileSystem.clear();

        //read file system
        std::string dataPF = mPath + "/" + mFileName;
        FILE * f = fopen(dataPF.c_str(), "rb");
        if(f)
        {
            DWORD Size,From;
            DWORD ElemCount;

            fseek(f,0,SEEK_END);
            Size=ftell(f);
            fseek(f,-4,SEEK_CUR);
            fread(&From,4,1,f);
            fseek(f,From,0);
            fread(&ElemCount,4,1,f);

            DWORD FilePos=From+4;

            

            while(1)
            {
                if(FilePos>=Size-4) break;

                DWORD Next,F_F,Offset,Length,Smthing;

                std::string itemName = readString(f, FilePos);

                fread(&Next,4,1,f);
                fread(&F_F,4,1,f);
                FilePos+=8;
                if(F_F==0xffffffff)//file
                {
                    FilePos+=10;
                    fread(&Offset,4,1,f);
                    fread(&Length,4,1,f);
                    fread(&Smthing,2,1,f);
                    fileSystem.push_back(PF::PackedFileItem(itemName,Next,Offset,Length));
                }
                else                // folder
                {
                    fileSystem.push_back(PF::PackedFileItem(itemName,Next,F_F));
                }
            }

            fclose(f);
        }
    }
}

bool PFLoader::checkPathCorrect(const std::string& path, const std::string& file) const
{
    bool res = false;

    std::string dataPF = path + "/" + file;

    FILE * f = fopen(dataPF.c_str(), "rb");
    if(f)
    {
        fclose(f);
        res = true;
    }

    return res;
}

FILE * PFLoader::getFile(const std::string& relativeDir, const std::string& file) const
{
    FILE * ret = NULL;

    if(mIsPathCorrect && !fileSystem.empty())
    {
        std::string dataPF = mPath + "/" + mFileName;
        ret = fopen(dataPF.c_str(), "rb");
        if(ret)
        {
            //find file offset
            size_t fileSize;
            size_t offset = findFile(relativeDir, file, fileSize);
            if(offset != 0)
            {
                fseek(ret, offset, SEEK_SET);
            }
            else
            {
                fclose(ret);
                ret = NULL;
            }
        }
    }

    return ret;
}

size_t PFLoader::getFileSize(const std::string& relativeDir, const std::string& file) const
{
    size_t ret = 0;

    if(mIsPathCorrect && !fileSystem.empty())
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

    std::vector<std::string> vPath = splitpath(relativeDir, delims);
    
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

std::string PFLoader::readString(FILE * f, DWORD& FilePos)
{
    std::string ret = "";
    char ch='!';
    char buffer[1024];
    DWORD charpos=0;
    while(ch)
    {
        fread(&ch,1,1,f);
        buffer[charpos] = ch;
        charpos++;
        FilePos++;
    }
    ret = buffer;
    return ret;
}

std::vector<std::string> PFLoader::splitpath(const std::string& str, const std::set<char> delimiters)
{
    //http://stackoverflow.com/questions/8520560/get-a-file-name-from-a-path

    std::vector<std::string> result;

    char const* pch = str.c_str();
    char const* start = pch;
    for(; *pch; ++pch)
    {
        if (delimiters.find(*pch) != delimiters.end())
        {
            if (start != pch)
            {
                std::string str(start, pch);
                result.push_back(str);
            }
            else
            {
                result.push_back("");
            }
            start = pch + 1;
        }
    }
    result.push_back(start);

    return result;
}