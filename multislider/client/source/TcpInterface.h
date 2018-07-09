/**
* MultiSlider library
*
* The MIT License (MIT)
* Copyright (c) 2016 Alexey Gruzdev
*/

#ifndef _MULTI_SLIDER_TCP_INTERFACE_H_
#define _MULTI_SLIDER_TCP_INTERFACE_H_

#include <SFML/Network.hpp>

#include "CommonIncludes.h"
#include "Utility.h"

#if defined(__ANDROID__)
    #include <android/log.h>

    #define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO, "OGRE", __VA_ARGS__))
    #define LOGE(...) ((void)__android_log_print(ANDROID_LOG_ERROR, "OGRE", __VA_ARGS__)) 
#endif

namespace multislider
{
    class TcpInterface
    {
    public:
        const static size_t MAX_BUFFER_SIZE = 4 * 1024 * 1024; // 4 Mb

    private:
        sf::TcpSocket mSocket;

        TcpInterface(const TcpInterface &);
        TcpInterface & operator= (const TcpInterface &);

    public:
        TcpInterface(){ }

        bool connect(const std::string & ip, uint16_t port)
        {
#if defined(__ANDROID__)
            LOGI("TcpInterface[connect]: begin");
#endif

            sf::IpAddress serverAddress(ip);

            if(serverAddress == sf::IpAddress::None)
            {
                return false;
            }

            if (mSocket.connect(serverAddress, port) != sf::Socket::Done)
            {
                return false;
            }

            mSocket.setBlocking(false);

#if defined(__ANDROID__)
            LOGI("TcpInterface[connect]: end"); 
#endif

            return true;
        }

        bool send(const std::string & message)
        {
            bool ret = true;

            sf::Socket::Status status;

            std::vector<char> buf(message.begin(), message.end());
            size_t offset = 0;
            do
            {
                size_t sent;
                status = mSocket.send(&buf[offset], buf.size(), sent);
                offset += sent;
            }while(status == sf::Socket::Partial);

            if (status != sf::Socket::Done)
            {
                ret = false;
            }
            return ret;
        }

        std::string tryReceive(uint64_t timeoutMilliseconds, uint32_t attemptsTimeoutMilliseconds = 100)
        {
            std::string message;
            sf::Socket::Status status;
            uint64_t time = 0;
            while (time <= timeoutMilliseconds) {

                std::vector<char> buf(MAX_BUFFER_SIZE, 0);
                size_t recievedTotal = 0;
                size_t recieved;
                size_t offset = 0;
                do
                {
                    status = mSocket.receive(&buf[offset], MAX_BUFFER_SIZE, recieved);
                    offset += recieved;
                    recievedTotal += recieved;
                }while(status == sf::Socket::Partial);

                if (status == sf::Socket::Done)
                {
                    message = std::string(buf.begin(), buf.begin() + recievedTotal);
                    break;
                }

                if (timeoutMilliseconds > 0)
                {
                    sf::sleep(sf::milliseconds(attemptsTimeoutMilliseconds));
                    time += attemptsTimeoutMilliseconds;
                }
                else
                {
                    break;
                }
            }
            return message;
        }
    };

}

#endif
