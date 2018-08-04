
#ifndef AVI_FRAME_DESCRIPTION_H
#define AVI_FRAME_DESCRIPTION_H

#include <queue>
#include <utility>

typedef std::deque<std::pair<Ogre::uint64, Ogre::uint32>> FrameList;
typedef std::deque<Ogre::uint32> FrameDescription;
typedef FrameList::iterator FrameIterator;

#endif