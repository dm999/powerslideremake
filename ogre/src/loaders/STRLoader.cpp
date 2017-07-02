
#include <sstream>
#include <iterator>

#include "STRLoader.h"

std::string STRLoader::load(const PFLoader& pfLoader, const std::string& relativeDir, const std::string& file) const
{
    std::string res = "";

    Ogre::DataStreamPtr fileToLoad = pfLoader.getFile(relativeDir, file);
    if(fileToLoad.get() && fileToLoad->isReadable())
    {
        size_t sizeOfFile = pfLoader.getFileSize(relativeDir, file);
        std::vector<unsigned char> buf(sizeOfFile);
        fileToLoad->read(&buf[0], sizeOfFile);
        fileToLoad->close();

        for(size_t q = 0; q < sizeOfFile; ++q)
        {
            buf[q] ^= 0xED;
        }

        std::ostringstream oss;
        std::copy(buf.begin(), buf.end(), std::ostream_iterator<char>(oss, ""));

        res = oss.str();
    }
    else {assert(false && "No STR file");}

    return res;
}