
#include "AVIReadContainer.h"


const Ogre::uint32 RIFF_CC = CV_FOURCC('R', 'I', 'F', 'F');
const Ogre::uint32 LIST_CC = CV_FOURCC('L', 'I', 'S', 'T');
const Ogre::uint32 HDRL_CC = CV_FOURCC('h', 'd', 'r', 'l');
const Ogre::uint32 AVIH_CC = CV_FOURCC('a', 'v', 'i', 'h');
const Ogre::uint32 STRL_CC = CV_FOURCC('s', 't', 'r', 'l');
const Ogre::uint32 STRH_CC = CV_FOURCC('s', 't', 'r', 'h');
const Ogre::uint32 STRF_CC = CV_FOURCC('s', 't', 'r', 'f');
const Ogre::uint32 VIDS_CC = CV_FOURCC('v', 'i', 'd', 's');
const Ogre::uint32 AUDS_CC = CV_FOURCC('a', 'u', 'd', 's');
const Ogre::uint32 MJPG_CC = CV_FOURCC('M', 'J', 'P', 'G');
const Ogre::uint32 MOVI_CC = CV_FOURCC('m', 'o', 'v', 'i');
const Ogre::uint32 IDX1_CC = CV_FOURCC('i', 'd', 'x', '1');
const Ogre::uint32 AVI_CC = CV_FOURCC('A', 'V', 'I', ' ');
const Ogre::uint32 AVIX_CC = CV_FOURCC('A', 'V', 'I', 'X');
const Ogre::uint32 JUNK_CC = CV_FOURCC('J', 'U', 'N', 'K');
const Ogre::uint32 INFO_CC = CV_FOURCC('I', 'N', 'F', 'O');
const Ogre::uint32 ODML_CC = CV_FOURCC('o', 'd', 'm', 'l');
const Ogre::uint32 DMLH_CC = CV_FOURCC('d', 'm', 'l', 'h');
const Ogre::uint32 CINEPAK_CC = CV_FOURCC('c', 'v', 'i', 'd');
const Ogre::uint32 ADAPCM_CC = CV_FOURCC(0, 0, 0, 0);
const Ogre::uint32 AVIReadContainer::VIDEOFRAME_CC = CV_FOURCC('0', '0', 'd', 'c');
const Ogre::uint32 AVIReadContainer::AUDIOFRAME_CC = CV_FOURCC('0', '1', 'w', 'b');


std::string fourccToString(Ogre::uint32 fourcc)
{
    char buffer[256];
    sprintf(buffer, "%c%c%c%c", fourcc & 255, (fourcc >> 8) & 255, (fourcc >> 16) & 255, (fourcc >> 24) & 255);
    return std::string(buffer);
}


#pragma pack(push, 1)
struct AviMainHeader
{
    Ogre::uint32 dwMicroSecPerFrame;    //  The period between video frames
    Ogre::uint32 dwMaxBytesPerSec;      //  Maximum data rate of the file
    Ogre::uint32 dwReserved1;           // 0
    Ogre::uint32 dwFlags;               //  0x10 AVIF_HASINDEX: The AVI file has an idx1 chunk containing an index at the end of the file.
    Ogre::uint32 dwTotalFrames;         // Field of the main header specifies the total number of frames of data in file.
    Ogre::uint32 dwInitialFrames;       // Is used for interleaved files
    Ogre::uint32 dwStreams;             // Specifies the number of streams in the file.
    Ogre::uint32 dwSuggestedBufferSize; // Field specifies the suggested buffer size forreading the file
    Ogre::uint32 dwWidth;               // Fields specify the width of the AVIfile in pixels.
    Ogre::uint32 dwHeight;              // Fields specify the height of the AVIfile in pixels.
    Ogre::uint32 dwReserved[4];         // 0, 0, 0, 0
};

struct AviStreamHeader
{
    Ogre::uint32 fccType;              // 'vids', 'auds', 'txts'...
    Ogre::uint32 fccHandler;           // "cvid", "DIB "
    Ogre::uint32 dwFlags;               // 0
    Ogre::uint32 dwPriority;            // 0
    Ogre::uint32 dwInitialFrames;       // 0
    Ogre::uint32 dwScale;               // 1
    Ogre::uint32 dwRate;                // Fps (dwRate - frame rate for video streams)
    Ogre::uint32 dwStart;               // 0
    Ogre::uint32 dwLength;              // Frames number (playing time of AVI file as defined by scale and rate)
    Ogre::uint32 dwSuggestedBufferSize; // For reading the stream
    Ogre::uint32 dwQuality;             // -1 (encoding quality. If set to -1, drivers use the default quality value)
    Ogre::uint32 dwSampleSize;          // 0 means that each frame is in its own chunk
    struct {
        short int left;
        short int top;
        short int right;
        short int bottom;
    } rcFrame;                // If stream has a different size than dwWidth*dwHeight(unused)
};

struct AviIndex
{
    Ogre::uint32 ckid;
    Ogre::uint32 dwFlags;
    Ogre::uint32 dwChunkOffset;
    Ogre::uint32 dwChunkLength;
};

struct BitmapInfoHeader
{
    Ogre::uint32 biSize;                // Write header size of BITMAPINFO header structure
    Ogre::int32  biWidth;               // width in pixels
    Ogre::int32  biHeight;              // height in pixels
    Ogre::uint16  biPlanes;              // Number of color planes in which the data is stored
    Ogre::uint16  biBitCount;            // Number of bits per pixel
    Ogre::uint32 biCompression;         // Type of compression used (uncompressed: NO_COMPRESSION=0)
    Ogre::uint32 biSizeImage;           // Image Buffer. Quicktime needs 3 bytes also for 8-bit png
                                    //   (biCompression==NO_COMPRESSION)?0:xDim*yDim*bytesPerPixel;
    Ogre::int32  biXPelsPerMeter;       // Horizontal resolution in pixels per meter
    Ogre::int32  biYPelsPerMeter;       // Vertical resolution in pixels per meter
    Ogre::uint32 biClrUsed;             // 256 (color table size; for 8-bit only)
    Ogre::uint32 biClrImportant;        // Specifies that the first x colors of the color table. Are important to the DIB.
};

struct RiffChunk
{
    Ogre::uint32 m_four_cc;
    Ogre::uint32 m_size;
};

struct RiffList
{
    Ogre::uint32 m_riff_or_list_cc;
    Ogre::uint32 m_size;
    Ogre::uint32 m_list_type_cc;
};

class VideoInputStream
{
public:
    //VideoInputStream();
    //VideoInputStream(const std::string& filename);
    VideoInputStream(Ogre::DataStreamPtr stream);
    ~VideoInputStream();
    VideoInputStream& read(Ogre::uint8*, Ogre::uint64);
    VideoInputStream& seekg(Ogre::uint64);
    Ogre::uint64 tellg();
    //bool isOpened() const;
    //bool open(const std::string& filename);
    void close();
    operator bool();

private:
    VideoInputStream(const VideoInputStream&);
    VideoInputStream& operator=(const VideoInputStream&);

private:
    Ogre::DataStreamPtr input;
    bool    m_is_valid;
    //std::string  m_fname;
};
#pragma pack(pop)

inline VideoInputStream& operator >> (VideoInputStream& is, AviMainHeader& avih)
{
    is.read((Ogre::uint8*)(&avih), sizeof(AviMainHeader));
    return is;
}
inline VideoInputStream& operator >> (VideoInputStream& is, AviStreamHeader& strh)
{
    is.read((Ogre::uint8*)(&strh), sizeof(AviStreamHeader));
    return is;
}
inline VideoInputStream& operator >> (VideoInputStream& is, BitmapInfoHeader& bmph)
{
    is.read((Ogre::uint8*)(&bmph), sizeof(BitmapInfoHeader));
    return is;
}
inline VideoInputStream& operator >> (VideoInputStream& is, AviIndex& idx1)
{
    is.read((Ogre::uint8*)(&idx1), sizeof(idx1));
    return is;
}

inline VideoInputStream& operator >> (VideoInputStream& is, RiffChunk& riff_chunk)
{
    is.read((Ogre::uint8*)(&riff_chunk), sizeof(riff_chunk));
    return is;
}

inline VideoInputStream& operator >> (VideoInputStream& is, RiffList& riff_list)
{
    is.read((Ogre::uint8*)(&riff_list), sizeof(riff_list));
    return is;
}

static const int AVIH_STRH_SIZE = 56;
static const int STRF_SIZE = 40;
static const int AVI_DWFLAG = 0x00000910;
static const int AVI_DWSCALE = 1;
static const int AVI_DWQUALITY = -1;
static const int JUNK_SEEK = 4096;
static const int AVIIF_KEYFRAME = 0x10;
static const int MAX_BYTES_PER_SEC = 99999999;
static const int SUG_BUFFER_SIZE = 1048576;
/*
VideoInputStream::VideoInputStream() : m_is_valid(false)
{
    m_fname = std::string();
}

VideoInputStream::VideoInputStream(const std::string& filename) : m_is_valid(false)
{
    m_fname = filename;
    open(filename);
}*/

VideoInputStream::VideoInputStream(Ogre::DataStreamPtr stream) : input(stream), m_is_valid(true) {}
/*
bool VideoInputStream::isOpened() const
{
    return input.is_open();
}

bool VideoInputStream::open(const std::string& filename)
{
    close();
    input.open(filename.c_str(), std::ios_base::binary);
    m_is_valid = isOpened();
    return m_is_valid;
}*/

void VideoInputStream::close()
{
    //if (isOpened())
    {
        m_is_valid = false;
        input->close();
    }
}

VideoInputStream& VideoInputStream::read(Ogre::uint8* buf, Ogre::uint64 count)
{
    //if (isOpened())
    {
        input->read(buf, static_cast<std::streamsize>(count));
        //m_is_valid = (input->gcount() == (std::streamsize)count);
    }

    return *this;
}

VideoInputStream& VideoInputStream::seekg(Ogre::uint64 pos)
{
    //input.clear();
    input->seek(static_cast<std::streamoff>(pos));
    //m_is_valid = !input.eof();
    return *this;
}

Ogre::uint64 VideoInputStream::tellg()
{
    return input->tell();
}

VideoInputStream::operator bool()
{
    return m_is_valid;
}

VideoInputStream::~VideoInputStream()
{
    close();
}


AVIReadContainer::AVIReadContainer() : m_stream_id(0), m_movi_start(0), m_movi_end(0), m_width(0), m_height(0), m_fps(0), m_is_indx_present(false)
{
    //m_file_stream = std::make_shared<VideoInputStream>();
    //m_file_stream = CommonIncludes::shared_ptr<VideoInputStream>(new VideoInputStream);
}
/*
void AVIReadContainer::initStream(const std::string &filename)
{
    //m_file_stream = std::make_shared<VideoInputStream>(filename);
    m_file_stream = CommonIncludes::shared_ptr<VideoInputStream>(new VideoInputStream(filename));
}
*/
void AVIReadContainer::initStream(Ogre::DataStreamPtr m_file_stream_)
{
    m_file_stream = CommonIncludes::shared_ptr<VideoInputStream>(new VideoInputStream(m_file_stream_));;
}

void AVIReadContainer::close()
{
    m_file_stream->close();
}

bool AVIReadContainer::parseIndex(Ogre::uint32 index_size, FrameList& in_frame_list, FrameDescription& frame_description)
{
    Ogre::uint64 index_end = m_file_stream->tellg();
    index_end += index_size;
    bool result = false;

    std::vector<std::string> ckids;

    while (m_file_stream && (m_file_stream->tellg() < index_end))
    {
        AviIndex idx1;
        *m_file_stream >> idx1;

        if (idx1.ckid == m_stream_id)
        {
            Ogre::uint64 absolute_pos = m_movi_start + idx1.dwChunkOffset;

            if (absolute_pos < m_movi_end)
            {
                frame_description.push_back(idx1.ckid);
                in_frame_list.push_back(std::make_pair(absolute_pos, idx1.dwChunkLength));
            }
            else
            {
                //unsupported case
                fprintf(stderr, "Frame offset points outside movi section.\n");
            }
        }

        result = true;
    }

    return result;
}

bool AVIReadContainer::parseStrl(char stream_id, Codecs codec_)
{
    RiffChunk strh;
    *m_file_stream >> strh;

    if (m_file_stream && strh.m_four_cc == STRH_CC)
    {
        Ogre::uint64 next_strl_list = m_file_stream->tellg();
        next_strl_list += strh.m_size;

        AviStreamHeader strm_hdr;
        *m_file_stream >> strm_hdr;

        if (codec_ == CINEPAK)
        {
            if (strm_hdr.fccType == VIDS_CC && strm_hdr.fccHandler == CINEPAK_CC)//video stream
            {
                Ogre::uint8 first_digit = (stream_id / 10) + '0';
                Ogre::uint8 second_digit = (stream_id % 10) + '0';

                if (m_stream_id == 0)
                {
                    m_stream_id = CV_FOURCC(first_digit, second_digit, 'd', 'c');
                    m_fps = double(strm_hdr.dwRate) / strm_hdr.dwScale;
                }
                else
                {
                    //second mjpeg video stream found which is not supported
                    fprintf(stderr, "More than one video stream found within AVI/AVIX list. Stream %c%cdc would be ignored\n", first_digit, second_digit);
                }

                return true;
            }

            if (strm_hdr.fccType == AUDS_CC && strm_hdr.fccHandler == ADAPCM_CC)//audio stream
            {
                return true;
            }
        }
    }

    return false;
}

void AVIReadContainer::skipJunk(RiffChunk& chunk)
{
    if (chunk.m_four_cc == JUNK_CC)
    {
        m_file_stream->seekg(m_file_stream->tellg() + chunk.m_size);
        *m_file_stream >> chunk;
    }
}

void AVIReadContainer::skipJunk(RiffList& list)
{
    if (list.m_riff_or_list_cc == JUNK_CC)
    {
        //JUNK chunk is 4 bytes less than LIST
        m_file_stream->seekg(m_file_stream->tellg() + list.m_size - 4);
        *m_file_stream >> list;
    }
}

bool AVIReadContainer::parseHdrlList(Codecs codec_)
{
    bool result = false;

    RiffChunk avih;
    *m_file_stream >> avih;

    if (m_file_stream && avih.m_four_cc == AVIH_CC)
    {
        Ogre::uint64 next_strl_list = m_file_stream->tellg();
        next_strl_list += avih.m_size;

        AviMainHeader avi_hdr;
        *m_file_stream >> avi_hdr;

        if (m_file_stream)
        {
            m_is_indx_present = ((avi_hdr.dwFlags & 0x10) != 0);
            Ogre::uint32 number_of_streams = avi_hdr.dwStreams;
            assert(number_of_streams < 0xFF);
            m_width = avi_hdr.dwWidth;
            m_height = avi_hdr.dwHeight;

            //the number of strl lists must be equal to number of streams specified in main avi header
            for (Ogre::uint32 i = 0; i < number_of_streams; ++i)
            {
                m_file_stream->seekg(next_strl_list);
                RiffList strl_list;
                *m_file_stream >> strl_list;

                if (m_file_stream && strl_list.m_riff_or_list_cc == LIST_CC && strl_list.m_list_type_cc == STRL_CC)
                {
                    next_strl_list = m_file_stream->tellg();
                    //RiffList::m_size includes fourCC field which we have already read
                    next_strl_list += (strl_list.m_size - 4);

                    result = parseStrl((char)i, codec_);
                }
                else
                {
                    printError(strl_list, STRL_CC);
                }
            }
        }
    }
    else
    {
        printError(avih, AVIH_CC);
    }

    return result;
}

bool AVIReadContainer::parseAviWithFrameList(FrameList& in_frame_list, FrameDescription& frame_description, Codecs codec_)
{
    RiffList hdrl_list;
    *m_file_stream >> hdrl_list;

    if (m_file_stream && hdrl_list.m_riff_or_list_cc == LIST_CC && hdrl_list.m_list_type_cc == HDRL_CC)
    {
        Ogre::uint64 next_list = m_file_stream->tellg();
        //RiffList::m_size includes fourCC field which we have already read
        next_list += (hdrl_list.m_size - 4);
        //parseHdrlList sets m_is_indx_present flag which would be used later
        if (parseHdrlList(codec_))
        {
            m_file_stream->seekg(next_list);

            RiffList some_list;
            *m_file_stream >> some_list;

            //an optional section INFO
            if (m_file_stream && some_list.m_riff_or_list_cc == LIST_CC && some_list.m_list_type_cc == INFO_CC)
            {
                next_list = m_file_stream->tellg();
                //RiffList::m_size includes fourCC field which we have already read
                next_list += (some_list.m_size - 4);
                //not implemented
                //parseInfo();

                m_file_stream->seekg(next_list);
                *m_file_stream >> some_list;
            }

            //an optional section JUNK
            skipJunk(some_list);

            //we are expecting to find here movi list. Must present in avi
            if (m_file_stream && some_list.m_riff_or_list_cc == LIST_CC && some_list.m_list_type_cc == MOVI_CC)
            {
                bool is_index_found = false;

                m_movi_start = m_file_stream->tellg();
                m_movi_start -= 4;

                m_movi_end = m_movi_start + some_list.m_size;
                //if m_is_indx_present is set to true we should find index
                if (m_is_indx_present)
                {
                    //we are expecting to find index section after movi list
                    Ogre::uint32 indx_pos = (Ogre::uint32)m_movi_start + 4;
                    indx_pos += (some_list.m_size - 4);
                    m_file_stream->seekg(indx_pos);

                    RiffChunk index_chunk;
                    *m_file_stream >> index_chunk;

                    if (m_file_stream && index_chunk.m_four_cc == IDX1_CC)
                    {
                        is_index_found = parseIndex(index_chunk.m_size, in_frame_list, frame_description);
                        //we are not going anywhere else
                    }
                    else
                    {
                        printError(index_chunk, IDX1_CC);
                    }
                }
                //index not present or we were not able to find it
                //parsing movi list
                if (!is_index_found)
                {
                    //not implemented
                    //parseMovi(in_frame_list);

                    fprintf(stderr, "Failed to parse avi: index was not found\n");
                    //we are not going anywhere else
                }
            }
            else
            {
                printError(some_list, MOVI_CC);
            }
        }
    }
    else
    {
        printError(hdrl_list, HDRL_CC);
    }

    return in_frame_list.size() > 0;
}

std::vector<Ogre::uint8> AVIReadContainer::readFrame(FrameIterator it)
{
    m_file_stream->seekg(it->first);

    RiffChunk chunk;
    *(m_file_stream) >> chunk;

    // Assertion added to prevent complaints from static analysis tools
    // as the chunk size is read from a file then used to allocate
    // memory. 64MB was chosen arbitrarily as an upper bound but it may
    // be useful to make it configurable.
    assert(chunk.m_size <= 67108864);

    std::vector<Ogre::uint8> result;

    result.reserve(chunk.m_size);
    result.resize(chunk.m_size);

    m_file_stream->read(&(result[0]), chunk.m_size); // result.data() failed with MSVS2008

    return result;
}

bool AVIReadContainer::parseRiff(FrameList &m_mjpeg_frames_, FrameDescription& frame_description)
{
    bool result = false;
    while (*m_file_stream)
    {
        RiffList riff_list;

        *m_file_stream >> riff_list;

        if (*m_file_stream && riff_list.m_riff_or_list_cc == RIFF_CC &&
            ((riff_list.m_list_type_cc == AVI_CC) | (riff_list.m_list_type_cc == AVIX_CC)))
        {
            Ogre::uint64 next_riff = m_file_stream->tellg();
            //RiffList::m_size includes fourCC field which we have already read
            next_riff += (riff_list.m_size - 4);

            bool is_parsed = parseAvi(m_mjpeg_frames_, frame_description, CINEPAK);
            result = result || is_parsed;
            m_file_stream->seekg(next_riff);
        }
        else
        {
            break;
        }
    }
    return result;
}

void AVIReadContainer::printError(RiffList &list, Ogre::uint32 expected_fourcc)
{
    if (!m_file_stream)
    {
        printf("Unexpected end of file while searching for %s list\n", fourccToString(expected_fourcc).c_str());
    }
    else if (list.m_riff_or_list_cc != LIST_CC)
    {
        printf("Unexpected element. Expected: %s. Got: %s.\n", fourccToString(LIST_CC).c_str(), fourccToString(list.m_riff_or_list_cc).c_str());
    }
    else
    {
        printf("Unexpected list type. Expected: %s. Got: %s.\n", fourccToString(expected_fourcc).c_str(), fourccToString(list.m_list_type_cc).c_str());
    }
}

void AVIReadContainer::printError(RiffChunk &chunk, Ogre::uint32 expected_fourcc)
{
    if (!m_file_stream)
    {
        printf("Unexpected end of file while searching for %s chunk\n", fourccToString(expected_fourcc).c_str());
    }
    else
    {
        printf("Unexpected element. Expected: %s. Got: %s.\n", fourccToString(expected_fourcc).c_str(), fourccToString(chunk.m_four_cc).c_str());
    }
}