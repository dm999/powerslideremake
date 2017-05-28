#include "../pcheader.h"

#include <vector>

#include "TextureLoader.h"

Ogre::TexturePtr TextureLoader::load(const PFLoader& pfLoader, const std::string& subfolder, const std::string& filename, const std::string& texturename, const Ogre::String& group) const
{
    Ogre::TexturePtr res;

    Ogre::DataStreamPtr fileToLoad = pfLoader.getFile(subfolder, filename);
    if(fileToLoad.get() && fileToLoad->isReadable())
    {
        Ogre::String tex_ext;
        Ogre::String texture_path = filename.c_str();
        Ogre::String::size_type index_of_extension = texture_path.find_last_of('.');
        if (index_of_extension != Ogre::String::npos)
        {
            tex_ext = texture_path.substr(index_of_extension+1);

            size_t fileSize = pfLoader.getFileSize(subfolder, filename);
            std::vector<unsigned char> fileBuffer(fileSize);
            fileToLoad->read(&fileBuffer[0], fileSize);
            fileToLoad->close();

            Ogre::DataStreamPtr memoryStream(new Ogre::MemoryDataStream(&fileBuffer[0], fileSize, false, true));

            Ogre::Image img;
            img.load(memoryStream, tex_ext);

            res = Ogre::TextureManager::getSingleton().loadImage(texturename, group, img, Ogre::TEX_TYPE_2D);

            Ogre::LogManager::getSingleton().logMessage(Ogre::LML_NORMAL, "[TextureLoader::load]: [" + filename + "]");
        }

    }
    else {assert(false && "No texture file");}

    return res;
}

Ogre::TexturePtr TextureLoader::loadChroma( const PFLoader& pfLoader, 
                                            const std::string& subfolder, const std::string& filename, 
                                            const std::string& texturename, const Ogre::String& group,
                                            const Ogre::ColourValue& keyCol,
                                            float threshold,
                                            bool resize,
                                            size_t newSize) const
{
    Ogre::TexturePtr res;

    Ogre::DataStreamPtr fileToLoad = pfLoader.getFile(subfolder, filename);
    if(fileToLoad.get() && fileToLoad->isReadable())
    {
        Ogre::String tex_ext;
        Ogre::String texture_path = filename.c_str();
        Ogre::String::size_type index_of_extension = texture_path.find_last_of('.');
        if (index_of_extension != Ogre::String::npos)
        {
            tex_ext = texture_path.substr(index_of_extension+1);

            size_t fileSize = pfLoader.getFileSize(subfolder, filename);
            std::vector<unsigned char> fileBuffer(fileSize);
            fileToLoad->read(&fileBuffer[0], fileSize);
            fileToLoad->close();

            Ogre::DataStreamPtr memoryStream(new Ogre::MemoryDataStream(&fileBuffer[0], fileSize, false, true));

            Ogre::Image srcImg;
            srcImg.load(memoryStream, tex_ext);

            const Ogre::PixelFormat targetTextureFormat = Ogre::PF_A8R8G8B8;

            Ogre::uint width = srcImg.getWidth (), height = srcImg.getHeight ();
            // Since Ogre 1.6 Shoggoth, the OGRE_ALLOC_T memory macro must be used:
            Ogre::uchar* pixelData = OGRE_ALLOC_T (Ogre::uchar, Ogre::PixelUtil::getMemorySize (width, height, 1, targetTextureFormat), Ogre::MEMCATEGORY_GENERAL);
            Ogre::ulong pxDataIndex = 0, pxDataIndexStep = Ogre::PixelUtil::getNumElemBytes (targetTextureFormat); 

            for(Ogre::uint y = 0; y < height; ++y) 
            { 
                for(Ogre::uint x = 0; x < width; ++x) 
                { 
                    Ogre::ColourValue pixCol = srcImg.getColourAt (x, y, 0); 
                    Ogre::ColourValue diffCol = pixCol - keyCol; 
                    pixCol.a = ( (fabs(diffCol.r)<threshold) && (fabs(diffCol.g)<threshold) && (fabs(diffCol.b)<threshold) ) ? 0.0f : 1.0f; 
                    Ogre::PixelUtil::packColour (pixCol, targetTextureFormat, static_cast<void*> (pixelData + pxDataIndex) ); 
                    pxDataIndex += pxDataIndexStep; 
                }
            }

            Ogre::Image chromaKeyedImg;
            chromaKeyedImg.loadDynamicImage (pixelData, width, height, 1, targetTextureFormat, true); 
            if(resize) chromaKeyedImg.resize(newSize, newSize);

            res = Ogre::TextureManager::getSingleton().loadImage(texturename, group, chromaKeyedImg, Ogre::TEX_TYPE_2D);

            Ogre::LogManager::getSingleton().logMessage(Ogre::LML_NORMAL, "[TextureLoader::loadChroma]: [" + filename + "]");
        }

    }
    else {assert(false && "No texture file");}

    return res;
}