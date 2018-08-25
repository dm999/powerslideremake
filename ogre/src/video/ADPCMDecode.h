#ifndef ADPCM_DECODE_H
#define ADPCM_DECODE_H

#include <vector>
#include "../includes/OgreInclude.h"

class ADPCMDecode
{
public:

    ADPCMDecode(bool isMSADPCM, Ogre::uint32 packetSize, Ogre::uint16 channels, Ogre::uint32 samplesPerSec) : mIsMSADPCM(isMSADPCM), mPacketSize(packetSize), mChannels(channels), mSamplesPerSec(samplesPerSec){}

    bool decode(const std::vector<Ogre::uint8>& data, std::vector<Ogre::int16>& mSamples);

private:

    bool mIsMSADPCM;
    Ogre::uint32 mPacketSize;
    Ogre::uint16 mChannels;
    Ogre::uint32 mSamplesPerSec;
};

#endif