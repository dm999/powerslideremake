
#include "VideoPlayer.h"

#include "CinepackDecode.h"
#include "../loaders/PFLoader.h"

void VideoPlayer::init(const PFLoader& gameshell, const std::string& filePath, const std::string& fileName, const Ogre::String& textureName)
{
    clear();

    Ogre::DataStreamPtr videoFile = gameshell.getFile(filePath, fileName);

    if(videoFile.get() && videoFile->isReadable())
    {
        mCinepakDecode = CommonIncludes::shared_ptr<CinepackDecode>(new CinepackDecode());

        if(mCinepakDecode->init(videoFile))
        {
            mTexture = Ogre::TextureManager::getSingleton().getByName(textureName, TEMP_RESOURCE_GROUP_NAME);
            if(mTexture.get())
            {
                mVideoSPF = 1.0f / mCinepakDecode->getFPS();
                mIsInited = true;
            }
        }
    }
}

void VideoPlayer::start()
{
    mSecondsPassed = 1000.0f;
    mIsStarted = true; 
    mIsFinished = false;
}

void VideoPlayer::restart()
{
    start();

    if(mIsInited)
    {
        mCinepakDecode->resetCurrentFrame();
    }
}

void VideoPlayer::frameStarted(const Ogre::FrameEvent &evt)
{
    bool doUpdate = false;

    if(mSecondsPassed > mVideoSPF)
    {
        doUpdate = true;
        mSecondsPassed = 0.0f;
    }

    if(mIsInited)
    {
        if(mIsStarted && doUpdate)
        {
            if(mCinepakDecode->decodeFrame())
            {
                std::vector<Ogre::uint8>& frame = mCinepakDecode->getFrame();
                Ogre::PixelBox pb(mCinepakDecode->getWidth(), mCinepakDecode->getHeight(), 1, Ogre::PF_BYTE_RGB, &frame[0]);

#if !defined(__ANDROID__)
                Ogre::HardwarePixelBufferSharedPtr buffer = mTexture->getBuffer();
                buffer->blitFromMemory(pb);
#else
                std::vector<Ogre::uint8> frameResized(512 * 512 * 3);
                Ogre::PixelBox pbResized(512, 512, 1, Ogre::PF_BYTE_RGB, &frameResized[0]);

                Ogre::Image::scale(pb, pbResized);

                Ogre::HardwarePixelBufferSharedPtr buffer = mTexture->getBuffer();
                buffer->blitFromMemory(pbResized);
#endif

            }
            else
            {
                mIsFinished = true;
            }
        }
    }

    mSecondsPassed += evt.timeSinceLastFrame;
}

void VideoPlayer::clear()
{
    mSecondsPassed = 1000.0f;
    mIsInited = false;
    mIsStarted = false;

    if(mIsInited)
    {
        mTexture.setNull();
        mCinepakDecode.reset();
    }
}