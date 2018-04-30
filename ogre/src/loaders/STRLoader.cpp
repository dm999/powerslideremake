#include "STRLoader.h"

std::string STRLoader::load(const PFLoader& pfLoader, const std::string& relativeDir, const std::string& file) const
{
    std::string res = "";

    Ogre::DataStreamPtr fileToLoad = pfLoader.getFile(relativeDir, file);
    if(fileToLoad.get() && fileToLoad->isReadable())
    {
        size_t sizeOfFile = pfLoader.getFileSize(relativeDir, file);
        std::vector<char> buf(sizeOfFile);
        fileToLoad->read(&buf[0], sizeOfFile);
        fileToLoad->close();

        res = decode(buf);
    }
    //else {assert(false && "No STR file");}

    return res;
}

std::string STRLoader::decode(std::vector<char> buf) const
{
    for(size_t q = 0; q < buf.size(); ++q)
    {
        buf[q] ^= 0xED;
    }

    std::string ret(buf.begin(), buf.end());

    return ret;
}