#ifndef CINEPACK_DECODE_H
#define CINEPACK_DECODE_H

#include "../includes/CommonIncludes.h"
#include "../includes/OgreInclude.h"

#include "AVIFrameDescription.h"

class AVIReadContainer;
struct PalletteBlock;
struct CinepakContext;

//https://web.archive.org/web/20070205155111/http://www.csse.monash.edu.au:80/~timf/videocodec/cinepak_decode.c
//https://multimedia.cx/mirror/cinepak.txt
class CinepackDecode
{
public:

    CinepackDecode() : mIsInited(false), mCurFrame(0){}

    bool init(Ogre::DataStreamPtr stream);
    bool decodeFrame();
    void clear();

    void resetCurrentFrame();

    Ogre::Real getFPS() const;
    Ogre::uint16 getWidth() const;
    Ogre::uint16 getHeight() const;
    std::vector<Ogre::uint8>& getFrame() {return mFrame;}

private:

    static Ogre::uint16 RB16(const Ogre::uint8 * buf);
    static Ogre::uint32 RB32(const Ogre::uint8 * buf);

    void decode(size_t frameIndex, size_t amountOfFrames);
    void decodeStrip(size_t stripSize, size_t stripIndex);
    void decodeCodeBook(Ogre::uint16 chunkSize, Ogre::uint8 chunkID, PalletteBlock* codebook);
    void decodeVectors(Ogre::uint16 chunkSize, Ogre::uint8 chunkID, size_t stripIndex);

    CommonIncludes::shared_ptr<AVIReadContainer> mAviContainer;
    FrameList mFrameList;
    FrameDescription mFrameDescription;

    std::vector<Ogre::uint8> mFrame;

    CommonIncludes::shared_ptr<CinepakContext> mCinepakContext;

    bool mIsInited;
    size_t mCurFrame;
};

#endif