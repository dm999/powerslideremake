/**
* MultiSlider library
*
* The MIT License (MIT)
* Copyright (c) 2016 Alexey Gruzdev
*/

#ifndef _MULTI_SLIDER_COMMON_INCLUDES_H_
#define _MULTI_SLIDER_COMMON_INCLUDES_H_

#include <cassert>
#include <string>
#include <memory>

#if defined(_MSC_VER) && (_MSC_VER < 1800)
typedef          __int8  int8_t;
typedef          __int16 int16_t;
typedef          __int32 int32_t;
typedef          __int64 int64_t;
typedef unsigned __int8  uint8_t;
typedef unsigned __int16 uint16_t;
typedef unsigned __int32 uint32_t;
typedef unsigned __int64 uint64_t;
#else
#include <cstdint>
#endif

#if defined(_MSC_VER) && (_MSC_VER < 1800)
namespace multislider
{
    using std::tr1::shared_ptr;
}
#else
namespace multislider
{
    using std::shared_ptr;
}
#endif

namespace multislider
{
    class RoomInfo;
    class Lobby;
    class Session;

    typedef shared_ptr<Session> SessionPtr;
}

#endif
