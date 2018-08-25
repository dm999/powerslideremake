
#include "VideoPlayer.h"

#include "CinepackDecode.h"
#include "../loaders/PFLoader.h"

#include "../sound/SoundSource.h"

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

void VideoPlayer::start(Ogre::Real gain)
{
    mGain = gain;
    mSecondsPassedVideo = std::numeric_limits<Ogre::Real>::max();
    mSecondsPassedAudio = std::numeric_limits<Ogre::Real>::max();
    mLastAudioBufferSeconds = 0.0f;
    mIsStarted = true; 
    mIsFinished = false;
}

void VideoPlayer::stop()
{
    mIsStarted = false;
#ifndef NO_OPENAL
    if (mSound.get())
    {
        mSound->stopPlaying();
    }
#endif
}

void VideoPlayer::restart(Ogre::Real gain)
{
    start(gain);

    if(mIsInited)
    {
        mCinepakDecode->resetCurrentFrame();
#ifndef NO_OPENAL
        if (mSound.get())
        {
            mSound->stopPlaying();
        }
#endif
    }
}

void VideoPlayer::frameStarted(const Ogre::FrameEvent &evt)
{
    bool doUpdateVideo = false;
    bool doUpdateAudio = false;

    if(mSecondsPassedVideo > mVideoSPF)
    {
        doUpdateVideo = true;
        mSecondsPassedVideo = 0.0f;
    }

    if (mSecondsPassedAudio > mLastAudioBufferSeconds)
    {
        doUpdateAudio = true;
        mSecondsPassedAudio = 0.0f;
    }

    if(mIsInited)
    {
        if(mIsStarted && doUpdateVideo)
        {
            if(mCinepakDecode->decodeVideoFrame())
            {
                std::vector<Ogre::uint8>& frame = mCinepakDecode->getFrame();
                Ogre::PixelBox pb(mCinepakDecode->getWidth(), mCinepakDecode->getHeight(), 1, Ogre::PF_BYTE_RGB, &frame[0]);

                if (!Ogre::Root::getSingleton().getRenderSystem()->getCapabilities()->hasCapability(Ogre::RSC_NON_POWER_OF_2_TEXTURES))
                {
                    //same sizes as for VideoTexture UIMainMenuBackground::createBackgroundTextures
                    std::vector<Ogre::uint8> frameResized(512 * 512 * 3);
                    Ogre::PixelBox pbResized(512, 512, 1, Ogre::PF_BYTE_RGB, &frameResized[0]);

                    Ogre::Image::scale(pb, pbResized);

                    Ogre::HardwarePixelBufferSharedPtr buffer = mTexture->getBuffer();
                    buffer->blitFromMemory(pbResized);
                }
                else
                {
                    Ogre::HardwarePixelBufferSharedPtr buffer = mTexture->getBuffer();
                    buffer->blitFromMemory(pb);
                }
            }
            else
            {
                mIsFinished = true;
            }
        }

#ifndef NO_OPENAL
        doUpdateAudio = true;
        if (mSound.get())
        {
            doUpdateAudio = mSound->getSourceState() == SoundSource::Stopped;
        }

        if (mIsStarted && doUpdateAudio && mGain > 0.0f)
        {
            if (mCinepakDecode->decodeAudioFrame(mLastAudioBufferSeconds))
            {
                const std::vector<Ogre::int16>& samples = mCinepakDecode->getSamples();

                if (!mSound.get())
                {
                    mSound.reset(new SoundSource());
                }

                mSound->updateSamples(samples, samples.size(), mCinepakDecode->getAudioChannels(), mCinepakDecode->getAudioSamplesPerSec());
                mSound->setGain(mGain);
                mSound->startPlaying();
            }
        }
#endif
    }

    mSecondsPassedVideo += evt.timeSinceLastFrame;
    mSecondsPassedAudio += evt.timeSinceLastFrame;
}

void VideoPlayer::clear()
{
    mIsInited = false;
    mIsStarted = false;

    if(mIsInited)
    {
        mTexture.setNull();
        mCinepakDecode.reset();
    }
}