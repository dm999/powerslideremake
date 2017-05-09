/**
* MultiSlider library
*
* The MIT License (MIT)
* Copyright (c) 2016 Alexey Gruzdev
*/

#ifndef _MULTI_SLIDER_EXCEPTION_H_
#define _MULTI_SLIDER_EXCEPTION_H_

#include <stdexcept>

namespace multislider
{

    class RuntimeError
        : public std::runtime_error
    {
    public:
        RuntimeError(const std::string & msg)
            : std::runtime_error(msg)
        { }

        RuntimeError(const char* msg)
            : std::runtime_error(msg)
        { }

        virtual ~RuntimeError()
        { }
    };

    class NetworkError
        : public RuntimeError
    {
    public:
        NetworkError(const std::string & msg)
            : RuntimeError(msg)
        { }

        NetworkError(const char* msg)
            : RuntimeError(msg)
        { }
    };

    class ProtocolError
        : public RuntimeError
    {
    public:
        ProtocolError(const std::string & msg)
            : RuntimeError(msg)
        { }

        ProtocolError(const char* msg)
            : RuntimeError(msg)
        { }
    };

    class ServerError
        : public RuntimeError
    {
    public:
        ServerError(const std::string & msg)
            : RuntimeError(msg)
        { }

        ServerError(const char* msg)
            : RuntimeError(msg)
        { }
    };
}

#endif
