/**
* MultiSlider library
*
* The MIT License (MIT)
* Copyright (c) 2016 Alexey Gruzdev
*/

#ifndef _MULTI_SLIDER_TCP_INTERFACE_H_
#define _MULTI_SLIDER_TCP_INTERFACE_H_

#include <boost/asio.hpp>

#include "CommonIncludes.h"
#include "Utility.h"
#include "Pinger.h"

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
            boost::system::error_code err;
            boost::asio::ip::tcp::resolver resolver(mIoService);
            boost::asio::ip::tcp::resolver::query query(boost::asio::ip::tcp::v4(), ip, to_string(port));
            mEndpoint = *resolver.resolve(query, err);
            if (err) {
                return false;
            }
            mAsioSocket.open(boost::asio::ip::tcp::v4(), err);
            if (err) {
                return false;
            }
            mAsioSocket.connect(mEndpoint, err);

            mPinger.reset(new Pinger(mIoServicePinger, ip.c_str()));

            return !err;
        }

        bool send(const std::string & message)
        {
            boost::system::error_code err;
            size_t len = mAsioSocket.send(boost::asio::buffer(message), 0, err);
            return !err && (len == message.size());
        }

        std::string tryReceive(uint64_t timeoutMilliseconds, uint32_t attemptsTimeoutMilliseconds = 100)
        {
            std::string message;
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
