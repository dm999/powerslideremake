/**
* MultiSlider library
*
* The MIT License (MIT)
* Copyright (c) 2016 Alexey Gruzdev
*/

#if defined(_MSC_VER) && (_MSC_VER < 1800)
# include "CommonIncludes.h"
# ifndef _STDINT_H_
#  define _STDINT_H_
# endif
#endif

#include "RoomInfo.h"
#include "Utility.h"
#include "Constants.h"

#pragma warning(push)
#pragma warning(disable: 4127)
#include <jsonxx.h>
#pragma warning(pop)

using namespace jsonxx;

namespace multislider
{
    using namespace constants;

    void RoomInfo::swap(RoomInfo & other) throw()
    {
        if (this != &other) {
            using std::swap;
            swap(mRoomName, other.mRoomName);
            swap(mHostName, other.mHostName);
            swap(mDescription, other.mDescription);
            swap(mPlayersLimit, other.mPlayersLimit);
            swap(mPlayersNumber, other.mPlayersNumber);
            swap(mPlayersReserved, other.mPlayersReserved);
            swap(mPlayers, other.mPlayers);
            swap(mUserParameter, other.mUserParameter);
        }
    }

    bool RoomInfo::deserialize(const std::string & str)
    {
        Object infoJson;
        infoJson.parse(std::string(str.c_str(), str.size()));
        return deserialize(infoJson);
    }

    bool RoomInfo::deserialize(const Object & infoJson)
    {
        RoomInfo info;
        bool success = true;
        do {
            if (infoJson.has<jsonxx::String>(MESSAGE_KEY_NAME)) {
                info.mRoomName = infoJson.get<jsonxx::String>(MESSAGE_KEY_NAME);
            }
            else {
                success = false;
                break;
            }
            if (infoJson.has<jsonxx::String>(MESSAGE_KEY_HOST)) {
                info.mHostName = infoJson.get<jsonxx::String>(MESSAGE_KEY_HOST);
            }
            else {
                success = false;
                break;
            }
            if (infoJson.has<jsonxx::String>(MESSAGE_KEY_ROOM_DESC)) {
                info.mDescription = infoJson.get<jsonxx::String>(MESSAGE_KEY_ROOM_DESC);
            }
            else {
                success = false;
                break;
            }
            if (infoJson.has<jsonxx::Number>(MESSAGE_KEY_PLAYERS_LIMIT)) {
                info.mPlayersLimit = narrow_cast<uint32_t>(infoJson.get<jsonxx::Number>(MESSAGE_KEY_PLAYERS_LIMIT));
            }
            else {
                success = false;
                break;
            }
            if (infoJson.has<jsonxx::Number>(MESSAGE_KEY_PLAYERS_NUMBER)) {
                info.mPlayersNumber = narrow_cast<uint32_t>(infoJson.get<jsonxx::Number>(MESSAGE_KEY_PLAYERS_NUMBER));
            }
            else {
                success = false;
                break;
            }
            if (infoJson.has<jsonxx::Number>(MESSAGE_KEY_PLAYERS_RESERVED)) {
                info.mPlayersReserved = narrow_cast<uint32_t>(infoJson.get<jsonxx::Number>(MESSAGE_KEY_PLAYERS_RESERVED));
            }
            else {
                success = false;
                break;
            }
            if (infoJson.has<Array>(MESSAGE_KEY_PLAYERS)) {
                Array playersJsonArray = infoJson.get<Array>(MESSAGE_KEY_PLAYERS);
                for (size_t idx = 0; idx < playersJsonArray.size(); ++idx) {
                    info.mPlayers.push_back(playersJsonArray.get<jsonxx::String>(idx));
                }
                if (info.mPlayersNumber != narrow_cast<uint32_t>(info.mPlayers.size())) {
                    success = false;
                    break;
                }
            }
            if (infoJson.has<jsonxx::String>(MESSAGE_KEY_USER_PARAM)) {
                info.mUserParameter = infoJson.get<jsonxx::String>(MESSAGE_KEY_USER_PARAM);
            }
            else {
                success = false;
                break;
            }
        } while (false);
        if (success) {
            info.swap(*this);
        }
        return success;
    }

}


