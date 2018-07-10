/**
* MultiSlider library
*
* The MIT License (MIT)
* Copyright (c) 2016 Alexey Gruzdev
*/

#ifndef _MULTI_SLIDER_UDP_INTERFACE_H_
#define _MULTI_SLIDER_UDP_INTERFACE_H_

#include <SFML/Network.hpp>

#include "CommonIncludes.h"
#include "Utility.h"
#include "Exception.h"

namespace multislider
{
    class UdpInterface
    {
    public:

    private:

        sf::UdpSocket mSocket;
        std::string mIP;
        uint16_t mPort;

        UdpInterface(const UdpInterface &);
        UdpInterface & operator= (const UdpInterface &);

    public:
        UdpInterface(){ }

        bool open(const std::string & ip, uint16_t port)
        {
            mIP = ip;
            mPort = port;
            sf::IpAddress serverAddress(mIP);

            if(serverAddress == sf::IpAddress::None)
            {
                return false;
            }

            if (mSocket.bind(sf::Socket::AnyPort) != sf::Socket::Done)
            {
                return false;
            }

            mSocket.setBlocking(false);

            return true;
        }

        bool sendUpdDatagram(const std::string & message)
        {
            bool ret = true;

            sf::Socket::Status status;
            sf::IpAddress serverAddress(mIP);

            std::vector<char> buf(message.begin(), message.end());
            if(buf.size() > sf::UdpSocket::MaxDatagramSize)
            {
                throw RuntimeError("UdpInterface[sendUpdDatagram]: to large datagram!");
                //buf.resize(sf::UdpSocket::MaxDatagramSize);
            }
            status = mSocket.send(&buf[0], buf.size(), serverAddress, mPort);

            if (status != sf::Socket::Done)
            {
                ret = false;
            }

            return ret;
        }

        std::string awaitUdpDatagram(uint64_t timeoutMilliseconds, uint32_t attemptsTimeoutMilliseconds = 100)
        {
            std::string message;
            sf::Socket::Status status;
            uint64_t time = 0;
            while (time <= timeoutMilliseconds) {

                sf::IpAddress serverAddress;
                uint16_t port;

                std::vector<char> buf(sf::UdpSocket::MaxDatagramSize, 0);
                size_t recievedTotal = 0;
                size_t recieved;
                size_t offset = 0;
                do
                {
                    status = mSocket.receive(&buf[offset], sf::UdpSocket::MaxDatagramSize, recieved, serverAddress, port);
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
