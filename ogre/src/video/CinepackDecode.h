#ifndef CINEPACK_DECODE_H
#define CINEPACK_DECODE_H

#include "../includes/CommonIncludes.h"
#include "../includes/OgreInclude.h"

#include "AVIFrameDescription.h"

class AVIReadContainer;
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

    Ogre::Real getFPS() const;
    std::vector<Ogre::uint8>& getFrame() {return mFrame;}

private:

    CommonIncludes::shared_ptr<AVIReadContainer> mAviContainer;
    FrameList mFrameList;
    FrameDescription mFrameDescription;

    std::vector<Ogre::uint8> mFrame;

    CommonIncludes::shared_ptr<CinepakContext> mCinepakContext;

    bool mIsInited;
    size_t mCurFrame;
};

#endif