/**
* MultiSlider library
*
* The MIT License (MIT)
* Copyright (c) 2016 Alexey Gruzdev
*/

#ifndef _MULTI_SLIDER_UTILITY_H_
#define _MULTI_SLIDER_UTILITY_H_

#include <cstring>

#include "CommonIncludes.h"
#include "TCPInterface.h"
#include "RakSleep.h"

namespace multislider
{

    namespace details
    {
        // Deleter for packet
        template <typename _ConnectionPtr>
        struct PacketDeleter
        {
            _ConnectionPtr mConnection;
            PacketDeleter(const _ConnectionPtr & connection)
                : mConnection(connection)
            {
                assert(mConnection != NULL);
            }

            void operator()(RakNet::Packet * packet) const
            {
                if (packet != NULL) {
                    mConnection->DeallocatePacket(packet);
                }
            }
        };
    }

    /**
     *  Blocking receive 
     *  http://www.raknet.com/forum/index.php?topic=587.0
     */
    template <typename _ConnectionInterfacePtr>
    inline
    shared_ptr<RakNet::Packet> awaitResponse(const _ConnectionInterfacePtr & connection, uint64_t timeoutMilliseconds, uint32_t attemptsTimeoutMilliseconds = 100)
    {
        RakNet::Packet* packet(NULL);
        uint64_t time = 0;
        for (;;) {
            packet = connection->Receive();
            if (packet != NULL || time >= timeoutMilliseconds) {
                break;
            }
            RakSleep(attemptsTimeoutMilliseconds);
            time += attemptsTimeoutMilliseconds;
        }
        return shared_ptr<RakNet::Packet>(packet, details::PacketDeleter<_ConnectionInterfacePtr>(connection));
    }

    /**
     *  Non-bocking attempt to receive a package
     */
    template <typename _ConnectionInterfacePtr>
    inline
    shared_ptr<RakNet::Packet> awaitResponse(const _ConnectionInterfacePtr & connection)
    {
        return shared_ptr<RakNet::Packet>(connection->Receive(), details::PacketDeleter<_ConnectionInterfacePtr>(connection));
    }

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
    bool responsed(const RakNet::Packet* packet, const char* expected)
    {
        assert(std::strlen(expected) == 4);
        return (packet != NULL) && 
            (packet->length == 4) &&
            (*pointer_cast<const uint32_t*>(packet->data) == *pointer_cast<const uint32_t*>(expected));
    }

    /**
     *  Compare default server response
     *  @param bytes response bytes, should be at least 4 bytes
     *  @param expected expected response, should be exactly 4 bytes
     */
    inline
    bool responsed(const shared_ptr<RakNet::Packet> & packet, const char* expected)
    {
        return responsed(packet.get(), expected);
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
}


#endif
