/**
* MultiSlider library
*
* The MIT License (MIT)
* Copyright (c) 2016 Alexey Gruzdev
*/

#ifndef _MULTI_SLIDER_ROOM_INFO_H_
#define _MULTI_SLIDER_ROOM_INFO_H_

#include <string>
#include <vector>

#include "CommonIncludes.h"

namespace jsonxx
{
    class Object;
}

namespace multislider
{
    /**
     * Room descriptor for clients
     * @param name Name of the room
     * @param host Player name of the host
     * @param description Room description
     * @param playersLimit Maximum number of players in the room
     * @param playersReserved Amount of reserved players slots
     * @param playersNumber Current number of players, from 0 till playersLimit - playersReserved
     * @param players List of player names
     * @param userParam Arbitrary user parameter of the room
     */
    class RoomInfo
    {
        std::string mRoomName;
        std::string mHostName;
        std::string mDescription;
        uint32_t mPlayersLimit;
        uint32_t mPlayersNumber;
        uint32_t mPlayersReserved;
        std::string mUserParameter;
        std::vector<std::string> mPlayers;
        //-------------------------------------------------------
    
    public:
        RoomInfo() 
            : mPlayersLimit(0), mPlayersNumber(0), mPlayersReserved(0)
        { }

        /**
         *  Read room info from JSON string
         */
        bool deserialize(const std::string & str);

        /**
         *  Read room info from JSON 
         */
        bool deserialize(const jsonxx::Object & str);

        /**
         *  Noexcept swap
         */
        void swap(RoomInfo & other) throw();

        //-------------------------------------------------------
        // Getters

        const std::string & getName() const
        {
            return mRoomName;
        }

        const std::string & getHostName() const
        {
            return mHostName;
        }

        const std::string & getDescription() const
        {
            return mDescription;
        }

        uint32_t getPlayersLimit() const
        {
            return mPlayersLimit;
        }

        uint32_t getPlayersNumber() const
        {
            return mPlayersNumber;
        }

        uint32_t getReservedPlayersNumber() const
        {
            return mPlayersReserved;
        }

        const std::vector<std::string> & getPlayers() const
        {
            return mPlayers;
        }

        const std::string & getUserParameter() const
        {
            return mUserParameter;
        }
    };

    inline
    void swap(RoomInfo & one, RoomInfo & another)
    {
        one.swap(another);
    }
}

#endif 

