
#ifndef VIDEO_PLAYER_H
#define VIDEO_PLAYER_H

#include <string>

#include "../includes/CommonIncludes.h"
#include "../includes/OgreInclude.h"

class CinepackDecode;
class PFLoader;
class SoundSource;

class VideoPlayer
{
public:

    VideoPlayer() : mIsInited(false), mIsStarted(false), mIsFinished(false), mGain(1.0f){}

    /**
     * prepare everything to process, use start() to run player
     */
    void init(const PFLoader& gameshell, const std::string& filePath, const std::string& fileName, const std::string& textureName);

    /**
     * set internal flags to start (cheap)
     */
    void start(Ogre::Real gain);

    /**
     * set flag to stop (cheap), stop audio
     */
    void stop();

    /**
     * set internal flags to start, recreate decoder context and empty frame (expensive)
     */
    void restart(Ogre::Real gain);

    bool isFinished() const {return mIsFinished;}

    /**
     * event to process new frame
     */
    void frameStarted(const Ogre::FrameEvent &evt);

    /**
     * clean up memory
     */
    void clear();

private:

    void processVideoFrame();
    void drawVideoFrame();

    Ogre::TexturePtr mTexture;

    CommonIncludes::shared_ptr<CinepackDecode> mCinepakDecode;

    bool mIsInited;
    bool mIsStarted;
    bool mIsFinished;

    Ogre::Real mVideoFPS;

    int mCurrentFrame;
    Ogre::Timer mTimer;

    Ogre::Real mGain;
    CommonIncludes::shared_ptr<SoundSource> mSound;
};

#endif