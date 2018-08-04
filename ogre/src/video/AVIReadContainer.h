
#ifndef AVI_READ_CONTAINER_H
#define AVI_READ_CONTAINER_H

#include "../includes/CommonIncludes.h"
#include "../includes/OgreInclude.h"

#include "AVIFrameDescription.h"

//https://github.com/opencv/opencv/blob/master/modules/videoio/src/container_avi.cpp
//https://github.com/opencv/opencv/blob/master/modules/videoio/include/opencv2/videoio/container_avi.private.hpp

#define CV_FOURCC_MACRO(c1, c2, c3, c4) (((c1) & 255) + (((c2) & 255) << 8) + (((c3) & 255) << 16) + (((c4) & 255) << 24))

inline int CV_FOURCC(char c1, char c2, char c3, char c4)
{
    return CV_FOURCC_MACRO(c1, c2, c3, c4);
}

struct RiffChunk;
struct RiffList;
class VideoInputStream;

enum Codecs { CINEPAK };

class AVIReadContainer
{
public:
    AVIReadContainer();

    //void initStream(const std::string& filename);
    void initStream(Ogre::DataStreamPtr m_file_stream_);

    void close();
    //stores founded frames in m_frame_list which can be accessed via getFrames
    bool parseAvi(Codecs codec_) { return parseAviWithFrameList(m_frame_list, m_frame_description, codec_); }
    //stores founded frames in in_frame_list. getFrames() would return empty list
    bool parseAvi(FrameList& in_frame_list, FrameDescription& frame_description, Codecs codec_) { return parseAviWithFrameList(in_frame_list, frame_description, codec_); }
    size_t getFramesCount() { return m_frame_list.size(); }
    FrameList& getFrames() { return m_frame_list; }
    unsigned int getWidth() { return m_width; }
    unsigned int getHeight() { return m_height; }
    double getFps() { return m_fps; }
    std::vector<Ogre::uint8> readFrame(FrameIterator it);
    bool parseRiff(FrameList &m_mjpeg_frames, FrameDescription& frame_description);

    static const Ogre::uint32 VIDEOFRAME_CC;
    static const Ogre::uint32 AUDIOFRAME_CC;


protected:

    bool parseAviWithFrameList(FrameList& in_frame_list, FrameDescription& frame_description, Codecs codec_);
    void skipJunk(RiffChunk& chunk);
    void skipJunk(RiffList& list);
    bool parseHdrlList(Codecs codec_);
    bool parseIndex(unsigned int index_size, FrameList& in_frame_list, FrameDescription& frame_description);
    bool parseStrl(char stream_id, Codecs codec_);

    void printError(RiffList& list, unsigned int expected_fourcc);

    void printError(RiffChunk& chunk, unsigned int expected_fourcc);

    CommonIncludes::shared_ptr<VideoInputStream> m_file_stream;
    unsigned int   m_stream_id;
    unsigned long long int   m_movi_start;
    unsigned long long int    m_movi_end;
    FrameList m_frame_list;
    FrameDescription m_frame_description;
    unsigned int   m_width;
    unsigned int   m_height;
    double     m_fps;
    bool       m_is_indx_present;
};


#endif