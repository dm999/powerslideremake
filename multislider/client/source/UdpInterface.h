/**
* MultiSlider library
*
* The MIT License (MIT)
* Copyright (c) 2016 Alexey Gruzdev
*/

#ifndef _MULTI_SLIDER_UDP_INTERFACE_H_
#define _MULTI_SLIDER_UDP_INTERFACE_H_

#include <SFML/Network.hpp>

#include <boost/asio.hpp>

#include "CommonIncludes.h"
#include "Utility.h"
#include "Exception.h"

namespace multislider
{
    /**
     *  Singleton wrapper for ENet library
     */
    class UdpInterface
    {
    public:
        const static size_t MAX_BUFFER_SIZE;

    private:

        sf::UdpSocket mSocket;
        std::string mIP;
        uint16_t mPort;

        boost::asio::io_service mIoService;
        boost::asio::ip::udp::socket mAsioSocket;
        boost::asio::ip::udp::endpoint mEndpoint;

        boost::asio::streambuf mStreamBuffer;
        std::istream mReceiveStream;

        UdpInterface(const UdpInterface &);
        UdpInterface & operator= (const UdpInterface &);

    public:
        UdpInterface()
            : mIoService(), mAsioSocket(mIoService), mStreamBuffer(MAX_BUFFER_SIZE), mReceiveStream(&mStreamBuffer)
        { }

        bool open(const std::string & ip, uint16_t port)
        {
#if 1
            mIP = ip;
            mPort = port;
            sf::IpAddress serverAddress(mIP);

            if(serverAddress == sf::IpAddress::None)
            {
                return false;
            }

            if (mSocket.bind(port) != sf::Socket::Done)
            {
                return false;
            }

            mSocket.setBlocking(false);

            return true;
#else
            boost::system::error_code err;
            boost::asio::ip::udp::resolver resolver(mIoService);
            boost::asio::ip::udp::resolver::query query(boost::asio::ip::udp::v4(), ip, to_string(port));
            mEndpoint = *resolver.resolve(query, err);
            if (err) {
                return false;
            }
            mAsioSocket.open(boost::asio::ip::udp::v4(), err);
            return !err;
#endif
        }

        bool sendUpdDatagram(const std::string & message)
        {
#if 1
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
#else
            boost::system::error_code err;
            size_t len = mAsioSocket.send_to(boost::asio::buffer(message), mEndpoint, 0, err);
            return !err && (len == message.size());
#endif
        }

        std::string awaitUdpDatagram(uint64_t timeoutMilliseconds, uint32_t attemptsTimeoutMilliseconds = 100)
        {
            std::string message;
#if 1
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
#else
            uint64_t time = 0;
            while (time <= timeoutMilliseconds) {
                size_t available = mAsioSocket.available();
                if (available > 0) {
                    boost::asio::ip::udp::endpoint sender;
                    size_t len = mAsioSocket.receive_from(mStreamBuffer.prepare(available), sender);
                    if (sender == mEndpoint) {
                        mStreamBuffer.commit(len);
                        message = std::string(std::istreambuf_iterator<char>(mReceiveStream), std::istreambuf_iterator<char>());
                        break;
                    }
                    else {
                        mStreamBuffer.consume(len);
                        continue;
                    }
                }
                if (timeoutMilliseconds > 0) {
                    SLEEP_MULTI(attemptsTimeoutMilliseconds);
                    time += attemptsTimeoutMilliseconds;
                }
                else {
                    break;
                }
            }
#endif
            return message;
        }
    };

}

#endif
