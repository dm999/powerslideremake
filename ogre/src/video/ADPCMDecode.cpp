#include "ADPCMDecode.h"

//https://wiki.multimedia.cx/index.php/Microsoft_ADPCM
//http://we.easyelectronics.ru/Soft/szhatie-zvuka-v-ima-adpcm.html

namespace {
    int extendSign(int val)
    {
        return (val >> 15 == 0) ? val : (val | 0xFFFF0000);
    }

    Ogre::uint16 RB16Simple(const Ogre::uint8 * buf)
    {
        return *reinterpret_cast<const Ogre::uint16 *>(buf);
    }

    const int AdaptationTable[] = {
        230, 230, 230, 230, 307, 409, 512, 614,
        768, 614, 512, 409, 307, 230, 230, 230
    };
    const int AdaptCoeff1[] = { 256, 512, 0, 192, 240, 460, 392 };
    const int AdaptCoeff2[] = { 0, -256, 0, 64, 0, -208, -232 };

    struct ADPCMChannelStatus
    {
        int predictor;
        Ogre::int16 step_index;
        int step;
        int prev_sample;

        int sample1;
        int sample2;
        int coeff1;
        int coeff2;
        int idelta;
    };

    int msGetNibble(ADPCMChannelStatus &channelStatus, int nibble)
    {
        int predictor;

        predictor = (((channelStatus.sample1) * (channelStatus.coeff1)) + ((channelStatus.sample2) * (channelStatus.coeff2))) / 256;
        predictor += ((nibble & 0x08) ? (nibble - 0x10) : (nibble)) * channelStatus.idelta;

        channelStatus.sample2 = channelStatus.sample1;
        channelStatus.sample1 = Ogre::Math::Clamp<int>(predictor, std::numeric_limits<Ogre::int16>::min(), std::numeric_limits<Ogre::int16>::max());
        channelStatus.idelta = (AdaptationTable[(int)nibble] * channelStatus.idelta) / 256;
        channelStatus.idelta = Ogre::Math::Clamp<int>(channelStatus.idelta, 16, std::numeric_limits<Ogre::int16>::max());

        return channelStatus.sample1;
    }
}//anonymous

bool ADPCMDecode::decode(const std::vector<Ogre::uint8>& data, std::vector<Ogre::uint8>& mSamples, Ogre::Real &secondsDecoded)
{
    bool ret = false;

    secondsDecoded = std::numeric_limits<Ogre::Real>::max();

    size_t framesChunk = data.size() / mPacketSize;

    ADPCMChannelStatus status[2];

    if (mIsMSADPCM)
    {
        int nb_samples = (mPacketSize - 6 * mChannels) * 2 / mChannels;

        bool isStereo = mChannels == 2 ? true : false;

        secondsDecoded = framesChunk * nb_samples / static_cast<Ogre::Real>(mSamplesPerSec);

        mSamples.resize(sizeof(Ogre::uint16) * mChannels * nb_samples * framesChunk);

        for (size_t w = 0; w < framesChunk; ++w)
        {
            size_t frameStart = w * mPacketSize;
            size_t frameEnd = frameStart + mPacketSize;

            size_t offset = 0;
            int block_predictor = static_cast<Ogre::uint8>(data[frameStart + offset++]);
            if (block_predictor > 6)
            {
                break;
            }

            status[0].coeff1 = AdaptCoeff1[block_predictor];
            status[0].coeff2 = AdaptCoeff2[block_predictor];

            if (isStereo)
            {
                block_predictor = static_cast<Ogre::uint8>(data[frameStart + offset++]);
                if (block_predictor > 6)
                {
                    break;
                }
                status[1].coeff1 = AdaptCoeff1[block_predictor];
                status[1].coeff2 = AdaptCoeff2[block_predictor];
            }

            status[0].idelta = extendSign(RB16Simple((Ogre::uint8*)&data[frameStart + offset]));
            offset += 2;
            if (isStereo)
            {
                status[1].idelta = extendSign(RB16Simple((Ogre::uint8*)&data[frameStart + offset]));
                offset += 2;
            }

            status[0].sample1 = extendSign(RB16Simple((Ogre::uint8*)&data[frameStart + offset]));
            offset += 2;
            if (isStereo)
            {
                status[1].sample1 = extendSign(RB16Simple((Ogre::uint8*)&data[frameStart + offset]));
                offset += 2;
            }

            status[0].sample2 = extendSign(RB16Simple((Ogre::uint8*)&data[frameStart + offset]));
            offset += 2;
            if (isStereo)
            {
                status[1].sample2 = extendSign(RB16Simple((Ogre::uint8*)&data[frameStart + offset]));
                offset += 2;
            }

            size_t bufferSize = sizeof(Ogre::uint16) * mChannels * nb_samples;

            Ogre::int16 * samples = reinterpret_cast<Ogre::int16 *>(&mSamples[bufferSize * w]);
            *samples++ = status[0].sample2;
            if (isStereo)
            {
                *samples++ = status[1].sample2;
            }
            *samples++ = status[0].sample1;
            if (isStereo)
            {
                *samples++ = status[1].sample1;
            }

            size_t samplesToProcess = nb_samples - 2;
            if (!isStereo) samplesToProcess /= 2;

            for (size_t q = 0; q < samplesToProcess; ++q)
            {
                int byte = static_cast<Ogre::uint8>(data[frameStart + offset++]);
                *samples++ = msGetNibble(status[0], byte >> 4);//upper nibble
                *samples++ = msGetNibble(status[isStereo], byte & 0x0F);//lower nibble
            }

            //playAudio(&outputBuffer[0], bufferSize, channels, sampleRate, audioFrames);
        }

        ret = true;
    }
    else
    {
        //d.polubotko: TODO IMA ADPCM
    }

    return ret;
}