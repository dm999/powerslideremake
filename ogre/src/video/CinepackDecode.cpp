#include <string>
#include <iostream>
#include <fstream>
#include <cassert>

#ifdef _MSC_VER
    #pragma warning(disable:4800)
#endif

#include "CinepackDecode.h"

#include "AVIReadContainer.h"

#define MAX_STRIPS      32
#define PALETTE         256
#define MACROBLOCK      4

struct RgbTriplet
{
    Ogre::uint8 r, g, b;
    RgbTriplet() : r(0), g(0), b(0) {}
    RgbTriplet(Ogre::uint8 y, Ogre::uint8 u, Ogre::uint8 v) : r(y), g(u), b(v) {}

    static void copyTwoPairsToFour(RgbTriplet* dst, const RgbTriplet* src)
    {
        dst[0] = src[0];
        dst[1] = src[0];
        dst[2] = src[1];
        dst[3] = src[1];
    }

    static void copyTwoSeparateToFour(RgbTriplet* dst, const RgbTriplet* srcA, const RgbTriplet* srcB)
    {
        dst[0] = srcA[0];
        dst[1] = srcA[1];
        dst[2] = srcB[0];
        dst[3] = srcB[1];
    }
};

struct PalletteBlock
{
    RgbTriplet macroblock[MACROBLOCK];
};

struct cvid_strip
{
    Ogre::uint16          id;
    Ogre::uint16          x1, y1;
    Ogre::uint16          x2, y2;
    std::vector<PalletteBlock> v4_codebook;
    std::vector<PalletteBlock> v1_codebook;

    cvid_strip() : v4_codebook(PALETTE), v1_codebook(PALETTE) {}
};

struct CinepakContext
{
    const Ogre::uint8 *data;
    size_t offsetStrip;
    size_t offsetChunk;
    //size_t bufSize;

    size_t frameWidth, frameHeight;

    std::vector<cvid_strip> strips;

    CinepakContext() : strips(MAX_STRIPS) {}
    void setData(const Ogre::uint8 * d) { data = d; }
};


//https://stackoverflow.com/questions/105252/how-do-i-convert-between-big-endian-and-little-endian-values-in-c
template <typename T>
T swap_endian(T u)
{
    union
    {
        T u;
        unsigned char u8[sizeof(T)];
    } source, dest;

    source.u = u;

    for (size_t k = 0; k < sizeof(T); k++)
        dest.u8[k] = source.u8[sizeof(T) - k - 1];

    return dest.u;
}

Ogre::uint16 RB16(const Ogre::uint8 * buf)
{
    //from big indean (motorola)
    return swap_endian<Ogre::uint16>(*reinterpret_cast<const Ogre::uint16 *>(buf));
}

/*
size_t RB24(const unsigned char * buf)
{
//from big indean (motorola)
size_t ret = 0;
ret |= *(reinterpret_cast<const unsigned char *>(buf) + 0) << 16;
ret |= *(reinterpret_cast<const unsigned char *>(buf) + 1) << 8;
ret |= *(reinterpret_cast<const unsigned char *>(buf) + 2) << 0;

return ret;
}*/

Ogre::uint32 RB32(const Ogre::uint8 * buf)
{
    //from big indean (motorola)
    return swap_endian<Ogre::uint32>(*reinterpret_cast<const Ogre::uint32 *>(buf));
}


Ogre::uint8 clamp(int val)
{
    return std::max(std::min(val, 255), 0);
}


static void pgm_save(Ogre::uint8 *buf, int wrap, int xsize, int ysize, char *filename)
{
    FILE *f;
    int i;

    f = fopen(filename, "wt");
    fprintf(f, "P3\n%d %d\n%d\n", xsize, ysize, 255);
    for (i = 0; i < ysize; i++)
    {
        //fwrite(buf + i * wrap, 1, xsize, f);
        //fwrite(buf + i * wrap, 1, wrap, f);
        for (int j = 0; j < xsize; j++)
        {
            fprintf(f, "%d ", buf[i * wrap + j * 3 + 0]);
            fprintf(f, "%d ", buf[i * wrap + j * 3 + 1]);
            fprintf(f, "%d ", buf[i * wrap + j * 3 + 2]);

            //fwrite(&buf[i * wrap + j * 3 + 0], 1, 1, f);
            //fwrite(&buf[i * wrap + j * 3 + 1], 1, 1, f);
            //fwrite(&buf[i * wrap + j * 3 + 2], 1, 1, f);
        }
        fprintf(f, "\n");
    }
    fclose(f);
}

void decodeCodeBook(CinepakContext& s, Ogre::uint16 chunkSize, Ogre::uint8 chunkID, PalletteBlock* codebook)
{
    Ogre::uint32 mask = 0, flag = 0;

    Ogre::uint16 offset = 0;

    const bool isKeyFrame = !(chunkID & 0x01);

    for (size_t q = 0; q < PALETTE; ++q)
    {
        mask >>= 1;
        if (!isKeyFrame && !(q % 32))//non key frame (inter) or mask is empty
        {
            if ((offset + 4) > chunkSize) break;

            flag = RB32(&s.data[s.offsetStrip + s.offsetChunk + offset]);
            offset += 4;
            mask = 0x80000000;
        }

        bool doUpdate = flag & mask;
        if (isKeyFrame || doUpdate)//key frame (intra) or flag indicate update is necessary
        {
            if ((offset + 6) > chunkSize) break;

            Ogre::uint8 luminance[MACROBLOCK];

            memcpy(&luminance[0], &s.data[s.offsetStrip + s.offsetChunk + offset], sizeof(Ogre::uint8) * MACROBLOCK);
            offset += 4;

            Ogre::int8 u = *reinterpret_cast<const Ogre::int8 *>(&s.data[s.offsetStrip + s.offsetChunk + offset++]);
            Ogre::int8 v = *reinterpret_cast<const Ogre::int8 *>(&s.data[s.offsetStrip + s.offsetChunk + offset++]);

            for (size_t w = 0; w < MACROBLOCK; ++w)
            {
                int r = luminance[w] + u * 0 + v * 2;
                int g = luminance[w] - (u / 2) - v;
                int b = luminance[w] + u * 2 + v * 0;
                codebook[q].macroblock[w].r = clamp(r);
                codebook[q].macroblock[w].g = clamp(g);
                codebook[q].macroblock[w].b = clamp(b);
            }
        }

    }

}

void decodeVectors(CinepakContext &s, Ogre::uint16 chunkSize, Ogre::uint8 chunkID, size_t stripIndex, std::vector<Ogre::uint8>& frame)
{
    Ogre::uint32 flag = 0, mask = 0;
    Ogre::uint16 offset = 0;

    const cvid_strip& strip = s.strips[stripIndex];

    const bool isKeyFrame = !(chunkID & 0x01);
    const bool isV1Only = chunkID & 0x02;

    const size_t linesize = s.frameWidth * 3;

    for (Ogre::uint16 y = strip.y1; y < strip.y2; y += MACROBLOCK)
    {
        RgbTriplet *row0, *row1, *row2, *row3;

        row0 = reinterpret_cast<RgbTriplet *>(&frame[strip.x1 * 3 + (y * linesize)]);
        row1 = row0 + s.frameWidth;
        row2 = row1 + s.frameWidth;
        row3 = row2 + s.frameWidth;

        for (Ogre::uint16 x = strip.x1; x < strip.x2; x += MACROBLOCK)
        {

            if (isV1Only)//V1 - 1 palette used
            {
                if (offset > chunkSize) return;

                const PalletteBlock& codebook(strip.v1_codebook[s.data[s.offsetStrip + s.offsetChunk + offset++]]);

                RgbTriplet::copyTwoPairsToFour(&row0[0], &codebook.macroblock[0]);
                RgbTriplet::copyTwoPairsToFour(&row1[0], &codebook.macroblock[0]);
                RgbTriplet::copyTwoPairsToFour(&row2[0], &codebook.macroblock[2]);
                RgbTriplet::copyTwoPairsToFour(&row3[0], &codebook.macroblock[2]);
                continue;
            }

            if (!isKeyFrame)//need to select 3 subcases: 0 = the block is skipped, 10 = V1 coded block, 11 = V4 coded block
            {
                mask >>= 1;
                if (!mask)
                {
                    if ((offset + 4) > chunkSize) return;

                    flag = RB32(&s.data[s.offsetStrip + s.offsetChunk + offset]);
                    offset += 4;
                    mask = 0x80000000;
                }
            }

            bool doUpdate = flag & mask;
            if (isKeyFrame || doUpdate)
            {
                mask >>= 1;
                if (!mask)
                {
                    if ((offset + 4) > chunkSize) return;

                    flag = RB32(&s.data[s.offsetStrip + s.offsetChunk + offset]);
                    offset += 4;
                    mask = 0x80000000;
                }

                doUpdate = flag & mask;

                if (doUpdate)//V4 - 4 palettes used
                {
                    if ((offset + 4) > chunkSize) return;

                    const PalletteBlock& codebook0(strip.v4_codebook[s.data[s.offsetStrip + s.offsetChunk + offset++]]);
                    const PalletteBlock& codebook1(strip.v4_codebook[s.data[s.offsetStrip + s.offsetChunk + offset++]]);
                    const PalletteBlock& codebook2(strip.v4_codebook[s.data[s.offsetStrip + s.offsetChunk + offset++]]);
                    const PalletteBlock& codebook3(strip.v4_codebook[s.data[s.offsetStrip + s.offsetChunk + offset++]]);

                    RgbTriplet::copyTwoSeparateToFour(&row0[0], &codebook0.macroblock[0], &codebook1.macroblock[0]);
                    RgbTriplet::copyTwoSeparateToFour(&row1[0], &codebook0.macroblock[2], &codebook1.macroblock[2]);
                    RgbTriplet::copyTwoSeparateToFour(&row2[0], &codebook2.macroblock[0], &codebook3.macroblock[0]);
                    RgbTriplet::copyTwoSeparateToFour(&row3[0], &codebook2.macroblock[2], &codebook3.macroblock[2]);
                }
                else//V1 - 1 palette used
                {
                    if (offset > chunkSize) return;

                    const PalletteBlock& codebook(strip.v1_codebook[s.data[s.offsetStrip + s.offsetChunk + offset++]]);

                    RgbTriplet::copyTwoPairsToFour(&row0[0], &codebook.macroblock[0]);
                    RgbTriplet::copyTwoPairsToFour(&row1[0], &codebook.macroblock[0]);
                    RgbTriplet::copyTwoPairsToFour(&row2[0], &codebook.macroblock[2]);
                    RgbTriplet::copyTwoPairsToFour(&row3[0], &codebook.macroblock[2]);
                }
            }

            row0 += MACROBLOCK;
            row1 += MACROBLOCK;
            row2 += MACROBLOCK;
            row3 += MACROBLOCK;
        }
    }

    return;//return 0;
}

void decodeStrip(CinepakContext& s, size_t stripSize, size_t stripIndex, std::vector<Ogre::uint8>& frame)
{
    s.offsetChunk = 0;

    size_t index = 0;

    while (s.offsetChunk < stripSize)
    {
        Ogre::uint8 chunkID = s.data[s.offsetStrip + s.offsetChunk + 0];
        Ogre::uint16 chunkSize = RB16(&s.data[s.offsetStrip + s.offsetChunk + 2]);
        s.offsetChunk += 4; // skip header before decode chunk
        //printf("%2d chunk %d ", index++, chunkSize);
        switch (chunkID)
        {
        case 0x20:
        case 0x21:
            //printf("cinepak_decode_codebook %2x\n", chunkID);
            decodeCodeBook(s, chunkSize - 4, chunkID, &s.strips[stripIndex].v4_codebook[0]);
            break;

        case 0x22:
        case 0x23:
            //printf("cinepak_decode_codebook %2x\n", chunkID);
            decodeCodeBook(s, chunkSize - 4, chunkID, &s.strips[stripIndex].v1_codebook[0]);
            break;

        case 0x30:
        case 0x31:
        case 0x32:
            //printf("cinepak_decode_vectors %2x\n", chunkID);
            decodeVectors(s, chunkSize - 4, chunkID, stripIndex, frame);
            return;
        }

        s.offsetChunk -= 4;
        s.offsetChunk += chunkSize;
    }
}

void decode(CinepakContext& s, size_t frameIndex, size_t amountOfFrames, std::vector<Ogre::uint8>& frame)
{
    Ogre::uint8 frameFlags = s.data[0];
    //s.bufSize = RB24(&s.data[1]);
    s.frameWidth = RB16(&s.data[4]);
    s.frameHeight = RB16(&s.data[6]);
    size_t numStrips = RB16(&s.data[8]);

    s.offsetStrip = 10;//frame offset
    Ogre::uint16 y0 = 0;

    //frame.assign(frame.size(), 255);


    for (size_t q = 0; q < numStrips; ++q)
    {
        size_t stripID = s.data[s.offsetStrip + 0];
        size_t stripSize = RB16(&s.data[s.offsetStrip + 2]);

        //printf("%2d strip %d\n", q, stripSize);

        s.strips[q].y1 = RB16(&s.data[s.offsetStrip + 4]);
        if (!s.strips[q].y1)
        {
            s.strips[q].y1 = y0;
            s.strips[q].y2 = s.strips[q].y1 + RB16(&s.data[s.offsetStrip + 8]);
        }
        else
        {
            s.strips[q].y2 = RB16(&s.data[s.offsetStrip + 8]);
        }
        s.strips[q].x1 = RB16(&s.data[s.offsetStrip + 6]);
        s.strips[q].x2 = RB16(&s.data[s.offsetStrip + 10]);
        y0 = s.strips[q].y2;

        bool isKeyFrame = false;
        if (stripID == 0x10)
        {
            isKeyFrame = true;
        }

        if (q > 0 && !(frameFlags & 0x01))//use previous codebook
        {
            s.strips[q].v4_codebook = s.strips[q - 1].v4_codebook;
            s.strips[q].v1_codebook = s.strips[q - 1].v1_codebook;
        }

        s.offsetStrip += 12;//skip header before decode strip

        decodeStrip(s, stripSize - 12, q, frame);

        s.offsetStrip -= 12;
        s.offsetStrip += stripSize;
    }

    //if(frameIndex == amountOfFrames - 1)
    //if (frameIndex == amountOfFrames - 30)
    //if (frameIndex == 0)
    {
        //pgm_save(&frame[0], s.frameWidth * 3, s.frameWidth, s.frameHeight, "frame.pgm");
    }
}
/*
int main()
{

    AVIReadContainer container;
    container.initStream("d:/All_Files/Games/Back_Powerslide/Gameshell/data/gameshell/apple.avi");
    //container.initStream("d:/All_Files/Games/Back_Powerslide/Gameshell/data/gameshell/intro.avi");
    //container.initStream("d:/All_Files/Games/Back_Powerslide/Gameshell/data/gameshell/ratbag.avi");
    FrameList list;
    FrameDescription frame_description;
    bool parsed = container.parseRiff(list, frame_description);

    //FILE *fA = fopen("audio.dat", "wb");

    CinepakContext cinepakContext;
    std::vector<char> frame(container.getWidth() * container.getHeight() * 3, 0);//rgb

    for (size_t qq = 0; qq < frame_description.size(); ++qq)
    {

        if (frame_description[qq] == AVIReadContainer::AUDIOFRAME_CC)
        {
            std::vector<char> data = container.readFrame(list.begin() + qq);
            //fwrite(&data[0], 1, data.size(), fA);
            printf("audio %d\n", data.size());
        }
        if (frame_description[qq] == AVIReadContainer::VIDEOFRAME_CC)
        {
            std::vector<char> data = container.readFrame(list.begin() + qq);
            cinepakContext.setData(reinterpret_cast<unsigned char *>(&data[0]));
            printf("video %d\n", data.size());

            //https://web.archive.org/web/20070205155111/http://www.csse.monash.edu.au:80/~timf/videocodec/cinepak_decode.c
            //https://multimedia.cx/mirror/cinepak.txt

            decode(cinepakContext, qq, frame_description.size(), frame);
        }
    }

    //fclose(fA);

    return 0;
}*/

bool CinepackDecode::init(Ogre::DataStreamPtr stream)
{
    mIsInited = false;

    mAviContainer = CommonIncludes::shared_ptr<AVIReadContainer>(new AVIReadContainer());

    mAviContainer->initStream(stream);

    mIsInited = mAviContainer->parseRiff(mFrameList, mFrameDescription);

    if(mIsInited)
    {
        mCinepakContext = CommonIncludes::shared_ptr<CinepakContext>(new CinepakContext());
        mFrame.resize(mAviContainer->getWidth() * mAviContainer->getHeight() * 3);//rgb
        mCurFrame = 0;
    }

    return mIsInited;
}

bool CinepackDecode::decodeFrame()
{
    bool ret = false;
    if(mIsInited)
    {
        std::vector<Ogre::uint8> data = mAviContainer->readFrame(mFrameList.begin() + mCurFrame);
        mCinepakContext->setData(&data[0]);
        decode(*mCinepakContext.get(), mCurFrame, mFrameDescription.size(), mFrame);

        ++mCurFrame;
        ret = true;
    }

    return ret;
}

void CinepackDecode::clear()
{
    mIsInited = false;
    mAviContainer.reset();
    mFrameList.clear();
    mFrameDescription.clear();
    mCinepakContext.reset();
    mFrame.clear();
}

Ogre::Real CinepackDecode::getFPS() const
{
    Ogre::Real ret = 0.0f;

    if(mIsInited)
    {
        ret = static_cast<Ogre::Real>(mAviContainer->getFps());
    }

    return ret;
}