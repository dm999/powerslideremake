/**
* MultiSlider library
*
* The MIT License (MIT)
* Copyright (c) 2016 Alexey Gruzdev
*/

#ifndef _MULTI_SLIDER_UDP_INTERFACE_H_
#define _MULTI_SLIDER_UDP_INTERFACE_H_

#include <vector>

#include <enet/enet.h>
#include <RakSleep.h>

#include "CommonIncludes.h"
#include "Utility.h"

namespace multislider
{
    class UdpSocket
    {
        ENetSocket mEnetSocket;

    public:
        UdpSocket()
        {
            mEnetSocket = enet_socket_create(ENET_SOCKET_TYPE_DATAGRAM);
        }

        ~UdpSocket()
        {
            enet_socket_destroy(mEnetSocket);
        }

        ENetSocket & get()
        {
            return mEnetSocket;
        }

        operator ENetSocket()
        {
            return mEnetSocket;
        }
    };


    /**
     *  Singleton wrapper for ENet library
     */
    class UdpInterface
    {
    private:
        static UdpInterface msInstance;

        UdpInterface()
        {
            enet_initialize();
        }

        ~UdpInterface()
        {
            enet_deinitialize();
        }

        UdpInterface(const UdpInterface &);
        UdpInterface & operator= (const UdpInterface &);

    public:
        static UdpInterface & Instance()
        {
            return msInstance;
        }

        bool sendUpdDatagram(UdpSocket & socket, const::std::string & ip, uint16_t port, const std::string & message)
        {
            ENetAddress address;
            enet_address_set_host(&address, ip.c_str());
            address.port = port;

            ENetBuffer buffer;
            buffer.data = const_cast<void*>(pointer_cast<const void*>(message.c_str()));
            buffer.dataLength = message.size();
            return (enet_socket_send(socket, &address, &buffer, 1) > 0);
        }

        size_t awaitUdpDatagram(UdpSocket & socket, std::vector<uint8_t> & receiveBuffer, uint64_t timeoutMilliseconds, uint32_t attemptsTimeoutMilliseconds = 100)
        {
            ENetAddress address;
            ENetBuffer buffer;
            buffer.data = &receiveBuffer[0];
            buffer.dataLength = receiveBuffer.size();
            uint64_t time = 0;

            int len = 0;
            while (time < timeoutMilliseconds) {
                len = enet_socket_receive(socket, &address, &buffer, 1);
                if (len != 0) {
                    break;
                }
                RakSleep(attemptsTimeoutMilliseconds);
                time += attemptsTimeoutMilliseconds;
            }

            if (len < 0) {
                return 0;
            }
            else {
                return static_cast<size_t>(len);
            }
        }
    };

}

#endif
