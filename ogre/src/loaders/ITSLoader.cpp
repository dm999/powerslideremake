#include "../pcheader.h"

#include "ITSLoader.h"

namespace
{
    typedef unsigned int DWORD;
    typedef unsigned short WORD;
    typedef unsigned char BYTE;

    struct header
    {
        BYTE unknown[46];
        WORD bpp;
        DWORD length;
        BYTE unknown2[8];
        WORD something;
        BYTE unknown3[10];
        WORD something2;
        BYTE unknown4[6];
    };
}//anonymous namespace

size_t ITSLoader::load(const PFLoader& pfLoader, const std::string& subfolder, const std::string& filename, std::vector<unsigned char>& rawData) const
{
    size_t res = 0;
    Ogre::DataStreamPtr fileToLoad = pfLoader.getFile(subfolder, filename);
    if(fileToLoad.get() && fileToLoad->isReadable())
    {
        typedef unsigned int DWORD;
        typedef unsigned short WORD;
        typedef unsigned char BYTE;

        rawData.clear();

        header head;
        fileToLoad->read(&head, sizeof(header));
        size_t bufLen = head.length * head.bpp;

        std::vector<BYTE> buf(bufLen);
        rawData.resize(bufLen);

        fileToLoad->read(&buf[0], bufLen);

        res = bufLen;

        for(size_t q = 0; q < head.length; ++q)
        {
            //-128 ... 127 maps to 64 ... 191
            short tmpval = static_cast<char>(buf[q]);
            tmpval >>= 0x1;
            tmpval += 128;
            rawData[q] = static_cast<BYTE>(tmpval);
        }

        fileToLoad->close();
    }
    else {assert(false && "No ITS file");}

    return res;
}