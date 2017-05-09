/**
* MultiSlider library
*
* The MIT License (MIT)
* Copyright (c) 2016 Alexey Gruzdev
*/

#if defined(_MSC_VER) && (_MSC_VER < 1800)
# include "CommonIncludes.h"
# ifndef _STDINT_H_
#  define _STDINT_H_
# endif
#endif
#include "UdpInterface.h"

namespace multislider
{
    const size_t UdpInterface::MAX_BUFFER_SIZE = 4 * 1024 * 1024; // 4 Mb
}

