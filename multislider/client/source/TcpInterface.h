/**
* MultiSlider library
*
* The MIT License (MIT)
* Copyright (c) 2016 Alexey Gruzdev
*/

#ifndef _MULTI_SLIDER_TCP_INTERFACE_H_
#define _MULTI_SLIDER_TCP_INTERFACE_H_

#include <SFML/Network.hpp>

#include <boost/asio.hpp>

#include "CommonIncludes.h"
#include "Utility.h"
#include "Pinger.h"

#if defined(__ANDROID__)
    #include <android/log.h>

    #define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO, "OGRE", __VA_ARGS__))
    #define LOGE(...) ((void)__android_log_print(ANDROID_LOG_ERROR, "OGRE", __VA_ARGS__)) 
#endif

namespace multislider
{
    /**
     *  Singleton wrapper for ENet library
     */
    class TcpInterface
    {
    public:
        const static size_t MAX_BUFFER_SIZE;

    private:
        sf::TcpSocket mSocket;

        boost::asio::io_service mIoService;
        boost::asio::ip::tcp::socket mAsioSocket;
        boost::asio::ip::tcp::endpoint mEndpoint;

        boost::asio::streambuf mStreamBuffer;
        std::istream mReceiveStream;

        boost::asio::io_service mIoServicePinger;
        shared_ptr<Pinger> mPinger;

        TcpInterface(const TcpInterface &);
        TcpInterface & operator= (const TcpInterface &);

    public:
        TcpInterface()
            : mIoService(), mAsioSocket(mIoService), mStreamBuffer(MAX_BUFFER_SIZE), mReceiveStream(&mStreamBuffer)
        { }

        bool connect(const std::string & ip, uint16_t port)
        {
#if defined(__ANDROID__)
            LOGI("TcpInterface[connect]: begin");
#endif
#if 1
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
#else
            boost::system::error_code err;

            boost::asio::ip::address::from_string( ip, err );
            if (err) {
                return false;
            }

#if defined(__ANDROID__)
            LOGI("TcpInterface[connect]: adress checked");
#endif

            boost::asio::ip::tcp::resolver resolver(mIoService);
            boost::asio::ip::tcp::resolver::query query(boost::asio::ip::tcp::v4(), ip, to_string(port));
            mEndpoint = *resolver.resolve(query, err);
            if (err) {
                return false;
            }

#if defined(__ANDROID__)
            LOGI("TcpInterface[connect]: resolver created");
#endif

            mAsioSocket.open(boost::asio::ip::tcp::v4(), err);
            if (err) {
                return false;
            }

#if defined(__ANDROID__)
            LOGI("TcpInterface[connect]: port opened");
#endif

            mAsioSocket.connect(mEndpoint, err);

#if defined(__ANDROID__)
            LOGI("TcpInterface[connect]: connection performed");
#endif

            //d.polubotko: icmp seems not allowed on android?
#if !defined(__ANDROID__)
            //mPinger.reset(new Pinger(mIoServicePinger, ip.c_str()));
#endif
#endif
#if defined(__ANDROID__)
            LOGI("TcpInterface[connect]: end"); 
#endif

            return true;
        }

        bool send(const std::string & message)
        {
#if 1
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
#else
            boost::system::error_code err;
            size_t len = mAsioSocket.send(boost::asio::buffer(message), 0, err);
            return !err && (len == message.size());
#endif
        }

        std::string tryReceive(uint64_t timeoutMilliseconds, uint32_t attemptsTimeoutMilliseconds = 100)
        {
            std::string message;
#if 1
            sf::Socket::Status status;
            uint64_t time = 0;
            while (time <= timeoutMilliseconds) {

                std::vector<char> buf(MAX_BUFFER_SIZE);
                size_t recieved;
                size_t offset = 0;
                do
                {
                    status = mSocket.receive(&buf[offset], MAX_BUFFER_SIZE, recieved);
                    offset += recieved;
                }while(status == sf::Socket::Partial);

                if (status == sf::Socket::Done)
                {
                    message = std::string(buf.begin(), buf.end());
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
                    size_t len = mAsioSocket.receive(mStreamBuffer.prepare(available));
                    mStreamBuffer.commit(len);
                    message = std::string(std::istreambuf_iterator<char>(mReceiveStream), std::istreambuf_iterator<char>());
                    break;
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

        size_t getPing() const
        {
            size_t res = 0;
            if(mPinger.get())
            {
                res = mPinger->getLastPing();
            }

            return res;
        }

        void pollPing()
        {
            mIoServicePinger.poll();
        }
    };

}

#endif
