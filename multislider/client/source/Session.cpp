/**
* MultiSlider library
*
* The MIT License (MIT)
* Copyright (c) 2016 Alexey Gruzdev
*/

#include <limits>

#ifndef NOMINMAX
#define NOMINMAX
#endif

#if defined(_MSC_VER) && (_MSC_VER < 1800)
# include "CommonIncludes.h"
# ifndef _STDINT_H_
#  define _STDINT_H_
# endif
#endif

#include <boost/date_time.hpp>

#include "Session.h"
#include "Exception.h"
#include "Utility.h"
#include "Constants.h"
#include "UdpInterface.h"

#pragma warning(push)
#pragma warning(disable: 4127)
#include <jsonxx.h>
#pragma warning(pop)

using namespace jsonxx;

namespace multislider
{
    using namespace constants;

    struct MsgInfo
    {
        uint32_t seqIdx;
        uint64_t sentTime;
        std::string msgData;

        MsgInfo(uint32_t seqIdx, uint64_t sentTime, const std::string & msgData)
            : seqIdx(seqIdx), sentTime(sentTime), msgData(msgData)
        { }
    };


    //-------------------------------------------------------
    static const size_t MESSSAGES_BUFFER_SIZE = 32;
    static const size_t IDX_WRAP_MODULO = 1024;
    static const uint64_t CLOCK_SYNC_TIMEOUT_MS = 30 * 1000;

    void Session::destoyInstance(Session* ptr)
    {
        if (ptr != NULL) {
            delete ptr;
        }
    }
    //-------------------------------------------------------

    Session::Session(std::string ip, uint16_t port, const std::string & playerName, const std::string & sessionName, uint32_t sessionId)
        : mServerIp(ip), mServerPort(port), mPlayerName(playerName), mSessionName(sessionName), mSessionId(sessionId), mLastPing(0), mLastTimestamp(0), mStarted(false),
          mLocalSeqIdx(1), mRemoteSeqIdx(0), mPreviousIdxBits(0),
          mTimeShift(0), mLastSyncTime(0), mClockSyncId(0)
    {
        assert(!mServerIp.empty());
        assert(!mPlayerName.empty());
        assert(!mSessionName.empty());
    }
    //-------------------------------------------------------

    Session::~Session()
    {
        quit();
    }
    //-------------------------------------------------------

    void Session::quit()
    {
        if (mStarted) {
            Object quitJson;
            quitJson << MESSAGE_KEY_CLASS << backend::QUIT;
            quitJson << MESSAGE_KEY_PLAYER_NAME << mPlayerName;
            mUdpInterface->sendUpdDatagram(makeEnvelop(quitJson).write(JSON));
            mCallback->onQuit(this, getPlayerName(), false);
            mStarted = false;
        }
    }
    //-------------------------------------------------------

    uint32_t Session::getNextSeqIdx()
    {
        const uint32_t idx = mLocalSeqIdx;
        mLocalSeqIdx = (mLocalSeqIdx < IDX_WRAP_MODULO - 1) ? mLocalSeqIdx + 1 : 0;
        return idx;
    }
    //-------------------------------------------------------

    Object Session::makeEnvelop(const jsonxx::Object & obj) const
    {
        Object envelop;
        envelop << MESSAGE_KEY_CLASS << backend::ENVELOP;
        envelop << MESSAGE_KEY_SESSION_ID << mSessionId;
        envelop << MESSAGE_KEY_DATA << obj.write(JSON);
        return envelop;
    }
    //-------------------------------------------------------

    uint64_t Session::getTimeLocalMS() const
    {
        return boost::posix_time::microsec_clock::local_time().time_of_day().total_milliseconds();
    }
    //-------------------------------------------------------

    uint64_t Session::getTimeSyncronizedMS() const
    {
        return narrow_cast<uint64_t>(getTimeLocalMS() + mTimeShift);
    }
    //-------------------------------------------------------

    void Session::sendClockSync()
    {
        mClockSyncId = (mClockSyncId < 254) ? mClockSyncId + 1 : 0;
        Object clockSyncJson;
        clockSyncJson << MESSAGE_KEY_CLASS << backend::CLOCK_SYNC;
        clockSyncJson << MESSAGE_KEY_ID << mClockSyncId;
        clockSyncJson << MESSAGE_KEY_REQUEST_TIME << getTimeSyncronizedMS();
        clockSyncJson << MESSAGE_KEY_RESPONSE_TIME << 0;
        mUdpInterface->sendUpdDatagram(makeEnvelop(clockSyncJson).write(JSON));
    }
    //-------------------------------------------------------

    void Session::updatePing(uint64_t timestamp)
    {
        if (mLastSyncTime > 0) {
            const uint64_t currTime = getTimeSyncronizedMS();
            if (timestamp > currTime) {
                mLastPing = (mLastPing + (timestamp - currTime)) / 2;
            }
        }
    }
    //-------------------------------------------------------

    void Session::startup(SessionCallback* callback, uint64_t timeout)
    {
        if (callback == NULL) {
            throw RuntimeError("Session[Startup]: callback can't be null!");
        }

        mUdpInterface.reset(new UdpInterface());
        if (!mUdpInterface->open(mServerIp, mServerPort)) {
            throw RuntimeError("Session[Session]: Failed to setup socket");
        }
        mCallback = callback;

        // Now ready
        Object readyJson;
        readyJson << MESSAGE_KEY_CLASS << backend::READY;
        readyJson << MESSAGE_KEY_PLAYER_NAME << mPlayerName;
        const std::string readyMessage = makeEnvelop(readyJson).write(JSON);
        mUdpInterface->sendUpdDatagram(readyMessage);

        uint64_t time = 0;
        std::string receivedMessage;
        while ((receivedMessage = mUdpInterface->awaitUdpDatagram(DEFAULT_TIMEOUT_MS)).empty()) {
            time += DEFAULT_TIMEOUT_MS;
            if (time > timeout) {
                throw RuntimeError("Session[Startup]: Failed to get response from server");
            }
            // Resend
            mUdpInterface->sendUpdDatagram(readyMessage);
        }
        if(responsed(receivedMessage, RESPONSE_SUCC)) {
            while ((receivedMessage = mUdpInterface->awaitUdpDatagram(DEFAULT_TIMEOUT_MS)).empty()) {
                time += DEFAULT_TIMEOUT_MS;
                if (time > timeout) {
                    throw RuntimeError("Session[Startup]: Failed to get response from server");
                }
                // Resend
                mUdpInterface->sendUpdDatagram(readyMessage);
            }
            Object messageJson;
            messageJson.parse(receivedMessage);
            std::string messageClass(messageJson.get<std::string>(MESSAGE_KEY_CLASS, ""));
            if (!messageClass.empty()) {
                if (isMessageClass(messageClass, backend::START)) {
                    mStarted = true;
                    mCallback->onStart(this);
                    sendClockSync();
                    mLastTimestamp = getTimeSyncronizedMS();
                    return;
                }
            }
            throw RuntimeError("Session[Startup]: Unexpected server response!");
        }

        Object messageJson;
        messageJson.parse(receivedMessage);
        std::string messageClass(messageJson.get<std::string>(MESSAGE_KEY_CLASS, ""));
        if (!messageClass.empty()) {
            if (isMessageClass(messageClass, backend::START)) {
                mStarted = true;
                mCallback->onStart(this);
                sendClockSync();
                mLastTimestamp = getTimeSyncronizedMS();
                return;
            }
        }
        //else 
        throw RuntimeError("Session[Startup]: Unexpected server response!");
    }
    //-------------------------------------------------------

    void Session::broadcast(const std::string & data, const std::string & sharedData, bool forced)
    {
        if (!mStarted) {
            throw ProtocolError("Session[broadcast]: Session was not started!");
        }
        Object updateJson;
        updateJson << MESSAGE_KEY_CLASS << backend::UPDATE;
        updateJson << MESSAGE_KEY_PLAYER_NAME << mPlayerName;
        updateJson << MESSAGE_KEY_TIMESTAMP << getTimeLocalMS();
        updateJson << MESSAGE_KEY_FORCE_BROADCAST << forced;
        updateJson << MESSAGE_KEY_DATA << data;
        updateJson << MESSAGE_KEY_SHARED_DATA << sharedData;
        mUdpInterface->sendUpdDatagram(makeEnvelop(updateJson).write(JSON));
    }
    //-------------------------------------------------------

    void Session::broadcast(const std::string & data, bool forced)
    {
        broadcast(data, std::string(), forced);
    }
    //-------------------------------------------------------

    bool Session::sync(uint32_t syncId, uint64_t delay, bool reliable)
    {
        if (!mStarted) {
            throw ProtocolError("Session[broadcast]: Session was not started!");
        }
        Object syncJson;
        syncJson << MESSAGE_KEY_CLASS << backend::SYNC_REQUEST;
        syncJson << MESSAGE_KEY_PLAYER_NAME << mPlayerName;
        syncJson << MESSAGE_KEY_DELAY << delay;
        syncJson << MESSAGE_KEY_SYNC_ID << syncId;
        if (reliable) {
            if (mOutputQueue.size() >= MESSSAGES_BUFFER_SIZE) {
                return false;
            }
            uint32_t seqIdx = getNextSeqIdx();
            syncJson << MESSAGE_KEY_SEQ_INDEX << seqIdx;
            std::string syncMessage = makeEnvelop(syncJson).write(JSON);
            mOutputQueue.push_back(shared_ptr<MsgInfo>(new MsgInfo(seqIdx, getTimeSyncronizedMS(), syncMessage)));
            mUdpInterface->sendUpdDatagram(syncMessage);
        }
        else {
            syncJson << MESSAGE_KEY_SEQ_INDEX << (IDX_WRAP_MODULO + 1);
            mUdpInterface->sendUpdDatagram(makeEnvelop(syncJson).write(JSON));
        }
        return true;
    }
    //-------------------------------------------------------

    void Session::sync(uint32_t syncId, uint64_t delay)
    {
        sync(syncId, delay, false);
    }
    //-------------------------------------------------------

    void Session::say(const std::string & message)
    {
        if (!mStarted) {
            throw ProtocolError("Session[broadcast]: Session was not started!");
        }
        Object sayJson;
        sayJson << MESSAGE_KEY_CLASS  << backend::MESSAGE;
        sayJson << MESSAGE_KEY_SENDER << mPlayerName;
        sayJson << MESSAGE_KEY_DATA   << message;
        mUdpInterface->sendUpdDatagram(makeEnvelop(sayJson).write(JSON));
    }
    //-------------------------------------------------------

    void Session::keepAlive()
    {
        if (!mStarted) {
            throw ProtocolError("Session[keepAlive]: Session was not started!");
        }
        Object keepAliveJson;
        keepAliveJson << MESSAGE_KEY_CLASS << backend::KEEP_ALIVE;
        keepAliveJson << MESSAGE_KEY_PLAYER_NAME << mPlayerName;
        keepAliveJson << MESSAGE_KEY_TIMESTAMP << getTimeSyncronizedMS();
        mUdpInterface->sendUpdDatagram(makeEnvelop(keepAliveJson).write(JSON));
    }
    //-------------------------------------------------------

    uint32_t Session::receive(uint32_t messagesLimit)
    {
        uint32_t counter = 0;
        std::string receiveMessage;
        while(mStarted && counter < messagesLimit && !(receiveMessage = mUdpInterface->awaitUdpDatagram(0)).empty()) {
            ++counter;
            Object messageJson;
            messageJson.parse(receiveMessage);
            std::string messageClass(messageJson.get<std::string>(MESSAGE_KEY_CLASS, ""));
            if (isMessageClass(messageClass, backend::START)) {
                mCallback->onStart(this);
            }
            else if (isMessageClass(messageClass, backend::STATE)) {
                Array sessionDataJson;
                std::string dataJson = messageJson.get<std::string>(MESSAGE_KEY_DATA);
                sessionDataJson.parse(dataJson);
                SessionData sessionData;
                std::string shared;
                for (size_t i = 0; i < sessionDataJson.size(); ++i) {
                    Object entry = sessionDataJson.get<Object>(i);
                    PlayerData & data = sessionData[entry.get<std::string>(MESSAGE_KEY_NAME)];
                    data.data = entry.get<jsonxx::String>(MESSAGE_KEY_DATA, "");
                    data.timestamp = static_cast<uint64_t>(entry.get<jsonxx::Number>(MESSAGE_KEY_TIMESTAMP, 0));
                    data.alive = entry.get<jsonxx::Boolean>(MESSAGE_KEY_ALIVE, false);
                    data.weak  = entry.get<jsonxx::Boolean>(MESSAGE_KEY_WEAK, false);
                }
                PlayerData sharedData;
                sharedData.data = messageJson.get<jsonxx::String>(MESSAGE_KEY_SHARED_DATA, "");
                sharedData.timestamp = static_cast<uint64_t>(messageJson.get<jsonxx::Number>(MESSAGE_KEY_SHARED_TIMESTAMP, 0));
                sharedData.alive = true;
                mCallback->onUpdate(this, sessionData, sharedData);
            }
            else if (isMessageClass(messageClass, backend::SYNC)) {
                const uint32_t seqIdx = narrow_cast<uint32_t>(messageJson.get<jsonxx::Number>(MESSAGE_KEY_SEQ_INDEX, 0));
                if (seqIdx >= IDX_WRAP_MODULO || checkAcknowledgement(seqIdx)) {
                    mCallback->onSync(this, narrow_cast<uint32_t>(messageJson.get<jsonxx::Number>(MESSAGE_KEY_SYNC_ID)), false);
                }
            }
            else if (isMessageClass(messageClass, backend::KEEP_ALIVE)) {
                if (messageJson.has<jsonxx::Number>(MESSAGE_KEY_TIMESTAMP) && mLastSyncTime > 0) {
                    updatePing(narrow_cast<uint64_t>(messageJson.get<jsonxx::Number>(MESSAGE_KEY_TIMESTAMP)));
                }
                --counter; // KeepAlive messages are 'invisible'
            }
            else if (isMessageClass(messageClass, backend::ACK)) {
                const uint32_t ackIdx = narrow_cast<uint32_t>(messageJson.get<jsonxx::Number>(MESSAGE_KEY_SEQ_INDEX, 0));
                checkAcknowledgement(ackIdx);
            }
            else if (isMessageClass(messageClass, backend::CLOCK_SYNC)) {
                if (mClockSyncId == narrow_cast<uint8_t>(messageJson.get<jsonxx::Number>(MESSAGE_KEY_ID, 255))) {
                    if (messageJson.has<jsonxx::Number>(MESSAGE_KEY_REQUEST_TIME) && messageJson.has<jsonxx::Number>(MESSAGE_KEY_RESPONSE_TIME)) {
                        const uint64_t currTime = getTimeSyncronizedMS();
                        const uint64_t requestTime = narrow_cast<uint64_t>(messageJson.get<jsonxx::Number>(MESSAGE_KEY_REQUEST_TIME));
                        mLastSyncTime = currTime;
                        assert(currTime > requestTime);
                        mLastPing = (currTime - requestTime) / 2;
                        mTimeShift += static_cast<int64_t>(messageJson.get<jsonxx::Number>(MESSAGE_KEY_RESPONSE_TIME)) - static_cast<int64_t>(requestTime) - static_cast<int64_t>(mLastPing);
                    }
                }
            }
            else if (isMessageClass(messageClass, backend::QUIT)) {
                const std::string player = messageJson.get<jsonxx::String>(MESSAGE_KEY_PLAYER_NAME);
                mCallback->onQuit(this, player, false);
                if (player == mPlayerName) {
                    mStarted = false;
                }
            }
            else if (isMessageClass(messageClass, backend::MESSAGE)) {
                mCallback->onMessage(this, messageJson.get<jsonxx::String>(MESSAGE_KEY_SENDER, ""), messageJson.get<jsonxx::String>(MESSAGE_KEY_DATA));
            }
            else {
                throw RuntimeError("Session[receive]: Unknown datagram type!");
            }
            mLastTimestamp = getTimeSyncronizedMS();
        }
        const uint64_t currentTime = getTimeSyncronizedMS();
        if (mStarted) {
            // Check timeout
            if (currentTime > mLastTimestamp + KEEP_ALIVE_LIMIT) {
                mCallback->onQuit(this, getPlayerName(), true);
                mStarted = false;
            }
        }
        if (mStarted) {
            // Resend messages if necessary
            for (size_t idx = 0; idx < mOutputQueue.size(); ++idx) {
                shared_ptr<MsgInfo> & msg = mOutputQueue[idx];
                if (currentTime > msg->sentTime + RESEND_INTERVAL) {
                    mUdpInterface->sendUpdDatagram(msg->msgData);
                    msg->sentTime = currentTime;
                }
            }
            // Sync clock if necessary
            if (currentTime > mLastSyncTime + CLOCK_SYNC_TIMEOUT_MS) {
                sendClockSync();
            }
        }
        return counter;
    }
    //-------------------------------------------------------

    uint32_t Session::receive()
    {
        return receive(std::numeric_limits<uint32_t>::max());
    }
    //-------------------------------------------------------

    bool Session::checkAcknowledgement(uint32_t ackIdx)
    {
        // Same message like before
        if (ackIdx == mRemoteSeqIdx) {
            return false;
        }
        // Ack is before previous message
        if ((ackIdx > mRemoteSeqIdx) && (ackIdx - mRemoteSeqIdx < MESSSAGES_BUFFER_SIZE)) {
            removeAcknowledged(ackIdx);
            mPreviousIdxBits <<= (ackIdx - mRemoteSeqIdx);
            mRemoteSeqIdx = ackIdx;
            return true;
        }
        // Ack is before previous message as well but wrapped by modulo 
        if ((IDX_WRAP_MODULO + ackIdx > mRemoteSeqIdx) && (IDX_WRAP_MODULO + ackIdx - mRemoteSeqIdx) < MESSSAGES_BUFFER_SIZE) {
            removeAcknowledged(ackIdx);
            mPreviousIdxBits <<= (IDX_WRAP_MODULO + ackIdx - mRemoteSeqIdx);
            mRemoteSeqIdx = ackIdx;
            return true;
        }
        // Ack is before previous message
        if ((ackIdx > mRemoteSeqIdx) && ackIdx - mRemoteSeqIdx < MESSSAGES_BUFFER_SIZE) {
            const uint32_t ackBit = 0x1 << (ackIdx - mRemoteSeqIdx - 1);
            if (0 == (mPreviousIdxBits & ackBit)) {
                // First time message
                removeAcknowledged(ackIdx);
                mPreviousIdxBits = mPreviousIdxBits | ackBit;
                return true;
            }
        }
        return false;
    }
    //-------------------------------------------------------

    void Session::removeAcknowledged(uint32_t ackIdx) {
        size_t idx = 0;
        for (; idx < mOutputQueue.size(); ++idx) {
            if (mOutputQueue[idx]->seqIdx == ackIdx) {
                break;
            }
        }
        if (idx < mOutputQueue.size()) {
            mOutputQueue.erase(mOutputQueue.begin() + idx);
        }
    }
    //-------------------------------------------------------

    uint64_t Session::getLastPing() const
    {
        return mLastPing;
    }

    //-------------------------------------------------------

    uint64_t Session::getConnectionTimeout()
    {
        return KEEP_ALIVE_LIMIT;
    }
}



