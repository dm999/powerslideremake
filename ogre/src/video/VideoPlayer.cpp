
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
        mCinepakDecode = std::make_shared<CinepackDecode>();

        if(mCinepakDecode->init(videoFile))
        {
            mTexture = Ogre::TextureManager::getSingleton().getByName(textureName, TEMP_RESOURCE_GROUP_NAME);
            if(mTexture.get())
            {
                mVideoFPS = mCinepakDecode->getFPS();
                mIsInited = true;
            }
        }
    }
}

void VideoPlayer::start(Ogre::Real gain)
{
    mGain = gain;
    mCurrentFrame = 0;
    mTimer.reset();
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

void VideoPlayer::drawVideoFrame()
{
    std::vector<Ogre::uint8>& frame = mCinepakDecode->getFrame();
    Ogre::PixelBox pb(mCinepakDecode->getWidth(), mCinepakDecode->getHeight(), 1, Ogre::PF_BYTE_RGB, &frame[0]);

    if(!Ogre::Root::getSingleton().getRenderSystem()->getCapabilities()->hasCapability(Ogre::RSC_NON_POWER_OF_2_TEXTURES))
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

void VideoPlayer::processVideoFrame()
{
    int curFrame = static_cast<int>((mTimer.getMilliseconds() / 1000.0f) * mVideoFPS);

    if(mIsStarted)
    {

        if(curFrame < mCurrentFrame)
        {
            drawVideoFrame();
            return;
        }

        while(curFrame > mCurrentFrame)
        {
            mCinepakDecode->decodeVideoFrame();
            ++mCurrentFrame;
        }

        if(mCinepakDecode->decodeVideoFrame())
        {
            drawVideoFrame();
        }
        else
        {
            mIsFinished = true;
        }

        ++mCurrentFrame;
    }
}

void VideoPlayer::frameStarted(const Ogre::FrameEvent &evt)
{
    if(mIsInited)
    {
        processVideoFrame();

#ifndef NO_OPENAL
        bool doUpdateAudio = true;
        if (mSound.get())
        {
            doUpdateAudio = mSound->getSourceState() == SoundSource::Stopped;
        }

        if (mIsStarted && doUpdateAudio && mGain > 0.0f)
        {
            if (mCinepakDecode->decodeAudioFrame())
            {
                const std::vector<Ogre::int16>& samples = mCinepakDecode->getSamples();

                if (!mSound.get())
                {
                    mSound.reset(new SoundSource());
                }

                if (mCinepakDecode->getAudioChannels() == 2)
                {
                    mSound->updateSamples(samples, samples.size(), mCinepakDecode->getAudioChannels(), mCinepakDecode->getAudioSamplesPerSec());
                }
                else
                {
                    std::vector<Ogre::int16> samplesStereo(samples.size() * 2);
                    for (size_t q = 0; q < samples.size(); ++q)
                    {
                        samplesStereo[q * 2 + 0] = samples[q];
                        samplesStereo[q * 2 + 1] = samples[q];
                    }
                    mSound->updateSamples(samplesStereo, samplesStereo.size(), 2, mCinepakDecode->getAudioSamplesPerSec());
                }
                mSound->setGain(mGain);
                mSound->startPlaying();
            }
        }
#endif
    }
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