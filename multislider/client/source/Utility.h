/**
* MultiSlider library
*
* The MIT License (MIT)
* Copyright (c) 2016 Alexey Gruzdev
*/

#ifndef _MULTI_SLIDER_UTILITY_H_
#define _MULTI_SLIDER_UTILITY_H_

#include <cstring>
#include <sstream>

#include "CommonIncludes.h"

namespace multislider
{

    /**
     *  Safe cast for pointers
     */
    template <typename _TypeTo, typename _TypeFrom>
    inline 
    _TypeTo pointer_cast(_TypeFrom* ptr)
    {
        return static_cast<_TypeTo>(static_cast<void*>(ptr));
    }

    /**
     *  Safe cast for pointers
     */
    template <typename _TypeTo, typename _TypeFrom>
    inline 
    _TypeTo pointer_cast(const _TypeFrom* ptr)
    {
        return static_cast<_TypeTo>(static_cast<const void*>(ptr));
    }

    /**
    *  Compare default server response
    *  @param bytes response bytes, should be at least 4 bytes
    *  @param expected expected response, should be exactly 4 bytes
    */
    inline
    bool responsed(const uint8_t* bytes, size_t length, const char* expected)
    {
        assert(std::strlen(expected) == 4);
        return (bytes != NULL) &&
            (length == 4) &&
            (*pointer_cast<const uint32_t*>(bytes) == *pointer_cast<const uint32_t*>(expected));
    }

    /**
    *  Compare default server response
    *  @param bytes response bytes, should be at least 4 bytes
    *  @param expected expected response, should be exactly 4 bytes
    */
    inline
    bool responsed(const std::string & msg, const char* expected)
    {
        return responsed(pointer_cast<const uint8_t*>(&msg[0]), msg.size(), expected);
    }

    /**
     *  Compare message class
     */
    inline 
    bool isMessageClass(const std::string & classString, const char* expected)
    {
        return 0 == std::strcmp(classString.c_str(), expected);
    }

    /**
     *  Compare message class
     */
    inline
    bool isMessageClass(const uint8_t* bytes, size_t length, const char* expected)
    {
        return 0 == std::strncmp(pointer_cast<const char*>(bytes), expected, length);
    }

    /**
     *  Get size of a C-style array
     */
    template <typename _Type, size_t _Size>
    inline
    size_t arrayLengh(const _Type(&)[_Size])
    {
        return _Size;
    }

    /**
     *  Checked cast
     */
    template<typename _TypeTo, typename _TypeFrom>
    inline
    _TypeTo narrow_cast(const _TypeFrom & val)
    {
        assert(static_cast<_TypeFrom>(static_cast<_TypeTo>(val)) == val);
        return static_cast<_TypeTo>(val);
    }


    template <typename T>
    std::string to_string(const T & value)
    {
        std::stringstream ss;
        ss << value;
        return ss.str();
    }
}


#endif
